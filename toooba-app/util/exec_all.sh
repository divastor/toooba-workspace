#!/bin/bash

# USE IN DOCKER

make MAIN=mc_smc -s Mem.hex
./util/exe_HW_sim +tohost +fromhost +v1 
cat console.log >> console_complete.log

make MAIN=fp_smc -s Mem.hex
./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log

make MAIN=condBranchMispred -s Mem.hex
./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log

make MAIN=indirBranchMispred -s Mem.hex
./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log

make MAIN=returnStackBuffer -s Mem.hex
./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log
