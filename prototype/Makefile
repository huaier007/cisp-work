SUBDIRS := chklog chkRaW chkRS
BPF_CLANG_FLAGS := -g -O2 -target bpf -I.
USER_CFLAGS := -g -O2
LDLIBS := -lbpf -lelf -pthread -lz

chklog_BPF_SRC   := chklog/chkLog.bpf.c
chklog_BPF_OBJ   := chklog/chkLog.bpf.o
chklog_SKEL_HDR  := chklog/chkLog.skel.h
chklog_LOADER_SR := chklog/loader.c
chklog_LOADER_BIN:= chklog/loader

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
$(chklog_BPF_OBJ): $(chklog_BPF_SRC)
        @echo ">>> [chklog] Compiling BPF program: $<"
        clang $(BPF_CLANG_FLAGS) -c $< -o $@

$(chklog_SKEL_HDR): $(chklog_BPF_OBJ)
        @echo ">>> [chklog] Generating skeleton header: $@"
        bpftool gen skeleton $(chklog_BPF_OBJ) > $(chklog_SKEL_HDR)

$(chklog_LOADER_BIN): $(chklog_LOADER_SR) $(chklog_SKEL_HDR)
        @echo ">>> [chklog] Compiling loader: $<"
        gcc $(USER_CFLAGS) $< -o $@ -Ichklog $(LDLIBS)

chklog/loader: $(chklog_LOADER_BIN)

# chkRaW
$(chkRaW_BPF_OBJ): $(chkRaW_BPF_SRC)
        @echo ">>> [chkRaW] Compiling BPF program: $<"
        clang $(BPF_CLANG_FLAGS) -c $< -o $@

$(chkRaW_SKEL_HDR): $(chkRaW_BPF_OBJ)
        @echo ">>> [chkRaW] Generating skeleton header: $@"
        bpftool gen skeleton $(chkRaW_BPF_OBJ) > $(chkRaW_SKEL_HDR)

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
          $(chklog_BPF_OBJ)  $(chklog_SKEL_HDR)  $(chklog_LOADER_BIN) \
          $(chkRaW_BPF_OBJ)  $(chkRaW_SKEL_HDR)  $(chkRaW_LOADER_BIN) \
          $(chkRS_BPF_OBJ)   $(chkRS_SKEL_HDR)   $(chkRS_LOADER_BIN)
