// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TESTOBJ_MY_TEST_H_
#define FLATBUFFERS_GENERATED_TESTOBJ_MY_TEST_H_

#include "flatbuffers/flatbuffers.h"

namespace my {
namespace test {

struct Person;

struct Class;

enum Gender {
  Gender_Male = 1,
  Gender_Female = 2,
  Gender_NONE = 0,
  Gender_ANY = 3
};

inline const Gender (&EnumValuesGender())[2] {
  static const Gender values[] = {
    Gender_Male,
    Gender_Female
  };
  return values;
}

inline const char * const *EnumNamesGender() {
  static const char * const names[] = {
    "Male",
    "Female",
    nullptr
  };
  return names;
}

inline const char *EnumNameGender(Gender e) {
  const size_t index = static_cast<int>(e) - static_cast<int>(Gender_Male);
  return EnumNamesGender()[index];
}

struct Person FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_NAME = 4,
    VT_GENDER = 6,
    VT_AGE = 8
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  Gender gender() const {
    return static_cast<Gender>(GetField<int8_t>(VT_GENDER, 0));
  }
  int32_t age() const {
    return GetField<int32_t>(VT_AGE, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.Verify(name()) &&
           VerifyField<int8_t>(verifier, VT_GENDER) &&
           VerifyField<int32_t>(verifier, VT_AGE) &&
           verifier.EndTable();
  }
};

struct PersonBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Person::VT_NAME, name);
  }
  void add_gender(Gender gender) {
    fbb_.AddElement<int8_t>(Person::VT_GENDER, static_cast<int8_t>(gender), 0);
  }
  void add_age(int32_t age) {
    fbb_.AddElement<int32_t>(Person::VT_AGE, age, 0);
  }
  explicit PersonBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PersonBuilder &operator=(const PersonBuilder &);
  flatbuffers::Offset<Person> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Person>(end);
    return o;
  }
};

inline flatbuffers::Offset<Person> CreatePerson(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    Gender gender = static_cast<Gender>(0),
    int32_t age = 0) {
  PersonBuilder builder_(_fbb);
  builder_.add_age(age);
  builder_.add_name(name);
  builder_.add_gender(gender);
  return builder_.Finish();
}

inline flatbuffers::Offset<Person> CreatePersonDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    Gender gender = static_cast<Gender>(0),
    int32_t age = 0) {
  return my::test::CreatePerson(
      _fbb,
      name ? _fbb.CreateString(name) : 0,
      gender,
      age);
}

struct Class FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_NAME = 4,
    VT_STUDENTS = 6,
    VT_TEACHER = 8
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Person>> *students() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Person>> *>(VT_STUDENTS);
  }
  const Person *teacher() const {
    return GetPointer<const Person *>(VT_TEACHER);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.Verify(name()) &&
           VerifyOffset(verifier, VT_STUDENTS) &&
           verifier.Verify(students()) &&
           verifier.VerifyVectorOfTables(students()) &&
           VerifyOffset(verifier, VT_TEACHER) &&
           verifier.VerifyTable(teacher()) &&
           verifier.EndTable();
  }
};

struct ClassBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Class::VT_NAME, name);
  }
  void add_students(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Person>>> students) {
    fbb_.AddOffset(Class::VT_STUDENTS, students);
  }
  void add_teacher(flatbuffers::Offset<Person> teacher) {
    fbb_.AddOffset(Class::VT_TEACHER, teacher);
  }
  explicit ClassBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ClassBuilder &operator=(const ClassBuilder &);
  flatbuffers::Offset<Class> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Class>(end);
    return o;
  }
};

inline flatbuffers::Offset<Class> CreateClass(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Person>>> students = 0,
    flatbuffers::Offset<Person> teacher = 0) {
  ClassBuilder builder_(_fbb);
  builder_.add_teacher(teacher);
  builder_.add_students(students);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<Class> CreateClassDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    const std::vector<flatbuffers::Offset<Person>> *students = nullptr,
    flatbuffers::Offset<Person> teacher = 0) {
  return my::test::CreateClass(
      _fbb,
      name ? _fbb.CreateString(name) : 0,
      students ? _fbb.CreateVector<flatbuffers::Offset<Person>>(*students) : 0,
      teacher);
}

inline const my::test::Class *GetClass(const void *buf) {
  return flatbuffers::GetRoot<my::test::Class>(buf);
}

inline const my::test::Class *GetSizePrefixedClass(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<my::test::Class>(buf);
}

inline bool VerifyClassBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<my::test::Class>(nullptr);
}

inline bool VerifySizePrefixedClassBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<my::test::Class>(nullptr);
}

inline void FinishClassBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<my::test::Class> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedClassBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<my::test::Class> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace test
}  // namespace my

#endif  // FLATBUFFERS_GENERATED_TESTOBJ_MY_TEST_H_
