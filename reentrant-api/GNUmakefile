# Fuck gmake in the ear for existing and Linux for spreading all of
# its little ass-backwards, incompatible make syntax bullshit.

PROG = example
CXXFLAGS += -I${BOOST_INCDIR} -std=c++11 -stdlib=libc++
LDFLAGS += -stdlib=libc++ -L${BOOST_LIBDIR}

all: ${PROG}

clean::
	rm -f *.o ${PROG}

example.o:	example.cpp example.hpp
	${CXX} ${CXXFLAGS} -c -o $@ example.cpp

example_main.o:	example_main.cpp example.hpp
	${CXX} ${CXXFLAGS} -c -o $@ example_main.cpp

example:	example.o example_main.o
	${CXX} ${LDFLAGS} -o $@ $^ ${LDFLAGS} -lboost_system-mt -lboost_thread-mt
