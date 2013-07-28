//
//  MifareClassicScanner.cpp
//  NFCSystem2
//
//  Created by Will Yager on 7/26/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include "MifareClassicScanner.h"

MifareClassicScanner::MifareClassicScanner(){
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
    // Disable ISO14443-4 switching in order to read devices that emulate Mifare Classic with ISO14443-4 compliance.
    nfc_device_set_property_bool(pnd, NP_AUTO_ISO14443_4, false);
    this->ready = true;
}

MifareClassicScanner::~MifareClassicScanner(){
    nfc_close(this->pnd);
    nfc_exit(this->context);
}


MifareClassicCard MifareClassicScanner::scan_once()
{
    return MifareClassicCard(this->pnd);
}
MifareClassicCard MifareClassicScanner::scan_forever()
{
    MifareClassicCard card = this->scan_once();
    while(!card.is_ready()){
        card = this->scan_once();
    }
    
    return card;
}

bool MifareClassicScanner::turn_off_field()
{
    if(nfc_device_set_property_bool(this->pnd, NP_ACTIVATE_FIELD, false) < 0){
        std::cerr << "Could not turn off field.\n";
        return false;
    }
    return true;
    
}
bool MifareClassicScanner::turn_on_field()
{
    
    if(nfc_device_set_property_bool(this->pnd, NP_ACTIVATE_FIELD, true) < 0){
        std::cerr << "Could not turn on field.\n";
        return  false;
    }
    return true;
}
bool MifareClassicScanner::reset_card()
{
    return this->turn_off_field() && this->turn_on_field();
}