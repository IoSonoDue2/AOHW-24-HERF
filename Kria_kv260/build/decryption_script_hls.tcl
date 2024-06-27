open_project CKKS_DECRYPTION_KRIA
set_top ckks_dec_ker
add_files writer.hpp
add_files writer.cpp
add_files ZStructures.hpp
add_files ZStructures.cpp
add_files Utils.hpp
add_files Utils.cpp
add_files Structures.hpp
add_files Structures.cpp
add_files Scheme.hpp
add_files Scheme.cpp
add_files Parameters.hpp
add_files -tb testbench_ckks.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xck26-sfvc784-2LV-c}
create_clock -period 10 -name default
source "decryption_directives.tcl"

#csim_design -clean
#csynth_design
#cosim_design
#export_design -rtl verilog -format ip_catalog 

exit