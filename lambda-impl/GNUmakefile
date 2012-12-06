# Fuck gmake in the ear for existing and Linux for spreading all of
# its little ass-backwards, incompatible make syntax bullshit.

PROG_FOO_CPP03 = foo_cpp03
PROG_FOO_CPP11 = foo_cpp11

PROGS = ${PROG_FOO_CPP03} ${PROG_FOO_CPP11}

CXXFLAGS += -std=c++11 -stdlib=libc++
LDFLAGS += -stdlib=libc++

all: ${PROGS}

clean::
	rm -f *.o ${PROGS}

${PROG_FOO_CPP03}:	foo_cpp03.cpp foo.hpp
	${CXX} ${CXXFLAGS} -o $@ foo_cpp03.cpp

${PROG_FOO_CPP11}:	foo_cpp11.cpp foo.hpp
	${CXX} ${CXXFLAGS} -o $@ foo_cpp11.cpp
