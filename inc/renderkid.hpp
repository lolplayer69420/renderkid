#ifndef RENDERKID_HPP
#define RENDERKID_HPP

#include <glm/mat4x4.hpp>
#include <cstdint>

#define VERTEX_ATTRIBUTE 0
#define COLOR_RGBA_ATTRIBUTE 1
#define COLOR_RGB_ATTRIBUTE 2
#define TEXTURE_COORD_ATTRIBUTE 3
#define NORM_VECTOR_ATTRIBUTE 4

#define POINTS 0
#define LINES 1
#define TRIANGLES 2

namespace render {
void set_model_matrix(const glm::mat4 &matrix);
void set_view_matrix(const glm::mat4 &matrix);

void set_ortho_projection(float left, float right, float bottom, float top, float near, float far);
void set_ortho_projection(float left, float right, float top, float bottom);
void set_perspective_projection(float fovy, float aspect, float near, float far);
void init_window(int width, int height);

uint8_t create_vertex_buffer();
void destroy_vertex_buffer(uint8_t id);
void use_vertex_buffer(uint8_t id);
void add_vertex_attribute(uint8_t type, size_t position);
void load_data_into_vertex_buffer(float *data, size_t size);

uint8_t create_texture(int width, int height, int n_channels);
void load_data_to_texture(uint8_t *data);
void destroy_texture(uint8_t id);
void use_texture(uint8_t id);

uint8_t create_light(const glm::vec3 &initial_pos, const glm::vec3 &initial_color);
void destroy_light(uint8_t id);
void set_light_position(uint8_t id, const glm::vec3 &pos);
void set_light_color(uint8_t id, const glm::vec3 &color);
void set_view_position(const glm::vec3 &position);
void set_ambient_strength(float strength);
void set_specular_strength(float strength);

void set_viewport(int x, int y, int width, int height);
void draw(uint8_t primitive_type);
}
#endif
