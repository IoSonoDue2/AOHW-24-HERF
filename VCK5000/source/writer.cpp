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


#include "writer.hpp"

void ckks_cipher_writer(hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax, hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx, hls::stream<long>& ci_ax_degree, hls::stream<long>& ci_bx_degree, ap_int<MAX_Z_SIZE> ci_ax_out[MAX_ZZX_DEGREE], ap_int<MAX_Z_SIZE> ci_bx_out[MAX_ZZX_DEGREE], long *ci_ax_degree_out, long *ci_bx_degree_out){
    
    #pragma HLS INTERFACE axis port=ci_ax
    #pragma HLS INTERFACE axis port=ci_bx
    #pragma HLS INTERFACE axis port=ci_ax_degree
    #pragma HLS INTERFACE axis port=ci_bx_degree

    #pragma HLS INTERFACE m_axi port=ci_ax_out offset=slave bundle=gmem4
    #pragma HLS INTERFACE m_axi port=ci_bx_out offset=slave bundle=gmem5
    #pragma HLS INTERFACE m_axi port=ci_ax_degree_out offset=slave bundle=gmem6
    #pragma HLS INTERFACE m_axi port=ci_bx_degree_out offset=slave bundle=gmem7

    #pragma HLS INTERFACE s_axilite port=ci_ax_out bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_out bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_ax_degree_out bundle=control
    #pragma HLS INTERFACE s_axilite port=ci_bx_degree_out bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    long ax_degree = ci_ax_degree.read();
    long bx_degree = ci_bx_degree.read();
    int i;

    ci_ax_degree_out[0] = ax_degree;
    for(i = 0; i <= ax_degree; i++){
        ci_ax_out[i] = ci_ax.read();
    }

    
    ci_bx_degree_out[0] = bx_degree;
    for(i = 0; i <= bx_degree; i++){
        ci_bx_out[i] = ci_bx.read();
    }
    
}

void ckks_plaintext_writer(hls::stream<std::complex<double>>&vals, std::complex<double> vals_out[MAX_SLOTS]){

    #pragma HLS INTERFACE axis port=vals

    #pragma HLS INTERFACE m_axi port=vals_out offset=slave bundle=gmem5

    #pragma HLS INTERFACE s_axilite port=vals_out bundle=control

    #pragma HLS INTERFACE s_axilite port=return bundle=control

    for(int i = 0; i < MAX_SLOTS; i++){
        vals_out[i] = vals.read();
    }




}
