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


#ifndef READER_HPP
#define READER_HPP

#include "Parameters.hpp"
#include "hls_stream.h"
#include "ap_int.h"
#include <complex>


void ckks_plaintext_reader(std::complex<double>* in_vals, long slots, hls::stream<std::complex<double>>& vals, ap_int<MAX_Z_SIZE>  enc_ax[MAX_ZZX_DEGREE],
					long ax_degree, ap_int<MAX_Z_SIZE>  enc_bx[MAX_ZZX_DEGREE], long bx_degree, hls::stream<ap_int<MAX_Z_SIZE>>& enc_ax_stream,
					hls::stream<long>& enc_ax_degree, hls::stream<ap_int<MAX_Z_SIZE>>& enc_bx_stream, hls::stream<long>& enc_bx_degree, long rnd[RND_SIZE], hls::stream<long>& rnd_stream);

void ckks_add_reader(ap_int<MAX_Z_SIZE>  ci_ax_1[MAX_ZZX_DEGREE], long ci_ax_1_degree, ap_int<MAX_Z_SIZE> ci_bx_1[MAX_ZZX_DEGREE], long ci_bx_1_degree,
                     ap_int<MAX_Z_SIZE>  ci_ax_2[MAX_ZZX_DEGREE], long ci_ax_2_degree, ap_int<MAX_Z_SIZE> ci_bx_2[MAX_ZZX_DEGREE], long ci_bx_2_degree,
                     hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_1,hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_1, hls::stream<long>& ci_ax_degree_1, hls::stream<long>& ci_bx_degree_1,
                     hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream_2,hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream_2, hls::stream<long>& ci_ax_degree_2, hls::stream<long>& ci_bx_degree_2);

void ckks_decryption_reader(ap_int<MAX_Z_SIZE>  ci_ax[MAX_ZZX_DEGREE], long ci_ax_degree, ap_int<MAX_Z_SIZE> ci_bx[MAX_ZZX_DEGREE], long ci_bx_degree,
                            hls::stream<ap_int<MAX_Z_SIZE>>& ci_ax_stream, hls::stream<ap_int<MAX_Z_SIZE>>& ci_bx_stream, hls::stream<long>& ci_ax_degree_stream, hls::stream<long>& ci_bx_degree_stream);

#endif // READER_HPP