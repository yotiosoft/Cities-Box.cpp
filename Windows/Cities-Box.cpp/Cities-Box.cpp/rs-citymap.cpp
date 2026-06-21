#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#if __cplusplus >= 202002L
#include <ranges>
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif // __clang__
#endif // __GNUC__

namespace rust {
inline namespace cxxbridge1 {
// #include "rust/cxx.h"

#ifndef CXXBRIDGE1_PANIC
#define CXXBRIDGE1_PANIC
template <typename Exception>
void panic [[noreturn]] (const char *msg);
#endif // CXXBRIDGE1_PANIC

struct unsafe_bitcopy_t;

namespace {
template <typename T>
class impl;
} // namespace

template <typename T>
::std::size_t size_of();
template <typename T>
::std::size_t align_of();

#ifndef CXXBRIDGE1_RUST_STRING
#define CXXBRIDGE1_RUST_STRING
class String final {
public:
  String() noexcept;
  String(const String &) noexcept;
  String(String &&) noexcept;
  ~String() noexcept;

  String(const std::string &);
  String(const char *);
  String(const char *, std::size_t);
  String(const char16_t *);
  String(const char16_t *, std::size_t);
#ifdef __cpp_char8_t
  String(const char8_t *s);
  String(const char8_t *s, std::size_t len);
#endif

  static String lossy(const std::string &) noexcept;
  static String lossy(const char *) noexcept;
  static String lossy(const char *, std::size_t) noexcept;
  static String lossy(const char16_t *) noexcept;
  static String lossy(const char16_t *, std::size_t) noexcept;

  String &operator=(const String &) & noexcept;
  String &operator=(String &&) & noexcept;

  explicit operator std::string() const;

  const char *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  const char *c_str() noexcept;

  std::size_t capacity() const noexcept;
  void reserve(size_t new_cap) noexcept;

  using iterator = char *;
  iterator begin() noexcept;
  iterator end() noexcept;

  using const_iterator = const char *;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bool operator==(const String &) const noexcept;
  bool operator!=(const String &) const noexcept;
  bool operator<(const String &) const noexcept;
  bool operator<=(const String &) const noexcept;
  bool operator>(const String &) const noexcept;
  bool operator>=(const String &) const noexcept;

  void swap(String &) noexcept;

  String(unsafe_bitcopy_t, const String &) noexcept;

private:
  struct lossy_t;
  String(lossy_t, const char *, std::size_t) noexcept;
  String(lossy_t, const char16_t *, std::size_t) noexcept;
  friend void swap(String &lhs, String &rhs) noexcept { lhs.swap(rhs); }

  std::array<std::uintptr_t, 3> repr;
};
#endif // CXXBRIDGE1_RUST_STRING

#ifndef CXXBRIDGE1_RUST_SLICE
#define CXXBRIDGE1_RUST_SLICE
namespace detail {
template <bool>
struct copy_assignable_if {};

template <>
struct copy_assignable_if<false> {
  copy_assignable_if() noexcept = default;
  copy_assignable_if(const copy_assignable_if &) noexcept = default;
  copy_assignable_if &operator=(const copy_assignable_if &) & noexcept = delete;
  copy_assignable_if &operator=(copy_assignable_if &&) & noexcept = default;
};
} // namespace detail

template <typename T>
class Slice final
    : private detail::copy_assignable_if<std::is_const<T>::value> {
public:
  using value_type = T;

  Slice() noexcept;
  Slice(T *, std::size_t count) noexcept;

  template <typename C>
  explicit Slice(C &c) : Slice(c.data(), c.size()) {}

  Slice &operator=(const Slice<T> &) & noexcept = default;
  Slice &operator=(Slice<T> &&) & noexcept = default;

  T *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  T &operator[](std::size_t n) const noexcept;
  T &at(std::size_t n) const;
  T &front() const noexcept;
  T &back() const noexcept;

  Slice(const Slice<T> &) noexcept = default;
  ~Slice() noexcept = default;

  class iterator;
  iterator begin() const noexcept;
  iterator end() const noexcept;

  void swap(Slice &) noexcept;

private:
  class uninit;
  Slice(uninit) noexcept;
  friend impl<Slice>;
  friend void sliceInit(void *, const void *, std::size_t) noexcept;
  friend void *slicePtr(const void *) noexcept;
  friend std::size_t sliceLen(const void *) noexcept;

  std::array<std::uintptr_t, 2> repr;
};

#ifdef __cpp_deduction_guides
template <typename C>
explicit Slice(C &c)
    -> Slice<std::remove_reference_t<decltype(*std::declval<C>().data())>>;
#endif // __cpp_deduction_guides

template <typename T>
class Slice<T>::iterator final {
public:
#if __cplusplus >= 202002L
  using iterator_category = std::contiguous_iterator_tag;
#else
  using iterator_category = std::random_access_iterator_tag;
#endif
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = typename std::add_pointer<T>::type;
  using reference = typename std::add_lvalue_reference<T>::type;

  reference operator*() const noexcept;
  pointer operator->() const noexcept;
  reference operator[](difference_type) const noexcept;

  iterator &operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator &operator--() noexcept;
  iterator operator--(int) noexcept;

  iterator &operator+=(difference_type) noexcept;
  iterator &operator-=(difference_type) noexcept;
  iterator operator+(difference_type) const noexcept;
  friend inline iterator operator+(difference_type lhs, iterator rhs) noexcept {
    return rhs + lhs;
  }
  iterator operator-(difference_type) const noexcept;
  difference_type operator-(const iterator &) const noexcept;

  bool operator==(const iterator &) const noexcept;
  bool operator!=(const iterator &) const noexcept;
  bool operator<(const iterator &) const noexcept;
  bool operator<=(const iterator &) const noexcept;
  bool operator>(const iterator &) const noexcept;
  bool operator>=(const iterator &) const noexcept;

private:
  friend class Slice;
  void *pos;
  std::size_t stride;
};

#if __cplusplus >= 202002L
static_assert(std::ranges::contiguous_range<rust::Slice<const uint8_t>>);
static_assert(std::contiguous_iterator<rust::Slice<const uint8_t>::iterator>);
#endif

template <typename T>
Slice<T>::Slice() noexcept {
  sliceInit(this, reinterpret_cast<void *>(align_of<T>()), 0);
}

template <typename T>
Slice<T>::Slice(T *s, std::size_t count) noexcept {
  assert(s != nullptr || count == 0);
  sliceInit(this,
            s == nullptr && count == 0
                ? reinterpret_cast<void *>(align_of<T>())
                : const_cast<typename std::remove_const<T>::type *>(s),
            count);
}

template <typename T>
T *Slice<T>::data() const noexcept {
  return reinterpret_cast<T *>(slicePtr(this));
}

template <typename T>
std::size_t Slice<T>::size() const noexcept {
  return sliceLen(this);
}

template <typename T>
std::size_t Slice<T>::length() const noexcept {
  return this->size();
}

template <typename T>
bool Slice<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T &Slice<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto ptr = static_cast<char *>(slicePtr(this)) + size_of<T>() * n;
  return *reinterpret_cast<T *>(ptr);
}

template <typename T>
T &Slice<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Slice index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Slice<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Slice<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
typename Slice<T>::iterator::reference
Slice<T>::iterator::operator*() const noexcept {
  return *static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::pointer
Slice<T>::iterator::operator->() const noexcept {
  return static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::reference Slice<T>::iterator::operator[](
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ptr = static_cast<char *>(this->pos) + this->stride * n;
  return *reinterpret_cast<T *>(ptr);
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator++() noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator++(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator--() noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator--(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator+=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator-=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator+(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) + this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator-(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) - this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator::difference_type
Slice<T>::iterator::operator-(const iterator &other) const noexcept {
  auto diff = std::distance(static_cast<char *>(other.pos),
                            static_cast<char *>(this->pos));
  return diff / static_cast<typename Slice<T>::iterator::difference_type>(
                    this->stride);
}

template <typename T>
bool Slice<T>::iterator::operator==(const iterator &other) const noexcept {
  return this->pos == other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator!=(const iterator &other) const noexcept {
  return this->pos != other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<(const iterator &other) const noexcept {
  return this->pos < other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<=(const iterator &other) const noexcept {
  return this->pos <= other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>(const iterator &other) const noexcept {
  return this->pos > other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>=(const iterator &other) const noexcept {
  return this->pos >= other.pos;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::begin() const noexcept {
  iterator it;
  it.pos = slicePtr(this);
  it.stride = size_of<T>();
  return it;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::end() const noexcept {
  iterator it = this->begin();
  it.pos = static_cast<char *>(it.pos) + it.stride * this->size();
  return it;
}

template <typename T>
void Slice<T>::swap(Slice &rhs) noexcept {
  std::swap(*this, rhs);
}
#endif // CXXBRIDGE1_RUST_SLICE

#ifndef CXXBRIDGE1_RUST_BOX
#define CXXBRIDGE1_RUST_BOX
template <typename T>
class Box final {
public:
  using element_type = T;
  using const_pointer =
      typename std::add_pointer<typename std::add_const<T>::type>::type;
  using pointer = typename std::add_pointer<T>::type;

  Box() = delete;
  Box(Box &&) noexcept;
  ~Box() noexcept;

  explicit Box(const T &);
  explicit Box(T &&);

  Box &operator=(Box &&) & noexcept;

  const T *operator->() const noexcept;
  const T &operator*() const noexcept;
  T *operator->() noexcept;
  T &operator*() noexcept;

  template <typename... Fields>
  static Box in_place(Fields &&...);

  void swap(Box &) noexcept;

  static Box from_raw(T *) noexcept;

  T *into_raw() noexcept;

  /* Deprecated */ using value_type = element_type;

private:
  class uninit;
  class allocation;
  Box(uninit) noexcept;
  void drop() noexcept;

  friend void swap(Box &lhs, Box &rhs) noexcept { lhs.swap(rhs); }

  T *ptr;
};

template <typename T>
class Box<T>::uninit {};

template <typename T>
class Box<T>::allocation {
  static T *alloc() noexcept;
  static void dealloc(T *) noexcept;

public:
  allocation() noexcept : ptr(alloc()) {}
  ~allocation() noexcept {
    if (this->ptr) {
      dealloc(this->ptr);
    }
  }
  T *ptr;
};

template <typename T>
Box<T>::Box(Box &&other) noexcept : ptr(other.ptr) {
  other.ptr = nullptr;
}

template <typename T>
Box<T>::Box(const T &val) {
  allocation alloc;
  ::new (alloc.ptr) T(val);
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::Box(T &&val) {
  allocation alloc;
  ::new (alloc.ptr) T(std::move(val));
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::~Box() noexcept {
  if (this->ptr) {
    this->drop();
  }
}

template <typename T>
Box<T> &Box<T>::operator=(Box &&other) & noexcept {
  if (this->ptr) {
    this->drop();
  }
  this->ptr = other.ptr;
  other.ptr = nullptr;
  return *this;
}

template <typename T>
const T *Box<T>::operator->() const noexcept {
  return this->ptr;
}

template <typename T>
const T &Box<T>::operator*() const noexcept {
  return *this->ptr;
}

template <typename T>
T *Box<T>::operator->() noexcept {
  return this->ptr;
}

template <typename T>
T &Box<T>::operator*() noexcept {
  return *this->ptr;
}

template <typename T>
template <typename... Fields>
Box<T> Box<T>::in_place(Fields &&...fields) {
  allocation alloc;
  auto ptr = alloc.ptr;
  ::new (ptr) T{std::forward<Fields>(fields)...};
  alloc.ptr = nullptr;
  return from_raw(ptr);
}

template <typename T>
void Box<T>::swap(Box &rhs) noexcept {
  using std::swap;
  swap(this->ptr, rhs.ptr);
}

template <typename T>
Box<T> Box<T>::from_raw(T *raw) noexcept {
  Box box = uninit{};
  box.ptr = raw;
  return box;
}

template <typename T>
T *Box<T>::into_raw() noexcept {
  T *raw = this->ptr;
  this->ptr = nullptr;
  return raw;
}

template <typename T>
Box<T>::Box(uninit) noexcept {}
#endif // CXXBRIDGE1_RUST_BOX

#ifndef CXXBRIDGE1_RUST_BITCOPY_T
#define CXXBRIDGE1_RUST_BITCOPY_T
struct unsafe_bitcopy_t final {
  explicit unsafe_bitcopy_t() = default;
};
#endif // CXXBRIDGE1_RUST_BITCOPY_T

#ifndef CXXBRIDGE1_RUST_VEC
#define CXXBRIDGE1_RUST_VEC
template <typename T>
class Vec final {
public:
  using value_type = T;

  Vec() noexcept;
  Vec(std::initializer_list<T>);
  Vec(const Vec &);
  Vec(Vec &&) noexcept;
  ~Vec() noexcept;

  Vec &operator=(Vec &&) & noexcept;
  Vec &operator=(const Vec &) &;

  std::size_t size() const noexcept;
  bool empty() const noexcept;
  const T *data() const noexcept;
  T *data() noexcept;
  std::size_t capacity() const noexcept;

  const T &operator[](std::size_t n) const noexcept;
  const T &at(std::size_t n) const;
  const T &front() const noexcept;
  const T &back() const noexcept;

  T &operator[](std::size_t n) noexcept;
  T &at(std::size_t n);
  T &front() noexcept;
  T &back() noexcept;

  void reserve(std::size_t new_cap);
  void push_back(const T &value);
  void push_back(T &&value);
  template <typename... Args>
  void emplace_back(Args &&...args);
  void truncate(std::size_t len);
  void clear();

  using iterator = typename Slice<T>::iterator;
  iterator begin() noexcept;
  iterator end() noexcept;

  using const_iterator = typename Slice<const T>::iterator;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  void swap(Vec &) noexcept;

  Vec(unsafe_bitcopy_t, const Vec &) noexcept;

private:
  void reserve_total(std::size_t new_cap) noexcept;
  void set_len(std::size_t len) noexcept;
  void drop() noexcept;

  friend void swap(Vec &lhs, Vec &rhs) noexcept { lhs.swap(rhs); }

  std::array<std::uintptr_t, 3> repr;
};

template <typename T>
Vec<T>::Vec(std::initializer_list<T> init) : Vec{} {
  this->reserve_total(init.size());
  std::move(init.begin(), init.end(), std::back_inserter(*this));
}

template <typename T>
Vec<T>::Vec(const Vec &other) : Vec() {
  this->reserve_total(other.size());
  std::copy(other.begin(), other.end(), std::back_inserter(*this));
}

template <typename T>
Vec<T>::Vec(Vec &&other) noexcept : repr(other.repr) {
  new (&other) Vec();
}

template <typename T>
Vec<T>::~Vec() noexcept {
  this->drop();
}

template <typename T>
Vec<T> &Vec<T>::operator=(Vec &&other) & noexcept {
  this->drop();
  this->repr = other.repr;
  new (&other) Vec();
  return *this;
}

template <typename T>
Vec<T> &Vec<T>::operator=(const Vec &other) & {
  if (this != &other) {
    this->drop();
    new (this) Vec(other);
  }
  return *this;
}

template <typename T>
bool Vec<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T *Vec<T>::data() noexcept {
  return const_cast<T *>(const_cast<const Vec<T> *>(this)->data());
}

template <typename T>
const T &Vec<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto data = reinterpret_cast<const char *>(this->data());
  return *reinterpret_cast<const T *>(data + n * size_of<T>());
}

template <typename T>
const T &Vec<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Vec index out of range");
  }
  return (*this)[n];
}

template <typename T>
const T &Vec<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
const T &Vec<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
T &Vec<T>::operator[](std::size_t n) noexcept {
  assert(n < this->size());
  auto data = reinterpret_cast<char *>(this->data());
  return *reinterpret_cast<T *>(data + n * size_of<T>());
}

template <typename T>
T &Vec<T>::at(std::size_t n) {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Vec index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Vec<T>::front() noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Vec<T>::back() noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
void Vec<T>::reserve(std::size_t new_cap) {
  this->reserve_total(new_cap);
}

template <typename T>
void Vec<T>::push_back(const T &value) {
  this->emplace_back(value);
}

template <typename T>
void Vec<T>::push_back(T &&value) {
  this->emplace_back(std::move(value));
}

template <typename T>
template <typename... Args>
void Vec<T>::emplace_back(Args &&...args) {
  auto size = this->size();
  this->reserve_total(size + 1);
  ::new (reinterpret_cast<T *>(reinterpret_cast<char *>(this->data()) +
                               size * size_of<T>()))
      T(std::forward<Args>(args)...);
  this->set_len(size + 1);
}

template <typename T>
void Vec<T>::clear() {
  this->truncate(0);
}

template <typename T>
typename Vec<T>::iterator Vec<T>::begin() noexcept {
  return Slice<T>(this->data(), this->size()).begin();
}

template <typename T>
typename Vec<T>::iterator Vec<T>::end() noexcept {
  return Slice<T>(this->data(), this->size()).end();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::begin() const noexcept {
  return this->cbegin();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::end() const noexcept {
  return this->cend();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::cbegin() const noexcept {
  return Slice<const T>(this->data(), this->size()).begin();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::cend() const noexcept {
  return Slice<const T>(this->data(), this->size()).end();
}

template <typename T>
void Vec<T>::swap(Vec &rhs) noexcept {
  using std::swap;
  swap(this->repr, rhs.repr);
}

template <typename T>
Vec<T>::Vec(unsafe_bitcopy_t, const Vec &bits) noexcept : repr(bits.repr) {}
#endif // CXXBRIDGE1_RUST_VEC

#ifndef CXXBRIDGE1_RUST_OPAQUE
#define CXXBRIDGE1_RUST_OPAQUE
class Opaque {
public:
  Opaque() = delete;
  Opaque(const Opaque &) = delete;
  ~Opaque() = delete;
};
#endif // CXXBRIDGE1_RUST_OPAQUE

#ifndef CXXBRIDGE1_IS_COMPLETE
#define CXXBRIDGE1_IS_COMPLETE
namespace detail {
namespace {
template <typename T, typename = std::size_t>
struct is_complete : std::false_type {};
template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};
} // namespace
} // namespace detail
#endif // CXXBRIDGE1_IS_COMPLETE

#ifndef CXXBRIDGE1_LAYOUT
#define CXXBRIDGE1_LAYOUT
class layout {
  template <typename T>
  friend std::size_t size_of();
  template <typename T>
  friend std::size_t align_of();
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return T::layout::size();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return sizeof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      size_of() {
    return do_size_of<T>();
  }
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return T::layout::align();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return alignof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      align_of() {
    return do_align_of<T>();
  }
};

template <typename T>
std::size_t size_of() {
  return layout::size_of<T>();
}

template <typename T>
std::size_t align_of() {
  return layout::align_of<T>();
}
#endif // CXXBRIDGE1_LAYOUT

namespace detail {
template <typename T, typename = void *>
struct operator_new {
  void *operator()(::std::size_t sz) { return ::operator new(sz); }
};

template <typename T>
struct operator_new<T, decltype(T::operator new(sizeof(T)))> {
  void *operator()(::std::size_t sz) { return T::operator new(sz); }
};
} // namespace detail

template <typename T>
union ManuallyDrop {
  T value;
  ManuallyDrop(T &&value) : value(::std::move(value)) {}
  ~ManuallyDrop() {}
};

template <typename T>
union MaybeUninit {
  T value;
  void *operator new(::std::size_t sz) { return detail::operator_new<T>{}(sz); }
  MaybeUninit() {}
  ~MaybeUninit() {}
};
} // namespace cxxbridge1
} // namespace rust

#if __cplusplus >= 201402L
#define CXX_DEFAULT_VALUE(value) = value
#else
#define CXX_DEFAULT_VALUE(value)
#endif

namespace rust {
  namespace citymap {
    struct RawTileData;
    struct TimeStruct;
    struct RCOIFstruct;
    struct SimulationSnapshot;
    struct ResidentialTileState;
    struct SimulationMapStats;
    struct SimulationUpdate;
    struct LoadedObjectData;
    struct LoadedTileObjectData;
    struct LoadedRateData;
    struct LoadedWorkPlaceData;
    struct LoadedSchoolData;
    struct LoadedTileData;
    struct LoadedCityData;
    struct LoadCityResult;
    struct RustCityMap;
  }
}

namespace rust {
namespace citymap {
#ifndef CXXBRIDGE1_STRUCT_rust$citymap$RawTileData
#define CXXBRIDGE1_STRUCT_rust$citymap$RawTileData
struct RawTileData final {
  ::std::int32_t residents CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_commercial CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_office CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_industrial CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_farm CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_public CXX_DEFAULT_VALUE(0);
  ::std::int32_t students CXX_DEFAULT_VALUE(0);
  ::std::int32_t reservation CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$RawTileData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$TimeStruct
#define CXXBRIDGE1_STRUCT_rust$citymap$TimeStruct
struct TimeStruct final {
  ::std::int32_t year CXX_DEFAULT_VALUE(0);
  ::std::int32_t month CXX_DEFAULT_VALUE(0);
  ::std::int32_t date CXX_DEFAULT_VALUE(0);
  ::std::int32_t hour CXX_DEFAULT_VALUE(0);
  ::std::int32_t minutes CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$TimeStruct

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$RCOIFstruct
#define CXXBRIDGE1_STRUCT_rust$citymap$RCOIFstruct
struct RCOIFstruct final {
  double residential CXX_DEFAULT_VALUE(0);
  double commercial CXX_DEFAULT_VALUE(0);
  double office CXX_DEFAULT_VALUE(0);
  double industrial CXX_DEFAULT_VALUE(0);
  double farm CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$RCOIFstruct

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$SimulationSnapshot
#define CXXBRIDGE1_STRUCT_rust$citymap$SimulationSnapshot
struct SimulationSnapshot final {
  ::rust::citymap::TimeStruct time;
  ::std::int32_t population CXX_DEFAULT_VALUE(0);
  ::std::int32_t money CXX_DEFAULT_VALUE(0);
  ::std::int32_t temperature CXX_DEFAULT_VALUE(0);
  ::rust::citymap::RCOIFstruct demand;
  ::std::int32_t budget_police CXX_DEFAULT_VALUE(0);
  ::std::int32_t budget_fire CXX_DEFAULT_VALUE(0);
  ::std::int32_t budget_post CXX_DEFAULT_VALUE(0);
  ::std::int32_t budget_education CXX_DEFAULT_VALUE(0);
  double tax_residential CXX_DEFAULT_VALUE(0);
  double tax_commercial CXX_DEFAULT_VALUE(0);
  double tax_office CXX_DEFAULT_VALUE(0);
  double tax_industrial CXX_DEFAULT_VALUE(0);
  double tax_farm CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$SimulationSnapshot

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$ResidentialTileState
#define CXXBRIDGE1_STRUCT_rust$citymap$ResidentialTileState
struct ResidentialTileState final {
  ::std::int32_t x CXX_DEFAULT_VALUE(0);
  ::std::int32_t y CXX_DEFAULT_VALUE(0);
  ::std::int32_t residents CXX_DEFAULT_VALUE(0);
  ::std::int32_t maximum_capacity CXX_DEFAULT_VALUE(0);
  ::rust::Vec<::std::int32_t> ages;
  ::rust::Vec<::rust::String> genders;

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$ResidentialTileState

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$SimulationMapStats
#define CXXBRIDGE1_STRUCT_rust$citymap$SimulationMapStats
struct SimulationMapStats final {
  ::std::int32_t residential_tiles CXX_DEFAULT_VALUE(0);
  ::std::int32_t commercial_tiles CXX_DEFAULT_VALUE(0);
  ::std::int32_t office_tiles CXX_DEFAULT_VALUE(0);
  ::std::int32_t industrial_tiles CXX_DEFAULT_VALUE(0);
  ::std::int32_t farm_tiles CXX_DEFAULT_VALUE(0);
  ::std::int32_t police_stations CXX_DEFAULT_VALUE(0);
  ::std::int32_t fire_departments CXX_DEFAULT_VALUE(0);
  ::std::int32_t post_offices CXX_DEFAULT_VALUE(0);
  ::std::int32_t education_facilities CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$SimulationMapStats

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$SimulationUpdate
#define CXXBRIDGE1_STRUCT_rust$citymap$SimulationUpdate
struct SimulationUpdate final {
  ::rust::citymap::SimulationSnapshot snapshot;
  ::rust::Vec<::rust::citymap::ResidentialTileState> residential_tiles;

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$SimulationUpdate

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedObjectData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedObjectData
struct LoadedObjectData final {
  ::std::int32_t id CXX_DEFAULT_VALUE(0);
  ::rust::String addon_name;
  ::rust::String original_name;
  ::rust::String type_name;
  ::rust::String direction_name;
  ::std::int32_t origin_x CXX_DEFAULT_VALUE(0);
  ::std::int32_t origin_y CXX_DEFAULT_VALUE(0);
  bool visible CXX_DEFAULT_VALUE(false);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedObjectData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedTileObjectData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedTileObjectData
struct LoadedTileObjectData final {
  ::std::int32_t object_id CXX_DEFAULT_VALUE(0);
  ::std::int32_t relative_x CXX_DEFAULT_VALUE(0);
  ::std::int32_t relative_y CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedTileObjectData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedRateData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedRateData
struct LoadedRateData final {
  ::rust::String name;
  ::std::int32_t value CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedRateData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedWorkPlaceData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedWorkPlaceData
struct LoadedWorkPlaceData final {
  ::std::int32_t kind CXX_DEFAULT_VALUE(0);
  ::std::int32_t serial_number CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedWorkPlaceData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedSchoolData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedSchoolData
struct LoadedSchoolData final {
  ::std::int32_t kind CXX_DEFAULT_VALUE(0);
  ::std::int32_t serial_number CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedSchoolData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedTileData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedTileData
struct LoadedTileData final {
  ::std::int32_t residents CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_commercial CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_office CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_industrial CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_farm CXX_DEFAULT_VALUE(0);
  ::std::int32_t workers_public CXX_DEFAULT_VALUE(0);
  ::std::int32_t students CXX_DEFAULT_VALUE(0);
  ::std::int32_t reservation CXX_DEFAULT_VALUE(0);
  ::rust::String original_name;
  ::rust::Vec<::std::int32_t> ages;
  ::rust::Vec<::rust::String> genders;
  ::rust::Vec<::rust::citymap::LoadedTileObjectData> objects;
  ::rust::Vec<::rust::citymap::LoadedRateData> rates;
  ::rust::Vec<::rust::citymap::LoadedWorkPlaceData> work_places;
  ::rust::Vec<::rust::citymap::LoadedSchoolData> schools;

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedTileData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadedCityData
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadedCityData
struct LoadedCityData final {
  ::rust::String addon_set_name;
  ::std::int32_t map_width CXX_DEFAULT_VALUE(0);
  ::std::int32_t map_height CXX_DEFAULT_VALUE(0);
  ::rust::Vec<::rust::citymap::LoadedObjectData> objects;
  ::rust::Vec<::rust::citymap::LoadedTileData> tiles;

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadedCityData

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$LoadCityResult
#define CXXBRIDGE1_STRUCT_rust$citymap$LoadCityResult
struct LoadCityResult final {
  bool success CXX_DEFAULT_VALUE(false);
  ::rust::String error_message;
  ::rust::citymap::LoadedCityData city;

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$LoadCityResult

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$RustCityMap
#define CXXBRIDGE1_STRUCT_rust$citymap$RustCityMap
struct RustCityMap final : public ::rust::Opaque {
  ::rust::citymap::LoadCityResult load_city_map(::rust::String path) noexcept;
  bool commit_loaded_city_map() noexcept;
  void clear_objects() noexcept;
  void add_object(::std::int32_t id, ::rust::String addon_en, ::rust::String orig_name, ::rust::String type_id, ::rust::String dir_id, ::std::int32_t x, ::std::int32_t y, bool visible) noexcept;
  void set_tile_basic(::std::int32_t x, ::std::int32_t y, ::std::int32_t residents, ::std::int32_t students, ::std::int32_t reservation, ::rust::String orig_name) noexcept;
  void add_tile_object_ref(::std::int32_t x, ::std::int32_t y, ::std::int32_t obj_id, ::std::int32_t rel_x, ::std::int32_t rel_y, bool visible) noexcept;
  void set_tile_stats(::std::int32_t x, ::std::int32_t y, ::rust::Vec<::std::int32_t> ages, ::rust::Vec<::rust::String> genders) noexcept;
  void add_tile_rate(::std::int32_t x, ::std::int32_t y, ::rust::String key, ::std::int32_t value) noexcept;
  void add_tile_work_place(::std::int32_t x, ::std::int32_t y, ::std::int32_t kind, ::std::int32_t serial_number) noexcept;
  void add_tile_school(::std::int32_t x, ::std::int32_t y, ::std::int32_t kind, ::std::int32_t serial_number) noexcept;
  ::rust::citymap::SimulationSnapshot simulation_snapshot() const noexcept;
  bool will_run_daily_update(::std::int32_t minutes_delta) const noexcept;
  ::rust::citymap::SimulationUpdate update_world(::std::int32_t minutes_delta, ::rust::Vec<::rust::citymap::ResidentialTileState> residential_tiles, ::rust::citymap::SimulationMapStats map_stats) noexcept;
  void set_save_version(::std::int32_t version) noexcept;
  void bulk_set_tiles(::rust::Slice<::rust::citymap::RawTileData const> data, ::std::int32_t width, ::std::int32_t height) noexcept;
  bool save_to_file(::rust::String path) const noexcept;
  ~RustCityMap() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_rust$citymap$RustCityMap

extern "C" {
::std::size_t rust$citymap$cxxbridge1$192$RustCityMap$operator$sizeof() noexcept;
::std::size_t rust$citymap$cxxbridge1$192$RustCityMap$operator$alignof() noexcept;

::rust::citymap::RustCityMap *rust$citymap$cxxbridge1$192$new_city_map() noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$load_city_map(::rust::citymap::RustCityMap &self, ::rust::String *path, ::rust::citymap::LoadCityResult *return$) noexcept;

bool rust$citymap$cxxbridge1$192$RustCityMap$commit_loaded_city_map(::rust::citymap::RustCityMap &self) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$clear_objects(::rust::citymap::RustCityMap &self) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$add_object(::rust::citymap::RustCityMap &self, ::std::int32_t id, ::rust::String *addon_en, ::rust::String *orig_name, ::rust::String *type_id, ::rust::String *dir_id, ::std::int32_t x, ::std::int32_t y, bool visible) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_tile_basic(::rust::citymap::RustCityMap &self, ::std::int32_t x, ::std::int32_t y, ::std::int32_t residents, ::std::int32_t students, ::std::int32_t reservation, ::rust::String *orig_name) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$add_tile_object_ref(::rust::citymap::RustCityMap &self, ::std::int32_t x, ::std::int32_t y, ::std::int32_t obj_id, ::std::int32_t rel_x, ::std::int32_t rel_y, bool visible) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_tile_stats(::rust::citymap::RustCityMap &self, ::std::int32_t x, ::std::int32_t y, ::rust::Vec<::std::int32_t> *ages, ::rust::Vec<::rust::String> *genders) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$add_tile_rate(::rust::citymap::RustCityMap &self, ::std::int32_t x, ::std::int32_t y, ::rust::String *key, ::std::int32_t value) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$add_tile_work_place(::rust::citymap::RustCityMap &self, ::std::int32_t x, ::std::int32_t y, ::std::int32_t kind, ::std::int32_t serial_number) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$add_tile_school(::rust::citymap::RustCityMap &self, ::std::int32_t x, ::std::int32_t y, ::std::int32_t kind, ::std::int32_t serial_number) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$simulation_snapshot(::rust::citymap::RustCityMap const &self, ::rust::citymap::SimulationSnapshot *return$) noexcept;

bool rust$citymap$cxxbridge1$192$RustCityMap$will_run_daily_update(::rust::citymap::RustCityMap const &self, ::std::int32_t minutes_delta) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$update_world(::rust::citymap::RustCityMap &self, ::std::int32_t minutes_delta, ::rust::Vec<::rust::citymap::ResidentialTileState> *residential_tiles, ::rust::citymap::SimulationMapStats *map_stats, ::rust::citymap::SimulationUpdate *return$) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_save_version(::rust::citymap::RustCityMap &self, ::std::int32_t version) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$bulk_set_tiles(::rust::citymap::RustCityMap &self, ::rust::Slice<::rust::citymap::RawTileData const> data, ::std::int32_t width, ::std::int32_t height) noexcept;

bool rust$citymap$cxxbridge1$192$RustCityMap$save_to_file(::rust::citymap::RustCityMap const &self, ::rust::String *path) noexcept;
} // extern "C"

::std::size_t RustCityMap::layout::size() noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$operator$sizeof();
}

::std::size_t RustCityMap::layout::align() noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$operator$alignof();
}

::rust::Box<::rust::citymap::RustCityMap> new_city_map() noexcept {
  return ::rust::Box<::rust::citymap::RustCityMap>::from_raw(rust$citymap$cxxbridge1$192$new_city_map());
}

::rust::citymap::LoadCityResult RustCityMap::load_city_map(::rust::String path) noexcept {
  ::rust::MaybeUninit<::rust::citymap::LoadCityResult> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$load_city_map(*this, &path, &return$.value);
  return ::std::move(return$.value);
}

bool RustCityMap::commit_loaded_city_map() noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$commit_loaded_city_map(*this);
}

void RustCityMap::clear_objects() noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$clear_objects(*this);
}

void RustCityMap::add_object(::std::int32_t id, ::rust::String addon_en, ::rust::String orig_name, ::rust::String type_id, ::rust::String dir_id, ::std::int32_t x, ::std::int32_t y, bool visible) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$add_object(*this, id, &addon_en, &orig_name, &type_id, &dir_id, x, y, visible);
}

void RustCityMap::set_tile_basic(::std::int32_t x, ::std::int32_t y, ::std::int32_t residents, ::std::int32_t students, ::std::int32_t reservation, ::rust::String orig_name) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_tile_basic(*this, x, y, residents, students, reservation, &orig_name);
}

void RustCityMap::add_tile_object_ref(::std::int32_t x, ::std::int32_t y, ::std::int32_t obj_id, ::std::int32_t rel_x, ::std::int32_t rel_y, bool visible) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$add_tile_object_ref(*this, x, y, obj_id, rel_x, rel_y, visible);
}

void RustCityMap::set_tile_stats(::std::int32_t x, ::std::int32_t y, ::rust::Vec<::std::int32_t> ages, ::rust::Vec<::rust::String> genders) noexcept {
  ::rust::ManuallyDrop<::rust::Vec<::std::int32_t>> ages$(::std::move(ages));
  ::rust::ManuallyDrop<::rust::Vec<::rust::String>> genders$(::std::move(genders));
  rust$citymap$cxxbridge1$192$RustCityMap$set_tile_stats(*this, x, y, &ages$.value, &genders$.value);
}

void RustCityMap::add_tile_rate(::std::int32_t x, ::std::int32_t y, ::rust::String key, ::std::int32_t value) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$add_tile_rate(*this, x, y, &key, value);
}

void RustCityMap::add_tile_work_place(::std::int32_t x, ::std::int32_t y, ::std::int32_t kind, ::std::int32_t serial_number) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$add_tile_work_place(*this, x, y, kind, serial_number);
}

void RustCityMap::add_tile_school(::std::int32_t x, ::std::int32_t y, ::std::int32_t kind, ::std::int32_t serial_number) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$add_tile_school(*this, x, y, kind, serial_number);
}

::rust::citymap::SimulationSnapshot RustCityMap::simulation_snapshot() const noexcept {
  ::rust::MaybeUninit<::rust::citymap::SimulationSnapshot> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$simulation_snapshot(*this, &return$.value);
  return ::std::move(return$.value);
}

bool RustCityMap::will_run_daily_update(::std::int32_t minutes_delta) const noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$will_run_daily_update(*this, minutes_delta);
}

::rust::citymap::SimulationUpdate RustCityMap::update_world(::std::int32_t minutes_delta, ::rust::Vec<::rust::citymap::ResidentialTileState> residential_tiles, ::rust::citymap::SimulationMapStats map_stats) noexcept {
  ::rust::ManuallyDrop<::rust::Vec<::rust::citymap::ResidentialTileState>> residential_tiles$(::std::move(residential_tiles));
  ::rust::ManuallyDrop<::rust::citymap::SimulationMapStats> map_stats$(::std::move(map_stats));
  ::rust::MaybeUninit<::rust::citymap::SimulationUpdate> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$update_world(*this, minutes_delta, &residential_tiles$.value, &map_stats$.value, &return$.value);
  return ::std::move(return$.value);
}

void RustCityMap::set_save_version(::std::int32_t version) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_save_version(*this, version);
}

void RustCityMap::bulk_set_tiles(::rust::Slice<::rust::citymap::RawTileData const> data, ::std::int32_t width, ::std::int32_t height) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$bulk_set_tiles(*this, data, width, height);
}

bool RustCityMap::save_to_file(::rust::String path) const noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$save_to_file(*this, &path);
}
} // namespace citymap
} // namespace rust

extern "C" {
void cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$new(::rust::Vec<::rust::citymap::ResidentialTileState> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$drop(::rust::Vec<::rust::citymap::ResidentialTileState> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$len(::rust::Vec<::rust::citymap::ResidentialTileState> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$capacity(::rust::Vec<::rust::citymap::ResidentialTileState> const *ptr) noexcept;
::rust::citymap::ResidentialTileState const *cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$data(::rust::Vec<::rust::citymap::ResidentialTileState> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$reserve_total(::rust::Vec<::rust::citymap::ResidentialTileState> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$set_len(::rust::Vec<::rust::citymap::ResidentialTileState> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$truncate(::rust::Vec<::rust::citymap::ResidentialTileState> *ptr, ::std::size_t len) noexcept;

void cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$new(::rust::Vec<::rust::citymap::LoadedTileObjectData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$drop(::rust::Vec<::rust::citymap::LoadedTileObjectData> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$len(::rust::Vec<::rust::citymap::LoadedTileObjectData> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$capacity(::rust::Vec<::rust::citymap::LoadedTileObjectData> const *ptr) noexcept;
::rust::citymap::LoadedTileObjectData const *cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$data(::rust::Vec<::rust::citymap::LoadedTileObjectData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$reserve_total(::rust::Vec<::rust::citymap::LoadedTileObjectData> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$set_len(::rust::Vec<::rust::citymap::LoadedTileObjectData> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$truncate(::rust::Vec<::rust::citymap::LoadedTileObjectData> *ptr, ::std::size_t len) noexcept;

void cxxbridge1$rust_vec$rust$citymap$LoadedRateData$new(::rust::Vec<::rust::citymap::LoadedRateData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedRateData$drop(::rust::Vec<::rust::citymap::LoadedRateData> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedRateData$len(::rust::Vec<::rust::citymap::LoadedRateData> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedRateData$capacity(::rust::Vec<::rust::citymap::LoadedRateData> const *ptr) noexcept;
::rust::citymap::LoadedRateData const *cxxbridge1$rust_vec$rust$citymap$LoadedRateData$data(::rust::Vec<::rust::citymap::LoadedRateData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedRateData$reserve_total(::rust::Vec<::rust::citymap::LoadedRateData> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedRateData$set_len(::rust::Vec<::rust::citymap::LoadedRateData> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedRateData$truncate(::rust::Vec<::rust::citymap::LoadedRateData> *ptr, ::std::size_t len) noexcept;

void cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$new(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$drop(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$len(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$capacity(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> const *ptr) noexcept;
::rust::citymap::LoadedWorkPlaceData const *cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$data(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$reserve_total(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$set_len(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$truncate(::rust::Vec<::rust::citymap::LoadedWorkPlaceData> *ptr, ::std::size_t len) noexcept;

void cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$new(::rust::Vec<::rust::citymap::LoadedSchoolData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$drop(::rust::Vec<::rust::citymap::LoadedSchoolData> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$len(::rust::Vec<::rust::citymap::LoadedSchoolData> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$capacity(::rust::Vec<::rust::citymap::LoadedSchoolData> const *ptr) noexcept;
::rust::citymap::LoadedSchoolData const *cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$data(::rust::Vec<::rust::citymap::LoadedSchoolData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$reserve_total(::rust::Vec<::rust::citymap::LoadedSchoolData> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$set_len(::rust::Vec<::rust::citymap::LoadedSchoolData> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$truncate(::rust::Vec<::rust::citymap::LoadedSchoolData> *ptr, ::std::size_t len) noexcept;

void cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$new(::rust::Vec<::rust::citymap::LoadedObjectData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$drop(::rust::Vec<::rust::citymap::LoadedObjectData> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$len(::rust::Vec<::rust::citymap::LoadedObjectData> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$capacity(::rust::Vec<::rust::citymap::LoadedObjectData> const *ptr) noexcept;
::rust::citymap::LoadedObjectData const *cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$data(::rust::Vec<::rust::citymap::LoadedObjectData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$reserve_total(::rust::Vec<::rust::citymap::LoadedObjectData> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$set_len(::rust::Vec<::rust::citymap::LoadedObjectData> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$truncate(::rust::Vec<::rust::citymap::LoadedObjectData> *ptr, ::std::size_t len) noexcept;

void cxxbridge1$rust_vec$rust$citymap$LoadedTileData$new(::rust::Vec<::rust::citymap::LoadedTileData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileData$drop(::rust::Vec<::rust::citymap::LoadedTileData> *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedTileData$len(::rust::Vec<::rust::citymap::LoadedTileData> const *ptr) noexcept;
::std::size_t cxxbridge1$rust_vec$rust$citymap$LoadedTileData$capacity(::rust::Vec<::rust::citymap::LoadedTileData> const *ptr) noexcept;
::rust::citymap::LoadedTileData const *cxxbridge1$rust_vec$rust$citymap$LoadedTileData$data(::rust::Vec<::rust::citymap::LoadedTileData> const *ptr) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileData$reserve_total(::rust::Vec<::rust::citymap::LoadedTileData> *ptr, ::std::size_t new_cap) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileData$set_len(::rust::Vec<::rust::citymap::LoadedTileData> *ptr, ::std::size_t len) noexcept;
void cxxbridge1$rust_vec$rust$citymap$LoadedTileData$truncate(::rust::Vec<::rust::citymap::LoadedTileData> *ptr, ::std::size_t len) noexcept;

::rust::citymap::RustCityMap *cxxbridge1$box$rust$citymap$RustCityMap$alloc() noexcept;
void cxxbridge1$box$rust$citymap$RustCityMap$dealloc(::rust::citymap::RustCityMap *) noexcept;
void cxxbridge1$box$rust$citymap$RustCityMap$drop(::rust::Box<::rust::citymap::RustCityMap> *ptr) noexcept;
} // extern "C"

namespace rust {
inline namespace cxxbridge1 {
template <>
Vec<::rust::citymap::ResidentialTileState>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$new(this);
}
template <>
void Vec<::rust::citymap::ResidentialTileState>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::ResidentialTileState>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::ResidentialTileState>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$capacity(this);
}
template <>
::rust::citymap::ResidentialTileState const *Vec<::rust::citymap::ResidentialTileState>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$data(this);
}
template <>
void Vec<::rust::citymap::ResidentialTileState>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::ResidentialTileState>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$set_len(this, len);
}
template <>
void Vec<::rust::citymap::ResidentialTileState>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$ResidentialTileState$truncate(this, len);
}
template <>
Vec<::rust::citymap::LoadedTileObjectData>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$new(this);
}
template <>
void Vec<::rust::citymap::LoadedTileObjectData>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedTileObjectData>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedTileObjectData>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$capacity(this);
}
template <>
::rust::citymap::LoadedTileObjectData const *Vec<::rust::citymap::LoadedTileObjectData>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$data(this);
}
template <>
void Vec<::rust::citymap::LoadedTileObjectData>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::LoadedTileObjectData>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$set_len(this, len);
}
template <>
void Vec<::rust::citymap::LoadedTileObjectData>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileObjectData$truncate(this, len);
}
template <>
Vec<::rust::citymap::LoadedRateData>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$LoadedRateData$new(this);
}
template <>
void Vec<::rust::citymap::LoadedRateData>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedRateData>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedRateData>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$capacity(this);
}
template <>
::rust::citymap::LoadedRateData const *Vec<::rust::citymap::LoadedRateData>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$data(this);
}
template <>
void Vec<::rust::citymap::LoadedRateData>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::LoadedRateData>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$set_len(this, len);
}
template <>
void Vec<::rust::citymap::LoadedRateData>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$LoadedRateData$truncate(this, len);
}
template <>
Vec<::rust::citymap::LoadedWorkPlaceData>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$new(this);
}
template <>
void Vec<::rust::citymap::LoadedWorkPlaceData>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedWorkPlaceData>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedWorkPlaceData>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$capacity(this);
}
template <>
::rust::citymap::LoadedWorkPlaceData const *Vec<::rust::citymap::LoadedWorkPlaceData>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$data(this);
}
template <>
void Vec<::rust::citymap::LoadedWorkPlaceData>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::LoadedWorkPlaceData>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$set_len(this, len);
}
template <>
void Vec<::rust::citymap::LoadedWorkPlaceData>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$LoadedWorkPlaceData$truncate(this, len);
}
template <>
Vec<::rust::citymap::LoadedSchoolData>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$new(this);
}
template <>
void Vec<::rust::citymap::LoadedSchoolData>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedSchoolData>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedSchoolData>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$capacity(this);
}
template <>
::rust::citymap::LoadedSchoolData const *Vec<::rust::citymap::LoadedSchoolData>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$data(this);
}
template <>
void Vec<::rust::citymap::LoadedSchoolData>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::LoadedSchoolData>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$set_len(this, len);
}
template <>
void Vec<::rust::citymap::LoadedSchoolData>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$LoadedSchoolData$truncate(this, len);
}
template <>
Vec<::rust::citymap::LoadedObjectData>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$new(this);
}
template <>
void Vec<::rust::citymap::LoadedObjectData>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedObjectData>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedObjectData>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$capacity(this);
}
template <>
::rust::citymap::LoadedObjectData const *Vec<::rust::citymap::LoadedObjectData>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$data(this);
}
template <>
void Vec<::rust::citymap::LoadedObjectData>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::LoadedObjectData>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$set_len(this, len);
}
template <>
void Vec<::rust::citymap::LoadedObjectData>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$LoadedObjectData$truncate(this, len);
}
template <>
Vec<::rust::citymap::LoadedTileData>::Vec() noexcept {
  cxxbridge1$rust_vec$rust$citymap$LoadedTileData$new(this);
}
template <>
void Vec<::rust::citymap::LoadedTileData>::drop() noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$drop(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedTileData>::size() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$len(this);
}
template <>
::std::size_t Vec<::rust::citymap::LoadedTileData>::capacity() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$capacity(this);
}
template <>
::rust::citymap::LoadedTileData const *Vec<::rust::citymap::LoadedTileData>::data() const noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$data(this);
}
template <>
void Vec<::rust::citymap::LoadedTileData>::reserve_total(::std::size_t new_cap) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$reserve_total(this, new_cap);
}
template <>
void Vec<::rust::citymap::LoadedTileData>::set_len(::std::size_t len) noexcept {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$set_len(this, len);
}
template <>
void Vec<::rust::citymap::LoadedTileData>::truncate(::std::size_t len) {
  return cxxbridge1$rust_vec$rust$citymap$LoadedTileData$truncate(this, len);
}
template <>
::rust::citymap::RustCityMap *Box<::rust::citymap::RustCityMap>::allocation::alloc() noexcept {
  return cxxbridge1$box$rust$citymap$RustCityMap$alloc();
}
template <>
void Box<::rust::citymap::RustCityMap>::allocation::dealloc(::rust::citymap::RustCityMap *ptr) noexcept {
  cxxbridge1$box$rust$citymap$RustCityMap$dealloc(ptr);
}
template <>
void Box<::rust::citymap::RustCityMap>::drop() noexcept {
  cxxbridge1$box$rust$citymap$RustCityMap$drop(this);
}
} // namespace cxxbridge1
} // namespace rust
