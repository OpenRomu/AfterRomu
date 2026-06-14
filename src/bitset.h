/*
 * HL rendering engine
 * Copyright (c) 2000,2001 Bart Sekura
 *
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and
 * this permission notice appear in all copies of the software,
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * simple bitset
 */

#ifndef __bitset_h__
#define __bitset_h__

//////////////////////////////////////////////////////////////
// fast zero memory assuming dwords
inline static void zero_dwords(void *dest, int count)
{
    memset(dest, 0, (size_t)count * 4);
}

///////////////////////////////////////////////////////////////
class bitset
{
  public:
    bitset(int _size)
    {
        size = _size / 32 + 1;
        bits = new unsigned long[size];
        clear_all();
    }
    ~bitset()
    {
        if (bits)
            delete bits, bits = 0;
    }

    inline void set(int i)
    {
        bits[i >> 5] |= (1 << (i & 31));
    }
    inline void clear(int i)
    {
        bits[i >> 5] &= ~(1 << (i & 31));
    }

    inline int test(int i)
    {
        return bits[i >> 5] & (1 << (i & 31));
    }

    inline void clear_all()
    {
        zero_dwords(bits, size);
    }

  private:
    unsigned long *bits;
    int size;
};

#endif // __bitset_h__
