#pragma once

#include <new>
#include <cstdlib>
#include <cstddef>
#include <type_traits>


namespace gl {
  namespace shader {
    using ::std::size_t;

    template<typename...> struct LIST {};

    template<typename, typename> struct APPEND;

    template<typename U, typename... T>
    struct APPEND<LIST<T...>, U> {
      using TYPE = LIST<T..., U>;
    };

    template <char...> struct STRING {};

    template<typename N, typename V>
    struct PAIR {
      using NAME = N;
      using TYPE = V;
    };

    template<typename, typename, typename = void> struct LOOKUP;

    template<typename K, typename V, typename... L>
    struct LOOKUP<K, LIST<PAIR<K,V>, L...>, void> {
      using TYPE = V;
    };

    template<typename K, typename V, typename... L>
    struct LOOKUP<K, LIST<V, L...>, ::std::void_t<typename LOOKUP<K, LIST<L...>>::TYPE>> {
      using TYPE = typename LOOKUP<K, LIST<L...>>::TYPE;
    };

    template<typename K, typename L>
    using LOOKUP_T = typename LOOKUP<K,L>::TYPE;

    template<typename K, typename V, typename L, typename = void>
    struct INSERT {
      using TYPE = typename APPEND<L, PAIR<K,V>>::TYPE;
    };

    template<typename K, typename V, typename L>
    struct INSERT<K,V,L, ::std::void_t<LOOKUP_T<K,L>>> {
      static_assert(::std::is_same<V, LOOKUP_T<K,L>>::value);
      using TYPE = L;
    };

    template<typename, typename...> struct EXTEND;

    template<typename L>
    struct EXTEND<L> {
      using TYPE = L;
    };

    template<typename L, typename... U>
    struct EXTEND<L, LIST<>, U...> {
      using TYPE = typename EXTEND<L, U...>::TYPE;
    };

    template<typename L, typename T, typename... U, typename... V>
    struct EXTEND<L, LIST<T, U...>, V...> {
      using TYPE = typename EXTEND<typename INSERT<typename T::NAME, typename T::TYPE, L>::TYPE, LIST<U...>, V...>::TYPE;
    };

    template<typename... U>
    using MAKE_PAIR_LIST = typename EXTEND<LIST<>, U...>::TYPE;


    template<typename, typename...> struct VALIDATE;

    template<typename L>
    struct VALIDATE<L> {
      using TYPE = L;
    };

    template<typename L, typename... U>
    struct VALIDATE<L, LIST<>, U...> {
      using TYPE = typename VALIDATE<L, U...>::TYPE;
    };

    template<typename L, typename T, typename... U, typename... V>
    struct VALIDATE<L, LIST<T, U...>, V...> {
      static_assert(::std::is_same<typename T::TYPE, LOOKUP_T<typename T::NAME, L>>::value);
      using TYPE = typename VALIDATE<L, LIST<U...>, V...>::TYPE;
    };

    template<typename L, typename... T>
    using ENSURE_DEFINED = typename VALIDATE<L, T...>::TYPE;

    template<typename, typename ...>
    constexpr
    size_t INDEX_OF;

    template<typename T, typename... U>
    constexpr
    size_t INDEX_OF<T, T, U...> = 0;

    template<typename T, typename U, typename... V>
    constexpr
    size_t INDEX_OF<T, U, V...> = 1 + INDEX_OF<T, V...>;

    template<typename N, typename T, typename V, V* T::* x>
    struct MEMBER {
      using NAME = N;
      using TYPE = V;
      static constexpr auto POINTER = x;
    };

    template<typename> struct BINDING_DATA;

    template<typename... F>
    struct BINDING_DATA<LIST<F...>> {
      using FIELDS = LIST<F...>;
      void *ptr[sizeof...(F)];

      template<typename T>
      inline
      void
      set(LOOKUP_T<T, FIELDS> *p) {
        ptr[INDEX_OF<T, typename F::NAME...>] = p;
      }

      template<typename T, typename... M>
      inline
      void
      bind(T *x [[ gnu::unused ]], size_t n [[ gnu::unused ]]) {
        auto f [[ gnu::unused ]] = [](auto&a, auto *p){a = p;};
        (f(x->*(M::POINTER), lookup<M>(n)),...);
      }

      template<typename M>
      inline
      typename M::TYPE *
      lookup(size_t n) {
        return ((typename M::TYPE *)(ptr[INDEX_OF<typename M::NAME, typename F::NAME...>]) + n);
      }

      void
      alloc(size_t n) {
        new (&ptr) decltype(ptr) {::std::malloc(sizeof(typename F::TYPE)*n)...};
      }

      void
      free() {
        for(size_t i=0; i < sizeof...(F); i++)
          ::std::free(ptr[i]);
      }

    };

    template<typename FIELDS, typename ...M>
    struct BINDING {
      BINDING_DATA<FIELDS> data;

      template<typename T>
      inline
      void
      set(T, LOOKUP_T<T, FIELDS> *p) {
        data.set<T>(p);
      }

      template<typename T, typename... L>
      inline
      void
      bind(T *x, size_t n, LIST<L...>) {
        data.bind<T,L...>(x, n);
      }

      template<typename T, typename L = LOOKUP_T<T, LIST<M...>>>
      inline
      void
      bind(T *v, size_t n=0) {
        bind(v, n, L());
      }
    };


    template<typename, size_t>
    struct Counter{
      friend constexpr auto state(Counter);
    };

    template<typename T, size_t N, typename V>
    struct Set {
      friend constexpr auto state(Counter<T, N>) {
        return V {};
      }
    };

    template<typename T, size_t N=0>
    constexpr
    size_t
    next(float, Counter<T,N> = {}) {
      return N;
    }

    template<typename T, size_t N=0,
             typename = decltype(state(Counter<T,N>{}))
             >
    constexpr
    size_t
    next(int, Counter<T,N> = {}, size_t value = next(0, Counter<T,N+1>{})) {
      return value;
    }

    template<typename T> struct T_uniform;
    template<typename T> struct T_attribute;
    template<typename T> struct T_varying;

    template<typename T, size_t = sizeof(T)>
    constexpr
    bool
    defined(int) {
      return true;
    }

    template<typename T>
    constexpr
    bool
    defined(...) {
      return false;
    }

    template<template<typename> typename TYPE,
             typename T, typename N, typename V, V* T::* M,
             typename = ::std::enable_if_t<not defined<T>(0)>,
             size_t C = next<TYPE<T>>(0),
             typename = ::std::enable_if_t<(C > 0)>,
             typename State = decltype(state(Counter<TYPE<T>,C-1>{})),
             size_t = sizeof(Set<TYPE<T>, C, typename APPEND<State, MEMBER<N,T,V,M>>::TYPE>)
      >
    constexpr
    bool
    declare() {
      return true;
    }

    template<template<typename> typename TYPE,
             typename T,
             typename = ::std::enable_if_t<not defined<T>(0)>,
             size_t = sizeof(Set<TYPE<T>, 0, LIST<>>)
             >
    constexpr
    bool
    enable() {
      return true;
    }

    template<template<typename> typename TYPE, typename T,
             typename = ::std::enable_if_t<defined<T>(0)>,
             size_t C = next<TYPE<T>>(0),
             typename = ::std::enable_if_t<(C > 0)>,
             typename State = decltype(state(Counter<TYPE<T>,C-1>{}))
             >
    using MEMBERS = State;

    template<typename Program, typename... M>
    struct INTERPOLATION {
      char buf[(0 + ... + sizeof(typename M::TYPE))];

      INTERPOLATION(typename Program::vec3 const& P, decltype(Program::varying.data)& data, size_t i0, size_t i1, size_t i2) {
        char *p = buf;

        auto f = [&p,&P](size_t size, auto*x, auto*y, auto*z){
          *((decltype(x))p) = sl::interpolate(P, *x, *y, *z);
          p += size;
        };

        (f(sizeof(typename M::TYPE),
           data.template lookup<M>(i0),
           data.template lookup<M>(i1),
           data.template lookup<M>(i2)),...);
      }

      void
      bind(typename Program::Fragment *frag) {
        char *p = buf;

        auto f = [frag,&p](size_t size, auto m) {
          frag->*m = (::std::remove_reference_t<decltype(frag->*m)>)p;
          p += size;
        };

        (f(sizeof(typename M::TYPE), M::POINTER),...);
      }

    };

    template<typename T, template<typename> typename V, template<typename> typename F>
    struct Link {
      using Float = T;
      using Vertex = V<T>;
      using Fragment = F<T>;

      using vec2 = ::gl::sl::vec<2,T>;
      using vec3 = ::gl::sl::vec<3,T>;
      using vec4 = ::gl::sl::vec<4,T>;
      using mat2 = ::gl::sl::mat<2,T>;
      using mat3 = ::gl::sl::mat<3,T>;
      using mat4 = ::gl::sl::mat<4,T>;

      BINDING<
        MAKE_PAIR_LIST<MEMBERS<T_uniform, Vertex>, MEMBERS<T_uniform, Fragment>>,
        PAIR<Vertex, MEMBERS<T_uniform, Vertex>>,
        PAIR<Fragment, MEMBERS<T_uniform, Fragment>>
        > uniform;

      BINDING<
        MAKE_PAIR_LIST<MEMBERS<T_attribute, Vertex>>,
        PAIR<Vertex, MEMBERS<T_attribute, Vertex>>
        > attribute;

      BINDING<
        ENSURE_DEFINED<MAKE_PAIR_LIST<MEMBERS<T_varying, Vertex>>, MEMBERS<T_varying, Fragment>>,
        PAIR<Vertex, MEMBERS<T_varying, Vertex>>,
        PAIR<Fragment, MEMBERS<T_varying, Fragment>>
        > varying;

      size_t vertices;
      vec4 *gl_Position;

      Link(size_t n) : vertices(n) {
        gl_Position = (vec4 *)::std::malloc(sizeof(vec4) * n);
        varying.data.alloc(n);
      }

      ~Link() {
        varying.data.free();
        free(gl_Position);
      }

      template<typename L=MEMBERS<T_varying, Vertex>>
      static
      inline
      void
      fix_varying(Vertex *v, T w) {
        fix_varying(v, w, L());
      }

      template<typename... U>
      static
      inline
      void
      fix_varying(Vertex *v, T w, LIST<U...>) {
        auto f = [&w](auto *p){ (*p) = (*p) * w; };
        (f(v->*(U::POINTER)),...);
      }

      template<typename L=MEMBERS<T_varying, Fragment>>
      inline
      auto
      interpolate(vec3 const& P, size_t i0, size_t i1, size_t i2) {
        return interpolate(P, i0, i1, i2, L());
      }

      template<typename... U>
      inline
      auto
      interpolate(vec3 const& P, size_t i0, size_t i1, size_t i2, LIST<U...>) {
        return INTERPOLATION<Link, U...>(P, varying.data, i0, i1, i2);
      }
    };
  }

  using shader::Link;
}

template <typename T, T... chars>
constexpr ::gl::shader::STRING<chars...>
operator""_s() {
  return { };
}


#define _VAR(t, v, ...)                                 \
  union {                                               \
    ::std::add_lvalue_reference_t< __VA_ARGS__ > v;     \
    ::std::add_pointer_t< __VA_ARGS__ > _ptr_##v;       \
  };                                                    \
  static_assert(                                        \
                ::gl::shader::declare<t, __CLASS__,     \
                decltype(#v##_s),                       \
                __VA_ARGS__,                            \
                &__CLASS__::_ptr_##v>())


#define UNIFORM(v, ...) _VAR(::gl::shader::T_uniform, v, __VA_ARGS__)
#define ATTRIBUTE(v, ...) _VAR(::gl::shader::T_attribute, v, __VA_ARGS__)
#define VARYING(v, ...) _VAR(::gl::shader::T_varying, v, __VA_ARGS__)


#define VERTEX_SHADER(T,F)                                              \
  using __CLASS__ = T;                                                  \
  static_assert(::gl::shader::enable<::gl::shader::T_uniform,T>());     \
  static_assert(::gl::shader::enable<::gl::shader::T_attribute,T>());   \
  static_assert(::gl::shader::enable<::gl::shader::T_varying,T>());     \
  using vec2 = ::gl::sl::vec<2,F>;                                      \
  using vec3 = ::gl::sl::vec<3,F>;                                      \
  using vec4 = ::gl::sl::vec<4,F>;                                      \
  using mat2 = ::gl::sl::mat<2,F>;                                      \
  using mat3 = ::gl::sl::mat<3,F>;                                      \
  using mat4 = ::gl::sl::mat<4,F>;                                      \
  union {                                                               \
    vec4& gl_Position;                                                  \
    vec4* _ptr_gl_Position;                                             \
  }

#define FRAGMENT_SHADER(T,F)                                            \
  using __CLASS__ = T;                                                  \
  static_assert(::gl::shader::enable<::gl::shader::T_uniform,T>());     \
  static_assert(::gl::shader::enable<::gl::shader::T_varying,T>());     \
  using vec2 = ::gl::sl::vec<2,F>;                                      \
  using vec3 = ::gl::sl::vec<3,F>;                                      \
  using vec4 = ::gl::sl::vec<4,F>;                                      \
  using mat2 = ::gl::sl::mat<2,F>;                                      \
  using mat3 = ::gl::sl::mat<3,F>;                                      \
  using mat4 = ::gl::sl::mat<4,F>;                                      \
  union {                                                               \
    vec4& gl_FragColor;                                                 \
    vec4* _ptr_gl_FragColor;                                            \
  }
