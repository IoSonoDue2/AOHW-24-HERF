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
	double res = (double)val;
	res /= (1 << logp);
	
	return res;
}

ap_int<MAX_Z_SIZE> scaleUpToZZ(double val, long logp){
	ap_int<MAX_Z_SIZE> res;
	
	ap_fixed<256,128,AP_RND> valScaled = val;

	valScaled <<= logp;
	res = valScaled;
	return res;
}

void fftSpecial(std::complex<double>* vals, const long size, Context context){

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


void fftSpecialInv(std::complex<double>* vals, const long size, long rotGroup[MAX_ROT_GROUP_SIZE], std::complex<double> ksiPows[MAX_KSI_POWS_SIZE], long N){
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

void ckks_decode(hls::stream<std::complex<double>>& vals, long slots, hls::stream<ap_int<MAX_Z_SIZE>> & mx_stream){
	
	#pragma HLS INTERFACE axis port=vals
	#pragma HLS INTERFACE axis port=mx_stream

	#pragma HLS INTERFACE s_axilite port=slots bundle=control
	#pragma HLS INTERFACE s_axilite port=return bundle=control

	
	
	long i;

	long N = 1 << LOG_N;
	long Nh = N >> 1;
	long M = N << 1;
	long fivePows = 1;
	double angle;

	long j = 0;
	ap_int<MAX_Z_SIZE> trash,tmp;
	long gap = Nh / slots;
	std::complex<double> res[MAX_SLOTS];
	ap_int<MAX_Z_SIZE> mx[MAX_SLOTS*2];
	ap_int<MAX_Z_SIZE> q = (ap_int<MAX_Z_SIZE>)1 << (LOG_q);

	long rotGroupArr[MAX_ROT_GROUP_SIZE];
	std::complex<double> ksiPowsArr[MAX_KSI_POWS_SIZE];

	for (i = 0; i < Nh; ++i) {
		#pragma HLS pipeline II=1
		rotGroupArr[i] = fivePows;
		fivePows *= 5;
		fivePows %= M;
	}

	for (i = 0; i < M; ++i) {
		#pragma HLS pipeline II=1
		angle = 2.0 * M_PI * i / M;
		ksiPowsArr[i].real(cos(angle));
		ksiPowsArr[i].imag(sin(angle));

	}
	ksiPowsArr[M] = ksiPowsArr[0];


	for(i = 0; i < Nh*2; i++){
		#pragma HLS pipeline II=1
		if(i%gap == 0){
			j=i/gap;
			mx[j] = mx_stream.read();
		}
		else{
			trash = mx_stream.read();
		}
	}

	while(!mx_stream.empty()){
		#pragma HLS pipeline II=1
		trash = mx_stream.read();
	}

	for (i = 0; i < slots; ++i) {
		#pragma HLS pipeline II=1
		tmp = ((mx[i] % q)+q)%q;
		if(NumBits(tmp) == LOG_q) tmp -= q;
		res[i].real(scaleDownToReal(tmp, LOG_P));


		tmp = ((mx[i + slots] % q)+q)%q;
		if(NumBits(tmp) == LOG_q) tmp -= q;
		res[i].imag(scaleDownToReal(tmp, LOG_P));


	}

	long bit,idx;
	long len,lenh,lenq;
	std::complex<double> ctmp;
	std::complex<double> v;
	std::complex<double> u;

	for (i = 1, j = 0; i < slots; ++i) {
		bit = slots >> 1;
		for (; j >= bit; bit>>=1) {
			j -= bit;
		}

		j += bit;
		if(i < j) {
			ctmp= res[i];
			res[i] = res[j];
			res[j] = ctmp;
		}
	}

	for (len = 2; len <= slots; len <<= 1) {
		for ( i = 0; i < slots; i += len) {
			lenh = len >> 1;
			lenq = len << 2;
			for ( j = 0; j < lenh; ++j) {
				idx = ((rotGroupArr[j] % lenq)) * M / lenq;
				u = res[i + j];
				v = res[i + j + lenh];
				v *= ksiPowsArr[idx];
				res[i + j] = u + v;
				res[i + j + lenh] = u - v;
			}
		}
	}

	for (i = 0; i < slots; ++i) {
		vals.write(res[i]);
	}


	



}


void decode(ZZX& mx, long slots, Context context,std::complex<double> res[MAX_SLOTS]){
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

Plaintext decryptMsg(SecretKey& secretKey, Ciphertext& cipher,Context context){
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

void ckks_encode(hls::stream<std::complex<double>>& vals, long slots, hls::stream<ap_int<MAX_Z_SIZE>> & mx){

	#pragma HLS INTERFACE axis port=vals

	#pragma HLS INTERFACE axis port=mx



	#pragma HLS INTERFACE s_axilite port=slots bundle=control

	#pragma HLS INTERFACE s_axilite port=return bundle=control


	std::complex<double> uvals[MAX_SLOTS];
	long rotGroupArr[MAX_ROT_GROUP_SIZE];
	std::complex<double> ksiPowsArr[MAX_KSI_POWS_SIZE];

	long i, jdx, idx;
	long N = 1 << LOG_N;
	long Nh = N >> 1;
	long M = N << 1;
	long fivePows = 1;
	double angle;


	for (i = 0; i < slots; ++i) {
		#pragma HLS pipeline II=1
		uvals[i] = vals.read();
	}

	for (i = 0; i < Nh; ++i) {
		#pragma HLS pipeline II=1
		rotGroupArr[i] = fivePows;
		fivePows *= 5;
		fivePows %= M;
	}

	for (i = 0; i < M; ++i) {
		#pragma HLS pipeline II=1
		angle = 2.0 * M_PI * i / M;
		ksiPowsArr[i].real(cos(angle));
		ksiPowsArr[i].imag(sin(angle));

	}
	ksiPowsArr[M] = ksiPowsArr[0];


	fftSpecialInv(uvals, slots,rotGroupArr,ksiPowsArr,N);

	long j = 0;
	long gap = Nh / slots;
	ap_int<MAX_Z_SIZE> zero = 0;



	EncodeScaleUpLoop:	
	for (i = 0, idx = 0; i < slots; ++i, idx += gap) {
		#pragma HLS pipeline II=1

		while(j<idx){
			mx.write(zero);
			j++;
		}
		

		mx.write(scaleUpToZZ(uvals[i].real(), LOG_P+LOG_Q));
		j++;

	}

	for(i = 0, idx = Nh; i < slots; ++i, idx += gap){
		#pragma HLS pipeline II=1

		while(j<idx){
			mx.write(zero);
			j++;
		}

		mx.write(scaleUpToZZ(uvals[i].imag(), LOG_P+LOG_Q));
		j++;
	}

	while(j<=N){
		mx.write(zero);
		j++;
	}

	



}


void ckks_encrypt(hls::stream<ap_int<MAX_Z_SIZE>>& mx_stream, hls::stream<ap_int<MAX_Z_SIZE>>& enc_ax_stream, hls::stream<long>& enc_ax_degree, hls::stream<ap_int<MAX_Z_SIZE>>& enc_bx_stream, hls::stream<long>& enc_bx_degree, long slots, hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax, hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx, hls::stream<long>& ci_ax_degree, hls::stream<long>& ci_bx_degree, hls::stream<long>& rnd_stream){
	

	#pragma HLS INTERFACE axis port=mx_stream
	#pragma HLS INTERFACE axis port=enc_ax_stream
	#pragma HLS INTERFACE axis port=enc_ax_degree
	#pragma HLS INTERFACE axis port=enc_bx_stream
	#pragma HLS INTERFACE axis port=enc_bx_degree

	#pragma HLS INTERFACE axis port=ci_ax
	#pragma HLS INTERFACE axis port=ci_bx
	#pragma HLS INTERFACE axis port=ci_ax_degree
	#pragma HLS INTERFACE axis port=ci_bx_degree

	#pragma HLS INTERFACE axis port=rnd_stream

	#pragma HLS INTERFACE s_axilite port=slots bundle=control

	#pragma HLS INTERFACE s_axilite port=return bundle=control


	long i,N;


	N = 1 << LOG_N;
	ap_int<MAX_Z_SIZE> qQ = (ap_int<MAX_Z_SIZE>)1 << (LOG_q + LOG_Q);


	hls::stream<long,RND_SIZE> rnd_stream1_1("rnd_stream1_1");
	hls::stream<long,RND_SIZE> rnd_stream1_2("rnd_stream1_2");
	hls::stream<long,RND_SIZE> rnd_stream2_1("rnd_stream2_1");
	hls::stream<long,RND_SIZE> rnd_stream2_2("rnd_stream2_2");
	hls::stream<long,RND_SIZE> rnd_stream3_1("rnd_stream3_1");
	hls::stream<long,RND_SIZE> rnd_stream3_2("rnd_stream3_2");

	long val;

	for(i = 0; i<RND_SIZE; i++){
		#pragma HLS pipeline II=1
		val = rnd_stream.read();
		rnd_stream1_1.write(val);
		rnd_stream1_2.write(val);
		rnd_stream2_1.write(val);
		rnd_stream2_2.write(val);
		rnd_stream3_1.write(val);
		rnd_stream3_2.write(val);
	}


	hls::stream<ap_int<MAX_Z_SIZE>,MAX_ZZX_DEGREE> vx_stream[2];
	hls::stream<ap_int<MAX_Z_SIZE>,MAX_ZZX_DEGREE> ex_stream[2];

	hls::stream<ap_int<MAX_Z_SIZE>,MAX_ZZX_DEGREE> ax_stream[2];

	hls::stream<ap_int<MAX_Z_SIZE>,MAX_ZZX_DEGREE> bx_stream[3];

	#pragma HLS dataflow


	

	sampleZO(vx_stream, N, rnd_stream1_1, rnd_stream1_2);

	
	rMult(ax_stream[0],ci_ax_degree, vx_stream[0], enc_ax_stream,enc_ax_degree, qQ, N);


	sampleGauss(ex_stream[0], N, SIGMA, rnd_stream2_1, rnd_stream2_2);


	rAddStream(ax_stream[1],ax_stream[0], ex_stream[0], qQ, N);
	

	rMult(bx_stream[0], ci_bx_degree, vx_stream[1], enc_bx_stream,enc_bx_degree, qQ, N);

	
	sampleGauss(ex_stream[1], N, SIGMA, rnd_stream3_1, rnd_stream3_2);

	rAddStream(bx_stream[1],bx_stream[0], ex_stream[1], qQ, N);



	rAddStream(bx_stream[2],bx_stream[1], mx_stream, qQ, N);



	rRightShiftStream(ci_ax, ax_stream[1], LOG_Q, N);
	rRightShiftStream(ci_bx, bx_stream[2], LOG_Q, N);





}

void ckks_add(hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_1,hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_1, hls::stream<long>& ci_ax_degree_1, hls::stream<long>& ci_bx_degree_1,
			  hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_2,hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_2, hls::stream<long>& ci_ax_degree_2, hls::stream<long>& ci_bx_degree_2,
			  hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_out, hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_out, hls::stream<long>& ci_ax_degree_out, hls::stream<long>& ci_bx_degree_out){

	#pragma HLS INTERFACE axis port=ci_ax_stream_1
	#pragma HLS INTERFACE axis port=ci_bx_stream_1
	#pragma HLS INTERFACE axis port=ci_ax_degree_1
	#pragma HLS INTERFACE axis port=ci_bx_degree_1

	#pragma HLS INTERFACE axis port=ci_ax_stream_2
	#pragma HLS INTERFACE axis port=ci_bx_stream_2
	#pragma HLS INTERFACE axis port=ci_ax_degree_2
	#pragma HLS INTERFACE axis port=ci_bx_degree_2

	#pragma HLS INTERFACE axis port=ci_ax_stream_out
	#pragma HLS INTERFACE axis port=ci_bx_stream_out
	#pragma HLS INTERFACE axis port=ci_ax_degree_out
	#pragma HLS INTERFACE axis port=ci_bx_degree_out

	#pragma HLS INTERFACE s_axilite port=return bundle=control

	long N = 1 << LOG_N;
	long i;

	ap_int<MAX_Z_SIZE> q = (ap_int<MAX_Z_SIZE>)1 << (LOG_q);


	#pragma HLS dataflow

	rAddStream(ci_ax_stream_out, ci_ax_stream_1, ci_ax_stream_2, q, N);
	rAddStream(ci_bx_stream_out, ci_bx_stream_1, ci_bx_stream_2, q, N);

	ci_ax_degree_1.read();
	ci_bx_degree_1.read();

	ci_ax_degree_2.read();
	ci_bx_degree_2.read();

	ci_ax_degree_out.write(N);
	ci_bx_degree_out.write(N);




}

void ckks_decrypt( ap_int<MAX_Z_SIZE>  sx[MAX_ZZX_DEGREE], long sx_degree, hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream, hls::stream<long>& ci_ax_degree, hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream, hls::stream<long>& ci_bx_degree,hls::stream<ap_int<MAX_Z_SIZE>>& mx_out_stream ){

	#pragma HLS INTERFACE m_axi port=sx depth=MAX_ZZX_DEGREE offset=slave bundle=gmem4
	
	#pragma HLS INTERFACE axis port=ci_ax_stream depth=MAX_ZZX_DEGREE
	#pragma HLS INTERFACE axis port=ci_ax_degree
	#pragma HLS INTERFACE axis port=ci_bx_stream depth=MAX_ZZX_DEGREE
	#pragma HLS INTERFACE axis port=ci_bx_degree
	#pragma HLS INTERFACE axis port=mx_out_stream depth=MAX_ZZX_DEGREE

	#pragma HLS INTERFACE s_axilite port=sx bundle=control

	#pragma HLS INTERFACE s_axilite port=sx_degree bundle=control
	#pragma HLS INTERFACE s_axilite port=return bundle=control

	long N = 1 << LOG_N;
	long i;

	ap_int<MAX_Z_SIZE> q = (ap_int<MAX_Z_SIZE>)1 << (LOG_q);
	i = ci_bx_degree.read();

	hls::stream<ap_int<MAX_Z_SIZE>,MAX_ZZX_DEGREE> mx_stream;
	hls::stream<ap_int<MAX_Z_SIZE>,MAX_ZZX_DEGREE> sx_stream;

	for(i = 0; i<=sx_degree; i++){
		#pragma HLS pipeline II=1
		sx_stream.write(sx[i]);
	}
	
	#pragma HLS dataflow

	rMult(mx_stream, sx_stream, ci_ax_stream,ci_ax_degree, q, N);

	rAddStream(mx_out_stream, mx_stream, ci_bx_stream, q, N);

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


Ciphertext add(Ciphertext& cipher1, Ciphertext& cipher2,Context context) {
	ZZ q = context.qpowvec[LOG_q];
	ZZX ax, bx;

	Ciphertext c;

	ax.degree=0;
	bx.degree=0;


	rAdd(ax, cipher1.ax, cipher2.ax, q, context.N);

	rAdd(bx, cipher1.bx, cipher2.bx, q, context.N);

	c = initializeCiphertext(ax, bx, cipher1.slots, cipher1.isComplex);
	return c;
}
