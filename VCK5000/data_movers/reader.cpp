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


#include "reader.hpp"


void ckks_plaintext_reader(std::complex<double>* in_vals, long slots, hls::stream<std::complex<double>>& vals, ap_int<MAX_Z_SIZE>  enc_ax[MAX_ZZX_DEGREE],
					long ax_degree, ap_int<MAX_Z_SIZE>  enc_bx[MAX_ZZX_DEGREE], long bx_degree, hls::stream<ap_int<MAX_Z_SIZE>>& enc_ax_stream,
					hls::stream<long>& enc_ax_degree, hls::stream<ap_int<MAX_Z_SIZE>>& enc_bx_stream, hls::stream<long>& enc_bx_degree, long rnd[RND_SIZE], hls::stream<long>& rnd_stream){

	#pragma HLS INTERFACE m_axi port=in_vals depth=MAX_SLOTS offset=slave bundle=gmem0
	#pragma HLS INTERFACE m_axi port=enc_ax depth=MAX_ZZX_DEGREE offset=slave bundle=gmem1
	#pragma HLS INTERFACE m_axi port=enc_bx depth=MAX_ZZX_DEGREE offset=slave bundle=gmem2

	#pragma HLS INTERFACE m_axi port=rnd depth=RND_SIZE offset=slave bundle=gmem3

	#pragma HLS INTERFACE axis port=vals
	#pragma HLS INTERFACE axis port=enc_ax_stream
	#pragma HLS INTERFACE axis port=enc_ax_degree
	#pragma HLS INTERFACE axis port=enc_bx_stream
	#pragma HLS INTERFACE axis port=enc_bx_degree
	#pragma HLS INTERFACE axis port=rnd_stream

	#pragma HLS INTERFACE s_axilite port=in_vals bundle=control
	#pragma HLS INTERFACE s_axilite port=enc_ax bundle=control
	#pragma HLS INTERFACE s_axilite port=enc_bx bundle=control

	#pragma HLS INTERFACE s_axilite port=rnd bundle=control

	#pragma HLS INTERFACE s_axilite port=slots bundle=control
	#pragma HLS INTERFACE s_axilite port=ax_degree bundle=control
	#pragma HLS INTERFACE s_axilite port=bx_degree bundle=control

	#pragma HLS INTERFACE s_axilite port=return bundle=control

	long i;
	long tmp;


	for (i = 0; i < slots; i++){
		#pragma HLS pipeline II=1
		vals.write(in_vals[i]);
	}

	enc_ax_degree.write(ax_degree);
	enc_bx_degree.write(bx_degree);

	for(i = 0; i<RND_SIZE; i++){
		#pragma HLS pipeline II=1
		tmp = rnd[i];
		rnd_stream.write(tmp);
	}

	

	for(i = 0; i<=ax_degree; i++){
		#pragma HLS pipeline II=1
		enc_ax_stream.write((ap_int<MAX_Z_SIZE>)enc_ax[i]);
	}

	
	for(i = 0; i<=bx_degree; i++){
		#pragma HLS pipeline II=1
		enc_bx_stream.write((ap_int<MAX_Z_SIZE>)enc_bx[i]);
	}

	
	

}

void ckks_add_reader(ap_int<MAX_Z_SIZE>  ci_ax_1[MAX_ZZX_DEGREE], long ci_ax_1_degree, ap_int<MAX_Z_SIZE> ci_bx_1[MAX_ZZX_DEGREE], long ci_bx_1_degree,
                     ap_int<MAX_Z_SIZE>  ci_ax_2[MAX_ZZX_DEGREE], long ci_ax_2_degree, ap_int<MAX_Z_SIZE> ci_bx_2[MAX_ZZX_DEGREE], long ci_bx_2_degree,
                     hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_1,hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_1, hls::stream<long>& ci_ax_degree_1, hls::stream<long>& ci_bx_degree_1,
                     hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_2,hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_2, hls::stream<long>& ci_ax_degree_2, hls::stream<long>& ci_bx_degree_2){

    #pragma HLS INTERFACE m_axi port=ci_ax_1 offset=slave bundle=gmem0
    #pragma HLS INTERFACE m_axi port=ci_bx_1 offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=ci_ax_2 offset=slave bundle=gmem2
    #pragma HLS INTERFACE m_axi port=ci_bx_2 offset=slave bundle=gmem3

    #pragma HLS INTERFACE axis port=ci_ax_stream_1
    #pragma HLS INTERFACE axis port=ci_bx_stream_1
    #pragma HLS INTERFACE axis port=ci_ax_degree_1
    #pragma HLS INTERFACE axis port=ci_bx_degree_1

    #pragma HLS INTERFACE axis port=ci_ax_stream_2
    #pragma HLS INTERFACE axis port=ci_bx_stream_2
    #pragma HLS INTERFACE axis port=ci_ax_degree_2
    #pragma HLS INTERFACE axis port=ci_bx_degree_2

    #pragma HLS INTERFACE s_axilite port=ci_ax_1 bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_1 bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_ax_2 bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_2 bundle=control

    #pragma HLS INTERFACE s_axilite port=ci_ax_1_degree bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_1_degree bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_ax_2_degree bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_2_degree bundle=control

    #pragma HLS INTERFACE s_axilite port=return bundle=control

    int i;

    ci_ax_degree_1.write(ci_ax_1_degree);
    ci_bx_degree_1.write(ci_bx_1_degree);

    ci_ax_degree_2.write(ci_ax_2_degree);
    ci_bx_degree_2.write(ci_bx_2_degree);

    #pragma HLS dataflow

    for(i = 0; i <= ci_ax_1_degree; i++){
        #pragma HLS pipeline II=1
        ci_ax_stream_1.write(ci_ax_1[i]);
    }

    for(i = 0; i <= ci_ax_2_degree; i++){
        #pragma HLS pipeline II=1
        ci_ax_stream_2.write(ci_ax_2[i]);
    }

    for(i = 0; i <= ci_bx_1_degree; i++){
        #pragma HLS pipeline II=1
        ci_bx_stream_1.write(ci_bx_1[i]);
    }


    for(i = 0; i <= ci_bx_2_degree; i++){
        #pragma HLS pipeline II=1
        ci_bx_stream_2.write(ci_bx_2[i]);
    }


}


void ckks_decryption_reader(ap_int<MAX_Z_SIZE>  ci_ax[MAX_ZZX_DEGREE], long ci_ax_degree, ap_int<MAX_Z_SIZE> ci_bx[MAX_ZZX_DEGREE], long ci_bx_degree,
                            hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream, hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream, hls::stream<long>& ci_ax_degree_stream, hls::stream<long>& ci_bx_degree_stream){

    #pragma HLS INTERFACE m_axi port=ci_ax offset=slave bundle=gmem0
    #pragma HLS INTERFACE m_axi port=ci_bx offset=slave bundle=gmem1

    #pragma HLS INTERFACE axis port=ci_ax_stream
    #pragma HLS INTERFACE axis port=ci_bx_stream
    #pragma HLS INTERFACE axis port=ci_ax_degree_stream
    #pragma HLS INTERFACE axis port=ci_bx_degree_stream

    #pragma HLS INTERFACE s_axilite port=ci_ax bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx bundle=control

    #pragma HLS INTERFACE s_axilite port=ci_ax_degree bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_degree bundle=control

    #pragma HLS INTERFACE s_axilite port=return bundle=control

    int i;

    ci_ax_degree_stream.write(ci_ax_degree);
    ci_bx_degree_stream.write(ci_bx_degree);

    #pragma HLS dataflow

    for(i = 0; i <= ci_ax_degree; i++){
        #pragma HLS pipeline II=1
        ci_ax_stream.write(ci_ax[i]);
    }

    for(i = 0; i <= ci_bx_degree; i++){
        #pragma HLS pipeline II=1
        ci_bx_stream.write(ci_bx[i]);
    }

}
