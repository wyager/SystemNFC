//
//  main.cpp
//  NFCSystem2
//
//  Created by Will Yager on 7/26/13.
//  Copyright (c) 2013 Will Yager. All rights reserved.
//

#include <iostream>
#include <string>
#include "MifareClassicScanner.h"
#include "MifareClassicCard.h"
#include "HexConversion.h"

int main(int argc, const char * argv[])
{
    std::cerr << "Enter data to write.\n";
    uint8_t buf[64] = {0};
    std::cin.read((char*)&buf[16], 32);//Copy to middle 32 bits of buffer
    std::vector<uint8_t> card_data(buf, buf+64);
    
    MifareClassicScanner scanner;
    std::cerr << "Scanning...";
    MifareClassicCard card = scanner.scan_forever();
    
    std::cerr << "Got card!\n";
    if(!card.write_sector(0, card_data)){
        std::cerr << "Coult not write to sector 0.\n";
        return 1;
    }
    
    return 0;
    
    
    
//    MifareClassicScanner scanner;
//    MifareClassicCard card = scanner.scan_forever();
//    std::cerr << "Got card!\n";
//    if(!card.is_ready()){
//        std::cerr << "Error scanning for card.\n";
//        return 1;
//    }
//    //std::cout << card.basic_card_info() << "\n";
//    std::cerr << "Scanning sector 0.\n";
//    std::vector<uint8_t> sector0 = card.read_sector(0);
//    if (sector0.size() != 4*16) {
//        std::cerr << "Error reading sector 0.\n";
//        return 1;
//    }
//    std::cerr << binary_to_hex(sector0) << "\n";

    return 0;
}

