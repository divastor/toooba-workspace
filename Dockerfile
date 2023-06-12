FROM ubuntu:20.04

# Initialization
RUN apt update \
    && apt install -y git wget make g++ gcc libz-dev verilator

# Download git repos
RUN cd /home \
    && git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git \
    && git clone https://github.com/bluespec/Toooba.git

# Setup riscv-gnu-toolchain
RUN cd /home/riscv-gnu-toolchain\
    && DEBIAN_FRONTEND=noninteractive apt -y install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev \
    && mkdir -p out/Multilib \
    && ./configure --prefix=/home/riscv-gnu-toolchain/out/Multilib --enable-multilib --with-cmodel=medany \
    && make

# # Setup Bluespec's compiler
# RUN cd /home \
#     && wget "https://github.com/B-Lang-org/bsc/releases/download/2023.01/bsc-2023.01-ubuntu-20.04.tar.gz" \
#     && tar zxvf bsc-2023.01-ubuntu-20.04.tar.gz && rm bsc-2023.01-ubuntu-20.04.tar.gz \
#     && mv bsc-2023.01-ubuntu-20.04 /home/bsc-2023.01

ENV PATH="/home/riscv-gnu-toolchain/out/Multilib/bin:${PATH}"
ENV BUILD_ROOT="/home/Toooba/builds/RV64ACDFIMSU_Toooba_verilator/"

# Setup Toooba RTL simulator & elf_to_hex
COPY toooba-app/patch_files/write_to_console.patch /home/Toooba/builds/
RUN cd /home/Toooba/Tests/elf_to_hex \
    && apt install -y libelf-dev \
    && make elf_to_hex
RUN cd $BUILD_ROOT/ \
    # && export PATH=$PATH:/home/bsc-2023.01/bin \
    # && make compile \
    && patch -p3 < ../write_to_console.patch \
    && make simulator

# toooba-app
RUN mkdir -p /home/toooba-app
COPY toooba-app /home/toooba-app
RUN cp /home/Toooba/Tests/elf_to_hex/elf_to_hex /home/toooba-app/util/ \
    && cp $BUILD_ROOT/exe_HW_sim /home/toooba-app/util/exe_HW_sim

CMD ["/home/toooba-app/util/run.sh"]