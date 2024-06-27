# MIT License

# Copyright (c) 2024 Valentino Guerrini, Giuseppe Sorrentino

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
#This command will display available makefile options
help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) ""
	$(ECHO) "  make compile"
	$(ECHO) "      Command to generate xo kernel file"
	$(ECHO) ""
	$(ECHO) "  make testbench_ckks"
	$(ECHO) "      Command to compile the testbench for the ckks scheme"
	$(ECHO) ""
	$(ECHO) "  make run_testbench_ckks"
	$(ECHO) "      Command to run the testbench for the ckks scheme"
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make help"
	$(ECHO) "      Command to display this help message."
	$(ECHO) ""


PLATFORM := xilinx_vck5000_gen4x8_qdma_2_202220_1

TARGET := hw

.phony: clean



XOCCFLAGS := --platform $(PLATFORM) -t $(TARGET)  -s -g

data_gen: data_generator.cpp 
	g++ -std=c++14 -I. -I$(XILINX_HLS)/include -o $@ $^ Scheme.cpp Utils.cpp ZStructures.cpp Structures.cpp  


################## clean up
clean:
	$(RM) -rf *.xo *.xclbin *.xclbin.info *.xclbin.link_summary *.jou *.log *.xo.compile_summary _x .Xil
