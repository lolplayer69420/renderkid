#ifndef RASTERIZATION_HPP
#define RASTERIZATION_HPP

#include <cstdint>
#include <raylib.h>
#include <renderkid/vertex_stage.hpp>
#include <vector>
#include <array>
#include <map>

namespace _raster {
struct UserTexture {
  uint8_t *data;
  int channels;
  int width;
  int height;
};


class Rasterizer {
  public:
    Rasterizer(int width, int height);
    void draw_primitives(uint8_t primitive_type, std::vector<_vertex::Primitive> &primitives);
    uint8_t create_texture(int width, int height, int n_channels);
    
    void use_texture(uint8_t id) {
      current_texture = id;
    }
    
    void load_data_to_texture(uint8_t *data);
    void destroy_texture(uint8_t texture);
  private:
    void raster_triangle(const _vertex::Primitive &triangle);
    glm::vec4 gen_frag_color_from_attributes(const glm::vec4 &frag_coord, const _vertex::Vertex &a, const _vertex::Vertex &b,
                                             const _vertex::Vertex &c);
    glm::vec4 gen_frag_color_from_texture(const glm::vec4 &frag_coord, const glm::vec4 &a_coord,
                                          const glm::vec4 &b_coord, const glm::vec4 &c_coord);
    void draw_framebuffer();
    void clear_framebuffer();
    float *depth_buffer;
    std::map<uint8_t, UserTexture> textures;
    uint8_t current_texture;
    uint8_t next_texture_id;
    Image framebuffer;
    Texture framebuffer_texture;
    int width;
    int height;
};
}

#endif
