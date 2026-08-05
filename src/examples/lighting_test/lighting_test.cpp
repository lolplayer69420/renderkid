// TODO: Terminar de implementar la iluminacion

#include <renderkid.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <raylib.h>

#define SPEED 0.01f

float vertices[] = {
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f,  0.0f
};


int main() {
  render::init_window(800, 600);
  render::set_viewport(0, 0, 800, 600);
  glm::vec4 light_position = glm::vec4(1.2, 1.0f, 2.0f, 1.0f);

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::lookAt(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  render::set_perspective_projection(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
  render::set_ambient_strength(0.5);
  render::set_specular_strength(0.5);
  render::set_view_position(glm::vec3(1.2f, 1.0f, 2.0f));

  uint8_t light = render::create_light(glm::vec3(light_position.x, light_position.y, light_position.z),
                                       glm::vec3(1.0f));

  render::set_model_matrix(model);
  render::set_view_matrix(view);
  render::add_vertex_attribute(VERTEX_ATTRIBUTE, 0);
  render::add_vertex_attribute(COLOR_RGB_ATTRIBUTE, 3);
  render::add_vertex_attribute(NORM_VECTOR_ATTRIBUTE, 6);

  render::read_vertex_data(vertices, sizeof(vertices) / sizeof(float));

  while (true) {
    glm::mat4 light_rotation = glm::rotate(glm::mat4(1.0f), SPEED, glm::vec3(0.0f, 1.0f, 0.0f));
    light_position = light_position * light_rotation;
    render::set_light_position(light, glm::vec3(light_position.x, light_position.y, light_position.z));

    render::set_model_matrix(model);
    render::draw_unindexed(TRIANGLES);
  }
}

