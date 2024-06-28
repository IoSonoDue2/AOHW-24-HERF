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
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include "experimental/xrt_kernel.h"
#include "experimental/xrt_uuid.h"
#include <random>
#include <ctime>
#include <chrono>


#include "../source/Structures.cpp"
#include "../source/ZStructures.cpp"
#include "../source/Scheme.cpp"
#include "../source/Utils.cpp"
#include "../source/writer.cpp"

// For hw emulation, run in sw directory: source ./setup_emu.sh -s on

#define DEVICE_ID 0



#define arg_ckks_plaintext_reader_in_vals 0
#define arg_ckks_plaintext_reader_slots 1
#define arg_ckks_plaintext_reader_enc_ax 3
#define arg_ckks_plaintext_reader_ax_degree 4
#define arg_ckks_plaintext_reader_enc_bx 5
#define arg_ckks_plaintext_reader_bx_degree 6
#define arg_ckks_plaintext_reader_rnd 11

#define arg_ckks_add_reader_ci_ax_1 0
#define arg_ckks_add_reader_ci_ax_1_degree 1
#define arg_ckks_add_reader_ci_bx_1 2
#define arg_ckks_add_reader_ci_bx_1_degree 3
#define arg_ckks_add_reader_ci_ax_2 4
#define arg_ckks_add_reader_ci_ax_2_degree 5
#define arg_ckks_add_reader_ci_bx_2 6
#define arg_ckks_add_reader_ci_bx_2_degree 7

#define arg_ckks_decryption_reader_ci_ax 0
#define arg_ckks_decryption_reader_ci_ax_degree 1
#define arg_ckks_decryption_reader_ci_bx 2
#define arg_ckks_decryption_reader_ci_bx_degree 3

#define arg_ckks_encode_slots 1

#define arg_ckks_encrypt_slots 5

#define arg_ckks_decrypt_sx 0
#define arg_ckks_decrypt_sx_degree 1

#define arg_ckks_decode_slots 1

#define arg_ckks_cipher_writer_ci_ax_out 4
#define arg_ckks_cipher_writer_ci_bx_out 5
#define arg_ckks_cipher_writer_ci_ax_degree_out 6
#define arg_ckks_cipher_writer_ci_bx_degree_out 7

#define arg_ckks_plaintext_writer_vals_out 1


bool get_xclbin_path(std::string& xclbin_file,int kernel);
std::ostream& bold_on(std::ostream& os);
std::ostream& bold_off(std::ostream& os);


double round_to_15_decimal_places(double value) {
    return std::round(value * 1e15) / 1e15;
}


std::complex<double> round_complex_to_15_decimal_places(std::complex<double> value) {
    return std::complex<double>(
        round_to_15_decimal_places(value.real()),
        round_to_15_decimal_places(value.imag())
    );
}


void test_ckks_encryption(std::complex<double> messageVec[], int slots, Scheme scheme, Context context, long rnd[RND_SIZE]){

    std::string xclbin_file;
    if (!get_xclbin_path(xclbin_file,1))
        return;

    // Load xclbin
    std::cout << "[INFO] 6. Loading bitstream (" << xclbin_file << ")... ";
    xrt::device device = xrt::device(DEVICE_ID);
    xrt::uuid xclbin_uuid = device.load_xclbin(xclbin_file);
    std::cout << "Done" << std::endl;

    std::cout << "[INFO] 7. Creating kernel objects... ";

    xrt::kernel krnl_ckks_plaintext_reader = xrt::kernel(device, xclbin_uuid, "ckks_plaintext_reader");
    xrt::kernel krnl_ckks_encode = xrt::kernel(device, xclbin_uuid, "ckks_encode");
    xrt::kernel krnl_ckks_encrypt = xrt::kernel(device, xclbin_uuid, "ckks_encrypt");
    xrt::kernel krnl_ckks_cipher_writer = xrt::kernel(device, xclbin_uuid, "ckks_cipher_writer");


    std::cout << "Done" << std::endl;

    std::cout << "[INFO] 8. Getting memory bank groups... ";
 

    xrtMemoryGroup bank_ckks_plaintext_reader_in_vals = krnl_ckks_plaintext_reader.group_id(arg_ckks_plaintext_reader_in_vals);
    xrtMemoryGroup bank_ckks_plaintext_reader_enc_ax = krnl_ckks_plaintext_reader.group_id(arg_ckks_plaintext_reader_enc_ax);
    xrtMemoryGroup bank_ckks_plaintext_reader_enc_bx = krnl_ckks_plaintext_reader.group_id(arg_ckks_plaintext_reader_enc_bx);
    xrtMemoryGroup bank_ckks_plaintext_reader_rnd = krnl_ckks_plaintext_reader.group_id(arg_ckks_plaintext_reader_rnd);


    xrtMemoryGroup bank_ckks_cipher_writer_ci_ax_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_ax_out);
    xrtMemoryGroup bank_ckks_cipher_writer_ci_bx_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_bx_out);
    xrtMemoryGroup bank_ckks_cipher_writer_ci_ax_degree_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_ax_degree_out);
    xrtMemoryGroup bank_ckks_cipher_writer_ci_bx_degree_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_bx_degree_out);

    std::cout << "Done" << std::endl;

    std::cout<<"[INFO] 9. Creating device buffers... ";

    xrt::bo buffer_ckks_plaintext_reader_in_vals = xrt::bo(device, slots * sizeof(std::complex<double>), xrt::bo::flags::normal, bank_ckks_plaintext_reader_in_vals); //8*16 = 0.125KB = 0.125KB
    xrt::bo buffer_ckks_plaintext_reader_enc_ax = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_plaintext_reader_enc_ax);
    xrt::bo buffer_ckks_plaintext_reader_enc_bx = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_plaintext_reader_enc_bx);
    xrt::bo buffer_ckks_plaintext_reader_rnd = xrt::bo(device, RND_SIZE * sizeof(long), xrt::bo::flags::normal, bank_ckks_plaintext_reader_rnd);
    
    xrt::bo buffer_ckks_cipher_writer_ci_ax_out = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_ax_out);
    xrt::bo buffer_ckks_cipher_writer_ci_bx_out = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_bx_out);
    xrt::bo buffer_ckks_cipher_writer_ci_ax_degree_out = xrt::bo(device, sizeof(long), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_ax_degree_out);
    xrt::bo buffer_ckks_cipher_writer_ci_bx_degree_out = xrt::bo(device, sizeof(long), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_bx_degree_out);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 10. Creating runner instances... ";

    xrt::run run_ckks_plaintext_reader = xrt::run(krnl_ckks_plaintext_reader);
    xrt::run run_ckks_encode = xrt::run(krnl_ckks_encode);
    xrt::run run_ckks_encrypt = xrt::run(krnl_ckks_encrypt);
    xrt::run run_ckks_cipher_writer = xrt::run(krnl_ckks_cipher_writer);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 11. Setting kernel arguments... ";

    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_in_vals, buffer_ckks_plaintext_reader_in_vals);
    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_slots, slots);
    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_enc_ax, buffer_ckks_plaintext_reader_enc_ax);
    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_ax_degree, scheme.EncKey.ax.degree);
    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_enc_bx, buffer_ckks_plaintext_reader_enc_bx);
    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_bx_degree, scheme.EncKey.bx.degree);
    run_ckks_plaintext_reader.set_arg(arg_ckks_plaintext_reader_rnd, buffer_ckks_plaintext_reader_rnd);

    run_ckks_encode.set_arg(arg_ckks_encode_slots, slots);

    run_ckks_encrypt.set_arg(arg_ckks_encrypt_slots, slots);

    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_ax_out, buffer_ckks_cipher_writer_ci_ax_out);
    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_bx_out, buffer_ckks_cipher_writer_ci_bx_out);
    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_ax_degree_out, buffer_ckks_cipher_writer_ci_ax_degree_out);
    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_bx_degree_out, buffer_ckks_cipher_writer_ci_bx_degree_out);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 12. Writing data into the input buffers... ";

    auto write_to_buffer_start = std::chrono::high_resolution_clock::now();

    buffer_ckks_plaintext_reader_in_vals.write(messageVec);
    buffer_ckks_plaintext_reader_in_vals.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_plaintext_reader_enc_ax.write(scheme.EncKey.ax.coeff);
    buffer_ckks_plaintext_reader_enc_ax.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_plaintext_reader_enc_bx.write(scheme.EncKey.bx.coeff);
    buffer_ckks_plaintext_reader_enc_bx.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_plaintext_reader_rnd.write(rnd);
    buffer_ckks_plaintext_reader_rnd.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    auto write_to_buffer = std::chrono::high_resolution_clock::now() - write_to_buffer_start;

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 13. Running the kernel... ";
    //measure the exectuion time
    auto hw_time_start = std::chrono::high_resolution_clock::now();
    run_ckks_plaintext_reader.start();

    run_ckks_encode.start();

    run_ckks_encrypt.start();

    run_ckks_cipher_writer.start();

    std::cout<<"Done" << std::endl;

    run_ckks_cipher_writer.wait();
    run_ckks_encrypt.wait();
    run_ckks_encode.wait();
    run_ckks_plaintext_reader.wait();

    auto hw_duration = std::chrono::high_resolution_clock::now() - hw_time_start;

    buffer_ckks_cipher_writer_ci_ax_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    buffer_ckks_cipher_writer_ci_bx_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    buffer_ckks_cipher_writer_ci_ax_degree_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    buffer_ckks_cipher_writer_ci_bx_degree_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    ap_int<MAX_Z_SIZE> ci_ax_out[MAX_ZZX_DEGREE];
    ap_int<MAX_Z_SIZE> ci_bx_out[MAX_ZZX_DEGREE];
    long ci_ax_degree_out[1];
    long ci_bx_degree_out[1];

    auto read_from_buffer_start = std::chrono::high_resolution_clock::now();

    buffer_ckks_cipher_writer_ci_ax_out.read(ci_ax_out);
    buffer_ckks_cipher_writer_ci_bx_out.read(ci_bx_out);
    buffer_ckks_cipher_writer_ci_ax_degree_out.read(ci_ax_degree_out);
    buffer_ckks_cipher_writer_ci_bx_degree_out.read(ci_bx_degree_out);
    auto read_from_buffer = std::chrono::high_resolution_clock::now() - read_from_buffer_start;
    auto sw_time_start = std::chrono::high_resolution_clock::now();
    Ciphertext cipher1 = encrypt(messageVec, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);
    auto sw_duration = std::chrono::high_resolution_clock::now() - sw_time_start;
    bool error = false;
    
    if(ci_ax_degree_out[0] != cipher1.ax.degree){
        std::cout<<"\n[ERROR] encryption failed in chipher ax degree";
        error = true;
    }else if(ci_bx_degree_out[0] != cipher1.bx.degree){
        std::cout<<"\n[ERROR] encryption failed in chipher bx degree";
        error = true;
    }
    
    for(int i = 0; i<ci_ax_degree_out[0];i++){
        if (ci_ax_out[i] != cipher1.ax.coeff[i]){
            std::cout<<"\n[ERROR] CHIPHER ax polynimial coeffs: " << i <<" encryption failed. Kernel ax: "<<ci_ax_out[i] << " real:"<< cipher1.ax.coeff[i];
            error = true;
        }
    }

    for(int i = 0; i<ci_bx_degree_out[0];i++){
        if (ci_bx_out[i] != cipher1.bx.coeff[i]){
            std::cout<<"\n[ERROR] CHIPHER bx polynimial coeffs: " << i <<" encryption failed. Kernel bx: "<<ci_bx_out[i] << " real:"<< cipher1.bx.coeff[i];
            error = true;
        }
    }

    if(error){
        std::cout<<"\n[ERROR] CKKS_ENCRYPTION kernel failed"<<std::endl;
        return;
    }

    std::cout << "[INFO] All tests passed successfully" << std::endl;

    std::cout << "[INFO] Write to buffer time: " << std::chrono::duration_cast<std::chrono::microseconds>(write_to_buffer).count() << " microse" << std::endl;
    std::cout << "[INFO] HW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(hw_duration).count() << " microseconds" << std::endl;
    std::cout << "[INFO] Read from buffer time: " << std::chrono::duration_cast<std::chrono::microseconds>(read_from_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] Total HW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(hw_duration + read_from_buffer + write_to_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] SW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(sw_duration).count() << " microseconds" << std::endl;
    std::cout << "[INFO] HARDWARE SPEEDUP: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(sw_duration).count() / std::chrono::duration_cast<std::chrono::microseconds>(hw_duration).count() << "x"<< std::endl;
    


}

void test_ckks_add(std::complex<double> messageVec1[], std::complex<double> messageVec2[], int slots, Scheme scheme, Context context, long rnd[RND_SIZE]){

    std::string xclbin_file;
    if (!get_xclbin_path(xclbin_file,2))
        return ;

    // Load xclbin
    std::cout << "[INFO] 6. Loading bitstream (" << xclbin_file << ")... ";
    xrt::device device = xrt::device(DEVICE_ID);
    xrt::uuid xclbin_uuid = device.load_xclbin(xclbin_file);
    std::cout << "Done" << std::endl;

    std::cout << "[INFO] 7. Creating kernel objects... ";


    xrt::kernel krnl_ckks_add_reader = xrt::kernel(device, xclbin_uuid, "ckks_add_reader");
    xrt::kernel krnl_ckks_add = xrt::kernel(device, xclbin_uuid, "ckks_add");
    xrt::kernel krnl_ckks_cipher_writer = xrt::kernel(device, xclbin_uuid, "ckks_cipher_writer");


    std::cout << "Done" << std::endl;

    std::cout << "[INFO] 8. Getting memory bank groups... ";

    xrtMemoryGroup bank_ckks_add_reader_ci_ax_1 = krnl_ckks_add_reader.group_id(arg_ckks_add_reader_ci_ax_1);
    xrtMemoryGroup bank_ckks_add_reader_ci_bx_1 = krnl_ckks_add_reader.group_id(arg_ckks_add_reader_ci_bx_1);
    xrtMemoryGroup bank_ckks_add_reader_ci_ax_2 = krnl_ckks_add_reader.group_id(arg_ckks_add_reader_ci_ax_2);
    xrtMemoryGroup bank_ckks_add_reader_ci_bx_2 = krnl_ckks_add_reader.group_id(arg_ckks_add_reader_ci_bx_2);

    xrtMemoryGroup bank_ckks_cipher_writer_ci_ax_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_ax_out);
    xrtMemoryGroup bank_ckks_cipher_writer_ci_bx_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_bx_out);
    xrtMemoryGroup bank_ckks_cipher_writer_ci_ax_degree_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_ax_degree_out);
    xrtMemoryGroup bank_ckks_cipher_writer_ci_bx_degree_out = krnl_ckks_cipher_writer.group_id(arg_ckks_cipher_writer_ci_bx_degree_out);

    std::cout << "Done" << std::endl;

    std::cout<<"[INFO] 9. Creating device buffers... ";

    xrt::bo buffer_ckks_add_reader_ci_ax_1 = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_add_reader_ci_ax_1);
    xrt::bo buffer_ckks_add_reader_ci_bx_1 = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_add_reader_ci_bx_1);
    xrt::bo buffer_ckks_add_reader_ci_ax_2 = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_add_reader_ci_ax_2);
    xrt::bo buffer_ckks_add_reader_ci_bx_2 = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_add_reader_ci_bx_2);

    xrt::bo buffer_ckks_cipher_writer_ci_ax_out = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_ax_out);
    xrt::bo buffer_ckks_cipher_writer_ci_bx_out = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_bx_out);
    xrt::bo buffer_ckks_cipher_writer_ci_ax_degree_out = xrt::bo(device, sizeof(long), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_ax_degree_out);
    xrt::bo buffer_ckks_cipher_writer_ci_bx_degree_out = xrt::bo(device, sizeof(long), xrt::bo::flags::normal, bank_ckks_cipher_writer_ci_bx_degree_out);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 10. Creating runner instances... ";

    xrt::run run_ckks_add_reader = xrt::run(krnl_ckks_add_reader);
    xrt::run run_ckks_add = xrt::run(krnl_ckks_add);
    xrt::run run_ckks_cipher_writer = xrt::run(krnl_ckks_cipher_writer);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 11. Setting kernel arguments... ";

    Ciphertext cipher1 = encrypt(messageVec1, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);
    Ciphertext cipher2 = encrypt(messageVec2, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);

    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_ax_1, buffer_ckks_add_reader_ci_ax_1);
    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_ax_1_degree, cipher1.ax.degree);
    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_bx_1, buffer_ckks_add_reader_ci_bx_1);
    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_bx_1_degree, cipher1.bx.degree);

    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_ax_2, buffer_ckks_add_reader_ci_ax_2);
    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_ax_2_degree, cipher2.ax.degree);
    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_bx_2, buffer_ckks_add_reader_ci_bx_2);
    run_ckks_add_reader.set_arg(arg_ckks_add_reader_ci_bx_2_degree, cipher2.bx.degree);

    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_ax_out, buffer_ckks_cipher_writer_ci_ax_out);
    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_bx_out, buffer_ckks_cipher_writer_ci_bx_out);
    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_ax_degree_out, buffer_ckks_cipher_writer_ci_ax_degree_out);
    run_ckks_cipher_writer.set_arg(arg_ckks_cipher_writer_ci_bx_degree_out, buffer_ckks_cipher_writer_ci_bx_degree_out);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 12. Writing data into the input buffers... ";

    auto write_to_buffer_start = std::chrono::high_resolution_clock::now();

    buffer_ckks_add_reader_ci_ax_1.write(cipher1.ax.coeff);
    buffer_ckks_add_reader_ci_ax_1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_add_reader_ci_bx_1.write(cipher1.bx.coeff);
    buffer_ckks_add_reader_ci_bx_1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_add_reader_ci_ax_2.write(cipher2.ax.coeff);
    buffer_ckks_add_reader_ci_ax_2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_add_reader_ci_bx_2.write(cipher2.bx.coeff);
    buffer_ckks_add_reader_ci_bx_2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    auto write_to_buffer = std::chrono::high_resolution_clock::now() - write_to_buffer_start;

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 13. Running the kernel... ";

    auto hw_time_start = std::chrono::high_resolution_clock::now();

    run_ckks_add_reader.start();
    run_ckks_add.start();
    run_ckks_cipher_writer.start();

    std::cout<<"Done" << std::endl;

    run_ckks_cipher_writer.wait();
    run_ckks_add.wait();
    run_ckks_add_reader.wait();

    auto hw_time = std::chrono::high_resolution_clock::now() - hw_time_start;

    buffer_ckks_cipher_writer_ci_ax_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    buffer_ckks_cipher_writer_ci_bx_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    buffer_ckks_cipher_writer_ci_ax_degree_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    buffer_ckks_cipher_writer_ci_bx_degree_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    ap_int<MAX_Z_SIZE> ci_ax_out[MAX_ZZX_DEGREE];
    ap_int<MAX_Z_SIZE> ci_bx_out[MAX_ZZX_DEGREE];
    long ci_ax_degree_out[1];
    long ci_bx_degree_out[1];

    auto read_from_buffer_start = std::chrono::high_resolution_clock::now();

    buffer_ckks_cipher_writer_ci_ax_out.read(ci_ax_out);
    buffer_ckks_cipher_writer_ci_bx_out.read(ci_bx_out);
    buffer_ckks_cipher_writer_ci_ax_degree_out.read(ci_ax_degree_out);
    buffer_ckks_cipher_writer_ci_bx_degree_out.read(ci_bx_degree_out);

    auto read_from_buffer = std::chrono::high_resolution_clock::now() - read_from_buffer_start;

    auto sw_time_start = std::chrono::high_resolution_clock::now();
    Ciphertext cipher3 = add(cipher1, cipher2, context);

    auto sw_time = std::chrono::high_resolution_clock::now() - sw_time_start;

    std::cout<<"[INFO] 14. Checking results... ";

    bool error = false;

    if(ci_ax_degree_out[0] != cipher3.ax.degree){
        std::cout<<"\n[ERROR] add failed in chipher ax degree";
        error = true;
    }else if(ci_bx_degree_out[0] != cipher3.bx.degree){
        std::cout<<"\n[ERROR] add failed in chipher bx degree";
        error = true;
    }

    for(int i = 0; i<ci_ax_degree_out[0];i++){
        if (ci_ax_out[i] != cipher3.ax.coeff[i]){
            std::cout<<"\n[ERROR] CHIPHER ax polynimial coeffs: " << i <<" add failed. Kernel ax: "<<ci_ax_out[i] << " real:"<< cipher3.ax.coeff[i];
            error = true;
        }
    }

    for(int i = 0; i<ci_bx_degree_out[0];i++){
        if (ci_bx_out[i] != cipher3.bx.coeff[i]){
            std::cout<<"\n[ERROR] CHIPHER bx polynimial coeffs: " << i <<" add failed. Kernel bx: "<<ci_bx_out[i] << " real:"<< cipher3.bx.coeff[i];
            error = true;
        }
    }

    if(error){
        std::cout<<"\n[ERROR] CKKS_ADD kernel failed"<<std::endl;
        return;
    }

    std::cout<<"Done" << std::endl;

    std::cout << "[INFO] All tests passed successfully" << std::endl;

    std::cout << "[INFO] Write to buffer time: " << std::chrono::duration_cast<std::chrono::microseconds>(write_to_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] HW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(hw_time).count() << " microseconds" << std::endl;
    std::cout << "[INFO] Read from buffer time: " << std::chrono::duration_cast<std::chrono::microseconds>(read_from_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] Total HW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(hw_time + write_to_buffer + read_from_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] SW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(sw_time).count() << " microseconds" << std::endl;
    std::cout << "[INFO] HARDWARE SPEEDUP: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(sw_time).count() / std::chrono::duration_cast<std::chrono::microseconds>(hw_time).count() << "x" << std::endl;



}

void test_ckks_decryption(std::complex<double> messageVec[], int slots, Scheme scheme, Context context,SecretKey secretKey, long rnd[RND_SIZE]){

    std::string xclbin_file;
    if (!get_xclbin_path(xclbin_file,3))
        return;

    // Load xclbin
    std::cout << "[INFO] 6. Loading bitstream (" << xclbin_file << ")... ";
    xrt::device device = xrt::device(DEVICE_ID);
    xrt::uuid xclbin_uuid = device.load_xclbin(xclbin_file);
    std::cout << "Done" << std::endl;

    std::cout << "[INFO] 7. Creating kernel objects... ";

    xrt::kernel krnl_ckks_decryption_reader = xrt::kernel(device, xclbin_uuid, "ckks_decryption_reader");
    xrt::kernel krnl_ckks_decrypt = xrt::kernel(device, xclbin_uuid, "ckks_decrypt");
    xrt::kernel krnl_ckks_decode = xrt::kernel(device, xclbin_uuid, "ckks_decode");
    xrt::kernel krnl_ckks_plaintext_writer = xrt::kernel(device, xclbin_uuid, "ckks_plaintext_writer");

    std::cout << "Done" << std::endl;

    std::cout << "[INFO] 8. Getting memory bank groups... ";

    xrtMemoryGroup bank_ckks_decryption_reader_ci_ax = krnl_ckks_decryption_reader.group_id(arg_ckks_decryption_reader_ci_ax);
    xrtMemoryGroup bank_ckks_decryption_reader_ci_bx = krnl_ckks_decryption_reader.group_id(arg_ckks_decryption_reader_ci_bx);

    xrtMemoryGroup bank_ckks_decrypt_sx = krnl_ckks_decrypt.group_id(arg_ckks_decrypt_sx);

    xrtMemoryGroup bank_ckks_plaintext_writer_vals_out = krnl_ckks_plaintext_writer.group_id(arg_ckks_plaintext_writer_vals_out);

    std::cout << "Done" << std::endl;

    std::cout<<"[INFO] 9. Creating device buffers... ";

    xrt::bo buffer_ckks_decryption_reader_ci_ax = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_decryption_reader_ci_ax);
    xrt::bo buffer_ckks_decryption_reader_ci_bx = xrt::bo(device, MAX_ZZX_DEGREE * sizeof(ap_int<MAX_Z_SIZE>), xrt::bo::flags::normal, bank_ckks_decryption_reader_ci_bx);

    xrt::bo buffer_ckks_decrypt_sx = xrt::bo(device, slots * sizeof(std::complex<double>), xrt::bo::flags::normal, bank_ckks_decrypt_sx);

    xrt::bo buffer_ckks_plaintext_writer_vals_out = xrt::bo(device, slots * sizeof(std::complex<double>), xrt::bo::flags::normal, bank_ckks_plaintext_writer_vals_out);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 10. Creating runner instances... ";

    xrt::run run_ckks_decryption_reader = xrt::run(krnl_ckks_decryption_reader);
    xrt::run run_ckks_decrypt = xrt::run(krnl_ckks_decrypt);
    xrt::run run_ckks_decode = xrt::run(krnl_ckks_decode);
    xrt::run run_ckks_plaintext_writer = xrt::run(krnl_ckks_plaintext_writer);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 11. Setting kernel arguments... ";

    Ciphertext cipher = encrypt(messageVec, slots, scheme.EncKey.ax, scheme.EncKey.bx, context, rnd);

    run_ckks_decryption_reader.set_arg(arg_ckks_decryption_reader_ci_ax, buffer_ckks_decryption_reader_ci_ax);
    run_ckks_decryption_reader.set_arg(arg_ckks_decryption_reader_ci_ax_degree, cipher.ax.degree);
    run_ckks_decryption_reader.set_arg(arg_ckks_decryption_reader_ci_bx, buffer_ckks_decryption_reader_ci_bx);
    run_ckks_decryption_reader.set_arg(arg_ckks_decryption_reader_ci_bx_degree, cipher.bx.degree);

    run_ckks_decrypt.set_arg(arg_ckks_decrypt_sx, buffer_ckks_decrypt_sx);
    run_ckks_decrypt.set_arg(arg_ckks_decrypt_sx_degree, secretKey.sx.degree);

    

    run_ckks_decode.set_arg(arg_ckks_decode_slots, slots);

    run_ckks_plaintext_writer.set_arg(arg_ckks_plaintext_writer_vals_out, buffer_ckks_plaintext_writer_vals_out);

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 12. Writing data into the input buffers... ";

    auto write_to_buffer_start = std::chrono::high_resolution_clock::now();

    buffer_ckks_decryption_reader_ci_ax.write(cipher.ax.coeff);
    buffer_ckks_decryption_reader_ci_ax.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_decryption_reader_ci_bx.write(cipher.bx.coeff);
    buffer_ckks_decryption_reader_ci_bx.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    buffer_ckks_decrypt_sx.write(secretKey.sx.coeff);
    buffer_ckks_decrypt_sx.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    auto write_to_buffer = std::chrono::high_resolution_clock::now() - write_to_buffer_start;

    std::cout<<"Done" << std::endl;

    std::cout<<"[INFO] 13. Running the kernel... ";

    auto hw_time_start = std::chrono::high_resolution_clock::now();

    run_ckks_decryption_reader.start();
    run_ckks_decrypt.start();
    run_ckks_decode.start();
    run_ckks_plaintext_writer.start();

    std::cout<<"Done" << std::endl;

    run_ckks_plaintext_writer.wait();
    run_ckks_decode.wait();
    run_ckks_decrypt.wait();
    run_ckks_decryption_reader.wait();

    auto hw_time = std::chrono::high_resolution_clock::now() - hw_time_start;



    buffer_ckks_plaintext_writer_vals_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::complex<double> messageVecOut[slots];

    auto read_from_buffer_start = std::chrono::high_resolution_clock::now();

    buffer_ckks_plaintext_writer_vals_out.read(messageVecOut);

    auto read_from_buffer = std::chrono::high_resolution_clock::now() - read_from_buffer_start;

    std::cout<<"[INFO] 14. Checking results... ";

    bool error = false;

    std::complex<double> decryptedVec1_sw[MAX_SLOTS];
    auto sw_time_start = std::chrono::high_resolution_clock::now();
    decrypt(secretKey, cipher, context,decryptedVec1_sw);
    auto sw_time = std::chrono::high_resolution_clock::now() - sw_time_start;

    for (int i = 0; i < slots; i++) {
        auto rounded_messageVecOut = round_complex_to_15_decimal_places(messageVecOut[i]);
        auto rounded_decryptedVec1_sw = round_complex_to_15_decimal_places(decryptedVec1_sw[i]);

        if (rounded_messageVecOut != rounded_decryptedVec1_sw) {
            std::cout << "\n[ERROR] Decryption failed. Kernel: " << rounded_messageVecOut
                      << " real: " << rounded_decryptedVec1_sw;
            error = true;
        }
    }

    if(error){
        std::cout<<"\n[ERROR] CKKS_DECRYPTION kernel failed"<<std::endl;
        return;
    }

    std::cout<<"Done" << std::endl;

    std::cout << "[INFO] All tests passed successfully" << std::endl;
    std::cout << "[INFO] Write to buffer time: " << std::chrono::duration_cast<std::chrono::microseconds>(write_to_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] HW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(hw_time).count() << " microseconds" << std::endl;
    std::cout << "[INFO] Read from buffer time: " << std::chrono::duration_cast<std::chrono::microseconds>(read_from_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] Total HW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(hw_time).count() + std::chrono::duration_cast<std::chrono::microseconds>(write_to_buffer).count() + std::chrono::duration_cast<std::chrono::microseconds>(read_from_buffer).count() << " microseconds" << std::endl;
    std::cout << "[INFO] SW execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(sw_time).count() << " microseconds" << std::endl;
    std::cout << "[INFO] HARDWARE SPEEDUP: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(sw_time).count() / std::chrono::duration_cast<std::chrono::microseconds>(hw_time).count() << "x" << std::endl;




}

int checkResult(float* input, float* output, int size) {
    for (int i = 0; i < size; i++) {
        if (input[i] != output[i]) {
            std::cout << "Error at index " << i << ": " << input[i] << " != " << output[i] << std::endl;
            return EXIT_FAILURE;
        }
    }
    std::cout << "Test passed!" << std::endl;
    return EXIT_SUCCESS;
}

void generateRandomArray(long array[RND_SIZE]) {


    srand(time(nullptr)); 

    for (int i = 0; i < RND_SIZE; i++) {
        array[i] = static_cast<long>(rand()) << 32 | rand();
    }
}


int main(int argc, char *argv[]) {
    
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

    std::cout << "[INFO] The following kernels can be tested: " << std::endl;
    std::cout << "[INFO] 1- CKKS_ENCRYPTION" << std::endl;
    std::cout << "[INFO] 2- CKKS_ADD" << std::endl;
    std::cout << "[INFO] 3- CKKS_DECRYPTION" << std::endl;
    std::cout << "[INFO] 4- CKKS_SCHEME" << std::endl;
    std::cout << "[INPUT] Please select the kernel to test: ";

    int kernel;
    std::cin >> kernel;

    switch (kernel) {
        case 1:
            std::cout << "[EXEC] 5. Testing CKKS_ENCRYPTION... " << std::endl;
            test_ckks_encryption(messageVec1, slots, scheme, context,rnd);
            break;
        case 2:
            std::cout << "[EXEC] 5. Testing CKKS_ADD... " << std::endl;
            test_ckks_add(messageVec1, messageVec2, slots, scheme, context,rnd);
            break;
        case 3:
            std::cout << "[EXEC] 5. Testing CKKS_DECRYPTION... " << std::endl;
            test_ckks_decryption(messageVec1, slots, scheme, context, secretKey,rnd);
            break;
        default:
            std::cout << "[ERROR] Invalid kernel selected" << std::endl;
            return EXIT_FAILURE;
    }




}


bool get_xclbin_path(std::string& xclbin_file, int kernel) {
    // Judge emulation mode accoring to env variable
    char *env_emu;
    if (env_emu = getenv("XCL_EMULATION_MODE")) {
        std::string mode(env_emu);
        if (mode == "hw_emu")
        {
            std::cout << "Program running in hardware emulation mode" << std::endl;
            switch (kernel) {
                case 1:
                    xclbin_file = "overlay_enc_hw_emu.xclbin";
                    break;
                case 2:
                    xclbin_file = "overlay_add_hw_emu.xclbin";
                    break;
                case 3:
                    xclbin_file = "overlay_dec_hw_emu.xclbin";
                    break;
                default:
                    std::cout << "[ERROR] Invalid kernel selected" << std::endl;
            }
            
        }
        else
        {
            std::cout << "[ERROR] Unsupported Emulation Mode: " << mode << std::endl;
            return false;
        }
    }
    else {
        std::cout << bold_on << "Program running in hardware mode" << bold_off << std::endl;
        
        switch (kernel) {
            case 1:
                xclbin_file = "overlay_enc_hw.xclbin";
                break;
            case 2:
                xclbin_file = "overlay_add_hw.xclbin";
                break;
            case 3:
                xclbin_file = "overlay_dec_hw.xclbin";
                break;
            default:
                std::cout << "[ERROR] Invalid kernel selected" << std::endl;
            }
    }

    std::cout << std::endl << std::endl;
    return true;
}

std::ostream& bold_on(std::ostream& os)
{
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os)
{
    return os << "\e[0m";
}

