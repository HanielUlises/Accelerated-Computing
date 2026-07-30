# CUDA 12.9 build rules for the samples in this directory.
#
#   make                      build every .cu into build/
#   make build/block_warp     build one sample
#   make run FILE=block_warp  build + run one sample
#   make debug FILE=...       build with -g -G, then launch cuda-gdb
#   make ptx  FILE=...        dump PTX
#   make sass FILE=...        dump SASS (disassembled machine code)
#   make regs FILE=...        per-kernel register / shared-mem usage
#   make sanitize FILE=...    run under compute-sanitizer (memcheck)
#   make racecheck FILE=...   run under compute-sanitizer (racecheck)
#   make prof FILE=...        Nsight Compute kernel profile
#   make db                   regenerate compile_commands.json for clangd
#   make clean

CUDA_HOME ?= /usr/local/cuda-12.9
NVCC      := $(CUDA_HOME)/bin/nvcc
HOST_CXX  ?= g++-14
ARCH      ?= 86
STD       ?= c++20
BUILD     ?= build

GENCODE   := -gencode arch=compute_$(ARCH),code=sm_$(ARCH) \
             -gencode arch=compute_$(ARCH),code=compute_$(ARCH)

NVCCFLAGS := -std=$(STD) -ccbin $(HOST_CXX) $(GENCODE) \
             --expt-relaxed-constexpr --extended-lambda \
             -lineinfo \
             -Xcompiler -Wall,-Wextra
RELFLAGS  := -O3
DBGFLAGS  := -O0 -g -G
LDLIBS    := -lcudart

SRCS := $(wildcard *.cu)
# Several samples are kernel-only fragments with no main(); those are compiled
# to objects (still fully type-checked and codegen'd) instead of linked.
EXEC_SRCS := $(shell grep -l '^[[:space:]]*int[[:space:]]\+main' $(SRCS) 2>/dev/null)
OBJ_SRCS  := $(filter-out $(EXEC_SRCS),$(SRCS))
BINS := $(patsubst %.cu,$(BUILD)/%,$(EXEC_SRCS)) \
        $(patsubst %.cu,$(BUILD)/%.o,$(OBJ_SRCS))

# `make run FILE=block_warp` and `make run FILE=block_warp.cu` both work.
TARGET := $(basename $(FILE))

.PHONY: all run debug ptx sass regs sanitize racecheck prof db clean help require-file
.DEFAULT_GOAL := all

all: $(BINS)

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.o: %.cu | $(BUILD)
	$(NVCC) $(NVCCFLAGS) $(RELFLAGS) -MMD -MF $@.d -c $< -o $@

$(BUILD)/%: %.cu | $(BUILD)
	$(NVCC) $(NVCCFLAGS) $(RELFLAGS) -MMD -MF $@.d $< -o $@ $(LDLIBS)

-include $(wildcard $(BUILD)/*.d)

# --- guard: targets below need FILE= -------------------------------------
require-file:
	@test -n "$(TARGET)" || { echo "usage: make $(MAKECMDGOALS) FILE=<source.cu>"; exit 2; }

run: require-file $(BUILD)/$(TARGET)
	@echo "── running $(TARGET) ──"
	@$(BUILD)/$(TARGET)

debug: require-file | $(BUILD)
	$(NVCC) $(NVCCFLAGS) $(DBGFLAGS) $(TARGET).cu -o $(BUILD)/$(TARGET)-dbg $(LDLIBS)
	$(CUDA_HOME)/bin/cuda-gdb $(BUILD)/$(TARGET)-dbg

ptx: require-file | $(BUILD)
	$(NVCC) $(NVCCFLAGS) $(RELFLAGS) -ptx $(TARGET).cu -o $(BUILD)/$(TARGET).ptx
	@echo "── $(BUILD)/$(TARGET).ptx ──"

sass: require-file $(BUILD)/$(TARGET)
	$(CUDA_HOME)/bin/cuobjdump -sass $(BUILD)/$(TARGET)

regs: require-file | $(BUILD)
	$(NVCC) $(NVCCFLAGS) $(RELFLAGS) -Xptxas -v -c $(TARGET).cu -o /dev/null

sanitize: require-file $(BUILD)/$(TARGET)
	$(CUDA_HOME)/bin/compute-sanitizer --tool memcheck --leak-check=full $(BUILD)/$(TARGET)

racecheck: require-file $(BUILD)/$(TARGET)
	$(CUDA_HOME)/bin/compute-sanitizer --tool racecheck $(BUILD)/$(TARGET)

prof: require-file $(BUILD)/$(TARGET)
	$(CUDA_HOME)/bin/ncu --set full $(BUILD)/$(TARGET)

# compile_commands.json with *clang*-compatible flags (clangd parses .cu with
# clang's CUDA front end, not nvcc). The .clangd file strips anything nvcc-only
# that still slips through.
db:
	@printf '[\n' > compile_commands.json
	@n=$$(echo $(SRCS) | wc -w); i=0; \
	for f in $(SRCS); do \
	  i=$$((i+1)); \
	  printf '  {\n    "directory": "%s",\n    "file": "%s",\n    "command": "clang++ -xcuda -std=$(STD) --cuda-path=$(CUDA_HOME) --cuda-gpu-arch=sm_$(ARCH) -Wno-unknown-cuda-version --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/14 -I$(CUDA_HOME)/include -c %s"\n  }' \
	    "$(CURDIR)" "$(CURDIR)/$$f" "$$f" >> compile_commands.json; \
	  if [ $$i -lt $$n ]; then printf ',\n' >> compile_commands.json; else printf '\n' >> compile_commands.json; fi; \
	done
	@printf ']\n' >> compile_commands.json
	@echo "wrote compile_commands.json ($(words $(SRCS)) entries)"

clean:
	rm -rf $(BUILD)

help:
	@sed -n '3,20p' $(MAKEFILE_LIST)
