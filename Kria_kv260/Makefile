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

BUILD_DIR = hw
HLS_DIR = $(BUILD_DIR)/CKKS_HLS_PROJ
VIVADO_DIR = $(BUILD_DIR)/CKKS_VIVADO_PROJ
SOURCE_DIR = source

HLS_ENC_TCL_FILE = encryption_script_hls.tcl
HLS_ENC_DIR_FILE = encryption_directives.tcl
HLS_DEC_TCL_FILE = decryption_script_hls.tcl
HLS_DEC_DIR_FILE = decryption_directives.tcl

VIVADO_ENC_TCL_FILE = ckks_encryption_kria.tcl
VIVADO_DEC_TCL_FILE = ckks_decryption_kria.tcl

FILES_TO_PACK = host/data_gen host/data host/encryption_host.ipynb host/decryption_host.ipynb $(VIVADO_DIR)/ckks_decryption_wrapper.xsa $(VIVADO_DIR)/ckks_encryption_wrapper.xsa
ZIP_NAME = export.zip


help:
	@echo "Usage: make <target> [MODE=<mode>]"
	@echo "Targets:"
	@echo "  test_encryption MODE=<csim|cosim>   - Run encryption tests with either csim or cosim"
	@echo "  export_encryption_ip                - Export encryption IP in Vitis HLS"
	@echo "  build_encryption_hw                 - Build encryption hardware in Vivado"
	@echo "  test_decryption MODE=<csim|cosim>   - Run decryption tests with either csim or cosim"
	@echo "  export_decryption_ip                - Export decryption IP in Vitis HLS"
	@echo "  build_decryption_hw                 - Build decryption hardware in Vivado"
	@echo "  clean                               - Clean build directories and reset TCL files"
	@echo "  all                                 - Build both encryption and decryption hardware"
	@echo "  pack     							 - Takes host, hw builds, data_gen, and puts them in a zip file ready to send to kria"



MODE = cosim

dirs:
	@echo "Creating directories..."
	@mkdir -p $(HLS_DIR)
	@mkdir -p $(VIVADO_DIR)

copy_sources: dirs
	@echo "Copying source files to $(HLS_DIR)..."
	@cp $(SOURCE_DIR)/*.cpp $(HLS_DIR)
	@cp $(SOURCE_DIR)/*.hpp $(HLS_DIR)
	@cp $(BUILD_DIR)/$(HLS_ENC_TCL_FILE) $(HLS_DIR)
	@cp $(BUILD_DIR)/$(HLS_DEC_TCL_FILE) $(HLS_DIR)
	@cp $(BUILD_DIR)/$(HLS_ENC_DIR_FILE) $(HLS_DIR)
	@cp $(BUILD_DIR)/$(HLS_DEC_DIR_FILE) $(HLS_DIR)
	@cp $(BUILD_DIR)/$(VIVADO_ENC_TCL_FILE) $(VIVADO_DIR)
	@cp $(BUILD_DIR)/$(VIVADO_DEC_TCL_FILE) $(VIVADO_DIR)


test_encryption: copy_sources
ifeq ($(MODE), csim)
	@sed -i 's/^#csim_design -clean/csim_design -clean/' $(HLS_DIR)/$(HLS_ENC_TCL_FILE)

else ifeq ($(MODE), cosim)
	@sed -i 's/^#csynth_design/csynth_design/' $(HLS_DIR)/$(HLS_ENC_TCL_FILE)
	@sed -i 's/^#cosim_design/cosim_design/' $(HLS_DIR)/$(HLS_ENC_TCL_FILE)
else
	@echo "Mode $(MODE) is not supported."
endif
	@echo "Running high-level synthesys and C/RTL co-simulation in vitis_hls with $(HLS_DIR)/$(HLS_ENC_TCL_FILE)"
	@cd $(HLS_DIR) && vitis_hls -f $(HLS_ENC_TCL_FILE)

export_encryption_ip: copy_sources
	@sed -i 's/^#csynth_design/csynth_design/' $(HLS_DIR)/$(HLS_ENC_TCL_FILE)
	@sed -i 's/^#export_design -rtl verilog -format ip_catalog/export_design -rtl verilog -format ip_catalog/' $(HLS_DIR)/$(HLS_ENC_TCL_FILE)
	@echo "Exporting encryption ip in vitis_hls with $(HLS_DIR)/$(HLS_ENC_TCL_FILE)"
	@cd $(HLS_DIR) && vitis_hls -f $(HLS_ENC_TCL_FILE)

build_encryption_hw: export_encryption_ip
	@echo "Building encryption hardware in vivado with $(VIVADO_DIR)/$(VIVADO_ENC_TCL_FILE)"
	@cd $(VIVADO_DIR) && vivado -mode batch -source $(VIVADO_ENC_TCL_FILE)

test_decryption: copy_sources
ifeq ($(MODE), csim)
	@sed -i 's/^#csim_design -clean/csim_design -clean/' $(HLS_DIR)/$(HLS_DEC_TCL_FILE)
else ifeq ($(MODE), cosim)
	@sed -i 's/^#csynth_design/csynth_design/' $(HLS_DIR)/$(HLS_DEC_TCL_FILE)
	@sed -i 's/^#cosim_design/cosim_design/' $(HLS_DIR)/$(HLS_DEC_TCL_FILE)
else
	@echo "Mode $(MODE) is not supported."
endif
	@echo "Running high-level synthesis and C/RTL co-simulation in vitis_hls with $(HLS_DIR)/$(HLS_DEC_TCL_FILE)"
	@cd $(HLS_DIR) && vitis_hls -f $(HLS_DEC_TCL_FILE)

export_decryption_ip: copy_sources
	@sed -i 's/^#csynth_design/csynth_design/' $(HLS_DIR)/$(HLS_DEC_TCL_FILE)
	@sed -i 's/^#export_design -rtl verilog -format ip_catalog/export_design -rtl verilog -format ip_catalog/' $(HLS_DIR)/$(HLS_DEC_TCL_FILE)
	@echo "Exporting decryption IP in vitis_hls with $(HLS_DIR)/$(HLS_DEC_TCL_FILE)"
	@cd $(HLS_DIR) && vitis_hls -f $(HLS_DEC_TCL_FILE)

build_decryption_hw: export_decryption_ip
	@echo "Building decryption hardware in vivado with $(VIVADO_DIR)/$(VIVADO_DEC_TCL_FILE)"
	@cd $(VIVADO_DIR) && vivado -mode batch -source $(VIVADO_DEC_TCL_FILE)

all: build_encryption_hw build_decryption_hw

pack:
	@echo "Packing files into $(ZIP_NAME)..."
	@for file in $(FILES_TO_PACK); do \
		if [ -e "$$file" ]; then \
			echo "Adding $$file"; \
			zip -r $(ZIP_NAME) "$$file"; \
		else \
			echo "File $$file not found, skipping"; \
		fi \
	done
	@echo "Packing complete."

clean:
	@echo "Cleaning build directories..."
	@rm -rf $(HLS_DIR) $(VIVADO_DIR)