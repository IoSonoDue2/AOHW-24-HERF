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


#include "Structures.hpp"


//---------------------------------- Context ----------------------------------
Context initializeContext() {

    Context context;
    long i;
    long fivePows = 1;
    double angle;

 
    context.sigma = SIGMA;
    context.h = H;

    context.N = 1 << LOG_N;
    context.Nh = context.N >> 1;
    context.logNh = LOG_N - 1;
    context.M = context.N << 1;

    context.logQQ = LOG_Q << 1;
    context.Q = power2_ZZ(LOG_Q);
    context.QQ = (power2_ZZ(context.logQQ));


    
InitializeRotLoop:
	for (i = 0; i < context.Nh; ++i) {

		context.rotGroup[i] = fivePows;
		fivePows *= 5;
		fivePows %= context.M;
	}

InitializeKsiLoop:
    for (i = 0; i < context.M; ++i) {

		angle = 2.0 * M_PI * i / context.M;
		context.ksiPows[i].real(cos(angle));
		context.ksiPows[i].imag(sin(angle));
	}


    context.ksiPows[context.M] = context.ksiPows[0];

    context.qpowvec[0] = 1;

 InitializeQpowLoop:
    for (i = 1; i < context.logQQ + 1; ++i) {
		context.qpowvec[i] = context.qpowvec[i - 1] << 1;
	}
    

    return context;
}

//---------------------------------- SecretKey ----------------------------------

SecretKey initializeSecretKey(long rnd[RND_SIZE]) {
    SecretKey secretKey;

    long N = 1 << LOG_N;
    sampleHWT(secretKey.sx, N, 64, rnd);

    return secretKey;
}


//---------------------------------- Scheme ----------------------------------
Scheme initializeScheme(Context context, SecretKey secretKey,long rnd[RND_SIZE]) {
    Scheme scheme;

    scheme.context = context;


    addEncKey(secretKey, scheme,rnd);


	addMultKey(secretKey, scheme,rnd);


    return scheme;

   
}

void addEncKey(SecretKey& secretKey, Scheme& scheme,long rnd[RND_SIZE]) {
	ZZX ex, ax, bx;
    ex.degree = 0;
    ax.degree = 0;
    bx.degree = 0;

    Context context = scheme.context;



    sampleUniform2(ax, context.N, context.logQQ, rnd);

    sampleGauss(ex, context.N, context.sigma, rnd);

    rMult(bx, secretKey.sx, ax, context.QQ, context.N);


    rSub(bx, ex, bx, context.QQ, context.N);

    scheme.EncKey.ax = ax;
    scheme.EncKey.bx = bx;

}

void addMultKey(SecretKey& secretKey, Scheme& scheme,long rnd[RND_SIZE]) {
	ZZX ex, ax, bx, sxsx;
    ex.degree = 0;
    ax.degree = 0;
    bx.degree = 0;
    sxsx.degree = 0;

    Context context = scheme.context;

    rMult(sxsx, secretKey.sx, secretKey.sx, context.Q, context.N);
	rLeftShiftAndEqual(sxsx, LOG_Q, context.QQ, context.N);
	sampleUniform2(ax, context.N, context.logQQ, rnd);
	sampleGauss(ex, context.N, context.sigma, rnd);
	rAddAndEqual(ex, sxsx, context.QQ, context.N);
	rMult(bx, secretKey.sx, ax, context.QQ, context.N);

	rSub(bx, ex, bx, context.QQ, context.N);

    scheme.MulKey.ax = ax;
    scheme.MulKey.bx = bx;

}

//----------------Plaintext ----------------

Plaintext initializePlaintext(ZZX mx, long slots, bool isComplex){

    Plaintext p;
    p.mx = mx;

    p.slots = slots;
    p.isComplex = isComplex;

    return p;
}



//----------------Ciphertext ----------------

Ciphertext initializeCiphertext(ZZX ax, ZZX bx, long slots, bool isComplex){

    Ciphertext c;
    c.ax = ax;
    c.bx = bx;

    c.slots = slots;
    c.isComplex = isComplex;

    return c;
}
