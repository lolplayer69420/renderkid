// TODO: Probar inclinar el plano

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <renderkid.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/*
  Author: Gamjutsu
  License: CC-BY 3.0
  Source: https://opengameart.org/content/stone-wall-with-cracks
*/

#define TEXTURE_FILE "src/examples/texture_test/stone_wall02.png"

float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
};



int main() {
  render::init_window(800, 600);
  render::set_viewport(0, 0, 800, 600);

  glm::mat4 model_matrix = glm::mat4(1.0f);
  model_matrix = glm::rotate(model_matrix, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 view_matrix = glm::mat4(1.0f);
  view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -3.0f));
  int width;
  int height;
  int n_channels;
  uint8_t *data = stbi_load(TEXTURE_FILE, &width, &height, &n_channels, 0);
  uint8_t texture = render::create_texture(width, height, n_channels);
  render::use_texture(texture);
  render::load_data_to_texture(data);

  render::set_model_matrix(model_matrix);
  render::set_view_matrix(view_matrix);
  render::set_perspective_projection(glm::radians(45.0f), 800.0f / 600.0f, 0.5f, 100.0f);

  render::add_vertex_attribute(VERTEX_ATTRIBUTE, 0);
  render::add_vertex_attribute(TEXTURE_COORD_ATTRIBUTE, 3);
  render::read_vertex_data(vertices, sizeof(vertices) / sizeof(float));

  while (true) {
    render::draw_unindexed(TRIANGLES);
  }
}
