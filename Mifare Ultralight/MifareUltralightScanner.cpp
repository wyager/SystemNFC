//
//  MifareUltralightScanner.cpp
//  NFCSystem2
//
//  Created by William Yager on 7/28/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "MifareUltralightScanner.h"

MifareUltralightScanner::MifareUltralightScanner(){
    //Start libnfc
    nfc_init(&this->context);
    if(!this->context){
        std::cerr << "Error: Could not init libnfc.\n";
        return;
    }
    //Open reader device
    this->pnd = nfc_open(this->context, NULL);
    if(!this->pnd){
        std::cerr << "Error: Could not open NFC reader/writer.\n";
        return;
    }
    //Become an initiator
    if(nfc_initiator_init(this->pnd)<0){
        std::cerr << "Error: Could not become an initiator.\n";
        return;
    }
    
    this->ready = true;
}

MifareUltralightScanner::~MifareUltralightScanner(){
    nfc_close(this->pnd);
    nfc_exit(this->context);
}


MifareUltralightCard MifareUltralightScanner::scan_once()
{
    return MifareUltralightCard(this->pnd);
}
MifareUltralightCard MifareUltralightScanner::scan_forever()
{
    MifareUltralightCard card = this->scan_once();
    while(!card.is_ready()){
        card = this->scan_once();
    }
    
    return card;
}

bool MifareUltralightScanner::turn_off_field()
{
    if(nfc_device_set_property_bool(this->pnd, NP_ACTIVATE_FIELD, false) < 0){
        std::cerr << "Could not turn off field.\n";
        return false;
    }
    return true;
    
}
bool MifareUltralightScanner::turn_on_field()
{
    
    if(nfc_device_set_property_bool(this->pnd, NP_ACTIVATE_FIELD, true) < 0){
        std::cerr << "Could not turn on field.\n";
        return  false;
    }
    return true;
}
bool MifareUltralightScanner::reset_card()
{
    return this->turn_off_field() && this->turn_on_field();
}