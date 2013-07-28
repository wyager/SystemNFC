//
//  HexConversion.h
//  NFCReader
//
//  Created by William Yager on 7/25/13.
//  Copyright (c) 2013 William Yager. All rights reserved.
//

#ifndef NFCReader_HexConversion_h
#define NFCReader_HexConversion_h

#include <vector>
#include <sstream>

//Converts binary data to hex string
static std::string binary_to_hex(std::vector<uint8_t> source)
{
    static char syms[] = "0123456789ABCDEF";
    std::stringstream ss;
    for (std::vector<unsigned char>::const_iterator it = source.begin(); it != source.end(); it++)
        ss << syms[((*it >> 4) & 0xf)] << syms[*it & 0xf];
    return ss.str();
}

static std::string binary_to_hex(uint8_t*src, int len)
{
    std::vector<uint8_t> source(src, src+len);
    static char syms[] = "0123456789ABCDEF";
    std::stringstream ss;
    for (std::vector<unsigned char>::const_iterator it = source.begin(); it != source.end(); it++)
        ss << syms[((*it >> 4) & 0xf)] << syms[*it & 0xf];
    return ss.str();
}



#endif
