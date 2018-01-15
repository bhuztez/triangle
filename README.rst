========
triangle
========

.. image:: https://travis-ci.org/bhuztez/triangle.svg?branch=master
    :target: https://travis-ci.org/bhuztez/triangle

.. image:: https://codenvy.io/factory/resources/codenvy-contribute.svg
    :target: https://codenvy.io/f?url=https://github.com/bhuztez/triangle

There are many good OpenGL tutorials out there. They have good
explanation on how OpenGL works. However, most of them treat OpenGL as
a black box. It would be better to implement a small part of OpenGL to
see how things fit together.

The biggest challenge of emulating GLSL in C++ is shader linking. This
has been made possible by stateful metaprogramming. Like we did in
implementing precise garbage collector, we also record names of all
uniform, attribute, varying variables, so that when linking, we can
check if variables of same name have same type.

Another problem is that vertex and fragment shaders will be run many
times, and at each run, attribute and varying variables point to
different locations. To make it as close as possible to GLSL, and
avoid much boilerplate, there variables are declared as union. For
example, :code:`gl_Position`.

.. code:: c++

    union {
      vec4& gl_Position;
      vec4* _ptr_gl_Position;
    }

Let's see a complete example.

the vertex shader

.. code:: c++

    template<typename T>
    struct Vertex {
      VERTEX_SHADER(Vertex, T);

      UNIFORM(perspective, mat4);
      ATTRIBUTE(position, vec3);
      ATTRIBUTE(aColor, vec3);
      VARYING(vColor, vec3);

      void
      main() {
        gl_Position = perspective * vec4(position, 1.0);
        vColor = aColor;
      }
    };


the fragment shader

.. code:: c++

    template<typename T>
    struct Fragment {
      FRAGMENT_SHADER(Fragment, T);

      VARYING(vColor, vec3);

      void
      main() {
        gl_FragColor = vec4(vColor, 1.0);
      }
    };

link and then create program of 3 vertices

.. code:: c++

    using Program = ::gl::Link<float, Vertex, Fragment>;
    using T = typename Program::Float;
    using vec3 = typename Program::vec3;

    Program prog(3);

specify locations of variables.

.. code:: c++

    auto p = perspective<T>(::gl::sl::radians(90.0), T(width)/T(height), 0.1, 100.0);

    vec3 position[] = {
      {-0.5, -0.5, -1.0},
      { 0.5, -0.5, -1.0},
      { 0.5,  0.5, -5.0}
    };

    vec3 color[] = {
      {1.0, 0.0, 0.0},
      {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0}
    };

    prog.uniform.set("perspective"_s, &p);
    prog.attribute.set("position"_s, position);
    prog.attribute.set("aColor"_s, color);

draw

.. code:: c++

  ::gl::Context(width, height, buffer).draw(prog, ::gl::triangles);
