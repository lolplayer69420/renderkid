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

#define MAX_ATTRIBS 16

#define VERTEX_ATTRIBUTE 0
#define COLOR_RGBA_ATTRIBUTE 1
#define COLOR_RGB_ATTRIBUTE 2
#define TEXTURE_COORD_ATTRIBUTE 3

#define POINT_PRIMITIVE 0
#define LINE_PRIMITIVE 1
#define TRIANGLE_PRIMITIVE 2

namespace _vertex {

struct VertexAttrib {
  glm::vec4 data;
  uint8_t type;
};


struct VertexAttribData {
  size_t position;
  uint8_t type;
};


struct Vertex {
  Vertex(const float *data, const std::vector<VertexAttribData> &format) {
    attribs.fill((VertexAttrib){glm::vec4(0.0f), 0});

    for (size_t i = 0, j = 0; i < format.size(); ++i) {
      VertexAttrib attribute;
      attribute.type = format[i].type;
      size_t position = format[i].position;

      switch (attribute.type) {
        case VERTEX_ATTRIBUTE:
          coords = glm::vec4(data[position], data[position + 1], data[position + 2], 1.0f);
          break;
        case COLOR_RGBA_ATTRIBUTE:
          attribute.data = glm::vec4(data[position], data[position + 1], data[position + 2],
                                     data[position + 3]);
          attribs[j++] = attribute;

          break;
        case COLOR_RGB_ATTRIBUTE:
          attribute.data = glm::vec4(data[position], data[position + 1], data[position + 2], 1.0f);
          attribs[j++] = attribute;

          break;
        case TEXTURE_COORD_ATTRIBUTE:
          attribute.data = glm::vec4(data[position], data[position + 1], 0.0f, 1.0f);
          attribs[j++] = attribute;

          break;
      }
    }
  }

  Vertex() {}

  glm::vec4 coords;
  std::array<VertexAttrib, MAX_ATTRIBS> attribs;
};


extern std::vector<Vertex> vertex_data;


struct Primitive {
  uint8_t type;
  std::array<Vertex, 3> vertices;
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

    void process_vertex_data(uint8_t primitive_type, const std::vector<Vertex> &vertex_data);

    std::vector<Primitive> &get_primitives() {
      return primitive_list;
    };

  private:
    void do_object_to_clip_space_transform(glm::vec4 &vertex) {
      vertex = proj_matrix * view_matrix * model_matrix * vertex;
    }

    void do_clip_to_viewport_transform(_vertex::Vertex &vertex) {
      for (_vertex::VertexAttrib &attrib : vertex.attribs) {
        if (!attrib.type) {
          break;
        }

        attrib.data /= vertex.coords.w;
      }

      float inv_w = 1 / vertex.coords.w;

      vertex.coords /= vertex.coords.w;
      vertex.coords.w = inv_w;
      vertex.coords.x = ((width / 2) * vertex.coords.x) + ((width - 1) / 2);
      vertex.coords.y = (1.0f - (vertex.coords.y * 0.5f + 0.5f)) * height;
    }

    std::vector<Primitive> primitive_list;
    glm::mat4 proj_matrix;
    glm::mat4 view_matrix;
    glm::mat4 model_matrix;
    int width;
    int height;
};
}

#endif
