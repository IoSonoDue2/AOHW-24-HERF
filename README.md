# AOHW-24 Homomorphic Encryption Acceleration Through Reconfigurable Fabric

|| Information |
|----------------------|-----|
| **Team number**          | AOHW-339 |
| **Project name**         | Homomorphic Encryption Acceleration Through Reconfigurable Fabric |
| **YouTube's video**      | lollo |
| **Project's repository** | https://github.com/IoSonoDue2/AOHW-24-HERF |
| **University name**      | Politecnico di Milano |
| **Participant (email**)  | Valentino Guerrini (valentino.guerrini@mail.polimi.it) |
| **Supervisor name**      | Marco Domenico Santambrogio |
| **Supervisor email**     | marco.santambrogio@polimi.it |

## Boards used:
AMD Kria KV260, AMD-Xilinx Versal VCK5000

<img src="https://hackster.imgix.net/uploads/attachments/1343310/_aZQq9gdBBd.blob?auto=compress%2Cformat&w=900&h=675&fit=min" height="260" align="bottom" /> <img src="https://www.xilinx.com/content/dam/xilinx/imgs/products/versal/vck5000/vck5000.png" height="260" align="bottom" />

## Software Version:
Vitis HLS 2022.2, Vivado 2022.2, Vitis 2022.2 , PYNQ 3.0.1

## Brief description of project:
This project brings the CKKS homomorphic encryption scheme to hardware, leveraging the capabilities of AMD-Xilinx tools. This innovative solution accelerates encryption and decryption processes in both embedded environments with the Kria KV260 and data center accelerators using the Versal VCK5000. Uniquely, in the data center setting, we have enhanced the homomorphic addition operation, demonstrating the seamless transition from secure encryption in embedded systems to efficient homomorphic computation on ciphertexts in data centers. This project showcases the future of secure, scalable, and efficient data processing across diverse computing environments.


## Description of archive:
The repository contains the following main directories and files:
```
├─`Kria_kv260`: Supports the development and deployment of the CKKS scheme on the Kria KV260 SOM. It includes:
    └─`Makefile`            #Makefile for building the Kria implementation and packaging it into a zip file ready for deployment.
    └─`setup_kria.sh`       #Script to set up the environment for Vitis HLS and Vivado. 
    └─`DataGen/`            #Includes files required to compile the software version of the algorithm on the Kria KV260 arm cpu. The generated binary is used by the host code to create encryption keys, secret keys, reference values, and measure the time taken to test the hardware version's correctness and speedup.
    └─`hw/`                 #Directory for build outputs.
    └─`host/`               #Host code and all necessary files for testing.
    └─`hw/`                 #Hardware build files.
    └─`source/`             #Source code implementing the CKKS encryption scheme.
  
├─`VCK5000` : Supports the development and deployment of the CKKS scheme on the Versal VCK5000 platform. It includes:
    └─`Makefile`            #Automates the build process for compiling source code and generating binaries and hardware configuration files.
    └─`setup_versal.sh`     #Script to set up the environment for the Versal platform.   
    └─`hw/`                 #Contains configuration files for generating hardware overlays for CKKS scheme and homomorphic operations.
    └─`hw_build/`           #Stores build outputs (xclbins), including executable host binary file to program the FPGA.
    └─`source/`             #Contains source code for the CKKS encryption scheme, including implementation files, headers, and test benches.
    └─`sw/`                 #Includes software components for interfacing with VCK5000 hardware, host code, setup scripts for emulation, and runtime configuration files.
```

## Instruction to fastly try the code

1. Open a terminal, then clone the repository by running the following command
  ```sh
  git clone https://github.com/IoSonoDue2/AOHW-24-HERF.git
  ```
2.  move into the repository with 
  ```sh
  cd AOHW-24-HERF
  ```

#### For Kria KV260:
3. Navigate to the `Kria_kv260/` directory.
4. Connect through shell to your Kria KV260 with pynq installed.
5. Move the folder `ready_to_run/` to the board in `/home/root/jupiter_notebooks/` (this path can vary according to your installation of pynq)
6. Connect through Jupiter to the board and move in the `ready_to_run/` directory.
7. Open the host you want to test `<...>_host.ipynb` and run it.

#### For Versal VCK5000:

1. Move the `hw_build/` folder on the host machine connected to board, enter the folder, and run the host application with:
  ```sh
  ./host_overlay.exe
  ```
Note: The host will let you select which kernel to run and test

## Instructions to Build and Test Project

### Step 0 - Clone the repository
Open a terminal, then clone the repository by running the following command
```shell
git clone https://github.com/IoSonoDue2/AOHW-24-HERF.git
```
Then, move into the repository with 
```shell
cd AOHW-24-HERF
```

### Step 1: Environment Setup
Ensure that all necessary tools and dependencies are installed and configured. This includes Vitis HLS 2022.2 and Vivado 2022.2.

#### For Kria KV260:
1. Open a terminal.
2. Navigate to the `Kria_kv260/` directory.
3. Run the setup script:
   ```sh
   source setup_kria.sh
   ```
   Alternatively, if Vivado and Vitis_HLS are installed in different paths, then run these commands:
   ```sh
   source <PATH_TO_XILINX_VIVADO_2022_2>/settings64.sh
   source <PATH_TO_XILINX_VITIS_HLS_2022_2>/settings64.sh
   ```

#### For Versal VCK5000:
1. Open a terminal.
2. Navigate to the `VCK5000/` directory.
3. Run the setup script:
   ```sh
   source setup_versal.sh
   ```
   Alternatively, if xrt and Vitis are installed in different paths, then run these commands:
   ```sh
   source <PATH_TO_XILINX_XRT>/setup.sh
   source <PATH_TO_XILINX_VITIS_2022_2>/settings64.sh
   ```

### Step 2: Build the Project
Navigate to the appropriate directory (`Kria_kv260` or `VCK5000`) and run the Makefile to build the project.

#### For Kria KV260:
1. Navigate to the `Kria_kv260/` directory.
2. Run the Makefile:
   ```sh
   make all
   make pack
   ```
   Wait to finish and you will find a zip folder `<export.zip>` containing the vivado generated bitstreams along with hostcodes for encryption and decryption
3. Mode the `data_gen/` folder on your device (Kria KV260), enter the folder and run:
   ```sh
   make data_gen
   ```
   A binary executable named `data_gen` will be generated.

#### For Versal VCK5000:
1. Navigate to the `VCK5000/` directory.
2. Run the Makefile:
   ```sh
   make build_and_pack NAME=<build_name> TARTGET=<hw|hw_emu>
   ```
   Note: the default value for NAME is "hw_build/" and the default for TARGET is "hw".
   Wait to finish and you will find the folder containg the xclbins and the host code binary to program the FPGA

### Step 3: Run
After building the project, program and run the FPGA with the generated files.

#### For Kria KV260:
1. Connect to your Kria KV260 with pynq installed.
2. Create a folder `<folder_name>/` on the board in `/home/root/jupiter_notebooks/` (this path can vary according to your installation of pynq)
3. Move the files generated in the `<export.zip` and `data_gen` executable compiled on kria in this folder. In particular you need to obtain the following structure:
   ```
   ├─ <folder_name>/                
       └─ data_gen
       └─ decryption_host.ipynb
       └─ encryption_host.ipynb
       └─ ckks_encryption_wrapper.xsa
       └─ ckks_decryption_wrapper.xsa
       └─ data/   (this folder can be empty but it is necessary)      
   ```
4. Open the host you want to test `<...>_host.ipynb` and run it.


#### For Versal VCK5000:
Move the `<build_name>/` folder on the host machine connected to board, enter the folder, and run the host application with:
  ```sh
  ./host_overlay.exe
  ```
The host will let you select which kernel to run and test

### Additional Steps (if applicable):
- For further customization and testing, modify the source code in the `source` directories and rebuild the project.
- Refer to the `README.md` files in each directory for more detailed instructions and information about additional features and configurations.
