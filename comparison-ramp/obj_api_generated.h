// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_VECTOREXPERIMENT_COMPARISON_EXPERIMENT_H_
#define FLATBUFFERS_GENERATED_VECTOREXPERIMENT_COMPARISON_EXPERIMENT_H_

#include "flatbuffers/flatbuffers.h"

namespace Comparison {
namespace Experiment {

struct Vec3;
struct Vec3T;

struct Main;
struct MainT;

struct Child;
struct ChildT;

struct Vec3T : public flatbuffers::NativeTable {
  typedef Vec3 TableType;
  float x;
  float y;
  float z;
  Vec3T()
      : x(0.0f),
        y(0.0f),
        z(0.0f) {
  }
};

struct Vec3 FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef Vec3T NativeTableType;
  enum {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X) &&
           VerifyField<float>(verifier, VT_Y) &&
           VerifyField<float>(verifier, VT_Z) &&
           verifier.EndTable();
  }
  Vec3T *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(Vec3T *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<Vec3> Pack(flatbuffers::FlatBufferBuilder &_fbb, const Vec3T* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct Vec3Builder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Vec3::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Vec3::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Vec3::VT_Z, z, 0.0f);
  }
  explicit Vec3Builder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  Vec3Builder &operator=(const Vec3Builder &);
  flatbuffers::Offset<Vec3> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Vec3>(end);
    return o;
  }
};

inline flatbuffers::Offset<Vec3> CreateVec3(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f) {
  Vec3Builder builder_(_fbb);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

flatbuffers::Offset<Vec3> CreateVec3(flatbuffers::FlatBufferBuilder &_fbb, const Vec3T *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct MainT : public flatbuffers::NativeTable {
  typedef Main TableType;
  std::vector<int32_t> testVector1;
  std::vector<std::string> testVector2;
  std::vector<std::unique_ptr<ChildT>> testVector3;
  MainT() {
  }
};

struct Main FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef MainT NativeTableType;
  enum {
    VT_TESTVECTOR1 = 4,
    VT_TESTVECTOR2 = 6,
    VT_TESTVECTOR3 = 8
  };
  const flatbuffers::Vector<int32_t> *testVector1() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_TESTVECTOR1);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *testVector2() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_TESTVECTOR2);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Child>> *testVector3() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Child>> *>(VT_TESTVECTOR3);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_TESTVECTOR1) &&
           verifier.Verify(testVector1()) &&
           VerifyOffset(verifier, VT_TESTVECTOR2) &&
           verifier.Verify(testVector2()) &&
           verifier.VerifyVectorOfStrings(testVector2()) &&
           VerifyOffset(verifier, VT_TESTVECTOR3) &&
           verifier.Verify(testVector3()) &&
           verifier.VerifyVectorOfTables(testVector3()) &&
           verifier.EndTable();
  }
  MainT *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(MainT *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<Main> Pack(flatbuffers::FlatBufferBuilder &_fbb, const MainT* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct MainBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_testVector1(flatbuffers::Offset<flatbuffers::Vector<int32_t>> testVector1) {
    fbb_.AddOffset(Main::VT_TESTVECTOR1, testVector1);
  }
  void add_testVector2(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> testVector2) {
    fbb_.AddOffset(Main::VT_TESTVECTOR2, testVector2);
  }
  void add_testVector3(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Child>>> testVector3) {
    fbb_.AddOffset(Main::VT_TESTVECTOR3, testVector3);
  }
  explicit MainBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  MainBuilder &operator=(const MainBuilder &);
  flatbuffers::Offset<Main> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Main>(end);
    return o;
  }
};

inline flatbuffers::Offset<Main> CreateMain(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> testVector1 = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> testVector2 = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Child>>> testVector3 = 0) {
  MainBuilder builder_(_fbb);
  builder_.add_testVector3(testVector3);
  builder_.add_testVector2(testVector2);
  builder_.add_testVector1(testVector1);
  return builder_.Finish();
}

inline flatbuffers::Offset<Main> CreateMainDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<int32_t> *testVector1 = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *testVector2 = nullptr,
    const std::vector<flatbuffers::Offset<Child>> *testVector3 = nullptr) {
  return Comparison::Experiment::CreateMain(
      _fbb,
      testVector1 ? _fbb.CreateVector<int32_t>(*testVector1) : 0,
      testVector2 ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*testVector2) : 0,
      testVector3 ? _fbb.CreateVector<flatbuffers::Offset<Child>>(*testVector3) : 0);
}

flatbuffers::Offset<Main> CreateMain(flatbuffers::FlatBufferBuilder &_fbb, const MainT *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct ChildT : public flatbuffers::NativeTable {
  typedef Child TableType;
  std::string name;
  std::unique_ptr<Vec3T> pos;
  ChildT() {
  }
};

struct Child FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ChildT NativeTableType;
  enum {
    VT_NAME = 4,
    VT_POS = 6
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const Vec3 *pos() const {
    return GetPointer<const Vec3 *>(VT_POS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.Verify(name()) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           verifier.EndTable();
  }
  ChildT *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(ChildT *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<Child> Pack(flatbuffers::FlatBufferBuilder &_fbb, const ChildT* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct ChildBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Child::VT_NAME, name);
  }
  void add_pos(flatbuffers::Offset<Vec3> pos) {
    fbb_.AddOffset(Child::VT_POS, pos);
  }
  explicit ChildBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ChildBuilder &operator=(const ChildBuilder &);
  flatbuffers::Offset<Child> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Child>(end);
    return o;
  }
};

inline flatbuffers::Offset<Child> CreateChild(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<Vec3> pos = 0) {
  ChildBuilder builder_(_fbb);
  builder_.add_pos(pos);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<Child> CreateChildDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    flatbuffers::Offset<Vec3> pos = 0) {
  return Comparison::Experiment::CreateChild(
      _fbb,
      name ? _fbb.CreateString(name) : 0,
      pos);
}

flatbuffers::Offset<Child> CreateChild(flatbuffers::FlatBufferBuilder &_fbb, const ChildT *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline Vec3T *Vec3::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  auto _o = new Vec3T();
  UnPackTo(_o, _resolver);
  return _o;
}

inline void Vec3::UnPackTo(Vec3T *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = x(); _o->x = _e; };
  { auto _e = y(); _o->y = _e; };
  { auto _e = z(); _o->z = _e; };
}

inline flatbuffers::Offset<Vec3> Vec3::Pack(flatbuffers::FlatBufferBuilder &_fbb, const Vec3T* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateVec3(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<Vec3> CreateVec3(flatbuffers::FlatBufferBuilder &_fbb, const Vec3T *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const Vec3T* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _x = _o->x;
  auto _y = _o->y;
  auto _z = _o->z;
  return Comparison::Experiment::CreateVec3(
      _fbb,
      _x,
      _y,
      _z);
}

inline MainT *Main::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  auto _o = new MainT();
  UnPackTo(_o, _resolver);
  return _o;
}

inline void Main::UnPackTo(MainT *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = testVector1(); if (_e) { _o->testVector1.resize(_e->size()); for (flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { _o->testVector1[_i] = _e->Get(_i); } } };
  { auto _e = testVector2(); if (_e) { _o->testVector2.resize(_e->size()); for (flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { _o->testVector2[_i] = _e->Get(_i)->str(); } } };
  { auto _e = testVector3(); if (_e) { _o->testVector3.resize(_e->size()); for (flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { _o->testVector3[_i] = std::unique_ptr<ChildT>(_e->Get(_i)->UnPack(_resolver)); } } };
}

inline flatbuffers::Offset<Main> Main::Pack(flatbuffers::FlatBufferBuilder &_fbb, const MainT* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateMain(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<Main> CreateMain(flatbuffers::FlatBufferBuilder &_fbb, const MainT *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const MainT* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _testVector1 = _o->testVector1.size() ? _fbb.CreateVector(_o->testVector1) : 0;
  auto _testVector2 = _o->testVector2.size() ? _fbb.CreateVectorOfStrings(_o->testVector2) : 0;
  auto _testVector3 = _o->testVector3.size() ? _fbb.CreateVector<flatbuffers::Offset<Child>> (_o->testVector3.size(), [](size_t i, _VectorArgs *__va) { return CreateChild(*__va->__fbb, __va->__o->testVector3[i].get(), __va->__rehasher); }, &_va ) : 0;
  return Comparison::Experiment::CreateMain(
      _fbb,
      _testVector1,
      _testVector2,
      _testVector3);
}

inline ChildT *Child::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  auto _o = new ChildT();
  UnPackTo(_o, _resolver);
  return _o;
}

inline void Child::UnPackTo(ChildT *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); };
  { auto _e = pos(); if (_e) _o->pos = std::unique_ptr<Vec3T>(_e->UnPack(_resolver)); };
}

inline flatbuffers::Offset<Child> Child::Pack(flatbuffers::FlatBufferBuilder &_fbb, const ChildT* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateChild(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<Child> CreateChild(flatbuffers::FlatBufferBuilder &_fbb, const ChildT *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const ChildT* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  auto _pos = _o->pos ? CreateVec3(_fbb, _o->pos.get(), _rehasher) : 0;
  return Comparison::Experiment::CreateChild(
      _fbb,
      _name,
      _pos);
}

inline const Comparison::Experiment::Main *GetMain(const void *buf) {
  return flatbuffers::GetRoot<Comparison::Experiment::Main>(buf);
}

inline const Comparison::Experiment::Main *GetSizePrefixedMain(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<Comparison::Experiment::Main>(buf);
}

inline bool VerifyMainBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Comparison::Experiment::Main>(nullptr);
}

inline bool VerifySizePrefixedMainBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<Comparison::Experiment::Main>(nullptr);
}

inline void FinishMainBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Comparison::Experiment::Main> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedMainBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Comparison::Experiment::Main> root) {
  fbb.FinishSizePrefixed(root);
}

inline std::unique_ptr<MainT> UnPackMain(
    const void *buf,
    const flatbuffers::resolver_function_t *res = nullptr) {
  return std::unique_ptr<MainT>(GetMain(buf)->UnPack(res));
}

}  // namespace Experiment
}  // namespace Comparison

#endif  // FLATBUFFERS_GENERATED_VECTOREXPERIMENT_COMPARISON_EXPERIMENT_H_
