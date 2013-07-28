//
//  MifareUltralightCard.cpp
//  NFCSystem2
//
//  Created by William Yager on 7/28/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "MifareUltralightCard.h"
#include "MifareUltralightScanner.h"
#include "HexConversion.h"
extern "C"{
#include "mifare.h"
}
//Tries to scan for a card. If successful return MifareUltralightCard with
//ready bit set to true.
MifareUltralightCard::MifareUltralightCard(nfc_device* pnd){
    this->pnd = pnd;
    if(nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &this->nt)<=0){
        //No cards found.
        return;
    }
    //Card found.
    //this->nt now holds some basic information about the card.
    if(this->nt.nti.nai.abtAtqa[1] != 0x44){
        std::cerr << "Warning: Tag is not a MIFARE Ultralight.\n";
        return;
    }
    this->ready = true;
    return;
}

std::vector<uint8_t> MifareUltralightCard::unique_identifier(){
    return std::vector<uint8_t>(this->nt.nti.nai.abtUid, this->nt.nti.nai.abtUid + this->nt.nti.nai.szUidLen);
}

std::string MifareUltralightCard::basic_card_info(){
    std::string result;
    result.append("UID: ");
    result.append(binary_to_hex(this->unique_identifier()));
    return result;
}
//Block is 16 bytes. Card sends back 16 bytes at a time, instead of 4 bytes (1 page) at a time. Data is still accessed by page number.
//Just FYI: sending a READ for page 3 does return pages 3,4,5,6. There is no need to be 4-byte-aligned during page reads. I just choose to be.
std::vector<uint8_t> MifareUltralightCard::read_block(int block_number){
    mifare_param mifare_readwrite_information;//Stores information sent to and read from the card.
    int page_number = block_number*4;
    if(!nfc_initiator_mifare_cmd(this->pnd, MC_READ, page_number, &mifare_readwrite_information)){
        std::cerr << "Error: Failed to read block.\n";
        return std::vector<uint8_t>();
    }
    std::vector<uint8_t> result(16);
    memcpy(&result[0], mifare_readwrite_information.mpd.abtData, 16);
    return result;
}

//Page is 4 bytes. Only 4 bytes can be written at a time.
bool MifareUltralightCard::write_page(int page_number, std::vector<uint8_t> data){
    if (data.size() != 4) {
        std::cerr << "Error: Data vector to write is wrong size.\n";
        return false;
    }
    if(page_number <= 0x01){
        std::cerr << "Warning: Can not write to serial number memory.\n";
        return false;//I guess this is actually an error.
    }
    if(page_number == 0x02){
        std::cerr << "Warning: Not writing Lock Bytes. I guess you can, but it's risky. Please remove this check if you want to write OTP bytes.\n";
        return true; //It's not really an error, per se.
    }
    if(page_number == 0x03){
        std::cerr << "Warning: Not writing OTP Bytes. I guess you can, but it's risky. Please remove this check if you want to write OTP bytes.\n";
        return true; //It's not really an error, per se.
    }
    mifare_param mifare_readwrite_information;
    memcpy(mifare_readwrite_information.mpd.abtData, &data[0], 4);
    //On this card, this command apparently sends 16 bytes but only writes the first four. This is a compatibility mode command.
    if(!nfc_initiator_mifare_cmd(this->pnd, MC_WRITE, page_number, &mifare_readwrite_information)){
        std::cerr << "Error: Failed to write page.\n";
        return false;
    }
    return true;
}