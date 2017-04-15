CC=gcc
CFLAGS=
LDFLAGS=

all: simple-ns join-ns

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

simple-ns: simple-ns.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

join-ns: join-ns.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o simple-ns join-ns
