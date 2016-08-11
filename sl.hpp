#pragma once
#include <cstddef>
#include <cmath>
#include <type_traits>

namespace sl{
  template<typename T> struct vec2;
  template<typename T> struct vec3;
  template<typename T> struct vec4;

  template<typename T>
  struct vec2{
    union {
      T data[2];
      struct {T x, y;};
      struct {T r, g;};
      struct {T s, t;};
    };

    T& operator[](size_t n) { return data[n]; }
    T const& operator[](size_t n) const { return data[n]; }

    vec2() { }
    vec2(T v) : data {v,v} { }
    vec2(T v0, T v1) : data {v0, v1} { }
    vec2(vec3<T> const& v) : data {v[0], v[1]} { }
    vec2(vec4<T> const& v) : data {v[0], v[1]} { }
  };

  template<typename T>
  struct vec3{
    union {
      T data[3];
      struct {T x, y, z;};
      struct {T r, g, b;};
      struct {T s, t, p;};
    };

    T& operator[](size_t n) { return data[n]; }
    T const& operator[](size_t n) const { return data[n]; }

    vec3() { }
    vec3(T v) : data {v,v,v} { }
    vec3(T v0, T v1, T v2) : data {v0, v1, v2} { }
    vec3(vec2<T> const& u, T v) : data {u[0], u[1], v} { }
    vec3(T u, vec2<T> const& v) : data {u, v[0], v[1]} { }
    vec3(vec4<T> const& v) : data {v[0], v[1], v[2]} { }
  };

  template<typename T>
  struct vec4{
    union {
      T data[4];
      struct {T x, y, z, w;};
      struct {T r, g, b, a;};
      struct {T s, t, p, q;};
    };

    T& operator[](size_t n) { return data[n]; }
    T const& operator[](size_t n) const { return data[n]; }

    vec4() { }
    vec4(T v) : data {v,v,v,v} { }
    vec4(T v0, T v1, T v2, T v3) : data {v0, v1, v2, v3} {  }
    vec4(vec3<T> const& u, T v) : data {u[0], u[1], u[2], v} { }
    vec4(T u, vec3<T> const& v) : data {u, v[0], v[1], v[2]} { }
    vec4(vec2<T> const& u, vec2<T> const& v) : data {u[0], u[1], v[0], v[1]} { }
    vec4(vec2<T> const& u, T v, T w) : data {u[0], u[1], v, w} { }
    vec4(T u, vec2<T> const& v, T w) : data {u, v[0], v[1], w} { }
    vec4(T u, T v, vec2<T> const& w) : data {u, v, w[0], w[1]} { }
  };

  template<typename T>
  struct mat2{
    vec2<T> data[2];

    vec2<T>& operator[](size_t n) { return data[n]; }
    vec2<T> const& operator[](size_t n) const { return data[n]; }

    mat2() { }
    mat2(T v) : data {vec2<T>(v), vec2<T>(v)} { }
    mat2(vec2<T> const& v0, vec2<T> const& v1) : data {v0, v1} { }
  };

  template<typename T>
  struct mat3{
    vec3<T> data[3];

    vec3<T>& operator[](size_t n) { return data[n]; }
    vec3<T> const& operator[](size_t n) const { return data[n]; }

    mat3() { }
    mat3(T v) : data {vec3<T>(v), vec3<T>(v), vec3<T>(v)} { }
    mat3(mat2<T> const& v) : data{{v[0],0}, {v[1],0}, {0,0,1}} { }
    mat3(vec3<T> const& v0, vec3<T> const& v1, vec3<T> const& v2) : data {v0, v1, v2} { }
  };

  template<typename T>
  struct mat4{
    vec4<T> data[4];

    vec4<T>& operator[](size_t n) { return data[n]; }
    vec4<T> const& operator[](size_t n) const { return data[n]; }

    mat4() { }
    mat4(T v) : data {vec4<T>(v), vec4<T>(v), vec4<T>(v), vec4<T>(v)} { }
    mat4(mat3<T> const& v) : data{{v[0],0}, {v[1],0}, {v[2],0}, {0,0,0,1}} { }
    mat4(vec4<T> const& v0, vec4<T> const& v1, vec4<T> const& v2, vec4<T> const& v3) : data {v0, v1, v2, v3} { }
  };

#define MATMUL(N)                                               \
  template<typename T>                                          \
  mat##N<T> operator*(mat##N<T> const& u, mat##N<T> const& v) { \
    mat##N<T> r(0.0);                                           \
    for(size_t i=0; i<N; i++)                                   \
      for(size_t j=0; j<N; j++)                                 \
        for(size_t k=0; k<N; k++)                               \
          r[i][k] += u[j][k] * v[i][j];                         \
    return r;                                                   \
  }                                                             \
                                                                \
  template<typename T>                                          \
  vec##N<T> operator*(mat##N<T> const& u, vec##N<T> const& v) { \
    vec##N<T> r(0.0);                                           \
    for(size_t i=0; i<N; i++)                                   \
      for(size_t j=0; j<N; j++)                                 \
        r[j] += u[i][j] * v[i];                                 \
    return r;                                                   \
  }                                                             \
                                                                \
  template<typename T>                                          \
  vec##N<T> operator*(vec##N<T> const& u, mat##N<T> const& v) { \
    vec##N<T> r(0.0);                                           \
    for(size_t i=0; i<N; i++)                                   \
      for(size_t j=0; j<N; j++)                                 \
        r[i] += u[j] * v[i][j];                                 \
    return r;                                                   \
  }                                                             \

  MATMUL(2)
  MATMUL(3)
  MATMUL(4)

  template<typename T> struct component_type;
  template<typename T> struct component_type { typedef typename ::std::enable_if<::std::is_arithmetic<T>::value, T>::type type; };
  template<typename T> struct component_type<vec2<T>> { typedef T type; };
  template<typename T> struct component_type<vec3<T>> { typedef T type; };
  template<typename T> struct component_type<vec4<T>> { typedef T type; };
  template<typename T> struct component_type<mat2<T>> { typedef T type; };
  template<typename T> struct component_type<mat3<T>> { typedef T type; };
  template<typename T> struct component_type<mat4<T>> { typedef T type; };

  template<typename U, typename T>
  vec2<U>
  map1(U (*f)(T), vec2<T> const& v) {
    return {f(v[0]), f(v[1])};
  }

  template<typename U, typename T>
  vec3<U>
  map1(U (*f)(T), vec3<T> const& v) {
    return {f(v[0]), f(v[1]), f(v[2])};
  }

  template<typename U, typename T>
  vec4<U>
  map1(U (*f)(T), vec4<T> const& v) {
    return {f(v[0]), f(v[1]), f(v[2]), f(v[3])};
  }

  template<typename U, typename T>
  mat2<U>
  map1(U (*f)(T), mat2<T> const& v) {
    return {map1(f,v[0]), map1(f,v[1])};
  }

  template<typename U, typename T>
  mat3<U>
  map1(U (*f)(T), mat3<T> const& v) {
    return {map1(f,v[0]), map1(f,v[1]), map1(f,v[2])};
  }

  template<typename U, typename T>
  mat4<U>
  map1(U (*f)(T), mat4<T> const& v) {
    return {map1(f,v[0]), map1(f,v[1]), map1(f,v[2]), map1(f,v[3])};
  }

  template<typename U, typename T>
  vec2<U>
  map2(U (*f)(T,T), vec2<T> const& u, vec2<T> const& v) {
    return {f(u[0], v[0]), f(u[1], v[1])};
  }

  template<typename U, typename T>
  vec3<U>
  map2(U (*f)(T,T), vec3<T> const& u, vec3<T> const& v) {
    return {f(u[0], v[0]), f(u[1], v[1]), f(u[2], v[2])};
  }

  template<typename U, typename T>
  vec4<U>
  map2(U (*f)(T,T), vec4<T> const& u, vec4<T> const& v) {
    return {f(u[0], v[0]), f(u[1], v[1]), f(u[2], v[2]), f(u[3], v[3])};
  }

  template<typename U, typename T>
  mat2<U>
  map2(U (*f)(T,T), mat2<T> const& u, mat2<T> const& v) {
    return {map2(f, u[0], v[0]), map2(f, u[1], v[1])};
  }

  template<typename U, typename T>
  mat3<U>
  map2(U (*f)(T,T), mat3<T> const& u, mat3<T> const& v) {
    return {map2(f, u[0], v[0]), map2(f, u[1], v[1]), map2(f, u[2], v[2])};
  }

  template<typename U, typename T>
  mat4<U>
  map2(U (*f)(T,T), mat4<T> const& u, mat4<T> const& v) {
    return {map2(f, u[0], v[0]), map2(f, u[1], v[1]), map2(f, u[2], v[2]), map2(f, u[3], v[3])};
  }


#define OP1(R,F,O)                                                      \
  template<typename T>                                                  \
  typename ::std::enable_if<::std::is_arithmetic<T>::value, R>::type    \
  F(T x) {                                                              \
    return O x;                                                         \
  }                                                                     \

#define OP2(R,F,O)                                                      \
  template<typename T>                                                  \
  typename ::std::enable_if<::std::is_arithmetic<T>::value, R>::type    \
  F(T x, T y) {                                                         \
    return x O y;                                                       \
  }                                                                     \

#define FN(R,F)                                                         \
  template<typename T>                                                  \
  typename ::std::enable_if<::std::is_arithmetic<T>::value, typename ::std::conditional<::std::is_floating_point<T>::value, T, R>::type>::type \
    F                                                                   \

#define FN1(R,F) FN(R,F)(T x)
#define FN2(R,F) FN(R,F)(T x, T y)

#define MAP1(F,G)                                                       \
  template<typename V, typename Enable=typename ::std::enable_if<!::std::is_arithmetic<V>::value,void>::type> \
  auto                                                                  \
  F(V const& v) {                                                       \
    typedef typename component_type<V>::type VC;                        \
    typedef decltype(G(::std::declval<VC>())) R;                        \
    return map1<R,VC>(G, v);                                            \
  }                                                                     \

#define MAP2(F,G)                                                       \
  template<typename U, typename V>                                      \
  auto                                                                  \
  F(U const& u, V const& v) {                                           \
    typedef typename component_type<U>::type UC;                        \
    typedef typename component_type<V>::type VC;                        \
    typedef decltype(G(::std::declval<UC>(), ::std::declval<VC>())) R;  \
    return map2<R,UC>(G, u, v);                                         \
  }                                                                     \

#define CMP2(O, F)                                                      \
  template<typename U, typename V, typename Enable=typename ::std::enable_if<!(::std::is_arithmetic<U>::value && ::std::is_arithmetic<V>::value), void>::type> \
  bool                                                                  \
  operator O(U const& u, V const& v) {                                  \
    return all(F(u,v));                                                 \
  }                                                                     \

  OP2(T,    plus,             +)
  OP2(T,    minus,            -)
  OP2(T,    multiplies,       *)
  OP2(T,    divides,          /)
  OP2(T,    mod,              %)

  OP2(bool, lessThan,         <)
  OP2(bool, lessThanEqual,    <=)
  OP2(bool, greaterThan,      >)
  OP2(bool, greaterThanEqual, >=)
  OP2(bool, equal,            ==)
  OP2(bool, notEqual,         !=)
  OP1(bool, logical_not,      !)

  FN1(double, radians) { return x * M_PI / 180.0; }
  FN1(double, degrees) { return x / M_PI * 180.0; }
  using ::std::sin;  using ::std::cos;  using ::std::tan;
  using ::std::asin; using ::std::acos; using ::std::atan;
  FN2(double, atan) { return ::std::atan2(x,y); }
  using ::std::pow;
  using ::std::exp;  using ::std::log;
  using ::std::exp2; using ::std::log2;
  using ::std::sqrt;
  FN1(double, inversesqrt) { return 1.0 / sqrt(x);  }
  using ::std::abs;
  FN1(T, sign) { return (x>0)-(x<0); }
  using ::std::floor; using ::std::ceil;

  template<typename T>
  typename ::std::enable_if<::std::is_floating_point<T>::value, T>::type
  fract(T x) {
    return x - floor(x);
  }

  FN2(T, min) { return (x<y)?x:y; }
  FN2(T, max) { return (x>y)?x:y; }

  MAP2(operator+,        plus)
  MAP2(operator-,        minus)
  MAP2(operator*,        multiplies)
  MAP2(operator/,        divides)
  MAP2(operator%,        mod)

  MAP2(equal,            equal)
  MAP2(notEqual,         notEqual)
  MAP2(lessThan,         lessThan)
  MAP2(lessThanEqual,    lessThanEqual)
  MAP2(greaterThan,      greaterThan)
  MAP2(greaterThanEqual, greaterThanEqual)
  MAP1(logical_not,      logical_not)

  bool all(vec2<bool> const& v) { return v[0]&&v[1]; }
  bool all(vec3<bool> const& v) { return v[0]&&v[1]&&v[2]; }
  bool all(vec4<bool> const& v) { return v[0]&&v[1]&&v[2]&&v[3]; }

  bool all(mat2<bool> const& m) { return all(m[0])&&all(m[1]); }
  bool all(mat3<bool> const& m) { return all(m[0])&&all(m[1])&&all(m[2]); }
  bool all(mat4<bool> const& m) { return all(m[0])&&all(m[1])&&all(m[2])&&all(m[3]); }

  bool any(vec2<bool> const& v) { return v[0]||v[1]; }
  bool any(vec3<bool> const& v) { return v[0]||v[1]||v[2]; }
  bool any(vec4<bool> const& v) { return v[0]||v[1]||v[2]||v[3]; }

  bool any(mat2<bool> const& m) { return any(m[0])||any(m[1]); }
  bool any(mat3<bool> const& m) { return any(m[0])||any(m[1])||any(m[2]); }
  bool any(mat4<bool> const& m) { return any(m[0])||any(m[1])||any(m[2])||any(m[3]); }

  template<typename V, typename Enable=typename ::std::enable_if<!::std::is_arithmetic<V>::value,void>::type>
  bool
  operator!(V const& v) {
    return all(logical_not(v));
  }

  CMP2(==, equal)       CMP2(!=, notEqual)
  CMP2(<,  lessThan)    CMP2(<=, lessThanEqual)
  CMP2(>,  greaterThan) CMP2(>=, greaterThanEqual)

  MAP1(radians, radians)  MAP1(degrees, degrees)
  MAP1(sin,  sin)    MAP1(cos,  cos)    MAP1(tan,  tan)
  MAP1(asin, asin)   MAP1(acos, acos)   MAP1(atan, atan)
  MAP2(atan, atan)

  MAP2(pow,  pow)
  MAP1(exp,  exp)    MAP1(log, log)
  MAP1(exp2, exp2)   MAP1(log2, log2)
  MAP1(sqrt, sqrt)
  MAP1(inversesqrt, inversesqrt)
  MAP1(abs,  abs)    MAP1(sign, sign)
  MAP1(floor, floor) MAP1(ceil, ceil)
  MAP1(fract, fract) MAP2(mod, mod)
  MAP2(min,  min)    MAP2(max,  max)

  template<typename T> T dot(vec2<T> const& u, vec2<T> const& v) { return u[0]*v[0]+u[1]*v[1]; }
  template<typename T> T dot(vec3<T> const& u, vec3<T> const& v) { return u[0]*v[0]+u[1]*v[1]+u[2]*v[2]; }
  template<typename T> T dot(vec4<T> const& u, vec4<T> const& v) { return u[0]*v[0]+u[1]*v[1]+u[2]*v[2]+u[3]*v[3]; }

  template<typename T>
  vec3<T>
  cross(vec3<T> const& u, vec3<T> const& v) {
    return {
      u.y * v.z - v.y * u.z,
      u.z * v.x - v.z * u.x,
      u.x * v.y - v.x * u.y };
  }

  template<typename T>
  void
  varying_attribs(T w __attribute__((unused))) {
  }

  template<typename T, typename Arg, typename... Args>
  void
  varying_attribs(T w, Arg& arg, Args&... args) {
    arg = arg * w;
    varying_attribs(w, args...);
  }

  template<typename T, typename... Args>
  void
  varying(vec4<T>& gl_Position, Args&... args) {
    gl_Position = {vec3<T>(gl_Position) / gl_Position.w, T(1.0) / gl_Position.w};
    varying_attribs(gl_Position.w, args...);
  }

  template<typename T, typename U>
  U
  interpolate(T const& P, U const& x, U const& y, U const& z) {
    return P[0] * x + P[1] * y + P[2] * z;
  }

  template<typename T, typename U, typename I>
  auto
  interpolate(T const& P, U const& u, I const& idx) {
    return interpolate(P, u[idx[0]], u[idx[1]], u[idx[2]]);
  }
}
