//
//  MifareUltralightScanner.h
//  NFCSystem2
//
//  Created by William Yager on 7/28/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#ifndef __NFCSystem2__MifareUltralightScanner__
#define __NFCSystem2__MifareUltralightScanner__
#include <iostream>
#include "MifareUltralightCard.h"
#include <nfc/nfc.h>

class MifareUltralightScanner {
    nfc_device* pnd;
    nfc_context* context;
    bool ready = false;
    
public:
    MifareUltralightScanner();
    ~MifareUltralightScanner();
    MifareUltralightCard scan_once();
    MifareUltralightCard scan_forever();
    bool turn_off_field();
    bool turn_on_field();
    bool reset_card();
    bool is_ready(){
        return this->ready;
    }
};


#endif /* defined(__NFCSystem2__MifareUltralightScanner__) */
