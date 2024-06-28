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
#include "hls_stream.h"
#include "../Structures.hpp"
#include "../ZStructures.hpp"
#include "../Scheme.hpp"
#include "../writer.hpp"
#include "../reader.hpp"


void generateRandomArray(long array[RND_SIZE]) {


    srand(time(nullptr)); 

    for (int i = 0; i < RND_SIZE; i++) {
        array[i] = static_cast<long>(rand()) << 32 | rand();
    }
}


int main(){

    long rnd[RND_SIZE];

    std::cout<<"[EXEC] 1. Generating random array... ";
    generateRandomArray(rnd);
    std::cout << "Done" << std::endl;

    std::cout<<"[EXEC] 2. Initializing context... ";
    Context context = initializeContext();
    std::cout << "Done" << std::endl;


    int slots = 8;
    int i = 0;




    std::cout << "[INFO] ###### Context Parameters ######" << std::endl;
    std::cout << "[INFO] logQ: " << LOG_Q << std::endl;
    std::cout << "[INFO] logN: " << LOG_N << std::endl;
    std::cout << "[INFO] logP: " << LOG_P << std::endl;
    std::cout << "[INFO] logq: " << LOG_q << std::endl;
    std::cout << "[INFO] sigma: " << context.sigma << std::endl;
    std::cout << "[INFO] h: " << context.h << std::endl;
    std::cout << "[INFO] N: " << context.N << std::endl;
    std::cout << "[INFO] Nh: " << context.Nh << std::endl;
    std::cout << "[INFO] logNh: " << context.logNh << std::endl;
    std::cout << "[INFO] M: " << context.M << std::endl;
    std::cout << "[INFO] logQQ: " << context.logQQ << std::endl;
    std::cout << "[INFO] Q: " << context.Q << std::endl;
    std::cout << "[INFO] QQ: " << context.QQ << std::endl;

    std::cout<<"[EXEC] 3. Generating Secret Key... ";
    SecretKey secretKey = initializeSecretKey(rnd);
    std::cout << "Done" << std::endl;

    std::cout<<"[EXEC] 4. Initializing Scheme... ";

    Scheme scheme = initializeScheme(context, secretKey,rnd);

    std::cout << "Done" << std::endl;

    std::cout << "[INFO] Tests will run with " << slots << " slots test vectors" << std::endl;

    std::cout << "[INFO] Using the following complex test vectors: " << std::endl;

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

    for (i = 0; i < slots; i++) {
        std::cout << "[INFO] messageVec1[" << i << "]: "
                  << "(" << messageVec1[i].real() << "," << messageVec1[i].imag() << ")\n";
    }

    for (i = 0; i < slots; i++) {
        std::cout << "[INFO] messageVec2[" << i << "]: "
                  << "(" << messageVec2[i].real() << "," << messageVec2[i].imag() << ")\n";
    }

    std::cout << "[INFO] The following kernels will be tested: " << std::endl;
    std::cout << "[INFO] - CKKS_ENCODING" << std::endl;
    std::cout << "[INFO] - CKKS_ENCRYPTION" << std::endl;
    std::cout << "[INFO] - CKKS_ADD" << std::endl;
    std::cout << "[INFO] - CKKS_DECRYPTION" << std::endl;

    
    


//  TEST ENCODE KERNEL----------------------------------------------------------------------------

    bool error = false;

    std::cout << "[EXEC] 5. Testing CKKS_ENCODING kernel... ";
    
    hls::stream<std::complex<double>> encode_vals ("encode_vals");
    hls::stream<ap_int<MAX_Z_SIZE>> encode_mx ("encode_mx");

    Plaintext encode_plaintext = encode(messageVec1, slots, context.rotGroup, context.ksiPows, context.N);

    
    for(i = 0; i<slots; i++){
        encode_vals.write(messageVec1[i]);
    }

    ckks_encode(encode_vals, slots, encode_mx);

    ap_int<MAX_Z_SIZE> encodedmxVec[MAX_SLOTS];

    for(i = 0; i<=encode_plaintext.mx.degree; i++){
        ap_int<MAX_Z_SIZE> tmp = encode_mx.read();
        
        if(tmp != encode_plaintext.mx.coeff[i]){
            std::cout<<"\n[ERROR] Encode failed: polynomial coefficent "<<i<<"is different from the sw implementation";
            error =true;
        }
    }
    if(error){
        std::cout<<"\n[ERROR] CKKS_ENCODING kernel failed"<<std::endl;
        return 0;
    }
    std::cout << "Done" << std::endl;

//  TEST ENCRYPT KERNEL----------------------------------------------------------------------------

    std::cout << "[EXEC] 6. Testing CKKS_ENCRYPTION kernel... ";

    hls::stream<std::complex<double>> encrypt_vals ("encrypt_vals");
    hls::stream<ap_int<MAX_Z_SIZE>> encrypt_mx ("encrypt_mx");
    hls::stream<ap_int<MAX_Z_SIZE>> enc_ax_stream("enc_ax_stream");
    hls::stream<long> enc_ax_degree("enc_ax_degree_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> enc_bx_stream("enc_bx_stream");
    hls::stream<long> enc_bx_degree("enc_bx_degree_stream");
    hls::stream<long> rnd_stream("rnd_stream");

    hls::stream<ap_int<MAX_Z_SIZE>> ci_ax("ci_ax_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> ci_bx("ci_bx_stream");
    hls::stream<long> ci_ax_degree("ci_ax_degree_stream");
    hls::stream<long> ci_bx_degree("ci_bx_degree_stream");
    ap_int<MAX_Z_SIZE> ci_ax_out[MAX_ZZX_DEGREE];
    long  ax_degree_out[1];
    ap_int<MAX_Z_SIZE> ci_bx_out[MAX_ZZX_DEGREE];
    long  bx_degree_out[1];

    ckks_plaintext_reader(messageVec1, slots, encrypt_vals, scheme.EncKey.ax.coeff, scheme.EncKey.ax.degree, scheme.EncKey.bx.coeff, scheme.EncKey.bx.degree, enc_ax_stream, enc_ax_degree, enc_bx_stream, enc_bx_degree, rnd, rnd_stream);
    ckks_encode(encrypt_vals, slots, encrypt_mx);
    ckks_encrypt(encrypt_mx, enc_ax_stream, enc_ax_degree, enc_bx_stream, enc_bx_degree, slots, ci_ax, ci_bx, ci_ax_degree, ci_bx_degree, rnd_stream);
    ckks_cipher_writer(ci_ax, ci_bx, ci_ax_degree, ci_bx_degree, ci_ax_out, ci_bx_out, ax_degree_out, bx_degree_out);

    Ciphertext cipher1 = encrypt(messageVec1, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);
    Ciphertext cipher2 = encrypt(messageVec2, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);

    if(ax_degree_out[0] != cipher1.ax.degree){
        std::cout<<"\n[ERROR] encryption failed in chipher ax degree";
        error = true;
    }else if(bx_degree_out[0] != cipher1.bx.degree){
        std::cout<<"\n[ERROR] encryption failed in chipher bx degree";
        error = true;
    }
    
    for(i = 0; i<ax_degree_out[0];i++){
        if (ci_ax_out[i] != cipher1.ax.coeff[i])
            std::cout<<"\n[ERROR] CHIPHER ax polynimial coeffs: " << i <<" encryption failed. Kernel ax: "<<ci_ax_out[i] << " real:"<< cipher1.ax.coeff[i];
    }

    for(i = 0; i<bx_degree_out[0];i++){
        if (ci_bx_out[i] != cipher1.bx.coeff[i])
            std::cout<<"\n[ERROR] CHIPHER bx polynimial coeffs: " << i <<" encryption failed. Kernel bx: "<<ci_bx_out[i] << " real:"<< cipher1.bx.coeff[i];
    }

    if(error){
        std::cout<<"\n[ERROR] CKKS_ENCRYPTION kernel failed"<<std::endl;
        return 0;
    }

    std::cout << "Done" << std::endl;

//  TEST ADD KERNEL----------------------------------------------------------------------------

    std::cout << "[EXEC] 7. Testing CKKS_ADD kernel... ";

    hls::stream<ap_int<MAX_Z_SIZE>> ci_ax1("ci_ax1_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> ci_bx1("ci_bx1_stream");
    hls::stream<long> ci_ax1_degree("ci_ax1_degree_stream");
    hls::stream<long> ci_bx1_degree("ci_bx1_degree_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> ci_ax2("ci_ax2_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> ci_bx2("ci_bx2_stream");
    hls::stream<long> ci_ax2_degree("ci_ax2_degree_stream");
    hls::stream<long> ci_bx2_degree("ci_bx2_degree_stream");

    hls::stream<ap_int<MAX_Z_SIZE>> ci_ax_out_stream("ci_ax_out_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> ci_bx_out_stream("ci_bx_out_stream");
    hls::stream<long> ax_degree_out_stream("ax_degree_out_stream");
    hls::stream<long> bx_degree_out_stream("bx_degree_out_stream");

    ap_int<MAX_Z_SIZE> ci_ax_out2[MAX_ZZX_DEGREE];
    long  ax_degree_out2[1];
    ap_int<MAX_Z_SIZE> ci_bx_out2[MAX_ZZX_DEGREE];
    long  bx_degree_out2[1];

    
    Ciphertext addi = add(cipher1, cipher2, context);


    ckks_add_reader(cipher1.ax.coeff, cipher1.ax.degree, cipher1.bx.coeff, cipher1.bx.degree, cipher2.ax.coeff, cipher2.ax.degree, cipher2.bx.coeff, cipher2.bx.degree, ci_ax1, ci_bx1, ci_ax1_degree, ci_bx1_degree, ci_ax2, ci_bx2, ci_ax2_degree, ci_bx2_degree);
    
    ckks_add(ci_ax1, ci_bx1, ci_ax1_degree, ci_bx1_degree, ci_ax2, ci_bx2, ci_ax2_degree, ci_bx2_degree, ci_ax_out_stream, ci_bx_out_stream, ax_degree_out_stream, bx_degree_out_stream);
    
    ckks_cipher_writer(ci_ax_out_stream, ci_bx_out_stream, ax_degree_out_stream, bx_degree_out_stream, ci_ax_out2, ci_bx_out2, ax_degree_out2, bx_degree_out2);

    if(ax_degree_out2[0] != addi.ax.degree){
        std::cout<<"\n[ERROR] add failed in chipher ax degree";
        error = true;
    }else if(bx_degree_out2[0] != addi.bx.degree){
        std::cout<<"\n[ERROR] add failed in chipher bx degree";
        error = true;
    }

    for(i = 0; i<ax_degree_out2[0];i++){
        if (ci_ax_out2[i] != addi.ax.coeff[i])
            std::cout<<"\n[ERROR] CHIPHER ax polynimial coeffs: " << i <<" add failed. Kernel ax: "<<ci_ax_out2[i] << " real:"<< addi.ax.coeff[i];
    }

    for(i = 0; i<bx_degree_out2[0];i++){
        if (ci_bx_out2[i] != addi.bx.coeff[i])
            std::cout<<"\n[ERROR] CHIPHER bx polynimial coeffs: " << i <<" add failed. Kernel bx: "<<ci_bx_out2[i] << " real:"<< addi.bx.coeff[i];
    }

    if(error){
        std::cout<<"\n[ERROR] CKKS_ADD kernel failed"<<std::endl;
        return 0;
    }

    std::cout << "Done" << std::endl;

//  TEST DECRYPT KERNEL----------------------------------------------------------------------------

    std::cout << "[EXEC] 8. Testing CKKS_DECRYPTION kernel... ";

    hls::stream<ap_int<MAX_Z_SIZE>> dec_ci_ax_stream("dec_ci_ax_stream");
    hls::stream<ap_int<MAX_Z_SIZE>> dec_ci_bx_stream("dec_ci_bx_stream");
    hls::stream<long> dec_ci_ax_degree_stream("dec_ci_ax_degree_stream");
    hls::stream<long> dec_ci_bx_degree_stream("dec_ci_bx_degree_stream");

    hls::stream<std::complex<double>> dec_vals_out("dec_vals_out");
    hls::stream<ap_int<MAX_Z_SIZE>> dec_mx_out("dec_mx_out");

    std::complex<double> decryptedVec1_sw[MAX_SLOTS];
    std::complex<double> decryptedVec1_hw[MAX_SLOTS];

    std::complex<double> decryptedVec2_sw[MAX_SLOTS];
    std::complex<double> decryptedVec2_hw[MAX_SLOTS];

    std::complex<double> decryptedVec3_sw[MAX_SLOTS];
    std::complex<double> decryptedVec3_hw[MAX_SLOTS];

    ckks_decryption_reader(cipher1.ax.coeff, cipher1.ax.degree, cipher1.bx.coeff, cipher1.bx.degree, dec_ci_ax_stream, dec_ci_bx_stream, dec_ci_ax_degree_stream, dec_ci_bx_degree_stream);
    ckks_decrypt(secretKey.sx.coeff, secretKey.sx.degree, dec_ci_ax_stream, dec_ci_ax_degree_stream, dec_ci_bx_stream, dec_ci_bx_degree_stream, dec_mx_out);
    ckks_decode(dec_vals_out, slots, dec_mx_out);
    ckks_plaintext_writer(dec_vals_out, decryptedVec1_hw);


    
    

    decrypt(secretKey, cipher1, context,decryptedVec1_sw);





    for (i =0; i<slots; i++){
        if(decryptedVec1_sw[i] != decryptedVec1_hw[i]){
            std::cout<<"\n[ERROR] DECRYPTION DIFFERS IN MESSAGE 1: "<<decryptedVec1_sw[i] << " != " << decryptedVec1_hw[i];
            error = true;
        }
    }

    if(error){
        std::cout<<"\n[ERROR] CKKS_DECRYPTION kernel failed"<<std::endl;
        return 0;
    }

    std::cout << "Done" << std::endl;

    

    for (i =0; i<slots; i++){
        if(decryptedVec1_sw[i] != messageVec1[i]){
            std::cout<<"[WARNING] Noise detected during decryption in Message 1: "<<decryptedVec1_sw[i] << " != " << messageVec1[i] << "~~" << decryptedVec1_sw[i] - messageVec1[i]<<std::endl;
        }
    }

    

    decrypt(secretKey, cipher2, context,decryptedVec2_sw);

    for (i =0; i<slots; i++){
        if(decryptedVec2_sw[i] != messageVec2[i]){
            std::cout<<"[WARNING] Noise detected during decryption in Message 2: "<<decryptedVec2_sw[i] << " != " << messageVec2[i] << "~~" << decryptedVec2_sw[i] - messageVec2[i]<<std::endl;
        }
    }

    

    decrypt(secretKey, addi , context,decryptedVec3_sw);

    for (i =0; i<slots; i++){
        if(decryptedVec3_sw[i] != messageVec1[i] + messageVec2[i]){
            std::cout<<"[WARNING] Noise detected during decryption in add Message: "<<decryptedVec3_sw[i] << " != " << messageVec1[i] + messageVec2[i] << "~~" << decryptedVec3_sw[i] - (messageVec1[i] + messageVec2[i])<<std::endl;
        }
    }

    std::cout << "[INFO] All tests passed successfully" << std::endl;


  
    

    return 0;





}




