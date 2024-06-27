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


#ifndef SCHEME_HPP
#define SCHEME_HPP

#include "ZStructures.hpp"
#include "Parameters.hpp"
#include "Structures.hpp"
#include "Utils.hpp"
#include <complex>

//-------------UTILS FUNCTIONS----------------
double scaleDownToReal(ZZ val, long logp);
ap_int<MAX_Z_SIZE> scaleUpToZZ(double val, long logp);
void fftSpecial(std::complex<double>* vals, const long size, Context context);
void fftSpecialInv(std::complex<double>* vals, const long size, long rotGroup[MAX_ROT_GROUP_SIZE], std::complex<double> ksiPows[MAX_KSI_POWS_SIZE], long N);

//-------------SW CKKS SCHEME FUNCTIONS----------------
void decode(ZZX& mx, long slots, Context context,std::complex<double> res[MAX_SLOTS]);
Plaintext decryptMsg(SecretKey& secretKey, Ciphertext& cipher,Context context);
void decrypt(SecretKey& secretKey, Ciphertext& cipher, Context context, std::complex<double> res[MAX_SLOTS]);

Plaintext encode(std::complex<double>* vals, long slots, long rotGroup[MAX_ROT_GROUP_SIZE], std::complex<double> ksiPows[MAX_KSI_POWS_SIZE], long N);
Ciphertext encrypt(std::complex<double>* vals, long slots, ZZX Enc_ax, ZZX Enc_bx, Context context,long rnd[RND_SIZE]);




#endif // SCHEME_HPP
