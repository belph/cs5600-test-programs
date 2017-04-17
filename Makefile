CC=gcc
CFLAGS=-pthread -g
LDFLAGS=-lpthread
LNK_DEST=$(shell pwd)

export CC
export CFLAGS
export LDFLAGS
export LNK_DEST

all: run-test-simple-pthread run-t-test run-test-futex-lock run-test-fork run-test-basic

run-test-simple-pthread: $(wildcard test-simple-pthread/*.c)
	$(MAKE) -C test-simple-pthread

run-t-test: $(wildcard t-test1/*.c)
	$(MAKE) -C t-test1

run-test-futex-lock: $(wildcard futex-lock/*.c)
	$(MAKE) -C futex-lock

run-test-fork: $(wildcard test-fork/*.c)
	$(MAKE) -C test-fork

run-test-basic: $(wildcard test-basic/*.c)
	$(MAKE) -C test-basic

.PHONY: clean
clean:
	$(MAKE) -C t-test1 clean
	$(MAKE) -C test-simple-pthread clean
	$(MAKE) -C futex-lock clean
	$(MAKE) -C test-fork clean
	$(MAKE) -C test-basic clean
	rm -f ./run-*

.PHONY: clean-all
clean-all: clean
	rm -f dmtcp_restart*.sh
	rm -f ckpt_*.dmtcp

.PHONY: dist
dist:
	(cd .. && tar czf cs-5600-project.tar.gz cs-5600-project)
