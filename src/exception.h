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
 * simple exception handling
 */

#ifndef __exception_h__
#define __exception_h__

#include <string.h>

class basic_exception {
public:
    basic_exception(const char* msg)
    {
        if (msg) {
            strcpy_s(m_msg, msg);
        }
    }

    virtual const char* what() const { return m_msg; }

private:
    char m_msg[256];
};

class out_of_memory : public basic_exception {
public:
    out_of_memory()
        : basic_exception("out of memory")
    {
    }
};

#endif // __exception_h__.c_exception("out of memory") {}
