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

#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP


#define MAX_ROT_GROUP_SIZE 128 // maximum size of rotgroup array = context.Nh
#define MAX_KSI_POWS_SIZE 513 // maximum size of ksipows array = context.M +1
#define MAX_QPOW_VEC_SIZE 65 // maximum size of qpowvec array = context.logQQ + 1

#define SIGMA 3.2 
#define H 64 
#define RND_SIZE 257 // need to be 1 + N to work correctly with the streams (N = 1 << LOG_N)

//SCHEMES PARAMETERS
#define LOG_N 8
#define LOG_Q 32
#define LOG_q 32
#define LOG_P 30



// Parameters for ZZ struct --------------------------------------------

#define MAX_Z_SIZE 256 // maximum size of value in ZZ struct defined in ZStructures.hpp
#define MAX_ZZX_DEGREE 513 // maximum size of coeff array in ZZX struct defined in ZStructures.hpp CHANGES WITH N


#define MAX_SLOTS 8


#endif // PARAMETERS_HPP
