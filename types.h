#pragma once

#include <bitset>
#include <iostream>

using Addr   = uint16_t; // Addresses are 12 bits
using Byte   = uint8_t;
using Nibble = uint8_t;
using Crumb  = uint8_t;  // old-fashioned term for a 2-bit value
using Bit    = uint8_t;

// Forward declarations
static Nibble get_low_nibble(Byte b);

// Functions to mimic the 4004's operations on nibble values
// ALL operations on nibbles should go through these functions.

// The asserts check that nibbles and bits are in proper range (0-15) and (0-1).
// Otherwise, the value was probably changed outside these functions.

// What do computer scientists feed their dogs? Nibbles and bits.
static Nibble ADD(Nibble n1, Nibble n2, Bit carry, Bit &carry_out)
{
    assert((n1 < 16) && (n2 < 16) && (carry < 2) && (carry_out < 2));

    Nibble rval = n1 + n2 + carry;

    if (rval > 15) carry_out = 1;
    else carry_out = 0;

    return get_low_nibble(rval);
}

static Nibble COMPLEMENT(Nibble n)
{
    assert(n < 16);
    return n ^ 0b00001111;
}

static Nibble SUB(Nibble n1, Nibble n2, Bit carry, Bit &borrow)
{
    assert((n1 < 16) && (n2 < 16) && (carry < 2) && (borrow < 2));

    Nibble rval = n1 + COMPLEMENT(n2);
    if (carry == 0) rval++;

    if (rval > 15) borrow = 0;
    else borrow = 1;

    return get_low_nibble(rval);
}

static Nibble RAL(Nibble n, Bit carry, Bit &carry_out)
{
    assert((n < 16) && (carry < 2) && (carry_out < 2));

    carry_out = (n & 0b00001000) >> 3;

    Nibble rval = n << 1;
    if (carry == 1) rval++;

    return get_low_nibble(rval);
}

static Nibble RAR(Nibble n, Bit carry, Bit &carry_out)
{
    assert((n < 16) && (carry < 2) && (carry_out < 2));

    carry_out = (n & 0b00000001);

    Nibble rval = n >> 1;
    if (carry == 1) rval += 8;

    return get_low_nibble(rval);
}

static Nibble INC(Nibble n)
{
    assert(n < 16);

    return get_low_nibble(n+1);
}

static Nibble DEC(Nibble n)
{
    assert(n < 16);

    return (n == 0) ? 15 : (n-1);
}

static Byte nibbles_to_byte(Nibble n1, Nibble n2)
{
    return ((n1 << 4) | (n2 & 0b00001111));
}

static Addr bytes_to_addr(Byte b1, Byte b2)
{
    Addr a1 = static_cast<Addr>(b1 & 0x0F) << 8;
    Addr a2 = static_cast<Addr>(b2);
    return a1 | a2;
}

static Nibble get_low_nibble(Byte b)
{
    return (b & 0b00001111);
}

static Nibble get_high_nibble_from_byte(Byte b)
{
    return (b & 0b11110000) >> 4;
}

static Nibble get_high_nibble_from_addr(Addr a)
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

// Convert Nibble's numeric value to its character (not just a cast).
// So return a character [0-9] or [a-f].
static char nibble_numeric_value_to_char(Nibble n)
{
    // A Nibble is just a Byte where half the bits should always be zero.
    n &= 0x00FF;
    if (n < 10) n += 48;
    else n += 87;

    return byte_to_char(n);
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

