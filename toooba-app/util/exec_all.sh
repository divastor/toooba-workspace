#!/bin/bash

# USE IN DOCKER

make MAIN=condBranchMispred -s Mem.hex
echo "--- Running: condBranchMispred -------------------------------------------------"
rm console.log && ./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log
cat console.log
echo "--------------------------------------------------------------------------------"

make MAIN=indirBranchMispred -s Mem.hex
echo "--- Running: indirBranchMispred ------------------------------------------------"
rm console.log && ./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log
cat console.log
echo "--------------------------------------------------------------------------------"

make MAIN=returnStackBuffer -s Mem.hex
echo "--- Running: returnStackBuffer -------------------------------------------------"
rm console.log && ./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log
cat console.log
echo "--------------------------------------------------------------------------------"

make MAIN=mc_smc -s Mem.hex
echo "--- Running: mc_smc ------------------------------------------------------------"
rm console.log && ./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log
cat console.log
echo "--------------------------------------------------------------------------------"

make MAIN=mc_fp -s Mem.hex
echo "--- Running: mc_fp -------------------------------------------------------------"
rm console.log && ./util/exe_HW_sim +tohost +fromhost +v1 > /dev/null
cat console.log >> console_complete.log
cat console.log
echo "--------------------------------------------------------------------------------"
