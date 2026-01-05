#include <array>
#include <cstddef>
#include <cstdint>
#include <new>
#include <string>
#include <type_traits>
#include <utility>

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

struct unsafe_bitcopy_t;

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
    struct TimeStruct;
    struct RCOIFstruct;
    struct RustCityMap;
  }
}

namespace rust {
namespace citymap {
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

#ifndef CXXBRIDGE1_STRUCT_rust$citymap$RustCityMap
#define CXXBRIDGE1_STRUCT_rust$citymap$RustCityMap
struct RustCityMap final : public ::rust::Opaque {
  void set_status(::std::int32_t pop, ::std::int32_t money, ::std::int32_t temp, ::rust::citymap::TimeStruct time, ::rust::citymap::RCOIFstruct demand) noexcept;
  ::std::int32_t get_population() const noexcept;
  ::std::int32_t get_money() const noexcept;
  ::std::int32_t get_temperature() const noexcept;
  ::rust::citymap::RCOIFstruct get_demand() const noexcept;
  ::rust::citymap::TimeStruct city_time(::std::int32_t minutes_delta) noexcept;
  ::rust::citymap::TimeStruct update_world(::std::int32_t minutes_delta) noexcept;
  void set_city_metadata(::rust::String city_name, ::rust::String mayor_name, ::rust::String addon_set) noexcept;
  void set_financial_data(::std::int32_t money, ::std::int32_t population) noexcept;
  void set_budget(::std::int32_t police, ::std::int32_t fire, ::std::int32_t post, ::std::int32_t edu) noexcept;
  void set_tax(double r, double c, double o, double i, double f) noexcept;
  void set_environment(::std::int32_t year, ::std::int32_t month, ::std::int32_t date, ::std::int32_t hour, ::std::int32_t min, bool weather, bool night) noexcept;
  ::rust::String generate_save_json() const noexcept;
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

void rust$citymap$cxxbridge1$192$RustCityMap$set_status(::rust::citymap::RustCityMap &self, ::std::int32_t pop, ::std::int32_t money, ::std::int32_t temp, ::rust::citymap::TimeStruct *time, ::rust::citymap::RCOIFstruct *demand) noexcept;

::std::int32_t rust$citymap$cxxbridge1$192$RustCityMap$get_population(::rust::citymap::RustCityMap const &self) noexcept;

::std::int32_t rust$citymap$cxxbridge1$192$RustCityMap$get_money(::rust::citymap::RustCityMap const &self) noexcept;

::std::int32_t rust$citymap$cxxbridge1$192$RustCityMap$get_temperature(::rust::citymap::RustCityMap const &self) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$get_demand(::rust::citymap::RustCityMap const &self, ::rust::citymap::RCOIFstruct *return$) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$city_time(::rust::citymap::RustCityMap &self, ::std::int32_t minutes_delta, ::rust::citymap::TimeStruct *return$) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$update_world(::rust::citymap::RustCityMap &self, ::std::int32_t minutes_delta, ::rust::citymap::TimeStruct *return$) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_city_metadata(::rust::citymap::RustCityMap &self, ::rust::String *city_name, ::rust::String *mayor_name, ::rust::String *addon_set) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_financial_data(::rust::citymap::RustCityMap &self, ::std::int32_t money, ::std::int32_t population) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_budget(::rust::citymap::RustCityMap &self, ::std::int32_t police, ::std::int32_t fire, ::std::int32_t post, ::std::int32_t edu) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_tax(::rust::citymap::RustCityMap &self, double r, double c, double o, double i, double f) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$set_environment(::rust::citymap::RustCityMap &self, ::std::int32_t year, ::std::int32_t month, ::std::int32_t date, ::std::int32_t hour, ::std::int32_t min, bool weather, bool night) noexcept;

void rust$citymap$cxxbridge1$192$RustCityMap$generate_save_json(::rust::citymap::RustCityMap const &self, ::rust::String *return$) noexcept;
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

void RustCityMap::set_status(::std::int32_t pop, ::std::int32_t money, ::std::int32_t temp, ::rust::citymap::TimeStruct time, ::rust::citymap::RCOIFstruct demand) noexcept {
  ::rust::ManuallyDrop<::rust::citymap::TimeStruct> time$(::std::move(time));
  ::rust::ManuallyDrop<::rust::citymap::RCOIFstruct> demand$(::std::move(demand));
  rust$citymap$cxxbridge1$192$RustCityMap$set_status(*this, pop, money, temp, &time$.value, &demand$.value);
}

::std::int32_t RustCityMap::get_population() const noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$get_population(*this);
}

::std::int32_t RustCityMap::get_money() const noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$get_money(*this);
}

::std::int32_t RustCityMap::get_temperature() const noexcept {
  return rust$citymap$cxxbridge1$192$RustCityMap$get_temperature(*this);
}

::rust::citymap::RCOIFstruct RustCityMap::get_demand() const noexcept {
  ::rust::MaybeUninit<::rust::citymap::RCOIFstruct> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$get_demand(*this, &return$.value);
  return ::std::move(return$.value);
}

::rust::citymap::TimeStruct RustCityMap::city_time(::std::int32_t minutes_delta) noexcept {
  ::rust::MaybeUninit<::rust::citymap::TimeStruct> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$city_time(*this, minutes_delta, &return$.value);
  return ::std::move(return$.value);
}

::rust::citymap::TimeStruct RustCityMap::update_world(::std::int32_t minutes_delta) noexcept {
  ::rust::MaybeUninit<::rust::citymap::TimeStruct> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$update_world(*this, minutes_delta, &return$.value);
  return ::std::move(return$.value);
}

void RustCityMap::set_city_metadata(::rust::String city_name, ::rust::String mayor_name, ::rust::String addon_set) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_city_metadata(*this, &city_name, &mayor_name, &addon_set);
}

void RustCityMap::set_financial_data(::std::int32_t money, ::std::int32_t population) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_financial_data(*this, money, population);
}

void RustCityMap::set_budget(::std::int32_t police, ::std::int32_t fire, ::std::int32_t post, ::std::int32_t edu) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_budget(*this, police, fire, post, edu);
}

void RustCityMap::set_tax(double r, double c, double o, double i, double f) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_tax(*this, r, c, o, i, f);
}

void RustCityMap::set_environment(::std::int32_t year, ::std::int32_t month, ::std::int32_t date, ::std::int32_t hour, ::std::int32_t min, bool weather, bool night) noexcept {
  rust$citymap$cxxbridge1$192$RustCityMap$set_environment(*this, year, month, date, hour, min, weather, night);
}

::rust::String RustCityMap::generate_save_json() const noexcept {
  ::rust::MaybeUninit<::rust::String> return$;
  rust$citymap$cxxbridge1$192$RustCityMap$generate_save_json(*this, &return$.value);
  return ::std::move(return$.value);
}
} // namespace citymap
} // namespace rust

extern "C" {
::rust::citymap::RustCityMap *cxxbridge1$box$rust$citymap$RustCityMap$alloc() noexcept;
void cxxbridge1$box$rust$citymap$RustCityMap$dealloc(::rust::citymap::RustCityMap *) noexcept;
void cxxbridge1$box$rust$citymap$RustCityMap$drop(::rust::Box<::rust::citymap::RustCityMap> *ptr) noexcept;
} // extern "C"

namespace rust {
inline namespace cxxbridge1 {
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
