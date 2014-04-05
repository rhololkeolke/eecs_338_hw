.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall
OUTPUT = mother tina judy
RPC_FILES = Cookie.x
GEN_FILES = Cookie.h Cookie_svc.c Cookie_clnt.c

rpc: $(RPC_FILES)
	@rpcgen $<

clean:
	@rm -f $(OUTPUT) $(GEN_FILES)
