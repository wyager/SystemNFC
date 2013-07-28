//
//  MifareClassicCard.cpp
//  NFCSystem2
//
//  Created by Will Yager on 7/26/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "MifareClassicCard.h"
#include "MifareClassicScanner.h"
#include "HexConversion.h"
extern "C"{
    #include "mifare.h"
}
static uint8_t keys[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
    0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd,
    0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a,
    0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xab, 0xcd, 0xef, 0x12, 0x34, 0x56
};
const int num_keys = sizeof(keys)/6;

//Tries to scan for a card. If successful return MifareClassicCard with
//ready bit set to true.
MifareClassicCard::MifareClassicCard(nfc_device* pnd){
    this->pnd = pnd;
    if(nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &this->nt)<=0){
        //No cards found.
        return;
    }
    //Card found.
    this->ready = true;
    //this->nt now holds some basic information about the card.
    if((this->nt.nti.nai.btSak & 0x08)==0){
        std::cerr << "Warning: Probably not an MFC tag.\n";
    }
    return;
}

std::vector<uint8_t> MifareClassicCard::answer_to_request()//ATQA
{
    nfc_iso14443a_info nai = this->nt.nti.nai;
    return std::vector<uint8_t>(nai.abtAtqa, nai.abtAtqa+2);
}
std::vector<uint8_t>  MifareClassicCard::unique_identifier()//UID
{
    nfc_iso14443a_info nai = this->nt.nti.nai;
    return std::vector<uint8_t>(nai.abtUid, nai.abtUid+nai.szUidLen);
}
std::vector<uint8_t> MifareClassicCard::select_acknowledge()//SAK
{
    nfc_iso14443a_info nai = this->nt.nti.nai;
    return std::vector<uint8_t>({nai.btSak});
}
std::vector<uint8_t> MifareClassicCard::answer_to_select()//ATS
{
    nfc_iso14443a_info nai = this->nt.nti.nai;
    return std::vector<uint8_t>(nai.abtAts, nai.abtAts+nai.szAtsLen);
}

std::string MifareClassicCard::basic_card_info()
{
    std::string result;
    result.append("ATQA:");
    result.append(binary_to_hex(this->answer_to_request()));
    result.append("\nUID:");
    result.append(binary_to_hex(this->unique_identifier()));
    result.append("\nSAK:");
    result.append(binary_to_hex(this->select_acknowledge()));
    result.append("\nATS:");
    result.append(binary_to_hex(this->answer_to_select()));
    return result;
}

bool MifareClassicCard::is_first_block_in_sector(int block)
{
    if (block < 128)
        return ((block) % 4 == 0);
    else
        return ((block) % 16 == 0);
}
bool MifareClassicCard::is_trailer_block_in_sector(int block)
{
    if (block < 128)
        return ((block + 1) % 4 == 0);
    else
        return ((block + 1) % 16 == 0);
}
bool MifareClassicCard::next_trailer_block_in_sector(int block)
{
    uint32_t trailer_block = 0;
    if (block < 128) {
        trailer_block = block + (3 - (block % 4));
    } else {
        trailer_block = block + (15 - (block % 16));
    }
    return trailer_block;
}


//Returns the card size in (16 byte) blocks. Returns number of blocks minus one (that's how libnfc does it)
int MifareClassicCard::guess_card_size(){
    if ((nt.nti.nai.abtAtqa[1] & 0x02) == 0x02) return 0xFF;
    else if ((nt.nti.nai.btSak & 0x01) == 0x01) return 0x13;
    else{
        int blocks = 0x3f;
        std::vector<uint8_t> RATS = this->request_for_answer_to_select();
        long res = RATS.size();
        if ((res >= 10) && (RATS[5] == 0xc1) && (RATS[6] == 0x05)
            && (RATS[7] == 0x2f) && (RATS[8] == 0x2f)
            && ((nt.nti.nai.abtAtqa[1] & 0x02) == 0x00)) {
            // MIFARE Plus 2K
            blocks = 0x7f;
        }
        // Chinese magic emulation card, ATS=0978009102:dabc1910
        if ((res == 9)  && (RATS[5] == 0xda) && (RATS[6] == 0xbc)
            && (RATS[7] == 0x19) && (RATS[8] == 0x10)) {
            //magic2 = true; //Does magic chinese emulation stuff
            std::cerr << "Warning: Chinese emulation card. I do not have code written to deal with this.\n";
        }
        return blocks;
    }
}

//returns response from device. Empty vector if error.
std::vector<uint8_t> MifareClassicCard::transmit_bytes(std::vector<uint8_t> bytes_to_send){
    uint8_t rx_bytes[264];
    int bytes_recieved =nfc_initiator_transceive_bytes(this->pnd, &bytes_to_send[0], bytes_to_send.size(), rx_bytes, sizeof(rx_bytes), 0);
    if(bytes_recieved < 0){
        std::cerr << "Error while transcieving bytes.\n";
        return std::vector<uint8_t>();
    }
    return std::vector<uint8_t>(rx_bytes, rx_bytes+bytes_recieved);
}

bits_container MifareClassicCard::transmit_bits(bits_container bits_to_send){
    bits_container result;
    uint8_t rx_bits[264];
    int bits_recieved =nfc_initiator_transceive_bits(this->pnd, &bits_to_send.data[0], bits_to_send.num_bits, NULL, rx_bits, sizeof(rx_bits), NULL);
    if(bits_recieved < 0){
        std::cerr << "Error while transcieving bits.\n";
        return result;
    }
    result.data = std::vector<uint8_t>(rx_bits, rx_bits + sizeof(rx_bits));
    result.num_bits = bits_recieved;
    return result;
}

std::vector<uint8_t> MifareClassicCard::auth_unique_identifier()
{
    uint8_t* start = this->nt.nti.nai.abtUid + this->nt.nti.nai.szUidLen - 4;
    uint8_t* end = this->nt.nti.nai.abtUid + this->nt.nti.nai.szUidLen;
    return std::vector<uint8_t>(start, end);
}

//using_key_a means "Try to auth by guessing key a"
//returns a vector containing the key on success, an empty vector on failure
std::vector<uint8_t> MifareClassicCard::authenticate(int block, bool using_key_a)
{
    for (int key_number = 0; key_number < num_keys; key_number++) {
        std::vector<uint8_t> key(keys+key_number*6,keys+key_number*6+6);
        std::vector<uint8_t> uid = this->auth_unique_identifier();
        bool result = this->authenticate(block, key, uid, using_key_a);
        if(result){//Successful authentication!
            return key;
        }
    }
    std::cout << "Failed to guess key for authentication. Resetting card.\n";
    nfc_initiator_select_passive_target(this->pnd, nmMifare, this->nt.nti.nai.abtUid, this->nt.nti.nai.szUidLen, NULL);//Reset card
    return std::vector<uint8_t>();
}

//using_key_a just tells libnfc that the key we are providing is key a and not key b.
//Note that in the C example, authenticate writes the UID to mp.mpa.abtAthUid and the key to mp.mpa.abtKey, and the key to mtKeys.amb[uiBlock].mbt.abtKeyA/B
bool MifareClassicCard::authenticate(int block, std::vector<uint8_t> key, std::vector<uint8_t> uid, bool using_key_a)
{
    mifare_cmd command = using_key_a ? MC_AUTH_A : MC_AUTH_B;
    //Combine the 6-byte key and 4-byte uid (in that order)
    std::vector<uint8_t>auth_parameters = key;
    auth_parameters.insert(auth_parameters.end(), uid.begin(), uid.end());
    if(nfc_initiator_mifare_cmd(this->pnd, command, block, (mifare_param*)&auth_parameters[0]) == false){
        return false;
    }
    return true;
}

//Returns a vector. The length of the vector is the same thing as the result of get_rats in C.
//May return zero length vector even if everything worked.
std::vector<uint8_t> MifareClassicCard::request_for_answer_to_select(){
    std::vector<uint8_t> abtRats;
    abtRats.push_back(0xe0); abtRats.push_back(0x50);
    if(nfc_device_set_property_bool(this->pnd, NP_EASY_FRAMING, false)<0){
        std::cerr << "Failed to disable easy framing.\n";
        return std::vector<uint8_t>();
    }
    std::vector<uint8_t> result = this->transmit_bytes(abtRats);
    if(result.size()>0){
        // ISO14443-4 card, turn RF field off/on to access ISO14443-3 again
        nfc_device_set_property_bool(pnd, NP_ACTIVATE_FIELD, false);
        nfc_device_set_property_bool(pnd, NP_ACTIVATE_FIELD, true);
    }
    //reselect tag
    if(nfc_initiator_select_passive_target(this->pnd, nmMifare, NULL, 0, &this->nt) <= 0){
        std::cerr << "Failed to re-acquire tag.\n";
        return std::vector<uint8_t>();
    }
    return result;//May return zero length vector even if everything worked.
}


//true on success
bool MifareClassicCard::write_sector(int sector, std::vector<uint8_t> data){
    if(data.size() != 16*4){
        std::cerr << "Data to write to sector is not correct length.\n";
        return false;
    }
    mifare_param block_readwrite_information;//Where we put auth key AND where the resultant read goes
    int first_block = sector*4;
    int trailer_block = sector*4+3;
    int current_block = first_block;
    //In nfc-mfclassic.c, they auth on the first block instead of the last.
    std::vector<uint8_t> auth_key = this->authenticate(current_block, true);
    memcpy(block_readwrite_information.mpa.abtKey, &auth_key[0], 6);//Put auth key in params
    //May have to copy UID as well, not sure...
    if(auth_key.size() == 0){
        std::cerr << "Failed to auth first block while writing single sector.\n";
        return false;
    }
    if (current_block == 0){
        std::cerr << "Can't write to first block of entire card except on chinese fake cards.\n";
        std::cerr << "Only writing to blocks 1-3. Please modify code if you want to write to block 0.\n";
        current_block = 1;
    }
    for (; current_block < trailer_block; current_block++) {
        memcpy(block_readwrite_information.mpd.abtData, &data[current_block*16],16);
        if(!nfc_initiator_mifare_cmd(this->pnd, MC_WRITE, current_block, &block_readwrite_information)){
            std::cerr << "Failed to write to  block.\n";
            return false;
        }
    }
    //I'm not going to write to trailer in this function.
    return true;
}
std::vector<uint8_t> MifareClassicCard::read_sector(int sector){
    int first_block = sector*4;
    int trailer_block = sector*4+3;
    std::vector<uint8_t> sector_data;
    mifare_param block_readwrite_information;//Where we put auth key AND where the resultant read goes
    //I'm not doing the read_unlocked (i.e. already unlocked) stuff here.
    int current_block = trailer_block;
    {
        std::vector<uint8_t> auth_key = this->authenticate(current_block, true);
        if(auth_key.size() == 0){
            std::cerr << "Failed to auth trailer while reading single sector.\n";
            return std::vector<uint8_t>();
        }
        memcpy(block_readwrite_information.mpa.abtKey, &auth_key[0], 6);//Put auth key in params
        if (!nfc_initiator_mifare_cmd(this->pnd, MC_READ, current_block, &block_readwrite_information)) {
            std::cout << "Failed to read trailer block while reading single sector.\n";
            return std::vector<uint8_t>();
        } //All the data we just read is now in block_readwrite_information
        //We have to put in the proper keys ourselves, however. The crypto
        //keys don't copy properly I guess.
        std::vector<uint8_t> trailer_data(16);
        memcpy(&trailer_data[0], &auth_key[0], 6);
        memcpy(&trailer_data[6], &block_readwrite_information.mpd.abtData[6], 4);
        memset(&trailer_data[10], 0, 6);//We have no idea what key B is in this
        //particular function, so we're guessing 0x00*6
        sector_data.insert(sector_data.begin(), trailer_data.begin(), trailer_data.end());//Save trailer data
    }
    current_block--;//We have to read backwards from trailer (I think)
    for (; current_block >= first_block; current_block--) {
        if(!nfc_initiator_mifare_cmd(this->pnd, MC_READ, current_block, &block_readwrite_information)){
            std::cerr << "Failed to read data block while reading single sector.\n";
            return std::vector<uint8_t>();
        }
        std::vector<uint8_t> block_data(16);
        memcpy(&block_data[0], block_readwrite_information.mpd.abtData, 16);
        sector_data.insert(sector_data.begin(), block_data.begin(), block_data.end());
    }
    return sector_data;
}
//returns empty vector on failure
std::vector<uint8_t> MifareClassicCard::read_entire_card(){
    std::vector<uint8_t> card_data;
    int max_sector=this->guess_card_size()/4;
    for (int current_sector = max_sector; current_sector >= 0; current_sector--) {
        std::vector<uint8_t> sector_data = this->read_sector(current_sector);
        if(sector_data.size() == 0){
            std::cerr << "Problem reading entire card. Sector error.\n";
            return std::vector<uint8_t>();
        }
        card_data.insert(card_data.begin(), sector_data.begin(), sector_data.end());
    }
    std::cerr << "Done reading all blocks.\n";
    return card_data;
}