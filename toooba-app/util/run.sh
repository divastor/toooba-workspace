#!/bin/bash

# USE IN DOCKER

cd /home/toooba-app/
make clean
echo "--- RTL ------------------------------------------------------------------------"
echo "  Executing RTL Simulations:"
touch console.log
./util/exec_all.sh &
tail --follow=name --retry console.log
echo "--------------------------------------------------------------------------------"
