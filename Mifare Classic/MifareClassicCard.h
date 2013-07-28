//
//  MifareClassicCard.h
//  NFCSystem2
//
//  Created by Will Yager on 7/26/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __NFCSystem2__MifareClassicCard__
#define __NFCSystem2__MifareClassicCard__

#include <iostream>
#include <nfc/nfc.h>
#include <vector>
#include <array>

struct bits_container {
    std::vector<uint8_t> data;
    int num_bits = 0;
};
const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
};


class MifareClassicCard {
    bool ready = false;
    
    ////////all LibNFC specific members/////////
    nfc_device *pnd;
    nfc_target nt; //This is the big kahuna. It holds a lot of important data about the target. It's a union of a bunch of types that can hold data about any kind of NFC tag.
    /////////////////////////////////////////////
    
public:
    bool is_ready(){
        return this->ready;
    }
    //Tries to scan for a card. If successful return MifareClassicCard with
    //ready bit set to true.
    MifareClassicCard(nfc_device* pnd);
    std::vector<uint8_t> answer_to_request();//ATQA
    std::vector<uint8_t>  unique_identifier();//UID
    std::vector<uint8_t> select_acknowledge();//SAK
    std::vector<uint8_t> answer_to_select();//ATS
    std::string basic_card_info();
    static bool is_first_block_in_sector(int block);
    static bool is_trailer_block_in_sector(int block);
    static bool next_trailer_block_in_sector(int block);
    //returns response from device
    std::vector<uint8_t> transmit_bytes(std::vector<uint8_t> bytes_to_send);
    bits_container transmit_bits(bits_container bits_to_send);
    std::vector<uint8_t> auth_unique_identifier();
    //using_key_a means "Try to auth by guessing key a"
    //returns a vector containing the key on success, an empty vector on failure
    std::vector<uint8_t> authenticate(int block, bool using_key_a);
    bool authenticate(int block, std::vector<uint8_t> key, std::vector<uint8_t> uid, bool using_key_a);
    int guess_card_size();
    //Request for Answer To Select (RATS)
    std::vector<uint8_t> request_for_answer_to_select();
    std::vector<uint8_t> read_entire_card();
    std::vector<uint8_t> read_sector(int sector);
    bool write_sector(int sector, std::vector<uint8_t> data); //true on success. Write 64 bytes to specified sector (except zero and trailers. So really only 32-48 bytes)
};

#endif /* defined(__NFCSystem2__MifareClassicCard__) */
