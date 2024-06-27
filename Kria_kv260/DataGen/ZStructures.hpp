// MIT License

// Copyright (c) 2024 Valentino Guerrini

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef ZSTRUCTURES_HPP
#define ZSTRUCTURES_HPP

#include "Parameters.hpp"
#include "ap_int.h"

//----------------FUNCTIONS AND STRUCTS ARE USED BOTH BY THE SOGTWARE AND THE HARDWARE IMPLEMENTATION----------------

using ZZ = ap_int<MAX_Z_SIZE>;

struct ZZX {
    ap_int<MAX_Z_SIZE> coeff[MAX_ZZX_DEGREE];
    long degree;
};

ZZ power2_ZZ(long n);

ZZX normalize(const ZZX& a);

void setDegree(ZZX& x, long d);

void LeftShift(ap_int<MAX_Z_SIZE>& x, const ap_int<MAX_Z_SIZE>& a, long k);

void RightShift(ap_int<MAX_Z_SIZE>& x, const ap_int<MAX_Z_SIZE>& a, long k);

void SubMod(ZZ& x, const ZZ& a, const ZZ& b, const ZZ& n);

void AddMod(ap_int<MAX_Z_SIZE>& x, const ap_int<MAX_Z_SIZE>& a, const ap_int<MAX_Z_SIZE>& b, const ap_int<MAX_Z_SIZE>& n);

void mul(ZZX& c, const ZZX& a, const ZZX& b);

void rem(ZZ& r, const ZZ& a, const ZZ& b);

void rAdd(ZZX& res, ZZX& p1, ZZX& p2, ZZ& mod, const long degree);
#endif // ZSTRUCTURES_HPP
