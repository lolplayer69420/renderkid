#ifndef RASTERIZATION_HPP
#define RASTERIZATION_HPP

#define POINTS 1
#define LINES 2
#define TRIANGLES 3

#include <cstdint>
#include <raylib.h>
#include <renderkid/vertex_stage.hpp>
#include <vector>

namespace _raster {
class Rasterizer {
  public:
    Rasterizer(int width, int height);
    void draw_primitives(uint8_t primitive_type, std::vector<_vertex::Primitive> &primitives);
  private:
    void raster_triangle(const _vertex::Primitive &triangle);
    void draw_framebuffer();
    void clear_framebuffer();
    float *depth_buffer;
    Image framebuffer;
    Texture texture;
    int width;
    int height;
};
}

#endif
