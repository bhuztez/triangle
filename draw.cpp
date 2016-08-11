// taken from
// http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes

#include <stdio.h>
#include <string.h>
#include "gl.hpp"

extern "C" {
  extern const size_t width = 512;
  extern const size_t height = 512;
}

using namespace ::sl;

template<typename T>
static mat4<T>
perspective(T fovy, T aspect, T near, T far) {
  T f = 1.0 / tan(fovy / 2.0);
  T dz = near - far;
  return {
    { f/aspect, 0.0, 0.0,                0.0 },
    { 0.0,      f,   0.0,                0.0 },
    { 0.0,      0.0, (far+near)/dz,      1.0 },
    { 0.0,      0.0, far*near*T(2.0)/dz, 0.0 }
  };
}

extern "C" void
draw(unsigned char buffer[][4]) {
  memset(buffer, 0, sizeof(unsigned char)*height*width*4);

  struct Program {
    typedef float T;

    mat4<T> Perspective;
    vec3<T> *Position;
    vec3<T> *Color;

    vec4<T>
    vertex(size_t i) {
      vec4<T> gl_Position;
      [&gl_Position = gl_Position,
       &Position = Position[i],
       &Perspective = Perspective,
       &Color = Color[i]]{
        gl_Position = Perspective * vec4<T> {Position, 1.0};
        varying(gl_Position, Color);
      }();
      return gl_Position;
    }

    vec4<T>
    fragment(vec3<T> const& P, vec3<size_t> const& i){
      vec4<T> gl_FragColor;

      [&gl_FragColor = gl_FragColor,
       Color = interpolate(P, Color, i)] {
        gl_FragColor = {Color, 1.0};
      }();

      return gl_FragColor;
    }
  };

  Program prog;

  typedef Program::T T;

  vec3<T> Position[] = {
    { -48.0, -10.0,   82.0 },
    {  29.0, -15.0,   44.0 },
    {  13.0,  34.0,  114.0 },
  };

  vec3<T> Color[] = {
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},
  };

  prog.Perspective = perspective<T>(radians(90.0), T(width)/T(height), 0.1, 100.0);
  prog.Position = Position;
  prog.Color = Color;

  ::gl::Context(width, height, buffer).draw(prog, 3, ::gl::Triangles(3));
}
