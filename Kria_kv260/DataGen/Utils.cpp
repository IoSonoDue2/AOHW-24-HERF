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

#include "Utils.hpp"





long NumBits(ap_int<MAX_Z_SIZE> a){
	long i = 0;

	
	while(a != 0){
		a = a >> 1;
		i++;
	}
	return i;

}

void sampleZO(ZZX& res, const long size, long rnd[RND_SIZE]) {

	setDegree(res,size);
	
	long idx = 0;
	long j=0;

	ap_int<64> bitrnd = rnd[0];

	long tmp;
Samplez0Loop:	
	for (idx = 0; idx <= size; idx++) {
		tmp	= rnd[idx%RND_SIZE] % size;
		if(res.coeff[tmp] ==0){
			
			res.coeff[tmp] = (bitrnd[idx%64]==0) ? 1 : -1;
			j=(j+1)%RND_SIZE;
			bitrnd = rnd[j];
		}

	}


	

}


void sampleHWT(ZZX& res, const long size,const long h,long rnd[RND_SIZE]) {

	setDegree(res,size);
	
	long idx = 0;
	long j=0;

	ap_int<64> bitrnd = rnd[0];

	long tmp;
	while (h > idx) {
		tmp	= rnd[idx%RND_SIZE] % (size+1);
		if(res.coeff[tmp] ==0){
			
			res.coeff[tmp] = (bitrnd[idx%64]==0) ? 1 : -1;
			j=(j+1)%RND_SIZE;
			bitrnd = rnd[j];
		}
		idx++;


	}

}

void sampleUniform2(ZZX& res, const long size, const long bits, long rnd[RND_SIZE]) {

	setDegree(res,size);
	for (long i = 0; i <= size; i++) {
		res.coeff[i] = rnd[i%RND_SIZE]; 
	}
}

void sampleGauss(ZZX& res, const long size, const double stdev, long rnd[RND_SIZE]) {
	static double const Pi = 4.0 * atan(1.0);
	static long const bignum = 0xffffffff;
	double r1;
	double r2;
	double theta;
	double rr;
	long x1;
	long x2;
	long i;

	res.degree = 0;
	setDegree(res, size);

SampleGaussLoop:
	for (i = 0; i <= size; i+=2) {
		r1 = rnd[i%RND_SIZE]%bignum/((double)bignum + 1);
		r2 = rnd[(i+rnd[3])%RND_SIZE]%bignum/((double)bignum + 1);
		theta = 2 * Pi * r1;
		rr= sqrt(-2.0 * log(r2)) * stdev;

		x1 = (long) floor(rr * cos(theta) + 0.5);
		(ap_int<MAX_Z_SIZE>)res.coeff[i] = x1;
		if(i + 1 <= size) {
			x2 = (long) floor(rr * sin(theta) + 0.5);
			(ap_int<MAX_Z_SIZE>)res.coeff[i + 1] = x2;
		}
	}

}

//-------------------------------------- RINGS --------------------------------------

void rMult(ZZX& res, ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
	
	setDegree(res, degree);
	ZZX pp;
	long i;
	pp.degree = 0;

	mul(pp, p1, p2);
	
rmultLoop:	
	for (i = 0; i <= degree; ++i) {
		#pragma HLS PIPELINE

		rem(pp.coeff[i], pp.coeff[i], mod);

		rem(pp.coeff[i + degree], pp.coeff[i + degree], mod);

		SubMod(res.coeff[i], pp.coeff[i], pp.coeff[i + degree], mod);

	}

}


void rSub(ZZX& res, ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
	
	for (long i = 0; i <= degree; ++i) {
		AddMod(res.coeff[i], p1.coeff[i], -p2.coeff[i], mod);
	}
	setDegree(res, degree);
}

void rLeftShiftAndEqual(ZZX& p, const long bits, ZZ& mod, const long degree) {
	for (long i = 0; i <= degree; ++i) {
		LeftShift(p.coeff[i], p.coeff[i], bits);
		rem(p.coeff[i], p.coeff[i], mod);
	}
}

void rRightShiftAndEqual(ZZX& p, const long bits, const long degree) {
rShiftEqLoop:	
	for (long i = 0; i <= degree; ++i) {
		RightShift(p.coeff[i], p.coeff[i], bits);
	}
}



void rAddAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
rAddEqLoop:	
	for (long i = 0; i <= degree; ++i) {
		#pragma HLS unroll factor=1
		AddMod(p1.coeff[i], p1.coeff[i], p2.coeff[i], mod);
	}
}


void rSubAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
	for (long i = 0; i <= degree; ++i) {
		AddMod(p1.coeff[i], p1.coeff[i], -p2.coeff[i], mod);
	}
}


void rMultAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
	ZZX pp;
	pp.degree=0;
	mul(pp, p1, p2);
	setDegree(pp,2 * degree);

	for (long i = 0; i <= degree; ++i) {
		rem(pp.coeff[i], pp.coeff[i], mod);
		rem(pp.coeff[i + degree], pp.coeff[i + degree], mod);
		SubMod(p1.coeff[i], pp.coeff[i], pp.coeff[i + degree], mod);
	}
}




