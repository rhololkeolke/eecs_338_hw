CC = gcc
CFLAGS = -g -Wall
OUTPUT = main
LIBS = -pthread

all: $(OUTPUT)

main: main.c $(HEADERS)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $<

clean:
	@rm -f $(OUTPUT)

