#include <renderkid/rasterization.hpp>
#include <renderkid/vertex_stage.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/ext/scalar_common.hpp>
#include <glm/vec4.hpp>
#include <raylib.h>
#include <utility>
#include <cstdint>
#include <memory>
#include <vector>
#include <cmath>
#include <map>

std::allocator<float> depth_buffer_allocator;
std::allocator<uint8_t> texture_allocator;


inline float determinant(const glm::vec4 &a, const glm::vec4 &b, const glm::vec4 &p) {
  return (a.x - b.x) * (p.y - a.y) - (a.y - b.y) * (p.x - a.x);
}


_raster::Rasterizer::Rasterizer(int width, int height) {
  this->width = width;
  this->height = height;
  textures = std::map<uint8_t, _raster::UserTexture>();
  lights = std::map<uint8_t, _raster::Light>();

  int depth_buffer_size = width * height;

  depth_buffer = depth_buffer_allocator.allocate(depth_buffer_size);

  for (int i = 0; i < depth_buffer_size; ++i) {
    depth_buffer[i] = INFINITY;
  }

  next_texture_id = 0;
  framebuffer = GenImageColor(width, height, BLACK);
  framebuffer_texture = LoadTextureFromImage(framebuffer);
  SetTextureFilter(framebuffer_texture, TEXTURE_FILTER_POINT);
}


uint8_t _raster::Rasterizer::create_texture(int width, int height, int n_channels) {
  _raster::UserTexture new_texture;
  new_texture.width = width;
  new_texture.height = height;
  new_texture.channels = n_channels;
  new_texture.data = texture_allocator.allocate(width * height * n_channels);

  textures[next_texture_id] = new_texture;

  return next_texture_id++;
}


void _raster::Rasterizer::destroy_texture(uint8_t id) {
  int texture_size = textures[id].width * textures[id].height * textures[id].channels;
  texture_allocator.deallocate(textures[id].data, texture_size);
  textures.erase(id);
}


void _raster::Rasterizer::load_data_to_texture(uint8_t *data) {
  _raster::UserTexture texture = textures[current_texture];
  int texture_size = texture.width * texture.height * texture.channels;

  memcpy(textures[current_texture].data, data, texture_size);
}


inline bool is_left_or_top_edge(const glm::vec4 &start, const glm::vec4 &end) {
  glm::vec4 edge = end;
  edge -= start;

  return (edge.y > 0) || (edge.y == 0 && edge.x < 0);
}


glm::vec4 _gen_frag_color_from_vertex_color(const glm::vec4 &frag_coord,
                                            const glm::vec4 &a_color,
                                            const glm::vec4 &b_color,
                                            const glm::vec4 &c_color) {
  glm::vec4 color = a_color * frag_coord.x + b_color * frag_coord.y + c_color * frag_coord.z;
  color *= frag_coord.w;

  float color_a = glm::max(0, glm::min(255, (int)glm::floor(color.x * 256.0f)));
  float color_b = glm::max(0, glm::min(255, (int)glm::floor(color.y * 256.0f)));
  float color_c = glm::max(0, glm::min(255, (int)glm::floor(color.z * 256.0f)));

  return glm::vec4(color_a, color_b, color_c, 255.0);
}


glm::vec4 _raster::Rasterizer::gen_frag_color_from_texture(const glm::vec4 &frag_coord,
                                                           const glm::vec4 &a_coord,
                                                           const glm::vec4 &b_coord,
                                                           const glm::vec4 &c_coord) {
  glm::vec4 uv_coords = a_coord * frag_coord.x + b_coord * frag_coord.y + c_coord * frag_coord.z;
  uv_coords *= frag_coord.w;
  _raster::UserTexture texture = textures[current_texture];

  int coord_x = (int)(uv_coords.x * texture.width) % texture.width;
  int coord_y = (int)(uv_coords.y * texture.height) % texture.height;

  int idx = (coord_y * texture.width + coord_x) * texture.channels;

  return glm::vec4(texture.data[idx], texture.data[idx + 1], texture.data[idx + 2], texture.data[idx + 3]);
}


glm::vec4 _raster::Rasterizer::shade_fragment(const glm::vec4 &frag_coord, const glm::vec4 &a_world, const glm::vec4 &b_world,
                                              const glm::vec4 &c_world, const glm::vec4 &a_normal, const glm::vec4 &b_normal,
                                              const glm::vec4 &c_normal, const glm::vec4 &frag_color, float brightness) {
  glm::vec4 result = frag_color;
  result /= 255.0f;

  glm::vec4 frag_normal = a_normal * frag_coord.x + b_normal * frag_coord.y + c_normal * frag_coord.z;
  glm::vec4 frag_world = a_world * frag_coord.x + b_world * frag_coord.y + c_world * frag_coord.z;

  frag_normal *= frag_coord.w;

  glm::vec4 normal = glm::normalize(frag_normal);
  glm::vec4 view_direction = glm::normalize(view_position - frag_world);
  glm::vec4 diffuse = glm::vec4(1.0f);
  glm::vec4 specular = glm::vec4(1.0f);

  for (auto light : lights) {
    glm::vec4 light_direction = glm::normalize(glm::vec4(light.second.position, 0.0f) - frag_world);
    glm::vec4 reflect_direction = glm::reflect(-light_direction, normal);

    float diff = glm::max(glm::dot(frag_normal, light_direction), 0.0f);
    diffuse = diffuse * glm::vec4(light.second.color * diff, 1.0f);

    float spec = pow(glm::max(glm::dot(view_direction, reflect_direction), 0.0f), brightness);
    specular = specular * specular_strength * spec * glm::vec4(light.second.color, 1.0f);
  }

  glm::vec4 ambient = result * ambient_strength;
  result = (ambient + diffuse + specular) * result;

  result.x = glm::max(0, glm::min(255, (int)glm::floor(result.x * 256.0f)));
  result.y = glm::max(0, glm::min(255, (int)glm::floor(result.y * 256.0f)));
  result.z = glm::max(0, glm::min(255, (int)glm::floor(result.z * 256.0f)));
  result.w = 255.0f;

  return result;
}


glm::vec4 _raster::Rasterizer::gen_frag_color_from_attributes(const glm::vec4 &frag_coord, const _vertex::Vertex &a,
                                                              const _vertex::Vertex &b, const _vertex::Vertex &c,
                                                              float brightness) {
  glm::vec4 result = glm::vec4(1.0f);

  for (size_t i = 0; i < MAX_ATTRIBS; ++i) {
    switch (a.attribs[i].type) {
      case 0:
        return result;
      case COLOR_RGB_ATTRIBUTE:
      case COLOR_RGBA_ATTRIBUTE:
        result *= _gen_frag_color_from_vertex_color(frag_coord, a.attribs[i].data,
                                                    b.attribs[i].data, c.attribs[i].data);
        break;
      case TEXTURE_COORD_ATTRIBUTE:
        result *= gen_frag_color_from_texture(frag_coord, a.attribs[i].data,
                                              b.attribs[i].data, c.attribs[i].data);
        break;
      case NORM_VECTOR_ATTRIBUTE:
        result = shade_fragment(frag_coord, a.world_coords, b.world_coords, c.world_coords,
                                a.attribs[i].data, b.attribs[i].data, c.attribs[i].data, result, brightness);
        break;
    }
  }

  return result;
}


void _raster::Rasterizer::raster_triangle(const _vertex::Primitive &triangle, float brightness) {
  _vertex::Vertex v0 = triangle.vertices[0];
  _vertex::Vertex v2 = triangle.vertices[1];
  _vertex::Vertex v1 = triangle.vertices[2];

  const float xmin = glm::fmin(v0.coords.x, v1.coords.x, v2.coords.x);
  const float ymin = glm::fmin(v0.coords.y, v1.coords.y, v2.coords.y);
  const float xmax = glm::fmax(v0.coords.x, v1.coords.x, v2.coords.x);
  const float ymax = glm::fmax(v0.coords.y, v1.coords.y, v2.coords.y);

  for (float y = ymin; y <= ymax; ++y) {
    if (y < 0 || y > height) {
      continue;
    }

    for (float x = xmin; x <= xmax; ++x) {
      if (x < 0 || x > width) {
        continue;
      }

      glm::vec4 pixel = glm::vec4(x + 0.5, y + 0.5, 0.0f, 0.0f);

      float area = determinant(v0.coords, v1.coords, v2.coords);

      if (area < 0) {
        std::swap(v1, v2);
        area = -area;
      }

      float w0 = determinant(v1.coords, v2.coords, pixel);
      float w1 = determinant(v2.coords, v0.coords, pixel);
      float w2 = determinant(v0.coords, v1.coords, pixel);

      if (is_left_or_top_edge(v1.coords, v2.coords)) {
        --w0;
      }

      if (is_left_or_top_edge(v2.coords, v0.coords)) {
        --w1;
      }

      if (is_left_or_top_edge(v0.coords, v1.coords)) {
        --w2;
      }

      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        glm::vec4 bary_coords = glm::vec4((w0 + 1) / area, (w1 + 1) / area, (w2 + 1) / area, 0.0f);

        float frag_z = 1 / (v0.coords.w * bary_coords.x + v1.coords.w * bary_coords.y + v2.coords.w * bary_coords.z);
        bary_coords.w = frag_z;
        glm::vec4 frag_color;

        frag_color = gen_frag_color_from_attributes(bary_coords, v0, v1, v2, brightness);

        int buffer_index = ((int)y) * width + ((int)x);
        float previous_z = depth_buffer[buffer_index];

        if (frag_z < previous_z) {
          depth_buffer[buffer_index] = frag_z;

          ImageDrawPixel(&framebuffer, x, y, (Color){
            static_cast<uint8_t>(frag_color.x),
            static_cast<uint8_t>(frag_color.y),
            static_cast<uint8_t>(frag_color.z),
            static_cast<uint8_t>(frag_color.w)
          });
        }
      }
    }
  }
}


void _raster::Rasterizer::draw_framebuffer() {
  UpdateTexture(framebuffer_texture, framebuffer.data); 
  DrawTexture(framebuffer_texture, 0, 0, WHITE);
}


void _raster::Rasterizer::clear_framebuffer() {
  ImageClearBackground(&framebuffer, BLACK);
}


void _raster::Rasterizer::draw_primitives(uint8_t primitive_type, std::vector<_vertex::Primitive> &primitives,
                                          float brightness) {
  BeginDrawing();
  clear_framebuffer();
  int depth_buffer_size = width * height;

  for (int i = 0; i < depth_buffer_size; ++i) {
    depth_buffer[i] = INFINITY;
  }

  for (const _vertex::Primitive &primitive : primitives) {
    raster_triangle(primitive, brightness);
  }

  primitives.clear();
  draw_framebuffer();
  EndDrawing();
}
