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

std::optional<_vertex::VertexStage> vertex_stage;
std::optional<_raster::Rasterizer> rasterizer;

std::vector<_vertex::VertexAttribData> vertex_attribs;

size_t vertex_size = 0;


void render::add_vertex_attribute(uint8_t type, size_t position) {
  _vertex::VertexAttribData attrib_data = {
    position,
    type
  };

  switch (type) {
    case VERTEX_ATTRIBUTE:
      vertex_size += 3;
      break;
    case COLOR_RGBA_ATTRIBUTE:
      vertex_size += 4;
      break;
    case COLOR_RGB_ATTRIBUTE:
      vertex_size += 3;
      break;
    case TEXTURE_COORD_ATTRIBUTE:
      vertex_size += 2;
      break;
    case NORM_VECTOR_ATTRIBUTE:
      vertex_size += 3;
      break;
  }

  vertex_attribs.push_back(attrib_data);
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

  for (size_t i = 0; i < size; i += vertex_size) {
    _vertex::Vertex new_vertex = _vertex::Vertex(&data[i], vertex_attribs);
    _vertex::vertex_data.push_back(new_vertex);
  }
}


uint8_t render::create_texture(int width, int height, int n_channels) {
  return rasterizer->create_texture(width, height, n_channels);
}


void render::use_texture(uint8_t id) {
  rasterizer->use_texture(id);
}


void render::load_data_to_texture(uint8_t *data) {
  rasterizer->load_data_to_texture(data);
}


void render::destroy_texture(uint8_t id) {
  rasterizer->destroy_texture(id);
}


uint8_t render::create_light(const glm::vec3 &initial_pos, const glm::vec3 &initial_color) {
  return rasterizer->create_light(initial_pos, initial_color);
}


void render::destroy_light(uint8_t id) {
  rasterizer->destroy_light(id);
}


void render::set_light_position(uint8_t id, const glm::vec3 &position) {
  rasterizer->set_light_position(id, position);
}


void render::set_light_color(uint8_t id, const glm::vec3 &color) {
  rasterizer->set_light_color(id, color);
}


void render::set_ambient_strength(float strength) {
  rasterizer->set_ambient_strength(strength);
}


void render::set_specular_strength(float strength) {
  rasterizer->set_specular_strength(strength);
}


void render::set_view_position(const glm::vec3 &position) {
  rasterizer->set_view_position(position);
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
