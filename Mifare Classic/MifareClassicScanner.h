//
//  MifareClassicScanner.h
//  NFCSystem2
//
//  Created by Will Yager on 7/26/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __NFCSystem2__MifareClassicScanner__
#define __NFCSystem2__MifareClassicScanner__
#include <iostream>
#include "MifareClassicCard.h"
#include <nfc/nfc.h>

class MifareClassicScanner {
    nfc_device* pnd;
    nfc_context* context;
    bool ready = false;
    
public:
    MifareClassicScanner();
    ~MifareClassicScanner();
    MifareClassicCard scan_once();
    MifareClassicCard scan_forever();
    bool turn_off_field();
    bool turn_on_field();
    bool reset_card();
    bool is_ready(){
        return this->ready;
    }
};

#endif /* defined(__NFCSystem2__MifareClassicScanner__) */
