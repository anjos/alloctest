TYPES=std-vector-int mt-vector-int std-list-int mt-list-int std-string
PROGS=$(foreach comp, ${COMPILER}, ${TYPES:%=${comp}-%})
NTHREADS=1 2 3 4 6 8 10
NCYCLES=1000000
TESTOUT=${COMPILER}-test-out.csv

all: ${PROGS}

%-std-string: string.cxx
	${CXX} ${CXXFLAGS} $< -o $@

%-std-vector-int: vector.cxx
	${CXX} ${CXXFLAGS} $< -o $@

%-mt-vector-int: vector.cxx
	${CXX} ${CXXFLAGS} -DMTALLOC=1 $< -o $@

%-std-list-int: list.cxx
	${CXX} ${CXXFLAGS} -DLIST=1 $< -o $@

%-mt-list-int: list.cxx
	${CXX} ${CXXFLAGS} -DMTALLOC=1 -DLIST=1 $< -o $@

.PHONY: clean test

clean:
	rm -rf ${PROGS} *~ ${TESTOUT}

test: all
	@cat header.txt > ${TESTOUT}
	@for p in ${PROGS}; do \
		for t in ${NTHREADS}; do \
			for c in ${NCYCLES}; do \
				echo -n "Testing $$p $$t $$c..."; \
				./$$p $$t $$c >> ${TESTOUT}; \
				echo "done"; \
			done \
		done \
	done
