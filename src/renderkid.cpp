#include <renderkid/vertex_stage.hpp>
#include <renderkid/rasterization.hpp>
#include <renderkid.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <cstdint>
#include <map>
#include <exception>
#include <tuple>
#include <vector>
#include <iostream>
#include <raylib.h>
#include <optional>


struct VertexAttribData {
  size_t stride;
  size_t offset;
};


std::optional<_vertex::VertexStage> vertex_stage;
std::optional<_raster::Rasterizer> rasterizer;

std::map<uint8_t, VertexAttribData> vertex_attribs;


void render::add_vertex_attribute(uint8_t type, size_t stride, size_t offset) {
  VertexAttribData attrib_data = {
    stride,
    offset
  };

  vertex_attribs.insert({type, attrib_data});
}


void render::set_model_matrix(const glm::mat4 &matrix) {
  vertex_stage->set_model_matrix(matrix);
}


void render::set_view_matrix(const glm::mat4 &matrix) {
  vertex_stage->set_view_matrix(matrix);
}


void render::set_ortho_projection(float left, float right, float bottom, float top, float near, float far) {
  glm::mat4 matrix = glm::ortho(left, right, bottom, top, near, far);
  vertex_stage->set_proj_matrix(matrix);
} 


void render::set_ortho_projection(float left, float right, float top, float bottom) {
  glm::mat4 matrix = glm::ortho(left, right, top, bottom);
  vertex_stage->set_proj_matrix(matrix);
}


void render::set_perspective_projection(float fovy, float aspect, float near, float far) {
  glm::mat4 matrix = glm::perspective(fovy, aspect, near, far);
  vertex_stage->set_proj_matrix(matrix);
} 



void render::read_vertex_data(float *data, size_t size) {
  if (vertex_attribs.empty()) {
    throw std::runtime_error("No vertex attributes were set!");
  }

  for (const auto &attrib : vertex_attribs) {
    VertexAttribData attrib_data = attrib.second;
    size_t attrib_size;

    for (size_t i = attrib_data.offset; i < size; i += attrib_data.stride) {
      switch (attrib.first) {
        case VERTEX_ATTRIBUTE:
          std::get<VERTEX_ATTRIBUTE>(_vertex::vertex_data).emplace_back(
            data[i], data[i + 1], data[i + 2], 1.0
          );
          break;
        case COLOR_ATTRIBUTE:
          std::get<COLOR_ATTRIBUTE>(_vertex::vertex_data).emplace_back(
            data[i], data[i + 1],  data[i + 2], data[i + 3]
          );
          break;
      }
    }
  }
}


void render::init_window(int width, int height) {
  InitWindow(width, height, "Renderkid");
}


void render::set_viewport(int x, int y, int width, int height) {
  vertex_stage.emplace(x, y, width, height);
  rasterizer.emplace(width, height);
}


void render::draw_unindexed(uint8_t primitive_type) {
  vertex_stage->process_vertex_data(primitive_type, _vertex::vertex_data);
  rasterizer->draw_primitives(primitive_type, vertex_stage->get_primitives());
}
