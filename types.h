#pragma once

#include <bitset>
#include <iostream>

using Addr    = uint16_t; // Addresses are 12 bits
using Byte    = uint8_t;
using Nibble  = uint8_t;
using Crumb   = uint8_t;  // old-fashioned term for a 2-bit value
using Bit     = uint8_t;

static Byte nibbles_to_byte(Nibble n1, Nibble n2)
{
    return ((n1 << 4) | (n2 & 0b00001111));
}

static Nibble get_low_nibble (Byte b)
{
    return (b & 0b00001111);
}

static Nibble get_high_nibble(Addr a)
{
    return static_cast<Nibble>( (a & 0x0F00) >> 8 );
}

static Byte get_low_byte(Addr a)
{
    return static_cast<Byte>(a);
}

static char byte_to_char(Byte b)
{
    return static_cast<char>(b);
}

// T should be one of the type aliases defined above
template <typename T>
static bool is_even(T val) {return (val % 2 == 0);}
template <typename T>
static bool is_odd(T val)  {return (val % 2 == 1);}

template <typename T>
static Bit get_bit(T val, unsigned int bit_num)
{
    if (is_even(val >> bit_num)) return 0;
    return 1;
}

template <typename T>
static T set_bit(T val, unsigned int bit_num, Bit bit_val)
{
    unsigned int mask = 1;
    mask <<= bit_num;

    if (bit_val) return (val | mask);
    else return (val & (~mask));
}

template<unsigned int NUM_BITS, typename T>
static void print_bits(T val)
{
    std::cout << std::bitset<NUM_BITS>(val) << std::endl;
}

