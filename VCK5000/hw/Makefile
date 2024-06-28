# MIT License

# Copyright (c) 2024 Valentino Guerrini

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

ECHO=@echo

.PHONY: help

help::
	$(ECHO) ""
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<hw/hw_emu>"
	$(ECHO) "	   Command to generate xclbin files for encryption, addition, decryption and test"
	$(ECHO) ""
	$(ECHO) "  make encryption TARGET=<hw/hw_emu>"
	$(ECHO) "	   Command to generate xclbin file for encryption"
	$(ECHO) ""
	$(ECHO) "  make addition TARGET=<hw/hw_emu>"
	$(ECHO) "	   Command to generate xclbin file for addition"
	$(ECHO) ""
	$(ECHO) "  make decryption TARGET=<hw/hw_emu>"
	$(ECHO) "	   Command to generate xclbin file for decryption"
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

PLATFORM := xilinx_vck5000_gen4x8_qdma_2_202220_1

TARGET := hw

XOCCLFLAGS := --kernel_frequency 200 --platform $(PLATFORM) -t $(TARGET)  -s -g


XOS_ENC     := ../data_movers/ckks_plaintext_reader_$(TARGET).xo
XOS_ENC     += ../data_movers/ckks_encode_$(TARGET).xo
XOS_ENC     += ../data_movers/ckks_encrypt_$(TARGET).xo
XOS_ENC     += ../data_movers/ckks_cipher_writer_$(TARGET).xo

XOS_ADD     := ../data_movers/ckks_add_reader_$(TARGET).xo
XOS_ADD     += ../data_movers/ckks_add_$(TARGET).xo
XOS_ADD     += ../data_movers/ckks_cipher_writer_$(TARGET).xo

XOS_DEC     := ../data_movers/ckks_decryption_reader_$(TARGET).xo
XOS_DEC     += ../data_movers/ckks_decrypt_$(TARGET).xo
XOS_DEC     += ../data_movers/ckks_decode_$(TARGET).xo
XOS_DEC     += ../data_movers/ckks_plaintext_writer_$(TARGET).xo


XSA_OBJ_ENC := overlay_enc_$(TARGET).xsa
XCLBIN_ENC  := overlay_enc_$(TARGET).xclbin

XSA_OBJ_ADD := overlay_add_$(TARGET).xsa
XCLBIN_ADD  := overlay_add_$(TARGET).xclbin

XSA_OBJ_DEC := overlay_dec_$(TARGET).xsa
XCLBIN_DEC  := overlay_dec_$(TARGET).xclbin


.phony: clean

encryption: $(XCLBIN_ENC)

$(XCLBIN_ENC): $(XSA_OBJ_ENC)
	v++ -p -t $(TARGET) -f $(PLATFORM) $^ -o $@ --package.boot_mode=ospi

$(XSA_OBJ_ENC): $(XOS_ENC)
	v++ -l $(XOCCFLAGS) $(XOCCLFLAGS) --config xclbin_overlay_enc.cfg -o $@ $^

addition: $(XCLBIN_ADD)

$(XCLBIN_ADD): $(XSA_OBJ_ADD)
	v++ -p -t $(TARGET) -f $(PLATFORM) $^ -o $@ --package.boot_mode=ospi

$(XSA_OBJ_ADD): $(XOS_ADD)
	v++ -l $(XOCCFLAGS) $(XOCCLFLAGS) --config xclbin_overlay_add.cfg -o $@ $^

decryption: $(XCLBIN_DEC)

$(XCLBIN_DEC): $(XSA_OBJ_DEC)
	v++ -p -t $(TARGET) -f $(PLATFORM) $^ -o $@ --package.boot_mode=ospi

$(XSA_OBJ_DEC): $(XOS_DEC)
	v++ -l $(XOCCFLAGS) $(XOCCLFLAGS) --config xclbin_overlay_dec.cfg -o $@ $^

all: $(XCLBIN_ENC) $(XCLBIN_ADD) $(XCLBIN_DEC)

clean:
	$(RM) -r _x .Xil .ipcache *.ltx *.log *.sh *.jou *.info *.xclbin *.xo.* *.str *.xsa *.cdo.bin *bif *BIN *.package_summary *.link_summary *.txt *.bin && rm -rf cfg emulation_data sim
	
