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


#include "ZStructures.hpp"

ZZ power2_ZZ(long n) {
    ZZ result;

    result=0;
    result[n] = 1;

    return result;
}

ZZX normalize(const ZZX& a) {
    ZZX result;
    result.degree = 0;
    long n;

    n = a.degree;
    if (n == 0) return result;
    while (n > 0 && a.coeff[n] == 0) n--;


    setDegree(result, n);
    for (long i = 0; i <= n; i++) {
        result.coeff[i] = a.coeff[i];
        
    }

    return result;
}

void setDegree(ZZX& x, long d)
{

   for (long i = x.degree; i <= d; i++)
      x.coeff[i] = 0;

   x.degree = d;
}

void PlainMul(ZZX& x, const ZZX& a, const ZZX& b)
{   


    long da = a.degree;
    long db = b.degree;

    
    if ((da == 0 && a.coeff[0]==0) || (db == 0 && b.coeff[0]==0)) {

        x.degree = 0;
        x.coeff[0] = a.coeff[0] * b.coeff[0];
        return;
    }

    long d = da + db;


    setDegree(x, d);
    long i, j, jmax, jmin;

    ap_int<MAX_Z_SIZE> t, accum;

    (ap_int<MAX_Z_SIZE>)t=(ap_int<MAX_Z_SIZE>)0;


PmulLoopOut:    
    for (i = 0; i <= d; i++) {
        #pragma HLS PIPELINE II=1



        if (0 <= i - db) jmin = i - db;
        else jmin = 0;

        if (da <= i) jmax = da;
        else jmax = i;

        (ap_int<MAX_Z_SIZE>)accum = (ap_int<MAX_Z_SIZE>)0;

PmulLoopIn:        
        for (j = jmin; j <= jmax; j++) {
            #pragma HLS bind_op variable=t op=mul impl=dsp
            #pragma HLS bind_op variable=accum op=add impl=dsp
            #pragma HLS allocation operation instances=mul limit=1 
            #pragma HLS allocation operation instances=add limit=1 
            (ap_int<MAX_Z_SIZE>)t = (ap_int<MAX_Z_SIZE>) a.coeff[j] * (ap_int<MAX_Z_SIZE>)b.coeff[i-j];


            (ap_int<MAX_Z_SIZE>)accum = (ap_int<MAX_Z_SIZE>)accum + (ap_int<MAX_Z_SIZE>)t;
            
        }
        (ap_int<MAX_Z_SIZE>)x.coeff[i] = (ap_int<MAX_Z_SIZE>) accum;

    }

}

void RightShift(ap_int<MAX_Z_SIZE>& x, const ap_int<MAX_Z_SIZE>& a, long k)
{
    if (k<0){
        x = a << (-k);
    }else{
        x = a >> k;
    }
}

void LeftShift(ap_int<MAX_Z_SIZE>& x, const ap_int<MAX_Z_SIZE>& a, long k)
{
    if (k<0){
        x = a >> (-k);
    }else{
        x = a << k;
    }
}

void SubMod(ZZ& x, const ZZ& a, const ZZ& b, const ZZ& n)
{   


    x = (((a - b) % n)+n) % n;
}

void AddMod(ZZ& x, const ZZ& a, const ZZ& b, const ZZ& n){
    x = (a + b) % n;

}

void rAdd(ZZX& res, ZZX& p1, ZZX& p2, ZZ& mod, const long degree) {
	setDegree(res, degree);
	for (long i = 0; i <= degree; ++i) {
		AddMod(res.coeff[i], p1.coeff[i], p2.coeff[i], mod);
	}
}

void mul(ZZX& c, const ZZX& a, const ZZX& b)
{

    if((a.degree == 0 && a.coeff[0] == 0) || (b.degree == 0 && b.coeff[0] == 0)){
        c.degree = 0;
        c.coeff[0] = 0;
        return;
    }

    PlainMul(c, a, b);
    return;

}

void mul(ZZX& x, const ZZX& a, const ZZ& b)
{
   ap_uint<MAX_ZZX_DEGREE> t;
   long da, i;

   if (b == 0) {
      x.degree = 0;
      x.coeff[0] = 0;
      return;
   }

   t = b;
   da = a.degree;
   setDegree(x, da);

    for (i = 0; i <= da; i++) {
        x.coeff[i] = a.coeff[i] * t;
    }

}

void rem(ZZ& r, const ZZ& a, const ZZ& b){
    #pragma HLS bind_op variable=r op=sub impl=dsp
    #pragma HLS bind_op variable=r op=add impl=dsp
    #pragma HLS bind_op variable=r op=mul impl=dsp
    r = a % b;
}

