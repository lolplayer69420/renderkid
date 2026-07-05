#include <renderkid/vertex_stage.hpp>
#include <glm/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <tuple>
#include <iostream>
#include <stdlib.h>

_vertex::VertexData _vertex::vertex_data;


void _vertex::VertexStage::process_vertex_data(uint8_t primitive_type, const _vertex::VertexData &data) {
  std::vector<glm::vec4> vertices = std::get<VERTEX_ATTRIBUTE>(data);
  std::vector<glm::vec4> colors = std::get<COLOR_ATTRIBUTE>(data);

  for (size_t i = 0; i < vertices.size(); i += primitive_type) {
    _vertex::Primitive primitive;

    switch (primitive_type) {
      case POINT_PRIMITIVE:
        do_vertex_transformations(vertices[i]);
        primitive.point_coord = vertices[i];

        if (colors.size()) {
          primitive.point_color = colors[i];
        }

        break;
      case LINE_PRIMITIVE:
        do_vertex_transformations(vertices[i]);
        do_vertex_transformations(vertices[i + 1]);
        primitive.line_coords = {vertices[i], vertices[i + 1]};

        if (colors.size()) {
          primitive.line_colors = {colors[i], colors[i + 1]};
        }

        break;
      case TRIANGLE_PRIMITIVE:
        do_vertex_transformations(vertices[i]);
        do_vertex_transformations(vertices[i + 1]);
        do_vertex_transformations(vertices[i + 2]);
        primitive.triangle_coords = {vertices[i], vertices[i + 1], vertices[i + 2]};

        if (colors.size()) {
          primitive.triangle_colors = {colors[i], colors[i + 1], colors[i + 2]};
        }

        break;
    }

    primitive_list.push_back(primitive);
  }
}
