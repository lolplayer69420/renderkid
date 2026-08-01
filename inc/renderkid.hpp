#ifndef RENDERKID_HPP
#define RENDERKID_HPP

#include <glm/mat4x4.hpp>
#include <cstdint>

#define VERTEX_ATTRIBUTE 0
#define COLOR_RGBA_ATTRIBUTE 1
#define COLOR_RGB_ATTRIBUTE 2
#define TEXTURE_COORD_ATTRIBUTE 3

#define POINTS 0
#define LINES 1
#define TRIANGLES 2

namespace render {
void set_model_matrix(const glm::mat4 &matrix);
void set_view_matrix(const glm::mat4 &matrix);

void set_ortho_projection(float left, float right, float bottom, float top, float near, float far);
void set_ortho_projection(float left, float right, float top, float bottom);
void set_perspective_projection(float fovy, float aspect, float near, float far);
void read_vertex_data(float *data, size_t size);
void add_vertex_attribute(uint8_t type, size_t position);
void init_window(int width, int height);
uint8_t create_texture(int width, int height, int n_channels);
void load_data_to_texture(uint8_t *data);
void destroy_texture(uint8_t id);
void use_texture(uint8_t id);

void set_viewport(int x, int y, int width, int height);
void draw_unindexed(uint8_t primitive_type);
}
#endif
