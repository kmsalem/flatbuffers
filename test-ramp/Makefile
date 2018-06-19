.PHONY: clean

CXX = g++
CXXFLAGS := -Wall -g -rdynamic -std=c++11 -MMD -I../../rdma-migration-system/include/ -I../../rdma-migration-system/src/
LDFLAGS := ${LDFLAGS} -lrdmacm -libverbs -lpthread
# -std=c++11: Compile with the C++11 standard.
# -MMD: Autogenerate dependency files (.d).
APPS := myTest_server myTest_client nativeStruct_server nativeStruct_client fbNativeStruct_server fbNativeStruct_client
DEPENDS = myTest_server.d myTest_client.d nativeStruct_server.d nativeStruct_client.d fbNativeStruct_server.d fbNativeStruct_client.d

all: ${APPS}

#myTest_server: myTest_server.o
#	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

#myTest_client: myTest_client.o
#	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

nativeStruct_server: nativeStruct_server.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

nativeStruct_client: nativeStruct_client.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

fbNativeStruct_server: fbNativeStruct_server.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

fbNativeStruct_client: fbNativeStruct_client.o
	${CXX} -o $@ $^ ${CXXFLAGS} ${LDFLAGS}

-include ${DEPENDS}

clean:
	rm ${APPS} *.o core ${DEPENDS}
