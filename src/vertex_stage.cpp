#include <renderkid/vertex_stage.hpp>
#include <glm/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>

_vertex::VertexData _vertex::vertex_data;

const glm::vec4 clipping_equations[] = {
  {0.0f, 0.0f,  1.0f, 1.0f},
  {0.0f, 0.0f, -1.0f, 1.0f}
};


namespace {
_vertex::Vertex _clip_intersect_edge(const _vertex::Vertex &v0, const _vertex::Vertex &v1, float val0, float val1) {
  float a = val0 / (val0 - val1);

  _vertex::Vertex vertex;

  vertex.first = glm::mix(v0.first, v1.first, a);
  vertex.second = glm::mix(v0.second, v1.second, a);

  return vertex;
}
}


void _vertex::VertexStage::clip_triangle(const _vertex::Vertex triangle[3], const glm::vec4 &equation) {
  // I will probably change this in the future
  float values[] = {
    glm::dot(triangle[0].first, equation),
    glm::dot(triangle[1].first, equation),
    glm::dot(triangle[2].first, equation)
  }; 

  _vertex::Primitive triangle_1;
  _vertex::Primitive triangle_2;

  uint8_t mask = (values[0] < 0.0f ? 1 : 0) | (values[1] < 0.0f ? 2 : 0) | (values[2] < 0.0f ? 4 : 0);

  switch (mask) {
    case 0b000: {
      _vertex::Primitive new_triangle; 
      new_triangle.triangle_coords = {triangle[0].first, triangle[1].first, triangle[2].first};
      new_triangle.triangle_colors = {triangle[0].second, triangle[1].second, triangle[2].second};

      primitive_list.push_back(new_triangle);

      break;
    }

    case 0b001: {
      _vertex::Primitive triangle_1;
      _vertex::Primitive triangle_2;
      _vertex::Vertex v01 = _clip_intersect_edge(triangle[0], triangle[1], values[0], values[1]);
      _vertex::Vertex v02 = _clip_intersect_edge(triangle[0], triangle[2], values[0], values[2]);

      triangle_1.triangle_coords = {v01.first, triangle[1].first, triangle[2].first};
      triangle_1.triangle_colors = {v01.second, triangle[1].second, triangle[2].second};
      triangle_2.triangle_coords = {v01.first, triangle[2].first, v02.first};
      triangle_2.triangle_colors = {v01.second, triangle[2].second, v02.second};
    
      primitive_list.push_back(triangle_1);
      primitive_list.push_back(triangle_2);

      break;
    }

    case 0b010: {
      _vertex::Vertex v10 = _clip_intersect_edge(triangle[1], triangle[0], values[1], values[0]);
      _vertex::Vertex v12 = _clip_intersect_edge(triangle[1], triangle[2], values[1], values[2]);

      triangle_1.triangle_coords = {triangle[0].first, v10.first, triangle[2].first};
      triangle_1.triangle_colors = {triangle[0].second, v10.first, triangle[2].second};
      triangle_2.triangle_coords = {triangle[2].first, v10.first, v12.first};
      triangle_2.triangle_colors = {triangle[2].second, v10.second, v12.second};

      primitive_list.push_back(triangle_1);
      primitive_list.push_back(triangle_2);

      break;
    }

    case 0b011: {
      _vertex::Primitive new_triangle;
      
      _vertex::Vertex v02 = _clip_intersect_edge(triangle[0], triangle[2], values[0], values[2]);
      _vertex::Vertex v12 = _clip_intersect_edge(triangle[1], triangle[2], values[1], values[2]);
      new_triangle.triangle_coords = {v02.first, v12.first, triangle[2].first};
      new_triangle.triangle_coords = {v02.second, v12.second, triangle[2].second};

      primitive_list.push_back(new_triangle);

      break;
    }

    case 0b100: {
      _vertex::Vertex v20 = _clip_intersect_edge(triangle[2], triangle[0], values[2], values[0]);
      _vertex::Vertex v21 = _clip_intersect_edge(triangle[2], triangle[1], values[2], values[1]);

      triangle_1.triangle_coords = {triangle[0].first, triangle[1].first, v20.first};
      triangle_1.triangle_colors = {triangle[0].second, triangle[1].second, v20.second};
      triangle_2.triangle_coords = {v20.first, triangle[1].first, v21.first};
      triangle_2.triangle_colors = {v20.second, triangle[1].second, v21.second};
    
      primitive_list.push_back(triangle_1);
      primitive_list.push_back(triangle_2);

      break;
    }

    case 0b101: {
      _vertex::Primitive new_triangle;
      
      _vertex::Vertex v01 = _clip_intersect_edge(triangle[0], triangle[1], values[0], values[1]);
      _vertex::Vertex v21 = _clip_intersect_edge(triangle[2], triangle[1], values[2], values[1]);

      new_triangle.triangle_coords = {v01.first, triangle[1].first, v21.first};
      new_triangle.triangle_colors = {v01.second, triangle[1].second, v21.second};

      primitive_list.push_back(new_triangle);

      break;
    }

    case 0b110: {
      _vertex::Primitive new_triangle;

      _vertex::Vertex v10 = _clip_intersect_edge(triangle[1], triangle[0], values[1], values[0]);
      _vertex::Vertex v20 = _clip_intersect_edge(triangle[2], triangle[0], values[2], values[0]);

      new_triangle.triangle_coords = {triangle[0].first, v10.first, v20.first};
      new_triangle.triangle_colors = {triangle[0].second, v10.second, v20.second};

      primitive_list.push_back(new_triangle);

      break;
    }

    case 0b111:
      break;
  }
}


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

        Vertex triangle[] = {{vertices[i], colors[i]}, {vertices[i + 1], colors[i + 1]},
                             {vertices[i + 2], colors[i + 2]}};

        for (const glm::vec4 &equation : clipping_equations) {
          clip_triangle(triangle, equation);
        }

        break;
    }
  }
}
