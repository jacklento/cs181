
include ../makefile.inc

TESTBIN = p1test

all: librbf.a rbftest

test:
	make pretests
	make -f makefile.test

# lib file dependencies
librbf.a: librbf.a(pfm.o)  # and possibly other .o files
librbf.a: librbf.a(rbfm.o)

# c file dependencies
pfm.o: pfm.h
rbfm.o: rbfm.h

rbftest.o: pfm.h rbfm.h 

# binary dependencies
rbftest: rbftest.o librbf.a $(CODEROOT)/rbf/librbf.a


# ---- [ADDED] 
# project1 test
pretests:
	touch 00_exist.t
	touch 02_destroy.t
	touch 03_01_open.t
# ---- [/ADDED]


# dependencies to compile used libraries
.PHONY: $(CODEROOT)/rbf/librbf.a
$(CODEROOT)/rbf/librbf.a:
	$(MAKE) -C $(CODEROOT)/rbf librbf.a

.PHONY: clean
clean:
	-rm rbftest rbftest11a rbftest11b ${TESTBIN} *.a *.o *~ *.t *.out test_1
