
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++17 -Wall

run_test: maxtime_test
	./maxtime_test

headers: rubrictest.hh maxtime.hh

maxtime_test: headers maxtime_test.cc
	${CXX} maxtime_test.cc -o maxtime_test

clean:
	rm -f maxtime_test
