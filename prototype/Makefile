SUBDIRS := chkLog chkRaW chkRS
BPF_CLANG_FLAGS := -g -O2 -target bpf -I.
USER_CFLAGS := -g -O2
LDLIBS := -lbpf -lelf -pthread -lz

chkLog_BPF_SRC   := chkLog/chkLog.bpf.c
chkLog_BPF_OBJ   := chkLog/chkLog.bpf.o
chkLog_SKEL_HDR  := chkLog/chkLog.skel.h
chkLog_LOADER_SR := chkLog/loader.c
chkLog_LOADER_BIN:= chkLog/loader

chkRaW_BPF_SRC   := chkRaW/chkRaW.bpf.c
chkRaW_BPF_OBJ   := chkRaW/chkRaW.bpf.o
chkRaW_SKEL_HDR  := chkRaW/chkRaW.skel.h
chkRaW_LOADER_SR := chkRaW/loader.c
chkRaW_LOADER_BIN:= chkRaW/loader

chkRS_BPF_SRC    := chkRS/block_revsh.bpf.c
chkRS_BPF_OBJ    := chkRS/block_revsh.bpf.o
chkRS_SKEL_HDR   := chkRS/block_revsh.skel.h
chkRS_LOADER_SR  := chkRS/loader.c
chkRS_LOADER_BIN := chkRS/loader

.PHONY: all clean

all: $(foreach dir,$(SUBDIRS),$(dir)/loader)

#chkLog
$(chkLog_BPF_OBJ): $(chkLog_BPF_SRC)
        @echo ">>> [chkLog] Compiling BPF program: $<"
        clang $(BPF_CLANG_FLAGS) -c $< -o $@

$(chkLog_SKEL_HDR): $(chkLog_BPF_OBJ)
        @echo ">>> [chkLog] Generating skeleton header: $@"
        bpftool gen skeleton $(chkLog_BPF_OBJ) > $(chkLog_SKEL_HDR)

$(chkLog_LOADER_BIN): $(chkLog_LOADER_SR) $(chkLog_SKEL_HDR)
        @echo ">>> [chkLog] Compiling loader: $<"
        gcc $(USER_CFLAGS) $< -o $@ -IchkLog $(LDLIBS)

chkLog/loader: $(chkLog_LOADER_BIN)

# chkRaW
$(chkRaW_BPF_OBJ): $(chkRaW_BPF_SRC)
        @echo ">>> [chkRaW] Compiling BPF program: $<"
        clang $(BPF_CLANG_FLAGS) -c $< -o $@

$(chkRaW_SKEL_HDR): $(chkRaW_BPF_OBJ)
        @echo ">>> [chkRaW] Generating skeleton header: $@"
        bpftool gen skeleton $(chkRaW_BPF_OBJ) > $(chkRaW_SKEL_HDR)
10;rgb:cccc/cccc/cccc
$(chkRaW_LOADER_BIN): $(chkRaW_LOADER_SR) $(chkRaW_SKEL_HDR)
        @echo ">>> [chkRaW] Compiling loader: $<"
        gcc $(USER_CFLAGS) $< -o $@ -IchkRaW $(LDLIBS)

chkRaW/loader: $(chkRaW_LOADER_BIN)

# chkRS
$(chkRS_BPF_OBJ): $(chkRS_BPF_SRC)
        @echo ">>> [chkRS] Compiling BPF program: $<"
        clang $(BPF_CLANG_FLAGS) -c $< -o $@

$(chkRS_SKEL_HDR): $(chkRS_BPF_OBJ)
        @echo ">>> [chkRS] Generating skeleton header: $@"
        bpftool gen skeleton $(chkRS_BPF_OBJ) > $(chkRS_SKEL_HDR)

$(chkRS_LOADER_BIN): $(chkRS_LOADER_SR) $(chkRS_SKEL_HDR)
        @echo ">>> [chkRS] Compiling loader: $<"
        gcc $(USER_CFLAGS) $< -o $@ -IchkRS $(LDLIBS)

chkRS/loader: $(chkRS_LOADER_BIN)

#clear
clean:
        @echo ">>> Cleaning all build artifacts..."
        @rm -f \
          $(chkLog_BPF_OBJ)  $(chkLog_SKEL_HDR)  $(chkLog_LOADER_BIN) \
          $(chkRaW_BPF_OBJ)  $(chkRaW_SKEL_HDR)  $(chkRaW_LOADER_BIN) \
          $(chkRS_BPF_OBJ)   $(chkRS_SKEL_HDR)   $(chkRS_LOADER_BIN)
