# Fuck gmake in the ear for existing and Linux for spreading all of its little ass-backwards, incompatible make syntax bullshit.

PTR_EXAMPLE = pimpl_example_pointer
VAL_EXAMPLE = pimpl_example_value
PROGS       = ${PTR_EXAMPLE} ${VAL_EXAMPLE}
CXXFLAGS += -I${BOOST_INCDIR}

all: ${PROGS}

clean::
	rm -f *.o ${PROGS}

${PTR_EXAMPLE}:	${PTR_EXAMPLE}.o ${PTR_EXAMPLE}_impl.o
	${CXX} -o $@ $^

${PTR_EXAMPLE}.o:	${PTR_EXAMPLE}.cc ${PTR_EXAMPLE}.h
	${CXX} -c -o $@ ${CXXFLAGS} $<

${PTR_EXAMPLE}_impl.o:	${PTR_EXAMPLE}_impl.cc ${PTR_EXAMPLE}.h
	${CXX} -c -o $@ ${CXXFLAGS} $<

${VAL_EXAMPLE}:	${VAL_EXAMPLE}.o ${VAL_EXAMPLE}_impl.o
	${CXX} -o $@ $^

${VAL_EXAMPLE}.o:	${VAL_EXAMPLE}.cc ${VAL_EXAMPLE}.h
	${CXX} -c -o $@ ${CXXFLAGS} $<

${VAL_EXAMPLE}_impl.o:	${VAL_EXAMPLE}_impl.cc ${VAL_EXAMPLE}.h
	${CXX} -c -o $@ ${CXXFLAGS} $<
