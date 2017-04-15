
all: t-test

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $< $(LDFLAGS)

t-test: t-test1.o
	$(CC) $(CFLAGS) $< -o $@
ifneq ($(LNK_DEST),)
	rm -f $(LNK_DEST)/run-$@
	ln -s $(shell pwd)/$@ $(LNK_DEST)/run-$@
endif

clean:
	rm -f *.o t-test
