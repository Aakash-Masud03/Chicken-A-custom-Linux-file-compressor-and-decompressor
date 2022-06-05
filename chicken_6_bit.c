////////////////////////////////////////////////////////////////////////////////
// COMP1521 21T3 --- Assignment 2: `chicken', a simple file archiver          //
// <https://www.cse.unsw.edu.au/~cs1521/21T3/assignments/ass2/index.html>     //
//                                                                            //
// 2021-11-04   v1.0    Team COMP1521 <cs1521 at cse.unsw.edu.au>             //
//                                                                            //
///////////////////// YOU DO NOT NEED TO MODIFY THIS FILE. /////////////////////

////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
// THIS FILE IS NOT SUBMITTED AND WILL BE PROVIDED AS IS DURING TESTING AND MARKING //
////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////


// This file provides functions for egglet 6-bit format for subset 3


#include "chicken.h"
#include <stdint.h>

// lookup_table_to_6_bit[six_bit_value]
// contains the 8 bit value (byte) corresponding to six_bit_value

static const uint8_t lookup_table_to_6_bit[256] = {
    [0x00] = 0x00,
    [0x09] = 0x01,
    [0x0a] = 0x02,
    [0x0d] = 0x03,
    [0x20] = 0x04,
    [0x21] = 0x05,
    [0x22] = 0x06,
    [0x23] = 0x07,
    [0x24] = 0x08,
    [0x25] = 0x09,
    [0x26] = 0x0a,
    [0x27] = 0x0b,
    [0x28] = 0x0c,
    [0x29] = 0x0d,
    [0x2a] = 0x0e,
    [0x2b] = 0x0f,
    [0x2c] = 0x10,
    [0x2d] = 0x11,
    [0x2e] = 0x12,
    [0x2f] = 0x13,
    [0x30] = 0x14,
    [0x31] = 0x15,
    [0x32] = 0x16,
    [0x33] = 0x17,
    [0x34] = 0x18,
    [0x35] = 0x19,
    [0x36] = 0x1a,
    [0x37] = 0x1b,
    [0x38] = 0x1c,
    [0x39] = 0x1d,
    [0x3a] = 0x1e,
    [0x3b] = 0x1f,
    [0x3c] = 0x20,
    [0x3d] = 0x21,
    [0x3e] = 0x22,
    [0x3f] = 0x23,
    [0x40] = 0x24,
    [0x60] = 0x25,
    [0x61] = 0x26,
    [0x62] = 0x27,
    [0x63] = 0x28,
    [0x64] = 0x29,
    [0x65] = 0x2a,
    [0x66] = 0x2b,
    [0x67] = 0x2c,
    [0x68] = 0x2d,
    [0x69] = 0x2e,
    [0x6a] = 0x2f,
    [0x6b] = 0x30,
    [0x6c] = 0x31,
    [0x6d] = 0x32,
    [0x6e] = 0x33,
    [0x6f] = 0x34,
    [0x70] = 0x35,
    [0x71] = 0x36,
    [0x72] = 0x37,
    [0x73] = 0x38,
    [0x74] = 0x39,
    [0x75] = 0x3a,
    [0x76] = 0x3b,
    [0x77] = 0x3c,
    [0x78] = 0x3d,
    [0x79] = 0x3e,
    [0x7a] = 0x3f,
};

// lookup_table_from_6_bit[six_bit_value]
// contains the 8 bit value (byte) corresponding to six_bit_value

static const uint8_t lookup_table_from_6_bit[64] = {
    [0x00] = 0x00,
    [0x01] = 0x09,
    [0x02] = 0x0a,
    [0x03] = 0x0d,
    [0x04] = 0x20,
    [0x05] = 0x21,
    [0x06] = 0x22,
    [0x07] = 0x23,
    [0x08] = 0x24,
    [0x09] = 0x25,
    [0x0a] = 0x26,
    [0x0b] = 0x27,
    [0x0c] = 0x28,
    [0x0d] = 0x29,
    [0x0e] = 0x2a,
    [0x0f] = 0x2b,
    [0x10] = 0x2c,
    [0x11] = 0x2d,
    [0x12] = 0x2e,
    [0x13] = 0x2f,
    [0x14] = 0x30,
    [0x15] = 0x31,
    [0x16] = 0x32,
    [0x17] = 0x33,
    [0x18] = 0x34,
    [0x19] = 0x35,
    [0x1a] = 0x36,
    [0x1b] = 0x37,
    [0x1c] = 0x38,
    [0x1d] = 0x39,
    [0x1e] = 0x3a,
    [0x1f] = 0x3b,
    [0x20] = 0x3c,
    [0x21] = 0x3d,
    [0x22] = 0x3e,
    [0x23] = 0x3f,
    [0x24] = 0x40,
    [0x25] = 0x60,
    [0x26] = 0x61,
    [0x27] = 0x62,
    [0x28] = 0x63,
    [0x29] = 0x64,
    [0x2a] = 0x65,
    [0x2b] = 0x66,
    [0x2c] = 0x67,
    [0x2d] = 0x68,
    [0x2e] = 0x69,
    [0x2f] = 0x6a,
    [0x30] = 0x6b,
    [0x31] = 0x6c,
    [0x32] = 0x6d,
    [0x33] = 0x6e,
    [0x34] = 0x6f,
    [0x35] = 0x70,
    [0x36] = 0x71,
    [0x37] = 0x72,
    [0x38] = 0x73,
    [0x39] = 0x74,
    [0x3a] = 0x75,
    [0x3b] = 0x76,
    [0x3c] = 0x77,
    [0x3d] = 0x78,
    [0x3e] = 0x79,
    [0x3f] = 0x7a,
};


// lookup_table_valid_6_bit[byte]
// contains 1 iff byte can be converted to egglet 6 bit format
// it contains 0 otherwise.

static const uint8_t lookup_table_valid_6_bit[256] = {
    [0x00] = 1,
    [0x09] = 1,
    [0x0a] = 1,
    [0x0d] = 1,
    [0x20] = 1,
    [0x21] = 1,
    [0x22] = 1,
    [0x23] = 1,
    [0x24] = 1,
    [0x25] = 1,
    [0x26] = 1,
    [0x27] = 1,
    [0x28] = 1,
    [0x29] = 1,
    [0x2a] = 1,
    [0x2b] = 1,
    [0x2c] = 1,
    [0x2d] = 1,
    [0x2e] = 1,
    [0x2f] = 1,
    [0x30] = 1,
    [0x31] = 1,
    [0x32] = 1,
    [0x33] = 1,
    [0x34] = 1,
    [0x35] = 1,
    [0x36] = 1,
    [0x37] = 1,
    [0x38] = 1,
    [0x39] = 1,
    [0x3a] = 1,
    [0x3b] = 1,
    [0x3c] = 1,
    [0x3d] = 1,
    [0x3e] = 1,
    [0x3f] = 1,
    [0x40] = 1,
    [0x60] = 1,
    [0x61] = 1,
    [0x62] = 1,
    [0x63] = 1,
    [0x64] = 1,
    [0x65] = 1,
    [0x66] = 1,
    [0x67] = 1,
    [0x68] = 1,
    [0x69] = 1,
    [0x6a] = 1,
    [0x6b] = 1,
    [0x6c] = 1,
    [0x6d] = 1,
    [0x6e] = 1,
    [0x6f] = 1,
    [0x70] = 1,
    [0x71] = 1,
    [0x72] = 1,
    [0x73] = 1,
    [0x74] = 1,
    [0x75] = 1,
    [0x76] = 1,
    [0x77] = 1,
    [0x78] = 1,
    [0x79] = 1,
    [0x7a] = 1,
};

// DO NOT CHANGE CHANGE THESE FUNCTIONS - use them to convert from 8-bit encoding to 6-bit encoding and vice versa

// given an 8 bit value (byte), return the corresponding value in egglet 6-bit format
// return -1 if it is not possible to translate the 8 bit value to egglet 6-bit format

int egglet_to_6_bit(uint8_t eight_bit_value) {
    return lookup_table_valid_6_bit[eight_bit_value] ?
        lookup_table_to_6_bit[eight_bit_value] : -1;
}


// given a 6-bit egglet format value, return the corresponding 8-bit value (byte)
// return -1 if not given a valid 6-bit value

int egglet_from_6_bit(uint8_t six_bit_value) {
    return (six_bit_value < sizeof lookup_table_from_6_bit / sizeof lookup_table_from_6_bit[0]) ?
        lookup_table_from_6_bit[six_bit_value] : -1;
}

////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
// THIS FILE IS NOT SUBMITTED AND WILL BE PROVIDED AS IS DURING TESTING AND MARKING //
////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
