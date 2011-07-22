# Fuck gmake in the ear for existing and Linux for spreading all of its
# little ass-backwards, incompatible make syntax bullshit.

PROG        = time_unittests
CPPFLAGS   += -I${BOOST_INCDIR}
CXXFLAGS   += -g -Wall
LIBS       += -lboost_unit_test_framework-mt
LDFLAGS    += -L${BOOST_LIBDIR}

all: ${PROG}

clean::
	rm -f *.o ${PROG}

${PROG}:	${PROG}.o
	${CXX} ${CXXFLAGS} -o $@ ${LDFLAGS} $^ ${LIBS}

${PROG}.o:	${PROG}.cc
	${CXX} ${CXXFLAGS} -c -o $@ ${CPPFLAGS} $<
