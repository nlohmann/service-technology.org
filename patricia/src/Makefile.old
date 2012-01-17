CC=gcc
CFLAGS=-Wall -pthread -I.
LIBS=-lm

BIN=patricia
OBJS=main.o actions.o tools.o types.o utils.o tools/lola.o tools/sara.o

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LIBS)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f {.,tools}/*{.o,~} $(BIN)
