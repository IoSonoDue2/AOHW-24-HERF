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
TARGET := hw

PLATFORM := xilinx_vck5000_gen4x8_qdma_2_202220_1
help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make build_hw [TARGET=hw_emu]"
	$(ECHO) ""
	$(ECHO) "  make build_sw"
	$(ECHO) ""
	$(ECHO) "  make testbench_all"
	$(ECHO) "    Command to run the testbenches."
	$(ECHO) ""
	$(ECHO) "  make build_and_pack"
	$(ECHO) "    Command to build the hardware and software and pack the files."
	$(ECHO) ""
	$(ECHO) "  make pack"
	$(ECHO) "    Command to pack the files."
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) ""
# Build hardware (xclbin) objects
build_hw: compile_source hw_link

compile_source:
	make -C ./source compile TARGET=$(TARGET) PLATFORM=$(PLATFORM)
hw_link:
	make -C ./hw all TARGET=$(TARGET) PLATFORM=$(PLATFORM)
# Build software object
build_sw:
	make -C ./sw all
testbench_all:
	make -C ./source testbench_ckks

NAME := hw_build
pack:
	mkdir -p $(NAME)
	cp sw/host_overlay.exe $(NAME)/
	cp hw/overlay_enc_hw.xclbin $(NAME)/
	cp hw/overlay_add_hw.xclbin $(NAME)/
	cp hw/overlay_dec_hw.xclbin $(NAME)/
build_and_pack:
	$(info )
	$(info *********************** Building ***********************)
	$(info - NAME          $(NAME))
	$(info - TARGET        $(TARGET))
	$(info - PLATFORM      $(PLATFORM))
	$(info ********************************************************)
	$(info )
	make build_hw
	make build_sw
	make pack
# Clean objects
clean: clean_source clean_hw clean_sw

clean_source:
	make -C ./source clean
clean_hw:
	make -C ./hw clean
clean_sw:
	make -C ./sw clean