#ifndef VERTEX_STAGE_HPP
#define VERTEX_STAGE_HPP

#include <glm/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/vec4.hpp>
#include <cstdint>
#include <vector>
#include <utility>
#include <iostream>
#include <tuple>

#define VERTEX_ATTRIBUTE 0
#define COLOR_ATTRIBUTE 1

#define POINT_PRIMITIVE 1
#define LINE_PRIMITIVE 2
#define TRIANGLE_PRIMITIVE 3

namespace _vertex { 
typedef std::pair<glm::vec4, glm::vec4> Vertex;

typedef std::tuple<
  std::vector<glm::vec4>,
  std::vector<glm::vec4>
> VertexData;

extern VertexData vertex_data;


struct Primitive {
  union {
    glm::vec4 point_coord;
    std::array<glm::vec4, 2> line_coords;
    std::array<glm::vec4, 3> triangle_coords;
  };

  union {
    glm::vec4 point_color;
    std::array<glm::vec4, 2> line_colors;
    std::array<glm::vec4, 3> triangle_colors;
  };
};


class VertexStage {
  public:
    VertexStage(int x, int y, int width, int height) :
      width{width}, height{height} {};
    
    void set_proj_matrix(const glm::mat4 &matrix) {
      proj_matrix = matrix;
    };
    
    void set_view_matrix(const glm::mat4 &matrix) {
       view_matrix = matrix;
    };
    
    void set_model_matrix(const glm::mat4 &matrix) {
      model_matrix = matrix;
    };
    
    void process_vertex_data(uint8_t primitive_type, const VertexData &data);

    std::vector<Primitive> &get_primitives() {
      return primitive_list;
    };

  private:
    void do_vertex_transformations(glm::vec4 &vertex) {
      // Apply tranformations to a vertex and perform the perspective division on it
      vertex = proj_matrix * view_matrix * model_matrix * vertex;
      vertex /= vertex.w;
      vertex.x = ((width / 2) * vertex.x) + ((width - 1) / 2);
      vertex.y = (1.0f - (vertex.y * 0.5f + 0.5f)) * height;
    }

    void clip_triangle(const Vertex triangle[3], const glm::vec4 &equation);
    std::vector<Primitive> primitive_list;
    glm::mat4 proj_matrix;
    glm::mat4 view_matrix;
    glm::mat4 model_matrix;
    int width;
    int height;
};
}

#endif
