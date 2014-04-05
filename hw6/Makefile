.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall
OUTPUT = mother tina judy
RPC_FILES = Cookie.x
GEN_FILES = Cookie.h Cookie_svc.c Cookie_clnt.c

all: tina judy mother

tina: tina.c rpc
	$(CC) $(CFLAGS) -o tina tina.c Cookie_clnt.c

judy: judy.c rpc
	$(CC) $(CFLAGS) -o judy judy.c Cookie_clnt.c

mother: mother.c rpc
	$(CC) $(CFLAGS) -o mother mother.c Cookie_svc.c

rpc: $(RPC_FILES)
	@rpcgen $<

clean:
	@rm -f $(OUTPUT) $(GEN_FILES)
