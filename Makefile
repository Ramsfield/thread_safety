GXX=g++
CFLAGS=-Wall --pedantic
CLIBS=-fopenmp -pthread

EXE=threaded 

.PHONY:all
all: threaded

threaded: threaded.cpp
	${GXX} ${CFLAGS} -o threaded threaded.cpp ${CLIBS}

.PHONY:clean
clean:
	rm -rf ${EXE}
