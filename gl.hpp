#pragma once
#include "sl.hpp"

namespace gl {
  using namespace ::sl;

  template<typename T>
  T
  area2(vec2<T> const& v0, vec2<T> const& v1, vec2<T> const& v2) {
    return cross(vec3<T> {v1 - v0, 0.0}, vec3<T> {v2 - v0, 0.0}).z;
  }

  template<typename T>
  size_t
  min3(T x, T y, T z, size_t w) {
    return max(size_t(floor(min(min(x,y),z))), w);
  }

  template<typename T>
  size_t
  max3(T x, T y, T z, size_t w) {
    return min(size_t(ceil(max(max(x,y),z))), w);
  }

  template<typename T>
  bool
  outside(vec2<T> const& u, vec2<T> const& v, vec2<T> box[4]) {
    return (vec4<T> { area2(u,v,box[0]), area2(u,v,box[1]), area2(u,v,box[2]), area2(u,v,box[3]) }) < T(0.0);
  }

  struct Context {
    const size_t width, height;
    unsigned char (*buffer)[4];

    Context(size_t width, size_t height, unsigned char (*buffer)[4])
      : width(width), height(height), buffer(buffer) {
    }

    template<typename Prog, typename Triangle>
    void
    draw(Prog& prog, size_t N, Triangle const& triangles) {
      struct ID {
        size_t operator[](size_t n) const { return n; }
      };

      draw(prog, N, triangles, ID());
    }

    template<typename Prog, typename Triangle, typename Index>
    void
    draw(Prog& prog, size_t N, Triangle const& triangles, Index const& Idx){
      typedef typename Prog::T T;

      vec4<T> gl_Position[N];

      for(size_t i=0; i<N; i++) {
        gl_Position[i] = prog.vertex(i);
        gl_Position[i] = {((gl_Position[i] + T(1.0)) * T(0.5) * vec3<T> {T(width), T(height), 1.0}), gl_Position[i].w};
      }

      for(auto idx : triangles) {
        idx = {Idx[idx.x], Idx[idx.y], Idx[idx.z]};
        vec4<T> v[3] = {gl_Position[idx.x], gl_Position[idx.y], gl_Position[idx.z]};

        size_t left   = min3(v[0].x, v[1].x, v[2].x, 0);
        size_t right  = max3(v[0].x, v[1].x, v[2].x, width);
        size_t bottom = min3(v[0].y, v[1].y, v[2].y, 0);
        size_t top    = max3(v[0].y, v[1].y, v[2].y, height);

        if((left >= right) || (bottom >= top))
          continue;

        auto area = area2<T>(v[0], v[1], v[2]);

        for(size_t by=bottom; by<top; by+=4)
          for(size_t bx=left; bx<right; bx+=4) {
            size_t bx2 = min(bx+4, right);
            size_t by2 = min(by+4, top);
            vec2<T> box[4] = {
              {T(bx), T(by)},  {T(bx2), T(by)},
              {T(bx), T(by2)}, {T(bx2), T(by2)}
            };

            if (outside<T>(v[1], v[2], box) ||
                outside<T>(v[2], v[0], box) ||
                outside<T>(v[0], v[1], box))
              continue;

            for(size_t y=by; y<by2; ++y)
              for(size_t x=bx; x<bx2; ++x) {
                vec2<T> p = {T(x+0.5), T(y+0.5)};
                vec3<T> P = {area2<T>(v[1], v[2], p), area2<T>(v[2], v[0], p), area2<T>(v[0], v[1], p)};

                if (!(P >= T(0.0)))
                  continue;

                P = P / area;
                vec4<T> gl_FragCoord = {
                  p,
                  interpolate(P, v[0].z, v[1].z, v[2].z),
                  interpolate(P, v[0].w, v[1].w, v[2].w)
                };

                P = P / gl_FragCoord.w;
                vec4<T> gl_FragColor = prog.fragment(P, idx);

                for(int i=0; i<4; i++)
                  buffer[(height-1-y)*width+x][i] = gl_FragColor[i] * 255;
              }
          }

      }
    }
  };


  struct Triangles {
    size_t N;
    Triangles(size_t N) : N(N) { }
    struct Iterator {
      size_t N;
      Iterator(size_t N) : N(N) { }
      Iterator& operator++() { N += 3; return *this; }
      bool operator!=(Iterator const& o) const { return N != o.N; }
      auto operator*() const { return vec3<size_t> {N, N+1, N+2}; }
    };
    Iterator begin() const { return Iterator(0); }
    Iterator end()   const { return Iterator(N); }
  };


  struct TriangleStrip {
    size_t N;
    TriangleStrip(size_t N) : N(N) { }

    struct Iterator {
      size_t N;
      Iterator(size_t N) : N(N) { }
      Iterator& operator++() { N += 2; return *this; }
      bool operator!=(Iterator const& o) const { return N != o.N; }
      auto operator*() { return (N%4==0)?(vec3<size_t> {N, N+1, N+2}):(vec3<size_t> {N, N-1, N+1}); }
    };

    Iterator begin() const { return Iterator(0); }
    Iterator end()   const { return Iterator(N); }
  };
}
