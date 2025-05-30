BPF_CLANG_FLAGS = -g -O2 -target bpf -I.
USER_CFLAGS     = -g -O2
BPF_OBJ         = block_revsh.bpf.o
BPF_SKEL        = block_revsh.skel.h
LOADER_SRC      = loader.c
LOADER_BIN      = loader

.PHONY: all clean

all: $(LOADER_BIN)

$(BPF_OBJ): block_revsh.bpf.c
        clang $(BPF_CLANG_FLAGS) -c $< -o $@

$(BPF_SKEL): $(BPF_OBJ)
        bpftool gen skeleton $< > $@

$(LOADER_BIN): $(LOADER_SRC) $(BPF_SKEL)
        gcc $(USER_CFLAGS) $< -o $@ -lbpf -lelf -pthread -lz

clean:
        rm -f $(BPF_OBJ) $(BPF_SKEL) $(LOADER_BIN)
