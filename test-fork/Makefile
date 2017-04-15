

all: test-fork

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $< $(LDFLAGS)

test-fork: test-fork.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
ifneq ($(LNK_DEST),)
	rm -f $(LNK_DEST)/run-$@
	ln -s $(shell pwd)/$@ $(LNK_DEST)/run-$@
endif


clean:
	rm -f *.o test-fork
