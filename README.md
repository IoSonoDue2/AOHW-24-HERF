# AOHW-24-HERF

## Team number: AOHW-339

## Project name:
Homomorphic Encryption Acceleration Through Reconfigurable Fabric

## Link to YouTube Video(s):
[Link to Project Video](https://youtube.com/your_project_video)

## Link to project repository:
[GitHub Repository](https://github.com/IoSonoDue2/AOHW-24-HERF)

## University name:
Politecnico di Milano

## Participant:
- Participant Name: Valentino Guerrini
  - Email: valentino.guerrini@mail.polimi.it

## Supervisor name:
- Supervisor Name: Prof. Marco Domenico Santambrogio
  - Supervisor e-mail: marco.santambrogio@polimi.it

## Boards used:
Kria KV260, VCK5000

## Software Version:
Vitis HLS 2022.2, Vivado 2022.2

## Brief description of project:
This project brings the CKKS homomorphic encryption scheme to hardware, leveraging the capabilities of AMD-Xilinx tools. This innovative solution accelerates encryption and decryption processes in both embedded environments with the Kria KV260 and data center accelerators using the Versal VCK5000. Uniquely, in the data center setting, we have enhanced the homomorphic addition operation, demonstrating the seamless transition from secure encryption in embedded systems to efficient homomorphic computation on ciphertexts in data centers. This project showcases the future of secure, scalable, and efficient data processing across diverse computing environments.


## Description of archive:
The repository contains the following main directories and files:
- `Kria_kv260`: Contains all necessary components for the Kria KV260 System on Module (SOM).
  - `DataGen`: Includes files required to compile the software version of the algorithm on the Kria KV260 arm cpu. The generated binary is used by the host code to create encryption keys, secret keys, reference values, and measure the time taken to test the hardware version's correctness and speedup.
  - `hw`: Directory for build outputs.
  - `host`: Host code and all necessary files for testing.
  - `hw`: Hardware build files.
  - `source`: Source code implementing the CKKS encryption scheme.
  - `Makefile`: Makefile for building the project and packaging it into a zip file ready for deployment.
  - `setup_kria.sh`: Script to set up the environment for Vitis HLS and Vivado.
