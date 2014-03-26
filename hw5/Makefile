CC = gcc
CFLAGS = -g -Wall
OUTPUT = main
HEADERS = main.h
LIBS = -pthread

all: $(OUTPUT)

main: main.c $(HEADERS)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $<

clean:
	@rm -f $(OUTPUT)

