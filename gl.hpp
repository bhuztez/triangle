#pragma once
#include "sl.hpp"
#include "shader.hpp"

namespace gl {
  using namespace sl;

  template<typename T>
  T
  area2(vec<2,T> const& v0, vec<2,T> const& v1, vec<2,T> const& v2) {
    return cross(vec<3,T> {v1 - v0, 0.0}, vec<3,T> {v2 - v0, 0.0}).z;
  }

  template<typename T>
  bool
  outside(vec<2,T> const& u, vec<2,T> const& v, vec<2,T> box[4]) {
    return all(lessThan(vec<4,T>{ area2(u,v,box[0]), area2(u,v,box[1]), area2(u,v,box[2]), area2(u,v,box[3]) }, {0.0}));
  }

  struct ID {
    size_t operator[](size_t n) const { return n; }
  };

  struct Context {
    const size_t width, height;
    unsigned char (*buffer)[4];

    Context(size_t width, size_t height, unsigned char (*buffer)[4])
      : width(width), height(height), buffer(buffer) {
    }

    template<typename Prog>
    void
    draw(Prog& prog, void (*primitive)(Context&, Prog&, ID const&)) {
      draw(prog, ID(), primitive);
    }

    template<typename Prog, typename Index>
    void
    draw(Prog& prog, Index const& index, void (*primitive)(Context&, Prog&, Index const&)) {
      using T = typename Prog::Float;
      using Vertex = typename Prog::Vertex;

      for(size_t i=0; i<prog.vertices; i++) {
        char buf[sizeof(Vertex)] = {0};
        auto v = (Vertex *)buf;

        vec<4,T> p;

        v->_ptr_gl_Position = &p;
        prog.uniform.bind(v);
        prog.attribute.bind(v, i);
        prog.varying.bind(v, i);
        v->main();

        p = {vec<3,T>(p) / p.w, T(1.0) / p.w};

        Prog::fix_varying(v, p.w);

        prog.gl_Position[i] = {(vec<3,T> {p + T(1.0)} * T(0.5) * vec<3,T> {T(width), T(height), 1.0}), p.w};
      }

      primitive(*this, prog, index);
    }
  };


  template<typename Prog>
  void
  draw_triangle(Context& context, Prog& prog, ::std::size_t i0, ::std::size_t i1, ::std::size_t i2) {
      using T = typename Prog::Float;
      using Fragment = typename Prog::Fragment;

      ::std::size_t width = context.width;
      ::std::size_t height = context.height;

      auto v0 = prog.gl_Position[i0];
      auto v1 = prog.gl_Position[i1];
      auto v2 = prog.gl_Position[i2];

      auto area = area2<T>(v0, v1, v2);

      for(size_t by=0; by<height; by+=4)
        for(size_t bx=0; bx<width; bx+=4) {
          size_t bx2 = min(bx+4, width);
          size_t by2 = min(by+4, height);

          vec<2,T> box[4] = {
            {T(bx), T(by)},  {T(bx2), T(by)},
            {T(bx), T(by2)}, {T(bx2), T(by2)}
          };

          if (outside<T>(v1, v2, box) ||
              outside<T>(v2, v0, box) ||
              outside<T>(v0, v1, box))
            continue;

          for(size_t y=by; y<by2; ++y)
            for(size_t x=bx; x<bx2; ++x) {
              vec<2,T> p = {T(x+0.5), T(y+0.5)};
              vec<3,T> P = {area2<T>(v1, v2, p), area2<T>(v2, v0, p), area2<T>(v0, v1, p)};

              if (!all(greaterThan(P, {0.0})))
                continue;

              P = P / area;
              vec<4,T> gl_FragCoord = {
                  p,
                  interpolate(P, v0.z, v1.z, v2.z),
                  interpolate(P, v0.w, v1.w, v2.w)
              };

              P = P / gl_FragCoord.w;

              char buf[sizeof(Fragment)] = {0};
              auto f = (Fragment *)buf;

              vec<4,T> color;

              f->_ptr_gl_FragColor = &color;
              prog.uniform.bind(f);

              auto i = prog.interpolate(P, i0, i1, i2);
              i.bind(f);
              f->main();

              for(::std::size_t i=0; i<4; i++)
                context.buffer[(height-1-y)*width+x][i] = color[i] * 255;
            }

        }
  }

  template<typename Prog, typename Index>
  void
  triangles(Context& context, Prog& prog, Index const& index) {
    for(::std::size_t i=0; (i+2) < prog.vertices; i+=3) {
      draw_triangle(context, prog, index[i], index[i+1], index[i+2]);
    }
  }
}
