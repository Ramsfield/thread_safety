GXX=g++
CFLAGS=-Wall --pedantic
CLIBS=-fopenmp -pthread

EXE=threaded proc threaded_unsafe

.PHONY:all
all:$(EXE)

threaded: threaded.cpp
	${GXX} ${CFLAGS} -o threaded threaded.cpp ${CLIBS}

proc: proc.cpp
	${GXX} ${CFLAGS} -o proc proc.cpp ${CLIBS}

threaded_unsafe: threaded_unsafe.cpp
	${GXX} ${CFLAGS} -o threaded_unsafe threaded_unsafe.cpp ${CLIBS}

.PHONY:clean
clean:
	rm -rf ${EXE}
