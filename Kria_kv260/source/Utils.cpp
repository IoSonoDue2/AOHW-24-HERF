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

void sampleZO(hls::stream<ap_int<MAX_Z_SIZE>> out[2], const long size, hls::stream<long> &rnd1, hls::stream<long> &rnd2) {
	
	long idx = 0;
	long res[RND_SIZE+1];
	long tmp;

	for(idx = 0; idx < RND_SIZE; idx++){
		#pragma HLS PIPELINE II=1
		res[idx] = 0;
	}

	ap_int<64> bitrnd = rnd1.read();

	
Samplez0Loop:	
	for (idx = 0; idx <= size; idx++) {
		#pragma HLS PIPELINE II=1
		tmp	= rnd2.read() % size;
		if(res[tmp] ==0){
			
			res[tmp] = (bitrnd[idx%64]==0) ? 1 : -1;
			bitrnd = rnd1.read();
		}

	}

	while(!rnd1.empty()){
		bitrnd=rnd1.read();
	}
	while(!rnd2.empty()){
		bitrnd=rnd2.read();
	}


	for (idx = 0; idx <= size; idx++) {
		#pragma HLS PIPELINE II=1
		out[0].write(res[idx]);
		out[1].write(res[idx]);
	}


	

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

void sampleGauss(hls::stream<ap_int<MAX_Z_SIZE>>& res, const long size, const double stdev, hls::stream<long> &rnd1, hls::stream<long> &rnd2) {
	static double const Pi = 4.0 * atan(1.0);
	static long const bignum = 0xffffffff;
	double r1;
	double r2;
	double theta;
	double rr;
	ap_int<MAX_Z_SIZE> x1;
	ap_int<MAX_Z_SIZE> x2;
	long i;
	long tmp = rnd2.read();

SampleGaussLoop:
	for (i = 0; i <= size; i+=2) {
		r1 = rnd1.read()%bignum/((double)bignum + 1);
		r2 = rnd2.read()%bignum/((double)bignum + 1);
		theta = 2 * Pi * r1;
		rr= sqrt(-2.0 * log(r2)) * stdev;
		(ap_int<MAX_Z_SIZE>)x1 = (ap_int<MAX_Z_SIZE>) floor(rr * cos(theta) + 0.5);
		res.write((ap_int<MAX_Z_SIZE>)x1);
		if(i + 1 <= size) {
			(ap_int<MAX_Z_SIZE>)x2 = (ap_int<MAX_Z_SIZE>) floor(rr * sin(theta) + 0.5);
			res.write((ap_int<MAX_Z_SIZE>)x2);
		}
	}

	while(!rnd1.empty()){
		x1=rnd1.read();
	}
	while(!rnd2.empty()){
		x1=rnd2.read();
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

void rMult(hls::stream<ap_int<MAX_Z_SIZE>>& res_stream, hls::stream<ap_int<MAX_Z_SIZE>>& p1, hls::stream<ap_int<MAX_Z_SIZE>>& p2_stream,hls::stream<long>& p2_degree_stream, ZZ& mod, const long degree) {


	ZZX pp,p2;

	long i, j;
	ap_int<MAX_Z_SIZE> t, accum;

	pp.degree = 0;
	p2.degree = 0;
	

	long db,da;
	long d;

	db = p2_degree_stream.read();


	setDegree(p2,MAX_ZZX_DEGREE-1);

	for(i = 0; i <= db; i++){
		t = p2_stream.read();
		p2.coeff[i] = t;
	}
	

	if((db == 0 && p2.coeff[0] == 0)){
        pp.degree = 0;
        pp.coeff[0] = 0;

    }else{
		da = degree;
		

		d = da + db;


		setDegree(pp, d);

		

    	(ap_int<MAX_Z_SIZE>)t=(ap_int<MAX_Z_SIZE>)0;
		(ap_int<MAX_Z_SIZE>)accum = (ap_int<MAX_Z_SIZE>)0;

		

	PmulLoopOut:    
		for (i = 0; i <= da; i++) {
			#pragma HLS PIPELINE II=1
			t = p1.read();
			for (j = 0; j <= db; j++) {
				#pragma HLS PIPELINE II=1
				(ap_int<MAX_Z_SIZE>)accum = (ap_int<MAX_Z_SIZE>)pp.coeff[i+j] + (ap_int<MAX_Z_SIZE>)t * (ap_int<MAX_Z_SIZE>)p2.coeff[j];
				(ap_int<MAX_Z_SIZE>)pp.coeff[i+j] = (ap_int<MAX_Z_SIZE>)accum;
			

			}
			


		}

	}


	(ap_int<MAX_Z_SIZE>)t=(ap_int<MAX_Z_SIZE>)0;

	
	
rmultLoop:	
	for (i = 0; i <= degree; ++i) {
		#pragma HLS PIPELINE II = 1

		rem(pp.coeff[i], pp.coeff[i], mod);

		rem(pp.coeff[i + degree], pp.coeff[i + degree], mod);

		SubMod(t, pp.coeff[i], pp.coeff[i + degree], mod);

		res_stream.write(t);

	}
	
}


void rMult(hls::stream<ap_int<MAX_Z_SIZE>>& res_stream,hls::stream<long>& res_degree_stream, hls::stream<ap_int<MAX_Z_SIZE>>& p1, hls::stream<ap_int<MAX_Z_SIZE>>& p2_stream,hls::stream<long>& p2_degree_stream, ZZ& mod, const long degree) {
	
	ZZX pp,p2;

	long i, j;
	ap_int<MAX_Z_SIZE> t, accum;

	pp.degree = 0;
	p2.degree = 0;
	

	long db,da;
	long d;

	db = p2_degree_stream.read();

	res_degree_stream.write(degree);

	setDegree(p2,MAX_ZZX_DEGREE-1);

	for(i = 0; i <= db; i++){
		t = p2_stream.read();
		p2.coeff[i] = t;
	}
	

	if((db == 0 && p2.coeff[0] == 0)){
        pp.degree = 0;
        pp.coeff[0] = 0;

    }else{
		da = degree;
		

		d = da + db;


		setDegree(pp, d);

		

    	(ap_int<MAX_Z_SIZE>)t=(ap_int<MAX_Z_SIZE>)0;
		(ap_int<MAX_Z_SIZE>)accum = (ap_int<MAX_Z_SIZE>)0;

		

	PmulLoopOut:    
		for (i = 0; i <= da; i++) {
			#pragma HLS PIPELINE II=1
			t = p1.read();
			for (j = 0; j <= db; j++) {
				#pragma HLS PIPELINE II=1
				(ap_int<MAX_Z_SIZE>)accum = (ap_int<MAX_Z_SIZE>)pp.coeff[i+j] + (ap_int<MAX_Z_SIZE>)t * (ap_int<MAX_Z_SIZE>)p2.coeff[j];
				(ap_int<MAX_Z_SIZE>)pp.coeff[i+j] = (ap_int<MAX_Z_SIZE>)accum;
			

			}
			


		}

	}


	(ap_int<MAX_Z_SIZE>)t=(ap_int<MAX_Z_SIZE>)0;

	
	
rmultLoop:	
	for (i = 0; i <= degree; ++i) {
		#pragma HLS PIPELINE II = 1

		rem(pp.coeff[i], pp.coeff[i], mod);

		rem(pp.coeff[i + degree], pp.coeff[i + degree], mod);

		SubMod(t, pp.coeff[i], pp.coeff[i + degree], mod);

		res_stream.write(t);

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

void rRightShiftStream(hls::stream<ap_int<MAX_Z_SIZE>>& res,hls::stream<ap_int<MAX_Z_SIZE>>& p1, const long bits, const long degree) {
	ap_int<MAX_Z_SIZE> t;
rShiftStLoop:	
	for (long i = 0; i <= degree; ++i) {
		t= p1.read();
		RightShift(t, t, bits);
		res.write(t);
	}
}

void rAddAndEqual(ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
rAddEqLoop:	
	for (long i = 0; i <= degree; ++i) {
		#pragma HLS unroll factor=1
		AddMod(p1.coeff[i], p1.coeff[i], p2.coeff[i], mod);
	}
}

void rAddStream(hls::stream<ap_int<MAX_Z_SIZE>>& res,hls::stream<ap_int<MAX_Z_SIZE>>& p1, hls::stream<ap_int<MAX_Z_SIZE>>& p2, ZZ& mod, const long degree){
	ap_int<MAX_Z_SIZE> t1;
	ap_int<MAX_Z_SIZE> t2;

rAddStLoop:	
	for (long i = 0; i <= degree; ++i) {
		#pragma HLS unroll factor=1
		t1 = p2.read();
		t2 = p1.read();
		AddMod(t2, t2, t1, mod);
		res.write(t2);
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




