// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_VECTOREXPERIMENT_MAIN_EXPERIMENT_H_
#define FLATBUFFERS_GENERATED_VECTOREXPERIMENT_MAIN_EXPERIMENT_H_

#include "flatbuffers/flatbuffers.h"

namespace Main {
namespace Experiment {

struct Vec3T;

struct MainT;

struct ChildT;

struct Vec3T : public flatbuffers::NativeTable {
  float x;
  float y;
  float z;
  Vec3T(RampAlloc *alloc)
      : x(0.0f),
        y(0.0f),
        z(0.0f) {
  }
};

struct MainT : public flatbuffers::NativeTable {
  std::vector<int32_t, SAllocator<int32_t> > testVector1;
  std::vector<rString, SAllocator<rString> > testVector2;
  std::vector<ChildT *, SAllocator<ChildT *> > testVector3;
  MainT(RampAlloc *alloc)
      : testVector1(SAllocator<int32_t>(alloc)),
        testVector2(SAllocator<rString>(alloc)),
        testVector3(SAllocator<ChildT *>(alloc)) {
  }
};

struct ChildT : public flatbuffers::NativeTable {
  rString name;
  Vec3T * pos;
  ChildT(RampAlloc *alloc)
      : name(SAllocator<char>(alloc)) {
  }
};

}  // namespace Experiment
}  // namespace Main

#endif  // FLATBUFFERS_GENERATED_VECTOREXPERIMENT_MAIN_EXPERIMENT_H_
