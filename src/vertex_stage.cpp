#include <renderkid/vertex_stage.hpp>
#include <renderkid.hpp>
#include <glm/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>
#include <iostream>

std::vector<_vertex::Vertex> _vertex::vertex_data;


void _vertex::VertexStage::process_vertex_data(uint8_t primitive_type,
                                               const std::vector<_vertex::Vertex> &vertex_data) {
  size_t primitive_size = 0;

  switch (primitive_type) {
    case POINTS:
      primitive_size = 1;
      break;
    case LINES:
      primitive_size = 2;
      break;
    case TRIANGLES:
      primitive_size = 3;
      break;
  }

  for (size_t i = 0; i < vertex_data.size(); i += primitive_size) {
    _vertex::Vertex vertex_a = vertex_data[i];
    _vertex::Vertex vertex_b = vertex_data[i + 1];
    _vertex::Vertex vertex_c = vertex_data[i + 2];

    switch (primitive_type) {
      case POINT_PRIMITIVE:

        break;
      case LINE_PRIMITIVE:

        break;
      case TRIANGLE_PRIMITIVE:
        do_object_to_clip_space_transform(vertex_a.coords);
        do_object_to_clip_space_transform(vertex_b.coords);
        do_object_to_clip_space_transform(vertex_c.coords);
        do_clip_to_viewport_transform(vertex_a);
        do_clip_to_viewport_transform(vertex_b);
        do_clip_to_viewport_transform(vertex_c);

        _vertex::Primitive new_triangle;

        new_triangle.type = TRIANGLE_PRIMITIVE;
        new_triangle.vertices = {vertex_a, vertex_b, vertex_c};
        primitive_list.push_back(new_triangle);

        break;
    }
  }
}
