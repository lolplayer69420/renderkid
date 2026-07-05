#ifndef RENDERKID_HPP
#define RENDERKID_HPP

#include <glm/mat4x4.hpp>
#include <cstdint>

#define VERTEX_ATTRIBUTE 0
#define COLOR_ATTRIBUTE 1
#define POINTS 1
#define LINES 2
#define TRIANGLES 3

namespace render {
void set_model_matrix(const glm::mat4 &matrix);
void set_view_matrix(const glm::mat4 &matrix);
void set_projection_matrix(const glm::mat4 &matrix);
void read_vertex_data(float *data, size_t size);
void add_vertex_attribute(uint8_t type, size_t stride, size_t offset);
void init_window(int width, int height);
void set_viewport(int x, int y, int width, int height, int clip_near,
                  int clip_far);
void draw_unindexed(uint8_t primitive_type);
}
#endif
