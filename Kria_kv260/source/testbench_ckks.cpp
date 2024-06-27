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


#include <iostream>
#include <random>
#include <ctime>
#include "Structures.hpp"
#include "ZStructures.hpp"
#include "Scheme.hpp"
#include "writer.hpp"


void generateRandomArray(long array[RND_SIZE]) {


    srand(time(nullptr)); 

    for (int i = 0; i < RND_SIZE; i++) {
        array[i] = static_cast<long>(rand()) << 32 | rand();
    }
}


int main(){

    long rnd[RND_SIZE];

    generateRandomArray(rnd);


    Context context = initializeContext();
    std::cout<<"context initiazlied"<<std::endl;

    int slots = 8;
    int i = 0;



    //print the context 
    std::cout << "Context Parameters : " << std::endl;
    std::cout << "sigma: " << context.sigma << std::endl;
    std::cout << "h: " << context.h << std::endl;
    std::cout << "N: " << context.N << std::endl;
    std::cout << "Nh: " << context.Nh << std::endl;
    std::cout << "logNh: " << context.logNh << std::endl;
    std::cout << "M: " << context.M << std::endl;
    std::cout << "logQQ: " << context.logQQ << std::endl;
    std::cout << "Q: " << context.Q << std::endl;
    std::cout << "QQ: " << context.QQ << std::endl;


    SecretKey secretKey = initializeSecretKey(rnd);

    std::cout<<"secret key initialized"<<std::endl;

    Scheme scheme = initializeScheme(context, secretKey,rnd);

    std::cout<<"scheme initialized"<<std::endl;



    std::complex<double> messageVec1[] = {
        {0.24428, 0.651282},
        {0.868426, 0.678266},
        {0.777887, 0.301179},
        {0.883243, 0.13489},
        {0.991355, 0.510088},
        {0.710801, 0.774569},
        {0.809488, 0.386638},
        {0.609341, 0.486097}
    };

    std::complex<double> messageVec2[] = {
        {0.3, 0.3443545},
        {0.99999, 0.678266},
        {0.777887, 0.301179},
        {0.883243, 0.13489},
        {0.991355, 0.510088},
        {0.710801, 0.774569},
        {0.809488, 0.386638},
        {0.609341, 0.486097}
    };


//  TEST ENCODE KERNEL----------------------------------------------------------------------------

    ap_int<MAX_Z_SIZE> ci_ax_out[MAX_ZZX_DEGREE];
    long  ax_degree_out[1];
    ap_int<MAX_Z_SIZE> ci_bx_out[MAX_ZZX_DEGREE];
    long  bx_degree_out[1];

    std::complex<double> decryptedVec1_sw[MAX_SLOTS];
    std::complex<double> decryptedVec1_hw[MAX_SLOTS];


    Ciphertext cipher1 = encrypt(messageVec1, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);
    ckks_ker(messageVec1,slots, scheme.EncKey.ax.coeff, scheme.EncKey.ax.degree, scheme.EncKey.bx.coeff,scheme.EncKey.bx.degree, rnd, ci_ax_out, ci_bx_out, ax_degree_out, bx_degree_out);

    if(ax_degree_out[0] != cipher1.ax.degree)
        std::cout<<"simulation failed in chipher 1 ax degree"<<std::endl;
    else if(bx_degree_out[0] != cipher1.bx.degree)
        std::cout<<"simulation failed in chipher 1 bx degree"<<std::endl;

    bool error = false;
    
    for(i = 0; i<ax_degree_out[0];i++){
        if (ci_ax_out[i] != cipher1.ax.coeff[i]){
            std::cout<<"CHIPHER 1 ax polynimial coeffs: " << i <<" simulation failed kernel ax: "<<ci_ax_out[i] << " real:"<< cipher1.ax.coeff[i]<<std::endl;
            error = true;
        }
    }

    for(i = 0; i<bx_degree_out[0];i++){
        if (ci_bx_out[i] != cipher1.bx.coeff[i]){
            std::cout<<"CHIPHER 1 bx polynimial coeffs: " << i <<" simulation failed kernel bx: "<<ci_bx_out[i] << " real:"<< cipher1.bx.coeff[i] <<std::endl;
            error = true;
        }
    }
    if(error)
        std::cout<<"\033[31m"<<"WARNING: THE PREVIOUS PRINTS SHOULD BE TREATED AS AN ERROR IFF ENCRYPTION IS RUNNING IN C/RTL CO SIMULATION"<<"\033[0m"<<std::endl;

    decrypt(secretKey, cipher1, context,decryptedVec1_sw);

    for (i =0; i<slots; i++){
        if(decryptedVec1_sw[i] != messageVec1[i]){
            std::cout<<"Noise detected during decryption in Message 1: "<<decryptedVec1_sw[i] << " != " << messageVec1[i] << "~~" << decryptedVec1_sw[i] - messageVec1[i]<<std::endl;
        }
    }

    ckks_dec_ker(secretKey.sx.coeff, secretKey.sx.degree, ci_ax_out, ci_bx_out, ax_degree_out[0], bx_degree_out[0], slots, decryptedVec1_hw);
    for (i =0; i<slots; i++){
        if(decryptedVec1_sw[i] != decryptedVec1_hw[i]){
            std::cout<<"HARDWARE FAILURE DECRYPTION DIFFERS IN MESSAGE 1: "<<decryptedVec1_sw[i] << " != " << decryptedVec1_hw[i] << "~~" << decryptedVec1_sw[i] - decryptedVec1_hw[i]<<std::endl;
        }
    }





    return 0;
}




