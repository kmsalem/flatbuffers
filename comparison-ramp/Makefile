.PHONY: clean

CXX = g++
CXXFLAGS := -Wall -g -rdynamic -std=c++11 -MMD -I../../rdma-migration-system/include/ -I../../rdma-migration-system/src/
LDFLAGS := ${LDFLAGS} -lrdmacm -libverbs -lpthread
# -std=c++11: Compile with the C++11 standard.
# -MMD: Autogenerate dependency files (.d).
APPS := api_socket_experiment flatbuffer_ramp_experiment ramp_api_prepare_experiment ramp_api_experiment pack_int pack_string handcoded_pack_int handcoded_pack_string # ramp_api_buddy_alloc ramp_api_buddy_alloc_map ramp_api_bumb_alloc_map
DEPENDS = api_socket_experiment.d flatbuffer_ramp_experiment.d ramp_api_prepare_experiment.d ramp_api_experiment.d pack_int.d pack_string.d handcoded_pack_int.d handcoded_pack_string.d # ramp_api_buddy_alloc.d ramp_api_buddy_alloc_map.d ramp_api_bumb_alloc_map.d

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

handcoded_pack_int: handcoded_pack_int.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

handcoded_pack_string: handcoded_pack_string.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

#ramp_api_buddy_alloc: ramp_api_buddy_alloc.o
#	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

#ramp_api_buddy_alloc_map: ramp_api_buddy_alloc_map.o
#	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

#ramp_api_bumb_alloc_map: ramp_api_bumb_alloc_map.o
#	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

-include ${DEPENDS}

clean:
	rm ${APPS} *.o core ${DEPENDS}
