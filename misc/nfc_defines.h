//
//  nfc_defines.h
//  SystemNFCdevel
//
//  Created by William Yager on 8/6/13.
//  Copyright (c) 2013 William Yager. All rights reserved.
//

#ifndef SystemNFCdevel_nfc_defines_h
#define SystemNFCdevel_nfc_defines_h

struct bits_container {
    std::vector<uint8_t> data;
    int num_bits = 0;
};
const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
};


#endif
