# CS 5600 PID Namespacing Test Programs

Here are a small series of programs to play with in the DMTCP 
coordinator which are meant to stress-test the PID namespacing
implementation.

To build, run `make`.

**NOTE:** DMTCP will not like the symlinks created by `make`, so,
unless you're using `checkpoint.sh`, you will need to use the actual
programs instead (under their respective folders).

Additionally, `simple-ns` contains a simple example of creating and
joining a PID namespace.

Things are a bit messy, as this is all under construction!
