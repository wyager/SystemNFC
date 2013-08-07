//
//  MifareUltralightCard.h
//  NFCSystem2
//
//  Created by William Yager on 7/28/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __NFCSystem2__MifareUltralightCard__
#define __NFCSystem2__MifareUltralightCard__

#include <iostream>
#include <nfc/nfc.h>
#include <vector>
#include <array>
#include "nfc_defines.h"


class MifareUltralightCard {
    bool ready = false;
    
    ////////all LibNFC specific members/////////
    nfc_device *pnd;
    nfc_target nt; //This is the big kahuna. It holds a lot of important data about the target. It's a union of a bunch of types that can hold data about any kind of NFC tag.
    /////////////////////////////////////////////
    
public:
    bool is_ready(){
        return this->ready;
    }
    //Tries to scan for a card. If successful return MifareUltralightCard with
    //ready bit set to true.
    MifareUltralightCard(nfc_device* pnd);
    std::vector<uint8_t> unique_identifier();//UID
    std::string basic_card_info();
    //Block is 16 bytes
    std::vector<uint8_t> read_block(int block_number);
    //Takes just as long as read_block, but only reads 4 bytes at a time.
    std::vector<uint8_t> read_page(int page_number);
    //Page is 4 bytes. Returns true on success
    bool write_page(int page_number, std::vector<uint8_t> data);
    
};

#endif /* defined(__NFCSystem2__MifareUltralightCard__) */
