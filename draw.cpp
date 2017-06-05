// taken from
// http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes

#include <cstdio>
#include <cstring>
#include "gl.hpp"

extern "C" {
  extern const size_t width = 512;
  extern const size_t height = 512;
}

template<typename T>
static ::gl::sl::mat<4,T>
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

template<typename T>
struct Vertex {
  VERTEX_SHADER(Vertex, T);

  UNIFORM(perspective, mat4);
  ATTRIBUTE(position, vec3);
  ATTRIBUTE(aColor, vec3);
  VARYING(vColor, vec3);

  void
  main() {
    gl_Position = perspective * vec4 {position, 1.0};
    vColor = aColor;
  }
};

template<typename T>
struct Fragment {
  FRAGMENT_SHADER(Fragment, T);

  VARYING(vColor, vec3);

  void
  main() {
    gl_FragColor = {vColor, 1.0};
  }
};


extern "C" void
draw(unsigned char buffer[][4]) {
  memset(buffer, 0, sizeof(unsigned char)*height*width*4);

  using Program = ::gl::Link<float, Vertex, Fragment>;
  using T = typename Program::Float;
  using vec3 = typename Program::vec3;

  Program prog(3);

  auto p = perspective<T>(::gl::sl::radians(90.0), T(width)/T(height), 0.1, 100.0);

  vec3 position[] = {
    { -48.0, -10.0,   82.0 },
    {  29.0, -15.0,   44.0 },
    {  13.0,  34.0,  114.0 },
  };

  vec3 color[] = {
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},
  };

  prog.uniform.set("perspective"_s, &p);
  prog.attribute.set("position"_s, position);
  prog.attribute.set("aColor"_s, color);

  ::gl::Context(width, height, buffer).draw(prog, ::gl::triangles);
}
