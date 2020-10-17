// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.
#pragma once

#include <logger/Logger.h>

#include <assert.h>
#include <cstddef>
#include <exception>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#undef GOOGLE_DISALLOW_EVIL_CONSTRUCTORS
#define GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(TypeName)                            \
  TypeName(const TypeName &);                                                  \
  void operator=(const TypeName &)

typedef unsigned int uint;

#ifdef _MSC_VER
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#else
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif

// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#undef GOOGLE_LONGLONG
#undef GOOGLE_ULONGLONG
#undef GOOGLE_LL_FORMAT

#ifdef _MSC_VER
#define GOOGLE_LONGLONG(x) x##I64
#define GOOGLE_ULONGLONG(x) x##UI64
#define GOOGLE_LL_FORMAT "I64" // As in printf("%I64d", ...)
#else
#define GOOGLE_LONGLONG(x) x##LL
#define GOOGLE_ULONGLONG(x) x##ULL
#define GOOGLE_LL_FORMAT "ll" // As in "%lld". Note that "q" is poor form also.
#endif

static const int32 kint32max = 0x7FFFFFFF;
static const int32 kint32min = -kint32max - 1;
static const int64 kint64max = GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64 kint64min = -kint64max - 1;
static const uint32 kuint32max = 0xFFFFFFFFu;
static const uint64 kuint64max = GOOGLE_ULONGLONG(0xFFFFFFFFFFFFFFFF);

// ===================================================================
// from google3/base/basictypes.h

// The GOOGLE_ARRAYSIZE(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.
//
// GOOGLE_ARRAYSIZE catches a few type errors.  If you see a compiler error
//
//   "warning: division by zero in ..."
//
// when using GOOGLE_ARRAYSIZE, you are (wrongfully) giving it a pointer.
// You should only use GOOGLE_ARRAYSIZE on statically allocated arrays.
//
// The following comments are on the implementation details, and can
// be ignored by the users.
//
// ARRAYSIZE(arr) works by inspecting sizeof(arr) (the # of bytes in
// the array) and sizeof(*(arr)) (the # of bytes in one array
// element).  If the former is divisible by the latter, perhaps arr is
// indeed an array, in which case the division result is the # of
// elements in the array.  Otherwise, arr cannot possibly be an array,
// and we generate a compiler error to prevent the code from
// compiling.
//
// Since the size of bool is implementation-defined, we need to cast
// !(sizeof(a) & sizeof(*(a))) to size_t in order to ensure the final
// result has type size_t.
//
// This macro is not perfect as it wrongfully accepts certain
// pointers, namely where the pointer size is divisible by the pointee
// size.  Since all our code has to go through a 32-bit compiler,
// where a pointer is 4 bytes, this means all pointers to a type whose
// size is 3 or greater than 4 will be (righteously) rejected.
//
// Kudos to Jorg Brown for this simple and elegant implementation.

#undef GOOGLE_ARRAYSIZE
#define GOOGLE_ARRAYSIZE(a)                                                    \
  ((sizeof(a) / sizeof(*(a))) /                                                \
   static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template <typename To, typename From> inline To implicit_cast(From const &f) {
  return f;
}

// When you upcast (that is, cast a pointer from type Foo to type
// SuperclassOfFoo), it's fine to use implicit_cast<>, since upcasts
// always succeed.  When you downcast (that is, cast a pointer from
// type Foo to type SubclassOfFoo), static_cast<> isn't safe, because
// how do you know the pointer is really of type SubclassOfFoo?  It
// could be a bare Foo, or of type DifferentSubclassOfFoo.  Thus,
// when you downcast, you should use this macro.  In debug mode, we
// use dynamic_cast<> to double-check the downcast is legal (we die
// if it's not).  In normal mode, we do the efficient static_cast<>
// instead.  Thus, it's important to test in debug mode to make sure
// the cast is legal!
//    This is the only place in the code we should use dynamic_cast<>.
// In particular, you SHOULDN'T be using dynamic_cast<> in order to
// do RTTI (eg code like this:
//    if (dynamic_cast<Subclass1>(foo)) HandleASubclass1Object(foo);
//    if (dynamic_cast<Subclass2>(foo)) HandleASubclass2Object(foo);
// You should design the code some other way not to need this.

template <typename To, typename From> // use like this: down_cast<T*>(foo);
inline To down_cast(From *f) {        // so we only accept pointers
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false) {
    implicit_cast<From *, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL); // RTTI: debug mode only!
#endif
  return static_cast<To>(f);
}


// The COMPILE_ASSERT macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the
// size of a static array:
//
//   COMPILE_ASSERT(ARRAYSIZE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   COMPILE_ASSERT(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.


template <bool> struct CompileAssert {};


#undef GOOGLE_COMPILE_ASSERT
#define GOOGLE_COMPILE_ASSERT(expr, msg)                                       \
  typedef CompileAssert<(bool(expr))>            \
      msg[bool(expr) ? 1 : -1]

// Implementation details of COMPILE_ASSERT:
//
// - COMPILE_ASSERT works by defining an array type that has -1
//   elements (and thus is invalid) when the expression is false.
//
// - The simpler definition
//
//     #define COMPILE_ASSERT(expr, msg) typedef char msg[(expr) ? 1 : -1]
//
//   does not work, as gcc supports variable-length arrays whose sizes
//   are determined at run-time (this is gcc's extension and not part
//   of the C++ standard).  As a result, gcc fails to reject the
//   following code with the simple definition:
//
//     int foo;
//     COMPILE_ASSERT(foo, msg); // not supposed to compile as foo is
//                               // not a compile-time constant.
//
// - By using the type CompileAssert<(bool(expr))>, we ensures that
//   expr is a compile-time constant.  (Template arguments must be
//   determined at compile-time.)
//
// - The outter parentheses in CompileAssert<(bool(expr))> are necessary
//   to work around a bug in gcc 3.4.4 and 4.0.1.  If we had written
//
//     CompileAssert<bool(expr)>
//
//   instead, these compilers will refuse to compile
//
//     COMPILE_ASSERT(5 > 0, some_message);
//
//   (They seem to think the ">" in "5 > 0" marks the end of the
//   template argument list.)
//
// - The array size is (bool(expr) ? 1 : -1), instead of simply
//
//     ((expr) ? 1 : -1).
//
//   This is to avoid running into a bug in MS VC 7.1, which
//   causes ((0.0) ? 1 : -1) to incorrectly evaluate to 1.

// ===================================================================
// from google3/base/scoped_ptr.h

//  This is an implementation designed to match the anticipated future TR2
//  implementation of the scoped_ptr class, and its closely-related brethren,
//  scoped_array, scoped_ptr_malloc, and make_scoped_ptr.

template <class C> class scoped_ptr;
template <class C> class scoped_array;

// A scoped_ptr<T> is like a T*, except that the destructor of scoped_ptr<T>
// automatically deletes the pointer it holds (if any).
// That is, scoped_ptr<T> owns the T object that it points to.
// Like a T*, a scoped_ptr<T> may hold either NULL or a pointer to a T object.
//
// The size of a scoped_ptr is small:
// sizeof(scoped_ptr<C>) == sizeof(C*)
template <class C> class scoped_ptr {
public:
  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_ptr.
  // The input parameter must be allocated with new.
  explicit scoped_ptr(C *p = NULL) : ptr_(p) {}

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(C) };
    delete ptr_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C *p = NULL) {
    if (p != ptr_) {
      enum { type_must_be_complete = sizeof(C) };
      delete ptr_;
      ptr_ = p;
    }
  }

  // Accessors to get the owned object.
  // operator* and operator-> will assert() if there is no current object.
  C &operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }
  C *operator->() const {
    assert(ptr_ != NULL);
    return ptr_;
  }
  C *get() const { return ptr_; }

  // Comparison operators.
  // These return whether two scoped_ptr refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C *p) const { return ptr_ == p; }
  bool operator!=(C *p) const { return ptr_ != p; }

  // Swap two scoped pointers.
  void swap(scoped_ptr &p2) {
    C *tmp = ptr_;
    ptr_ = p2.ptr_;
    p2.ptr_ = tmp;
  }

  // Release a pointer.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C *release() {
    C *retVal = ptr_;
    ptr_ = NULL;
    return retVal;
  }

private:
  C *ptr_;

  // Forbid comparison of scoped_ptr types.  If C2 != C, it totally doesn't
  // make sense, and if C2 == C, it still doesn't make sense because you should
  // never have the same object owned by two different scoped_ptrs.
  template <class C2> bool operator==(scoped_ptr<C2> const &p2) const;
  template <class C2> bool operator!=(scoped_ptr<C2> const &p2) const;

  // Disallow evil constructors
  scoped_ptr(const scoped_ptr &);
  void operator=(const scoped_ptr &);
};

// scoped_array<C> is like scoped_ptr<C>, except that the caller must allocate
// with new [] and the destructor deletes objects with delete [].
//
// As with scoped_ptr<C>, a scoped_array<C> either points to an object
// or is NULL.  A scoped_array<C> owns the object that it points to.
//
// Size: sizeof(scoped_array<C>) == sizeof(C*)
template <class C> class scoped_array {
public:
  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_array.
  // The input parameter must be allocated with new [].
  explicit scoped_array(C *p = NULL) : array_(p) {}

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_array() {
    enum { type_must_be_complete = sizeof(C) };
    delete[] array_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C *p = NULL) {
    if (p != array_) {
      enum { type_must_be_complete = sizeof(C) };
      delete[] array_;
      array_ = p;
    }
  }

  // Get one element of the current object.
  // Will assert() if there is no current object, or index i is negative.
  C &operator[](std::ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  // Get a pointer to the zeroth element of the current object.
  // If there is no current object, return NULL.
  C *get() const { return array_; }

  // Comparison operators.
  // These return whether two scoped_array refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C *p) const { return array_ == p; }
  bool operator!=(C *p) const { return array_ != p; }

  // Swap two scoped arrays.
  void swap(scoped_array &p2) {
    C *tmp = array_;
    array_ = p2.array_;
    p2.array_ = tmp;
  }

  // Release an array.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C *release() {
    C *retVal = array_;
    array_ = NULL;
    return retVal;
  }

private:
  C *array_;

  // Forbid comparison of different scoped_array types.
  template <class C2> bool operator==(scoped_array<C2> const &p2) const;
  template <class C2> bool operator!=(scoped_array<C2> const &p2) const;

  // Disallow evil constructors
  scoped_array(const scoped_array &);
  void operator=(const scoped_array &);
};


template <typename T>
T *CheckNotNull(const char *file, int line, const char *name, T *val) {
  if (val == NULL) {
    LOG_FATAL << name;
  }
  return val;
}

// ===================================================================
// emulates google3/base/callback.h

// Abstract interface for a callback.  When calling an RPC, you must provide
// a Closure to call when the procedure completes.  See the Service interface
// in service.h.
//
// To automatically construct a Closure which calls a particular function or
// method with a particular set of parameters, use the NewCallback() function.
// Example:
//   void FooDone(const FooResponse* response) {
//     ...
//   }
//
//   void CallFoo() {
//     ...
//     // When done, call FooDone() and pass it a pointer to the response.
//     Closure* callback = NewCallback(&FooDone, response);
//     // Make the call.
//     service->Foo(controller, request, response, callback);
//   }
//
// Example that calls a method:
//   class Handler {
//    public:
//     ...
//
//     void FooDone(const FooResponse* response) {
//       ...
//     }
//
//     void CallFoo() {
//       ...
//       // When done, call FooDone() and pass it a pointer to the response.
//       Closure* callback = NewCallback(this, &Handler::FooDone, response);
//       // Make the call.
//       service->Foo(controller, request, response, callback);
//     }
//   };
//
// Currently NewCallback() supports binding zero, one, or two arguments.
//
// Callbacks created with NewCallback() automatically delete themselves when
// executed.  They should be used when a callback is to be called exactly
// once (usually the case with RPC callbacks).  If a callback may be called
// a different number of times (including zero), create it with
// NewPermanentCallback() instead.  You are then responsible for deleting the
// callback (using the "delete" keyword as normal).
//
// Note that NewCallback() is a bit touchy regarding argument types.  Generally,
// the values you provide for the parameter bindings must exactly match the
// types accepted by the callback function.  For example:
//   void Foo(string s);
//   NewCallback(&Foo, "foo");          // WON'T WORK:  const char* != string
//   NewCallback(&Foo, string("foo"));  // WORKS
// Also note that the arguments cannot be references:
//   void Foo(const string& s);
//   string my_str;
//   NewCallback(&Foo, my_str);  // WON'T WORK:  Can't use referecnes.
// However, correctly-typed pointers will work just fine.
class Closure {
public:
  Closure() {}
  virtual ~Closure(){};

  virtual void Run() = 0;

private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Closure);
};


class FunctionClosure0 : public Closure {
public:
  typedef void (*FunctionType)();

  FunctionClosure0(FunctionType function, bool self_deleting)
      : function_(function), self_deleting_(self_deleting) {}
  ~FunctionClosure0(){};

  void Run() {
    bool needs_delete = self_deleting_; // read in case callback deletes
    function_();
    if (needs_delete)
      delete this;
  }

private:
  FunctionType function_;
  bool self_deleting_;
};

template <typename Class> class MethodClosure0 : public Closure {
public:
  typedef void (Class::*MethodType)();

  MethodClosure0(Class *object, MethodType method, bool self_deleting)
      : object_(object), method_(method), self_deleting_(self_deleting) {}
  ~MethodClosure0() {}

  void Run() {
    bool needs_delete = self_deleting_; // read in case callback deletes
    (object_->*method_)();
    if (needs_delete)
      delete this;
  }

private:
  Class *object_;
  MethodType method_;
  bool self_deleting_;
};

template <typename Arg1> class FunctionClosure1 : public Closure {
public:
  typedef void (*FunctionType)(Arg1 arg1);

  FunctionClosure1(FunctionType function, bool self_deleting, Arg1 arg1)
      : function_(function), self_deleting_(self_deleting), arg1_(arg1) {}
  ~FunctionClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_; // read in case callback deletes
    function_(arg1_);
    if (needs_delete)
      delete this;
  }

private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Class, typename Arg1> class MethodClosure1 : public Closure {
public:
  typedef void (Class::*MethodType)(Arg1 arg1);

  MethodClosure1(Class *object, MethodType method, bool self_deleting,
                 Arg1 arg1)
      : object_(object), method_(method), self_deleting_(self_deleting),
        arg1_(arg1) {}
  ~MethodClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_; // read in case callback deletes
    (object_->*method_)(arg1_);
    if (needs_delete)
      delete this;
  }

private:
  Class *object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Arg1, typename Arg2>
class FunctionClosure2 : public Closure {
public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

  FunctionClosure2(FunctionType function, bool self_deleting, Arg1 arg1,
                   Arg2 arg2)
      : function_(function), self_deleting_(self_deleting), arg1_(arg1),
        arg2_(arg2) {}
  ~FunctionClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_; // read in case callback deletes
    function_(arg1_, arg2_);
    if (needs_delete)
      delete this;
  }

private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template <typename Class, typename Arg1, typename Arg2>
class MethodClosure2 : public Closure {
public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

  MethodClosure2(Class *object, MethodType method, bool self_deleting,
                 Arg1 arg1, Arg2 arg2)
      : object_(object), method_(method), self_deleting_(self_deleting),
        arg1_(arg1), arg2_(arg2) {}
  ~MethodClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_; // read in case callback deletes
    (object_->*method_)(arg1_, arg2_);
    if (needs_delete)
      delete this;
  }

private:
  Class *object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};


// See Closure.
inline Closure *NewCallback(void (*function)()) {
  return new FunctionClosure0(function, true);
}

// See Closure.
inline Closure *NewPermanentCallback(void (*function)()) {
  return new FunctionClosure0(function, false);
}

// See Closure.
template <typename Class>
inline Closure *NewCallback(Class *object, void (Class::*method)()) {
  return new MethodClosure0<Class>(object, method, true);
}

// See Closure.
template <typename Class>
inline Closure *NewPermanentCallback(Class *object, void (Class::*method)()) {
  return new MethodClosure0<Class>(object, method, false);
}

// See Closure.
template <typename Arg1>
inline Closure *NewCallback(void (*function)(Arg1), Arg1 arg1) {
  return new FunctionClosure1<Arg1>(function, true, arg1);
}

// See Closure.
template <typename Arg1>
inline Closure *NewPermanentCallback(void (*function)(Arg1), Arg1 arg1) {
  return new FunctionClosure1<Arg1>(function, false, arg1);
}

// See Closure.
template <typename Class, typename Arg1>
inline Closure *NewCallback(Class *object, void (Class::*method)(Arg1),
                            Arg1 arg1) {
  return new MethodClosure1<Class, Arg1>(object, method, true, arg1);
}

// See Closure.
template <typename Class, typename Arg1>
inline Closure *NewPermanentCallback(Class *object, void (Class::*method)(Arg1),
                                     Arg1 arg1) {
  return new MethodClosure1<Class, Arg1>(object, method, false, arg1);
}

// See Closure.
template <typename Arg1, typename Arg2>
inline Closure *NewCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
                            Arg2 arg2) {
  return new FunctionClosure2<Arg1, Arg2>(function, true, arg1, arg2);
}

// See Closure.
template <typename Arg1, typename Arg2>
inline Closure *NewPermanentCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
                                     Arg2 arg2) {
  return new FunctionClosure2<Arg1, Arg2>(function, false, arg1,
                                                    arg2);
}

// See Closure.
template <typename Class, typename Arg1, typename Arg2>
inline Closure *NewCallback(Class *object, void (Class::*method)(Arg1, Arg2),
                            Arg1 arg1, Arg2 arg2) {
  return new MethodClosure2<Class, Arg1, Arg2>(object, method, true,
                                                         arg1, arg2);
}

// See Closure.
template <typename Class, typename Arg1, typename Arg2>
inline Closure *NewPermanentCallback(Class *object,
                                     void (Class::*method)(Arg1, Arg2),
                                     Arg1 arg1, Arg2 arg2) {
  return new MethodClosure2<Class, Arg1, Arg2>(object, method, false,
                                                         arg1, arg2);
}

class FatalException : public std::exception {
public:
  FatalException(const char *filename, int line, const std::string &message)
      : filename_(filename), line_(line), message_(message) {}
  virtual ~FatalException() throw();

  virtual const char *what() const throw();

  const char *filename() const { return filename_; }
  int line() const { return line_; }
  const std::string &message() const { return message_; }

private:
  const char *filename_;
  const int line_;
  const std::string message_;
};