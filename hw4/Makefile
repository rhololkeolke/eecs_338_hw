CC = gcc
CFLAGS = -g -Wall
OUTPUT = main agent.bin customer.bin bus.bin
HEADERS = main.h

all: $(OUTPUT)

agent.bin: agent.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

customer.bin: customer.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

bus.bin: bus.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

main: main.c $(HEADERS)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $<

clean:
	@rm -f $(OUTPUT)

