#include <renderkid.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <raylib.h>

float vertices[] = {
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f    // top 
};


int main() {
  render::init_window(800, 600);
  render::set_viewport(0, 0, 800, 600, 0.1f, 100.0f);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  render::set_model_matrix(model);
  render::set_view_matrix(view);
  render::set_projection_matrix(projection);
  render::add_vertex_attribute(VERTEX_ATTRIBUTE, 7, 0);
  render::add_vertex_attribute(COLOR_ATTRIBUTE, 7, 3);

  render::read_vertex_data(vertices, sizeof(vertices) / sizeof(float));

  while (true) {
    model = glm::rotate(model, (float)GetTime() * glm::radians(50.0f),
                        glm::vec3(0.5f, 1.0f, 0.0f));
    render::set_model_matrix(model);
    render::draw_unindexed(TRIANGLES);
  }
}

