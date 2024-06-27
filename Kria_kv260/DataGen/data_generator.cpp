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

#include <fstream>
#include <string>
#include <chrono>
#include "Structures.hpp"
#include "ZStructures.hpp"
#include "Scheme.hpp"

void generateARandomArray(long array[RND_SIZE]) {


    srand(time(nullptr)); 

    for (int i = 0; i < RND_SIZE; i++) {
        array[i] = static_cast<long>(rand()) << 32 | rand();
    }
}


template <typename T>
void writeArrayToFile(T array[], int n, const std::string& filename) {

    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile) {
        std::cerr << "Error opening the file " << filename << std::endl;
        return;
    }
    
    for (int i = 0; i < n; ++i) {
        outfile.write(reinterpret_cast<const char*>(&array[i]), sizeof(T));
    }
    
    outfile.close();
    

    if (!outfile) {
        std::cerr << "Error closing the file " << filename << std::endl;
    }
}


void writeValueToFile(long value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile) {
        std::cerr << "Error opening the file " << filename << std::endl;
        return;
    }

    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long));

    outfile.close();

    if (!outfile) {
        std::cerr << "Error closing the file " << filename << std::endl;
    }
}

int main(){

    long rnd[RND_SIZE];

    generateARandomArray(rnd);


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
    std::complex<double> decryptedVec1_sw[MAX_SLOTS];

    //write random to file 

    writeArrayToFile(rnd, RND_SIZE, "data/rnd.data");


    //wirite secret key to fil

    writeArrayToFile(secretKey.sx.coeff, secretKey.sx.degree + 1, "data/secretKey.data");
    writeValueToFile(secretKey.sx.degree, "data/secretKey.degree");
    

    //write enc key to file

    writeArrayToFile(scheme.EncKey.ax.coeff, scheme.EncKey.ax.degree + 1, "data/encKeyAx.data");
    writeValueToFile(scheme.EncKey.ax.degree, "data/encKeyAx.degree");

    writeArrayToFile(scheme.EncKey.bx.coeff, scheme.EncKey.bx.degree + 1, "data/encKeyBx.data");
    writeValueToFile(scheme.EncKey.bx.degree, "data/encKeyBx.degree");

    //write message

    writeArrayToFile(messageVec1, slots, "data/message.data");
    
    auto enc_time_start = std::chrono::high_resolution_clock::now();
    Ciphertext cipher1 = encrypt(messageVec1, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);
    auto enc_duration = std::chrono::high_resolution_clock::now() - enc_time_start;

    std::cout << "[INFO] SW encryption execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(enc_duration).count() << " microseconds" << std::endl;


    //write cipher to file

    writeArrayToFile(cipher1.ax.coeff, cipher1.ax.degree + 1, "data/cipherAx.data");
    writeValueToFile(cipher1.ax.degree, "data/cipherAx.degree");

    writeArrayToFile(cipher1.bx.coeff, cipher1.bx.degree + 1, "data/cipherBx.data");
    writeValueToFile(cipher1.bx.degree, "data/cipherBx.degree");

    auto dec_time_start = std::chrono::high_resolution_clock::now();
    decrypt(secretKey, cipher1, context,decryptedVec1_sw);
    auto dec_duration = std::chrono::high_resolution_clock::now() - dec_time_start;

    std::cout << "[INFO] SW decryption execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(dec_duration).count() << " microseconds" << std::endl;

    //write decrypted to file
    writeArrayToFile(decryptedVec1_sw, slots, "data/decrypted.data");








}