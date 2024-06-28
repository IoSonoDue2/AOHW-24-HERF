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


#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <complex>
#include "Parameters.hpp"
#include "ZStructures.hpp"
#include "Utils.hpp"

// Struct & functions for context of the scheme
struct Context {


	double sigma; ///< standard deviation for Gaussian distribution
	long h; ///< parameter for HWT distribution

	long N; ///< N is a power-of-two that corresponds to the ring Z[X]/(X^N + 1)
	long Nh; ///< Nh = N/2
	long logNh; ///< logNh = logN - 1
	long M; ///< M = 2N
	long logQQ; ///< log of PQ

	ZZ Q; ///< Q corresponds to the highest modulus
	ZZ QQ; ///< PQ = Q * Q

	long rotGroup[MAX_ROT_GROUP_SIZE]; ///< precomputed rotation group indexes
	std::complex<double> ksiPows[MAX_KSI_POWS_SIZE]; ///< precomputed ksi powers
	ZZ qpowvec[MAX_QPOW_VEC_SIZE]; ///< precomputed powers of 2

};

Context initializeContext();

// Struct for secret key

struct SecretKey {
    ZZX sx; ///< secret key
};
SecretKey initializeSecretKey(long rnd[RND_SIZE]);

// Struct for key

struct Key {
    ZZX ax;
    ZZX bx;
};


// Struct for scheme 

struct Scheme {
    Context context; ///< context of the scheme

    Key EncKey; ///< encryption key
    Key MulKey ; ///< multiplication key
    Key ConjKey; ///< conjugation key




};
Scheme initializeScheme(Context context, SecretKey secretKey,long rnd[RND_SIZE]);
void addEncKey(SecretKey& secretKey, Scheme& scheme,long rnd[RND_SIZE]);
void addMultKey(SecretKey& secretKey, Scheme& scheme,long rnd[RND_SIZE]);



struct Plaintext {
	
	ZZX mx; ///< message mod X^N + 1


	long slots; ///< number of slots in message

	bool isComplex; ///< option of Message with single real slot
};

Plaintext initializePlaintext(ZZX mx, long slots, bool isComplex);

struct Ciphertext{

	ZZX ax; ///< Ciphertext is an RLWE instance (ax, bx = mx + ex - ax * sx) in ring Z_q[X] / (X^N + 1);
	ZZX bx; ///< Ciphertext is an RLWE instance (ax, bx = mx + ex - ax * sx) in ring Z_q[X] / (X^N + 1);


	long slots; ///< number of slots in Ciphertext

	bool isComplex; ///< option of Ciphertext with single real slot


};

Ciphertext initializeCiphertext(ZZX ax, ZZX bx, long slots, bool isComplex);

#endif // STRUCTURES_HPP
