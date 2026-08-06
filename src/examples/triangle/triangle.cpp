#include <renderkid.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

float vertices[] = {
   // positions        // colors
   0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // bottom right
  -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // bottom left
   0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f   // top
};


int main() {
  render::init_window(800, 600);

  glm::mat4 model_matrix = glm::mat4(1.0f);
  glm::mat4 view_matrix = glm::mat4(1.0f);

  render::set_model_matrix(model_matrix);
  render::set_view_matrix(view_matrix);
  render::set_ortho_projection(-1.0f, 1.0f, -1.0f, 1.0f);

  uint8_t vertex_buffer = render::create_vertex_buffer();
  render::use_vertex_buffer(vertex_buffer);

  render::add_vertex_attribute(VERTEX_ATTRIBUTE, 0);
  render::add_vertex_attribute(COLOR_RGBA_ATTRIBUTE, 3);
  render::load_data_into_vertex_buffer(vertices, sizeof(vertices) / sizeof(float));
  render::set_viewport(0, 0, 800, 600);

  while (true) {
    render::draw(TRIANGLES);
  }
}
