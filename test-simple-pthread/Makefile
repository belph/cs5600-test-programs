

all: test-simple-pthread

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $< $(LDFLAGS)

test-simple-pthread: test-1.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
ifneq ($(LNK_DEST),)
	rm -f $(LNK_DEST)/run-$@
	ln -s $(shell pwd)/$@ $(LNK_DEST)/run-$@
endif


clean:
	rm -f *.o test-simple-pthread
