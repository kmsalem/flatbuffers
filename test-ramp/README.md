## Introduction

This is a test directory containing sample programs to test flatbuffer-RAMP


## Setup
1. Download and set up RDMA-migration-system as required: https://git.uwaterloo.ca/huawei-rdma/rdma-migration-system/tree/master
2. Run following commands (under /flatbuffers-RAMP) to build the project and install flatc

    cmake .
    
    make
    
    sudo make install
    
    sudo ldconfig
    
3. Use 'make' to build the sample programs
4. Modify config.txt file with the number of machines and their IP addresses before running the tests


### Note:
1. Generated files by flatc have already been included. You don't have to run schema compiler again.
2. Use command './<program_name> config.txt server_id size' to run the programs
