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


#ifndef UTILS_HPP
#define UTILS_HPP

#include "ZStructures.hpp"
#include "hls_math.h"
#include "hls_stream.h"

long NumBits(ap_int<MAX_Z_SIZE> a);

void sampleZO(hls::stream<ap_int<MAX_Z_SIZE>> out[2], const long size, hls::stream<long> &rnd1, hls::stream<long> &rnd2); 
void sampleZO(ZZX& res, const long size, long rnd[RND_SIZE]);
void sampleGauss(hls::stream<ap_int<MAX_Z_SIZE>>& res, const long size, const double stdev, hls::stream<long> &rnd1, hls::stream<long> &rnd2);
void sampleGauss(ZZX& res, const long size, const double stdev, long rnd[RND_SIZE]);


void sampleHWT(ZZX& res, const long size,const long h,long rnd[RND_SIZE]);

void sampleUniform2(ZZX& res, const long size, const long bits,long rnd[RND_SIZE]);



void rLeftShiftAndEqual(ZZX& p, const long bits, ZZ& mod, const long degree);
void rRightShiftAndEqual(ZZX& p, const long bits, const long degree);

void rRightShiftStream(hls::stream<ap_int<MAX_Z_SIZE>>& res,hls::stream<ap_int<MAX_Z_SIZE>>& p1, const long bits, const long degree);
void rAddStream(hls::stream<ap_int<MAX_Z_SIZE>>& res,hls::stream<ap_int<MAX_Z_SIZE>>& p1, hls::stream<ap_int<MAX_Z_SIZE>>& p2, ZZ& mod, const long degree);

void rAddAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree);

void rMult(ZZX& res, ZZX& p1, ZZX& p2, ZZ& mod, const long degree);
void rMult(hls::stream<ap_int<MAX_Z_SIZE>>& res_stream,hls::stream<long>& res_degree_stream, hls::stream<ap_int<MAX_Z_SIZE>>& p1, hls::stream<ap_int<MAX_Z_SIZE>>& p2_stream,hls::stream<long>& p2_degree_stream, ZZ& mod, const long degree);
void rMult(hls::stream<ap_int<MAX_Z_SIZE>>& res_stream, hls::stream<ap_int<MAX_Z_SIZE>>& p1, hls::stream<ap_int<MAX_Z_SIZE>>& p2_stream,hls::stream<long>& p2_degree_stream, ZZ& mod, const long degree);
void rSub(ZZX& res, ZZX& p1, ZZX& p2, ZZ& mod, const long degree);
void rSubAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree);

void rMultAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree);
#endif // UTILS_HPP
