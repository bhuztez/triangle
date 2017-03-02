#pragma once
#include <cstddef>
#include <cmath>
#include <algorithm>
#include <type_traits>

namespace gl {
  namespace sl {
    using ::std::size_t;

    template<size_t N, typename T> struct vec;

    template<typename T>
    struct vec<2,T> {
      union {
        T data[2];
        struct {T x, y;};
        struct {T r, g;};
        struct {T s, t;};
      };

      T& operator[](size_t n) { return data[n]; }
      T const& operator[](size_t n) const { return data[n]; }

      vec() { }
      vec(T v) : data {v, v} { }
      vec(T v0, T v1) : data {v0, v1} { }
      vec(vec<3,T> const& v) : data {v[0], v[1]} { }
      vec(vec<4,T> const& v) : data {v[0], v[1]} { }

      template<typename U>
      vec(vec<2,U> const& v) : data {T(v[0]), T(v[1])} {}
    };


    template<typename T>
    struct vec<3,T> {
      union {
        T data[3];
        struct {T x, y, z;};
        struct {T r, g, b;};
        struct {T s, t, p;};
      };

      T& operator[](size_t n) { return data[n]; }
      T const& operator[](size_t n) const { return data[n]; }

      vec() { }
      vec(T v) : data {v, v, v} { }
      vec(T v0, T v1, T v2) : data {v0, v1, v2} { }
      vec(vec<2,T> const& u, T v) : data {u[0], u[1], v} { }
      vec(T u, vec<2,T> const& v) : data {u, v[0], v[1]} { }
      vec(vec<4,T> const& v) : data {v[0], v[1], v[2]} { }

      template<typename U>
      vec(vec<3,U> const& v) : data {T(v[0]), T(v[1]), T(v[2])} {}
    };


    template<typename T>
    struct vec<4,T> {
      union {
        T data[4];
        struct {T x, y, z, w;};
        struct {T r, g, b, a;};
        struct {T s, t, p, q;};
      };

      T& operator[](size_t n) { return data[n]; }
      T const& operator[](size_t n) const { return data[n]; }

      vec() { }
      vec(T v) : data {v, v, v, v} { }
      vec(T v0, T v1, T v2, T v3) : data {v0, v1, v2, v3} {  }
      vec(vec<3,T> const& u, T v) : data {u[0], u[1], u[2], v} { }
      vec(T u, vec<3,T> const& v) : data {u, v[0], v[1], v[2]} { }
      vec(vec<2,T> const& u, vec<2,T> const& v) : data {u[0], u[1], v[0], v[1]} { }
      vec(vec<2,T> const& u, T v, T w) : data {u[0], u[1], v, w} { }
      vec(T u, vec<2,T> const& v, T w) : data {u, v[0], v[1], w} { }
      vec(T u, T v, vec<2,T> const& w) : data {u, v, w[0], w[1]} { }

      template<typename U>
      vec(vec<4,U> const& v) : data {T(v[0]), T(v[1]), T(v[2]), T(v[3])} {}
    };

    template<size_t N, typename T> struct mat;

    template<typename T>
    struct mat<2,T>{
      vec<2,T> data[2];

      vec<2,T>& operator[](size_t n) { return data[n]; }
      vec<2,T> const& operator[](size_t n) const { return data[n]; }

      mat() { }
      mat(T const& v) : mat(v, v) { }
      mat(vec<2,T> const& v0, vec<2,T> const& v1) : data {v0, v1} { }
    };

    template<typename T>
    struct mat<3,T>{
      vec<3,T> data[3];

      vec<3,T>& operator[](size_t n) { return data[n]; }
      vec<3,T> const& operator[](size_t n) const { return data[n]; }

      mat() { }
      mat(T const& v) : mat(v, v, v) { }
      mat(vec<3,T> const& v0, vec<3,T> const& v1, vec<3,T> const& v2) : data {v0, v1, v2} { }
    };

    template<typename T>
    struct mat<4,T>{
      vec<4,T> data[4];

      vec<4,T>& operator[](size_t n) { return data[n]; }
      vec<4,T> const& operator[](size_t n) const { return data[n]; }

      mat() { }
      mat(T const& v) : mat(v, v, v, v) { }
      mat(vec<4,T> const& v0, vec<4,T> const& v1, vec<4,T> const& v2, vec<4,T> const& v3) : data {v0, v1, v2, v3} { }
    };

    template<typename F, size_t N, typename T>
    auto
    map1(F const& f, vec<N,T> const& u) -> vec<N,decltype(f(u[0]))> {
      using R = decltype(f(u[0]));
      vec<N,R> r(0.0);
      for(size_t i=0; i<N; i++)
        r[i] = f(u[i]);
      return r;
    }

    template<typename F, size_t N, typename T>
    auto
    map1(F const& f, mat<N,T> const& u) -> mat<N,decltype(f(u[0][0]))> {
      using R = decltype(f(u[0][0]));
      mat<N,R> r(0.0);
      for(size_t i=0; i<N; i++)
        r[i] = map1(f, u[i]);
      return r;
    }

    template<typename F, size_t N, typename T>
    auto
    map2(F const& f, vec<N,T> const& u, vec<N,T> const& v) -> vec<N,decltype(f(u[0],v[0]))> {
      using R = decltype(f(u[0],v[0]));
      vec<N,R> r(0.0);
      for(size_t i=0; i<N; i++)
        r[i] = f(u[i], v[i]);
      return r;
    }

    template<typename F, size_t N, typename T>
    auto
    map2(F const& f, mat<N,T> const& u, mat<N,T> const& v) -> mat<N,decltype(f(u[0][0],v[0][0]))> {
      using R = decltype(f(u[0][0],v[0][0]));
      mat<N,R> r(0.0);
      for(size_t i=0; i<N; i++)
        r[i] = map2(f, u[i], v[i]);
      return r;
    }

    template<size_t N, typename T>
    mat<N,T>
    operator*(mat<N,T> const& u, mat<N,T> const& v) {
      mat<N,T> r(0.0);
      for(size_t i=0; i<N; i++)
        for(size_t j=0; j<N; j++)
          for(size_t k=0; k<N; k++)
            r[i][k] += u[j][k] * v[i][j];
      return r;
    }

    template<size_t N, typename T>
    vec<N,T>
    operator*(mat<N,T> const& u, vec<N,T> const& v) {
      vec<N,T> r(0.0);
      for(size_t i=0; i<N; i++)
        for(size_t j=0; j<N; j++)
          r[j] += u[i][j] * v[i];
      return r;
    }

    template<size_t N, typename T>
    vec<N,T>
    operator*(vec<N,T> const& u, mat<N,T> const& v) {
      vec<N,T> r(0.0);
      for(size_t i=0; i<N; i++)
        for(size_t j=0; j<N; j++)
          r[i] += u[j] * v[i][j];
      return r;
    }

    template<size_t N, typename T>
    T
    dot(vec<N,T> const& u, vec<N,T> const& v) {
      T r(0.0);
      for(size_t i=0; i<N; i++)
        r += u[i]*v[i];
      return r;
    }

    template<typename T>
    vec<3,T>
    cross(vec<3,T> const& u, vec<3,T> const& v) {
      return {
        u.y * v.z - v.y * u.z,
        u.z * v.x - v.z * u.x,
        u.x * v.y - v.x * u.y };
    }

    template<size_t N, typename T>
    mat<N,T>
    matrixCompMult(mat<N,T> const& u, mat<N,T> const& v) {
      return map2([](T a, T b){return a*b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    operator+(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a+b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    operator+(T const& u, vec<N,T> const& v) {
      return vec<N,T>(u) + v;
    }

    template<size_t N, typename T>
    vec<N,T>
    operator+(vec<N,T> const& u, T const& v) {
      return u + vec<N,T>(v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator+(mat<N,T> const& u, mat<N,T> const& v) {
      return map2([](T a, T b){return a+b;}, u, v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator+(T const& u, mat<N,T> const& v) {
      return mat<N,T>(u) + v;
    }

    template<size_t N, typename T>
    mat<N,T>
    operator+(mat<N,T> const& u, T const& v) {
      return u + mat<N,T>(v);
    }


    template<size_t N, typename T>
    vec<N,T>
    operator-(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a-b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    operator-(T const& u, vec<N,T> const& v) {
      return vec<N,T>(u) - v;
    }

    template<size_t N, typename T>
    vec<N,T>
    operator-(vec<N,T> const& u, T const& v) {
      return u - vec<N,T>(v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator-(mat<N,T> const& u, mat<N,T> const& v) {
      return map2([](T a, T b){return a-b;}, u, v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator-(T const& u, mat<N,T> const& v) {
      return mat<N,T>(u) - v;
    }

    template<size_t N, typename T>
    mat<N,T>
    operator-(mat<N,T> const& u, T const& v) {
      return u - mat<N,T>(v);
    }


    template<size_t N, typename T>
    vec<N,T>
    operator*(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a*b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    operator*(T const& u, vec<N,T> const& v) {
      return vec<N,T>(u) * v;
    }

    template<size_t N, typename T>
    vec<N,T>
    operator*(vec<N,T> const& u, T const& v) {
      return u * vec<N,T>(v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator*(T const& u, mat<N,T> const& v) {
      return matrixCompMult(mat<N,T>(u), v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator*(mat<N,T> const& u, T const& v) {
      return matrixCompMult(u, mat<N,T>(v));
    }


    template<size_t N, typename T>
    vec<N,T>
    operator/(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a/b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    operator/(T const& u, vec<N,T> const& v) {
      return vec<N,T>(u) / v;
    }

    template<size_t N, typename T>
    vec<N,T>
    operator/(vec<N,T> const& u, T const& v) {
      return u / vec<N,T>(v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator/(mat<N,T> const& u, mat<N,T> const& v) {
      return map2([](T a, T b){return a/b;}, u, v);
    }

    template<size_t N, typename T>
    mat<N,T>
    operator/(T const& u, mat<N,T> const& v) {
      return mat<N,T>(u) / v;
    }

    template<size_t N, typename T>
    mat<N,T>
    operator/(mat<N,T> const& u, T const& v) {
      return u / mat<N,T>(v);
    }

    template<size_t N, typename T>
    vec<N, bool>
    lessThan(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a<b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N, bool>
    lessThanEqual(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a<=b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N, bool>
    greaterThan(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a>b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N, bool>
    greaterThanEqual(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a>=b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N, bool>
    equal(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a==b;}, u, v);
    }

    template<size_t N, typename T>
    vec<N, bool>
    notEqual(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return a!=b;}, u, v);
    }

    template<size_t N>
    bool
    any(vec<N,bool> const& u) {
      return !(u == vec<N,bool> {false});
    }

    template<size_t N>
    bool
    all(vec<N,bool> const& u) {
      return u == vec<N,bool> {true};
    }

    template<size_t N, typename T>
    bool
    operator==(vec<N,T> const& u, vec<N,T> const& v) {
      for(size_t i=0; i<N; i++)
        if (u[i] != v[i])
          return false;
      return true;
    }

    template<size_t N, typename T>
    bool
    operator==(mat<N,T> const& u, mat<N,T> const& v) {
      for(size_t i=0; i<N; i++)
        if (u[i] != v[i])
          return false;
      return true;
    }

    template<size_t N, typename T>
    bool
    operator!=(vec<N,T> const& u, vec<N,T> const& v) {
      return !(u == v);
    }

    template<size_t N, typename T>
    bool
    operator!=(mat<N,T> const& u, mat<N,T> const& v) {
      return !(u == v);
    }

    double
    radians(double x) {
      return x * M_PI / 180.0;
    }

    double
    degrees(double x) {
      return x / M_PI * 180.0;
    }

    template<size_t N, typename T>
    vec<N,T>
    radians(vec<N,T> const& u) {
      return map1([](T a){return radians(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    degrees(vec<N,T> const& u) {
      return map1([](T a){return degrees(a);}, u);
    }

    using ::std::sin;
    using ::std::cos;
    using ::std::tan;
    using ::std::asin;
    using ::std::acos;
    using ::std::atan;

    double
    atan(double x, double y) {
      return ::std::atan2(x,y);
    }

    template<size_t N, typename T>
    vec<N,T>
    sin(vec<N,T> const& u) {
      return map1([](T a){return sin(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    cos(vec<N,T> const& u) {
      return map1([](T a){return cos(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    tan(vec<N,T> const& u) {
      return map1([](T a){return tan(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    asin(vec<N,T> const& u) {
      return map1([](T a){return asin(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    acos(vec<N,T> const& u) {
      return map1([](T a){return acos(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    atan(vec<N,T> const& u) {
      return map1([](T a){return atan(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    atan(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return atan(a,b);}, u, v);
    }

    using ::std::pow;
    using ::std::exp;
    using ::std::log;
    using ::std::exp2;
    using ::std::log2;

    template<size_t N, typename T>
    vec<N,T>
    pow(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a, T b){return pow(a,b);}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    exp(vec<N,T> const& u) {
      return map1([](T a){return exp(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    log(vec<N,T> const& u) {
      return map1([](T a){return log(a);}, u);
    }


    template<size_t N, typename T>
    vec<N,T>
    exp2(vec<N,T> const& u) {
      return map1([](T a){return exp2(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    log2(vec<N,T> const& u) {
      return map1([](T a){return log2(a);}, u);
    }

    using ::std::sqrt;

    double
    inversesqrt(double x) {
      return 1.0 / sqrt(x);
    }

    template<size_t N, typename T>
    vec<N,T>
    sqrt(vec<N,T> const& u) {
      return map1([](T a){return sqrt(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    inversesqrt(vec<N,T> const& u) {
      return map1([](T a){return inversesqrt(a);}, u);
    }

    using ::std::abs;

    template<typename T>
    T
    sign(T x) {
      return (x>0)-(x<0);
    }

    template<size_t N, typename T>
    vec<N,T>
    abs(vec<N,T> const& u) {
      return map1([](T a){return abs(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    sign(vec<N,T> const& u) {
      return map1([](T a){return sign(a);}, u);
    }

    using ::std::floor;
    using ::std::ceil;

    double
    fract(double x) {
      return x - floor(x);
    }

    template<size_t N, typename T>
    vec<N,T>
    floor(vec<N,T> const& u) {
      return map1([](T a){return floor(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    ceil(vec<N,T> const& u) {
      return map1([](T a){return ceil(a);}, u);
    }

    template<size_t N, typename T>
    vec<N,T>
    fract(vec<N,T> const& u) {
      return map1([](T a){return fract(a);}, u);
    }

    template<typename T>
    T
    mod(T x, T y) {
      return x % y;
    }

    template<size_t N, typename T>
    vec<N,T>
    mod(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a){return mod(a);}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    mod(vec<N,T> const& u, T v) {
      return mod(u, vec<N,T>(v));
    }

    using ::std::min;
    using ::std::max;

    template<size_t N, typename T>
    vec<N,T>
    min(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a){return min(a);}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    min(vec<N,T> const& u, T v) {
      return min(u, vec<N,T>(v));
    }

    template<size_t N, typename T>
    vec<N,T>
    max(vec<N,T> const& u, vec<N,T> const& v) {
      return map2([](T a){return max(a);}, u, v);
    }

    template<size_t N, typename T>
    vec<N,T>
    max(vec<N,T> const& u, T v) {
      return max(u, vec<N,T>(v));
    }

    template<typename T, typename U>
    U
    interpolate(T const& P, U const& x, U const& y, U const& z) {
      return P[0] * x + P[1] * y + P[2] * z;
    }
  }
}
