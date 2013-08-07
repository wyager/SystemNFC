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
#include "MifareUltralightScanner.h"
#include "MifareUltralightCard.h"
#include "HexConversion.h"

//Pre-declaring functions
//They are defined farther down
int write_mfultralight_data();
int read_mfultralight_data();
int write_mfc_sector0();
int read_mfc_sector0();

int main(int argc, const char * argv[])
{
    std::cerr << "What do you want to do?\n";
    std::cerr << "1: Read Mifare Ultralight (NFC ring)\n";
    std::cerr << "2: Write Mifare Ultralight (NFC ring)\n";
    std::cerr << "3: Read Mifare Classic\n";
    std::cerr << "4: Write Mifare Classic\n";
    std::cerr << "Anything else: exit\n";
    std::cerr << ">";
    std::string user_input;
    std::cin >> user_input;
    if(user_input.size() != 1){ //Not a valid response
        std::cerr << "Exiting\n";
        return 0;
    }
    //The following are a number of simple test functions to show
    //how to easily do simple tasks with this library.
    //None of these functions should do anything dangerous. I tried to make
    //them not write to any sensitive parts of the NFC device.
    switch (user_input[0]) {
        case '1':
            read_mfultralight_data(); //Read the first 64 bytes from a Mifare Ultralight (or NFC ring)
            break;
        case '2':
            write_mfultralight_data(); //Write 4 bytes (starting at byte 40) to a Mifare Ultralight (or NFC ring)
            break;
        case '3':
            read_mfc_sector0();//Read first 64 bytes from a Mifare Classic card
            break;
        case '4':
            write_mfc_sector0();//Write first 64 bytes (skipping over the first and last 16 bytes) to a Mifare Classic cards
            break;
        default:
            std::cerr << "Exiting\n";
            break;
    }
    return 0;
}

/////////////////////////EXAMPLES////////////////////////////////


//////////////////WRITING Mifare Ultralight//////////////////////
/////////////////McClear NFC ring compatible/////////////////////
//Writes the tenth page
//A page is 4 bytes
int write_mfultralight_data(){
    MifareUltralightScanner scanner;
    std::cerr << "Scanning...\n";
    MifareUltralightCard card = scanner.scan_forever();
    std::cerr << "Got card!\n";
    if(!card.is_ready()){
        std::cerr << "Error scanning for card.\n";
        return 1;
    }
    std::cerr << "Got card!\n";
    std::cerr << "Card info:\n" << card.basic_card_info() << "\n";
    
    std::vector<uint8_t> data_to_write = {0xDE,0xAD,0xBE,0xEF};
    if(!card.write_page(10, data_to_write)){
        std::cerr << "Error. Could not write to card.\n";
        return 1;
    }
    return 0;
}

//////////////////READING Mifare Ultralight//////////////////////
/////////////////McClear NFC ring compatible/////////////////////
//Reads the first block
//A block is 4 pages
//A page is 4 bytes
int read_mfultralight_data(){
    MifareUltralightScanner scanner;
    std::cerr << "Scanning...\n";
    MifareUltralightCard card = scanner.scan_forever();
    if(!card.is_ready()){
        std::cerr << "Error scanning for card.\n";
        return 1;
    }
    std::cerr << "Got card!\n";
    std::cerr << "Basic card info:\n" << card.basic_card_info() << "\n";
    std::cerr << "Reading 64 bytes, one block (16 bytes) at a time:\n";
    for (int i=0; i<4; i++) {
        std::vector<uint8_t> block = card.read_block(i);
        if (block.size() != 16) {
            std::cerr << "Error reading block " << i << "\n";
            return 1;
        }
        std::cerr << binary_to_hex(block) << "\n";
    }
    std::cerr << "Reading the same data 1 page (4 bytes) at a time.\n4 times as slow as block-by-block.\n";
    for(int i=0; i<16; i++){
        std::vector<uint8_t> page_data = card.read_page(i);
        if(page_data.size()!=4){
            std::cerr << "Error reading page " << i << "\n";
            return 1;
        }
        std::cerr << binary_to_hex(page_data) << "\n";
    }
    return 0;
}
/////////////////////////////////////////////////////////////////

//////////////////READING Mifare Classic//////////////////////
//Reads the first sector
//A sector is 4 blocks
//A block is 16 bytes
int read_mfc_sector0(){
    MifareClassicScanner scanner;
    std::cerr << "Scanning...\n";
    MifareClassicCard card = scanner.scan_forever();
    if(!card.is_ready()){
        std::cerr << "Error scanning for card.\n";
        return 1;
    }
    std::cerr << "Got card!\n";
    std::cout << "Got card:\n" <<  card.basic_card_info() << "\n";
    std::cerr << "Scanning sector 0.\n";
    std::vector<uint8_t> sector0 = card.read_sector(0);
    if (sector0.size() != 4*16) {
        std::cerr << "Error reading sector 0.\n";
        return 1;
    }
    std::cerr << binary_to_hex(sector0) << "\n";
    return 0;//Success
}
/////////////////////////////////////////////////////////////////


//////////////////WRITING Mifare Classic//////////////////////
//Writes the first sector
//A sector is 4 blocks
//A block is 16 bytes
//This function will NOT write to the fourth block of a sector
//  because that can lock the data on the sector. If you want
//  to write there, please modify the write_sector function.
//This function will NOT write to the first block of sector 0
//  because that is where manufacturer data is stored. If you
//  have a card that allows you to write there, you can modify
//  the code to support writing to that area.
int write_mfc_sector0(){
    //The last uint8_t holds the terminating zero. We are only going to send 64 bytes
    uint8_t buf[65] = "blahblahblahblahThis is what does get written :)blahblahblahblah";
    std::vector<uint8_t> card_data(buf, buf+64);

    MifareClassicScanner scanner;
    std::cerr << "Scanning...";
    MifareClassicCard card = scanner.scan_forever();
    if(!card.is_ready()){
        std::cerr << "Error scanning for card.\n";
        return 1;
    }
    std::cerr << "Got card!\n";
    if(!card.write_sector(0, card_data)){
        std::cerr << "Coult not write to sector 0.\n";
        return 1;
    }
    return 0;
}
/////////////////////////////////////////////////////////////////

