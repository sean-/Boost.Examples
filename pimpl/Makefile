PROG=pimpl_example_pointer
CXXFLAGS?=-I${BOOST_INCDIR}

all: ${PROG}

clean::
	rm -f *.o ${PROG}

${PROG}:	${PROG}.o ${PROG}_impl.o
	${CXX} -o $@ $^

${PROG}.o:	${PROG}.cc ${PROG}.h
	${CXX} -c -o $@ ${CXXFLAGS} $<

${PROG}_impl.o:	${PROG}_impl.cc ${PROG}.h
	${CXX} -c -o $@ ${CXXFLAGS} $<
