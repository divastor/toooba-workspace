#!/bin/bash

# USE IN DOCKER

cd /home/toooba-app/
echo "--- RTL ------------------------------------------------------------------------"
echo "  Executing RTL Simulations:"
touch console.log
./util/exec_all.sh &
tail -f ./console.log
echo "--------------------------------------------------------------------------------"
