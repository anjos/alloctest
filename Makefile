COMPILERS=gcc295 gcc323 gcc343 icc8

all: ${COMPILERS}

.PHONY: ${COMPILERS}

gcc295:	
	@export PATH=/usr/local/gcc-alt-2.95.2/bin:$(PATH); \
	export LD_LIBRARY_PATH=/usr/local/gcc-alt-2.95.2/lib:$(LD_LIBRARY_PATH); \
	type g++; \
	COMPILER=gcc295 CXX=g++ CXXFLAGS="-O3 -pthread" /usr/bin/make -f work.mk all test;

gcc323:
	@export PATH=/usr/local/gcc-alt-3.2.3/bin:$(PATH); \
	export LD_LIBRARY_PATH=/usr/local/gcc-alt-3.2.3/lib:$(LD_LIBRARY_PATH); \
	type g++; \
	COMPILER=gcc323 CXX=g++ CXXFLAGS="-O3 -pthread" /usr/bin/make -f work.mk all test;

gcc343:
	@export PATH=/usr/local/gcc-alt-3.4.3/bin:$(PATH); \
	export LD_LIBRARY_PATH=/usr/local/gcc-alt-3.4.3/lib:$(LD_LIBRARY_PATH); \
	type g++; \
	COMPILER=gcc343 CXX=g++ CXXFLAGS="-O3 -pthread" /usr/bin/make -f work.mk all test;

icc8:
	source /afs/cern.ch/sw/lcg/external/icc/7.1.006/rh73/opt/intel/compiler70/ia32/bin/iccvars.sh; \
	type icc; \
	CXX=icc CXXFLAGS="-O2 -pthread" COMPILER=icc8 /usr/bin/make -f work.mk all test;

clean:
	rm -rf *~ gcc* icc*

