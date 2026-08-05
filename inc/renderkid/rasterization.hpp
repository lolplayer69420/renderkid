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


struct Light {
  glm::vec3 color;
  glm::vec3 position;
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

    uint8_t create_light(const glm::vec3 &initial_pos, const glm::vec3 &initial_color) {
      Light new_light;
      new_light.position = initial_pos;
      new_light.color = initial_color;

      lights[next_light_id] = new_light;

      return next_light_id++;
    }

    void destroy_light(uint8_t id) {
      lights.erase(id);
    }

    void set_light_position(uint8_t id, const glm::vec3 &position) {
      lights[id].position = position;
    }

    void set_light_color(uint8_t id, const glm::vec3 &color) {
      lights[id].color = color;
    }

    void set_ambient_strength(float strength) {
      ambient_strength = strength;
    }

   void set_specular_strength(float strength) {
      specular_strength = strength;
    }

    void set_view_position(const glm::vec3 &position) {
      view_position = glm::vec4(position, 0.0f);
    }

 private:
    void raster_triangle(const _vertex::Primitive &triangle);
    glm::vec4 gen_frag_color_from_attributes(const glm::vec4 &frag_coord, const _vertex::Vertex &a, const _vertex::Vertex &b,
                                             const _vertex::Vertex &c);
    glm::vec4 gen_frag_color_from_texture(const glm::vec4 &frag_coord, const glm::vec4 &a_coord,
                                          const glm::vec4 &b_coord, const glm::vec4 &c_coord);
    glm::vec4 shade_fragment(const glm::vec4 &frag_coord, const glm::vec4 &a_world, const glm::vec4 &b_world, const glm::vec4 &c_world,
                             const glm::vec4 &a_normal, const glm::vec4 &b_normal, const glm::vec4 &c_normal, const glm::vec4 &frag_color);
    void draw_framebuffer();
    void clear_framebuffer();
    float *depth_buffer;
    float ambient_strength;
    float specular_strength;
    glm::vec4 view_position;
    std::map<uint8_t, UserTexture> textures;
    std::map<uint8_t, Light> lights;
    uint8_t current_texture;
    uint8_t next_texture_id;
    uint8_t next_light_id;
    Image framebuffer;
    Texture framebuffer_texture;
    int width;
    int height;
};
}

#endif
