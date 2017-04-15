

all: test-futex-lock

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $< $(LDFLAGS)

test-futex-lock: futex-lock.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
ifneq ($(LNK_DEST),)
	rm -f $(LNK_DEST)/run-$@
	ln -s $(shell pwd)/$@ $(LNK_DEST)/run-$@
endif


clean:
	rm -f *.o test-futex-lock
