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


#include "Scheme.hpp"

double scaleDownToReal(ZZ val, long logp) {
	double res = val;
	res /= (1 << logp);
	
	return res;
}

ap_int<MAX_Z_SIZE> scaleUpToZZ(double val, long logp) {
	ap_int<MAX_Z_SIZE> res;
	
	ap_fixed<256,128,AP_RND> valScaled = val;

	valScaled <<= logp;

	res = valScaled;
	return res;
}

void fftSpecial(std::complex<double>* vals, const long size, Context context) {

	std::complex<double> tmp;
	long i,j;

	for (i = 1, j = 0; i < size; ++i) {
		long bit = size >> 1;
		for (; j >= bit; bit>>=1) {
			j -= bit;
		}

		j += bit;
		if(i < j) {
			tmp= vals[i];
			vals[i] = vals[j];
			vals[j] = tmp;
		}
	}

	for (long len = 2; len <= size; len <<= 1) {
		for ( i = 0; i < size; i += len) {
			long lenh = len >> 1;
			long lenq = len << 2;
			for ( j = 0; j < lenh; ++j) {
				long idx = ((context.rotGroup[j] % lenq)) * context.M / lenq;
				std::complex<double> u = vals[i + j];
				std::complex<double> v = vals[i + j + lenh];
				v *= context.ksiPows[idx];
				vals[i + j] = u + v;
				vals[i + j + lenh] = u - v;
			}
		}
	}
}


void fftSpecialInv(std::complex<double>* vals, const long size, long rotGroup[MAX_ROT_GROUP_SIZE], std::complex<double> ksiPows[MAX_KSI_POWS_SIZE], long N) {
	long i,j,len,lenh,lenq,idx,bit;
	long M = N << 1;
	std::complex<double> u;
	std::complex<double> v;
	std::complex<double> tmp;

	
	
fftSpecInvLoop1:
	for (len = size; len >= 1; len >>= 1) {
fftSpecInvLoop2:
		for (i = 0; i < size; i += len) {
			lenh = len >> 1;
			lenq = len << 2;
fftSpecInvLoop3:
			for ( j = 0; j < lenh; ++j) {
				idx = (lenq - (rotGroup[j] % lenq)) * M / lenq;
				u = vals[i + j] + vals[i + j + lenh];
				v = vals[i + j] - vals[i + j + lenh];
				v *= ksiPows[idx];
				vals[i + j] = u;
				vals[i + j + lenh] = v;
			}
		}
	}

	

fftSpecInvLoop4:
	for (i = 1, j = 0; i < size; ++i) {
		bit = size >> 1;
fftSpecInvLoop5:
		for (; j >= bit; bit>>=1) {
			j -= bit;
		}

		j += bit;
		if(i < j) {
			tmp= vals[i];
			vals[i] = vals[j];
			vals[j] = tmp;
		}
	}

fftSpecInvLoop6:
	for ( i = 0; i < size; ++i) {
		vals[i] /= size;
	}
}



void decode(ZZX& mx, long slots, Context context,std::complex<double> res[MAX_SLOTS]) {
	ZZ q = context.qpowvec[LOG_q];
	long gap = context.Nh / slots;
	long i,idx;
	
	ZZ tmp;
	

	for (i = 0, idx = 0; i < slots; ++i, idx += gap) {

		tmp = ((mx.coeff[idx] % q)+q)%q;
		if(NumBits(tmp) == LOG_q) tmp -= q;
		res[i].real(scaleDownToReal(tmp, LOG_P));


		tmp = ((mx.coeff[idx + context.Nh] % q)+q)%q;
		if(NumBits(tmp) == LOG_q) tmp -= q;
		res[i].imag(scaleDownToReal(tmp, LOG_P));

	}

	fftSpecial(res, slots, context);
}

Plaintext encode(std::complex<double>* vals, long slots, long rotGroup[MAX_ROT_GROUP_SIZE], std::complex<double> ksiPows[MAX_KSI_POWS_SIZE], long N) {
    Plaintext p;


	

	std::complex<double> uvals[MAX_SLOTS];

	long i, jdx, idx;
	long Nh = N >> 1;


	for (i = 0; i < slots; ++i) {
		uvals[i] = vals[i];
	}
	

	


	ZZX mx;
	mx.degree=0;

	setDegree(mx,N);

	long gap = Nh / slots;

	fftSpecialInv(uvals, slots,rotGroup,ksiPows,N);




EncodeScaleUpLoop:	
	for (i = 0, jdx = Nh, idx = 0; i < slots; ++i, jdx += gap, idx += gap) {

		mx.coeff[idx] = scaleUpToZZ(uvals[i].real(), LOG_P+LOG_Q);
		mx.coeff[jdx] = scaleUpToZZ(uvals[i].imag(), LOG_P+LOG_Q);
	}
    
	p = initializePlaintext(mx, slots, true);
	return p;
}


Plaintext decryptMsg(SecretKey& secretKey, Ciphertext& cipher,Context context) {
	ZZ q = context.qpowvec[LOG_q];

	Plaintext p;

	ZZX mx;
	mx.degree=0;

	rMult(mx, cipher.ax, secretKey.sx, q, context.N);


	rAddAndEqual(mx, cipher.bx, q, context.N);

	p = initializePlaintext(mx, cipher.slots, cipher.isComplex);
	return p;
}

void decrypt(SecretKey& secretKey, Ciphertext& cipher, Context context, std::complex<double> res[MAX_SLOTS]) {
	Plaintext msg = decryptMsg(secretKey, cipher,context);

	


	decode(msg.mx, msg.slots, context, res);
}

Ciphertext encrypt(std::complex<double>* vals, long slots, ZZX Enc_ax, ZZX Enc_bx, Context context,long rnd[RND_SIZE]) {

	Plaintext msg = encode(vals, slots, context.rotGroup, context.ksiPows, context.N);

	

	ZZX ax, bx, vx, ex;
	ax.degree=0,
	bx.degree=0;
	vx.degree=0;
	ex.degree=0;
	ZZ qQ = context.qpowvec[LOG_q + LOG_Q];
	Ciphertext c;

	sampleZO(vx, context.N, rnd);

	rMult(ax, vx, Enc_ax, qQ, context.N);

	sampleGauss(ex, context.N, context.sigma, rnd);
	rAddAndEqual(ax, ex, qQ, context.N);

	ex.degree=0;
	
	

	rMult(bx, vx, Enc_bx, qQ, context.N);


	
	sampleGauss(ex, context.N, context.sigma, rnd);

	rAddAndEqual(bx, ex, qQ, context.N);


	rAddAndEqual(bx, msg.mx, qQ, context.N);

	rRightShiftAndEqual(ax, LOG_Q, context.N);
	rRightShiftAndEqual(bx, LOG_Q, context.N);

	c = initializeCiphertext(ax, bx, msg.slots, msg.isComplex);

	return c;
}


