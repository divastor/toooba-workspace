# Bluespec's Toooba - A How To Guide

Toooba is a RISC-V processor that is created by Bluespec and uses (for the most part) MIT's Riscy-OOO. It is a highly parameterized processor, implementing most of the extensions of 64bit RISC-V ISA.

- RISC-V documentation can be found on [the official site](https://riscv.org/technical/specifications/).
- RISC-V GNU Toolchain is fetched from the [Github repository](https://github.com/riscv-collab/riscv-gnu-toolchain).
- The Toooba core implementation has a [Github repository](https://github.com/bluespec/Toooba) for the FPGA verilog code and RTL simulation.
- Riscy-OOO's source files are integrated in the Toooba repository, with some modifications that better suit the implementation. It can also be found as is [on Github](https://github.com/csail-csg/riscy-OOO). Documentation can be found on the [documentation repository](https://github.com/csail-csg/RiscyOO_design_doc).

## Setup

There are a two ways you can setup this project. The first one is to build and run the Docker container, and the other is to build and run the simulation. I provide guidelines for the two methods mentioned.

### Running the Docker container <sup>(RECOMMENDED/Multi-platform)</sup>

You can find out how to install Docker [here](https://docs.docker.com/get-docker/). After installing it, run:
```bash
docker build -t toooba-app:1.0 .
docker run -it toooba-app:1.0
```

### Building and running the app with Verilator <sup>(Linux)</sup>

#### Step 1 - Verilog

Install verilog as described for your Linux distribution: https://iverilog.fandom.com/wiki/Installation_Guide.

#### Step 2 - RISC-V GNU Toolchain

In order to build the code, we need to first install the RISC-V GNU Toolchain. I suggest you download the toolchain and build it as demonstrated below, but some Linux distributions provide the binaries in the apt repository (not tested).
To build the RISC-V GNU Toolchain, download the source code from https://github.com/riscv-collab/riscv-gnu-toolchain and install it with:

```bash
cd riscv-gnu-toolchain
./configure --prefix=/path/to/out --enable-multilib --with-cmodel=medany && make
```

You can specify any directory to output the binaries. It takes about ~3-4 hours on a 4-cores/8-threads CPU + 8GB RAM machine. Consult the repo's `README.md` for dependencies and detailed installation guides.

#### Step 3 - Toooba core simulation.

The simulation program is designed in such a way that each retired instruction has its information (address, type, opcode, etc.) displayed on the console along with any standard output from the program. I have created a patch file attempting to fix that, so that the standard output is also saved in a file called `console.log`.

To produce the new simulation program, the patch should be applied and the verilog code be rebuilt with the new changes. In order to do that, first download the [Toooba repository](https://github.com/bluespec/Toooba). Then, use the following commands, substituting `$TOOOBA_APP` and `$TOOOBA_ROOT` with the local path to the `toooba-app` folder and Toooba's root folder respectively:

```bash
# input the paths to the "toooba-app" folder and "Toooba" repository respectively
export TOOOBA_APP=<path/to/toooba-app>
export TOOOBA_ROOT=<path/to/Toooba>
```

```bash
cd $TOOOBA_ROOT/builds/RV64ACDFIMSU_Toooba_verilator/
cp $TOOOBA_APP/patch_files/write_to_console.patch $TOOOBA_ROOT/builds/
patch -p3 < ../write_to_console.patch
make simulator
cp $BUILD_ROOT/exe_HW_sim $TOOOBA_APP/util/exe_HW_sim
```

The simulator is neatly put in the `toooba-app/util/` folder. We also need a tool to convert the ELF files created by the RISC-V toolchain to `.hex` files that the simulation uses. This tool can be found in the [Toooba repository](https://github.com/bluespec/Toooba), but it must be built first. To do that follow these commands:

```bash
cd $TOOOBA_ROOT/Tests/elf_to_hex
make elf_to_hex
cp elf_to_hex $TOOOBA_APP/util/
```

The simulation can be run using the following command:

```bash
./util/exe_HW_sim +tohost +fromhost +v1 2>&1 | tee exec.log
```

### Makefile

The `Makefile` can be used as follows:

- `make` : Will compile the `Mem.hex` as described in the `Makefile` for the RTL simulation to use. At the moment Spectre Variant 1 is being used (`condBranchMispred.c`), but other targets can be specified (`indirBranchMispred.c`, `returnStackBuffer.c`, `mc_smc.c`, `mc_fp.c`) by changing the `MAIN` variable of the filename (without the extension `.c`) you want to compile:

  ```bash
  make MAIN=mc_smc
  ```

- `make clean`: removes the generated files from the directory.

### Source files

- `src/defines.h`: various definitions.
- `src/crt0.s`: entry/exit point definitions necessary to execute a RISC-V program on Toooba
- `src/printf.c`: minimal printing support found in the `Core-SweRV` git repository designed for SweRV. To get it running for Toooba, `whisperPutc` got replaced with `myputc` that writes each character to the console's IO address, denoted in `src/defines.h` under the `CONSOLE_ADDR` constant.
- `src/link.ld`: our linker script.
- `src/cache.h`: cache's flush function that evicts an area from the cache.
- `src/pwntools.c`: 
- `src/condBranchMispred.c`/`src/indirBranchMispred.c`: Spectre v1/v2 proof of concepts.
- `src/returnStackBuffer.c`/`src/dropStack.s`: Spectre-RSB proof of concept harness and vulnerable code respectively.
- `src/mc_smc.c`/`src/smc_snippet.c`: Self-Modifying Code Machine Clear's proof of concept harness and the vulenrable code respectively.
- `src/mc_fp.c`/`src/fp_snippet.c`: Floating Point Machine Clear's proof of concept harness and the vulenrable code respectively.
- `secret.s`: describes the structure of the memory around the area of the secret to be extracted in the scenarios of SMC and FP machine clears.

## Configuring and testing Bluespec's Toooba

Toooba can be modified to suit different needs. For example, one could develop upon the code base, modifying the internal components as defined by the Bluespec SystemVerilog. It is also possible to use the preexisting build present in the `//builds` folder of the Toooba repository to modify some of it's parameters (many of which are inside the `//builds/Resources` folder). In any case, changes like that can be made using Bluespec's compiler (BSC). 

### BSC - Bluespec SystemVerilog compiler 

BSC can be used to generate the Verilog RTL code. It can be downloaded from the [BSC repository](https://github.com/B-Lang-org/bsc). Download the latest release from the [releases tab](https://github.com/B-Lang-org/bsc/releases). For Linux, the `//bin/` folder contains the compiler and should be added to the `PATH` before compiling the project.

### Building Toooba

The same Testbench can be used for the new build. BSC will try to use any preexisting Verilog files instead of regenerating them which may cause problems when building the simulator. To avoid that, delete the `Verilog_RTL/*` files. Assuming changes have been made to the project, the new processor can be built using the following commands:

```bash
cd $TOOOBA_ROOT/builds/RV64ACDFIMSU_Toooba_verilator
rm Verilog_RTL/*
make compile
```

Next, we can build the simulator, using the same patch that dumps stdout to the console and, also, copy the simulator program to the Toooba workspace:

```bash
patch -p3 < $TOOOBA_APP/patch_files/write_to_console.patch
make simulator
cp exe_HW_sim $TOOOBA_APP/util/exe_HW_sim
```

### Testing Toooba's correctness

You can try all (or some) of RISC-V tests using the folder `//Tests` of the Toooba repository. `//Tests/README.txt` provides all the necessary details to run batch tests. A working simulator should pass all `//Tests/isa/*` supported tests.