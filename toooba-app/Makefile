# toolchain
# use a toolchain that is compiled with --enable-multilib and --with-cmodel=medany
# (on riscv-gnu-toolchain: ./configure --prefix=/path/to/out --enable-multilib --with-cmodel=medany )
TOOLCHAIN = riscv64-unknown-elf
GCC = $(TOOLCHAIN)-gcc
CPP = $(TOOLCHAIN)-cpp
AS = $(TOOLCHAIN)-as
LD = $(TOOLCHAIN)-ld
OBJCOPY = $(TOOLCHAIN)-objcopy
OBJDUMP = $(TOOLCHAIN)-objdump

# program creation flags
ABI = -mabi=lp64d -march=rv64imafdc
MAIN ?= condBranchMispred
OUT_DIR = out
SRC_DIR = src

# ============================================================================ #

all: clean Mem.hex

# ============================================================================ #

Mem.hex: $(OUT_DIR)/crt0.o $(OUT_DIR)/secret.o $(OUT_DIR)/fp_snippet.o $(OUT_DIR)/smc_snippet.o $(OUT_DIR)/dropStack.o $(OUT_DIR)/$(MAIN).o
	@echo Building $(MAIN)
	mkdir -p $(OUT_DIR)
	$(GCC) $(ABI) -o $(OUT_DIR)/$(MAIN).exe \
		-nostartfiles -mcmodel=medany \
		-Wl,-Map=$(OUT_DIR)/$(MAIN).map -T$(SRC_DIR)/link.ld -lgcc $^

	$(OBJCOPY) -O verilog $(OUT_DIR)/$(MAIN).exe $(OUT_DIR)/$(MAIN).vh
	$(OBJCOPY) -O binary $(OUT_DIR)/$(MAIN).exe $(OUT_DIR)/$(MAIN).bin
	$(OBJDUMP) -D -S $(OUT_DIR)/$(MAIN).exe > $(OUT_DIR)/$(MAIN).dis
	./util/elf_to_hex $(OUT_DIR)/$(MAIN).exe Mem.hex
	grep "<fromhost>:" $(OUT_DIR)/$(MAIN).dis | awk '{sub(/^0+/, "", $$1); str = sprintf("fromhost  0x%s",$$1); print str}' >> symbol_table.txt
	@echo Completed building $(MAIN)

$(OUT_DIR)/%.o : $(SRC_DIR)/%.c
	mkdir -p $(OUT_DIR)
	$(GCC) -mcmodel=medany $(ABI) -nostartfiles -g -c $< -o $@

$(OUT_DIR)/%.o : $(SRC_DIR)/%.s
	mkdir -p $(OUT_DIR)
	$(CPP) -mcmodel=medany -g $< > $(OUT_DIR)/$*.cpp.s
	$(AS) $(ABI) $(OUT_DIR)/$*.cpp.s -o $@

# ============================================================================ #

clean:
	rm -rf $(OUT_DIR) *.hex obj_dir *.log *.vcd *.csv symbol_table.txt
