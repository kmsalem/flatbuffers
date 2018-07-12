.PHONY: clean

CXX = g++
CXXFLAGS := -Wall -g -rdynamic -std=c++11 -MMD -I../../rdma-migration-system/include/ -I../../rdma-migration-system/src/
LDFLAGS := ${LDFLAGS} -lrdmacm -libverbs -lpthread
# -std=c++11: Compile with the C++11 standard.
# -MMD: Autogenerate dependency files (.d).
APPS := ramp_api_experiment api_socket_experiment flatbuffer_ramp_experiment
DEPENDS = ramp_api_experiment.d api_socket_experiment.d flatbuffer_ramp_experiment.d


all: ${APPS}

ramp_api_experiment: ramp_api_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

api_socket_experiment: api_socket_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

flatbuffer_ramp_experiment: flatbuffer_ramp_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}
	
-include ${DEPENDS}

clean:
	rm ${APPS} *.o core ${DEPENDS}