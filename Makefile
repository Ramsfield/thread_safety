GXX=g++
CFLAGS=-Wall --pedantic
CLIBS=-fopenmp -pthread

EXE=threaded proc

.PHONY:all
all:$(EXE)

threaded: threaded.cpp
	${GXX} ${CFLAGS} -o threaded threaded.cpp ${CLIBS}

proc: proc.cpp
	${GXX} ${CFLAGS} -o proc proc.cpp ${CLIBS}

.PHONY:clean
clean:
	rm -rf ${EXE}
