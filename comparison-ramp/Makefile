.PHONY: clean

CXX = g++
CXXFLAGS := -Wall -g -rdynamic -std=c++11 -MMD -I../../rdma-migration-system/include/ -I../../rdma-migration-system/src/
LDFLAGS := ${LDFLAGS} -lrdmacm -libverbs -lpthread
# -std=c++11: Compile with the C++11 standard.
# -MMD: Autogenerate dependency files (.d).
APPS := ramp_api_experiment ramp_api_prepare_experiment api_socket_experiment flatbuffer_ramp_experiment pack_int pack_string
DEPENDS = ramp_api_experiment.d ramp_api_prepare_experiment.d api_socket_experiment.d flatbuffer_ramp_experiment.d pack_int.d pack_string.d

all: ${APPS}

api_socket_experiment: api_socket_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

flatbuffer_ramp_experiment: flatbuffer_ramp_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

ramp_api_prepare_experiment: ramp_api_prepare_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

ramp_api_experiment: ramp_api_experiment.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

pack_int: pack_int.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

pack_string: pack_string.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

-include ${DEPENDS}

clean:
	rm ${APPS} *.o core ${DEPENDS}
