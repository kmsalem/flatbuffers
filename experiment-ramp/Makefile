.PHONY: clean

CXX = g++
CXXFLAGS := -Wall -g -rdynamic -std=c++11 -MMD -I../../rdma-migration-system/include/ -I../../rdma-migration-system/src/
LDFLAGS := ${LDFLAGS} -lrdmacm -libverbs -lpthread
# -std=c++11: Compile with the C++11 standard.
# -MMD: Autogenerate dependency files (.d).
APPS := vector_experiment1 vector_experiment2 vector_experiment3 map_experiment
DEPENDS = vector_experiment1.d vector_experiment2.d vector_experiment3.d map_experiment.d

all: ${APPS}

vector_experiment1: vector_experiment1.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

vector_experiment2: vector_experiment2.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

vector_experiment3: vector_experiment3.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

map_experiment3: map_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

-include ${DEPENDS}

clean:
	rm ${APPS} *.o core ${DEPENDS}