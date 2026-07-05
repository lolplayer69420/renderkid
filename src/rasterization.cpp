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

std::allocator<float> depth_buffer_allocator;


inline float determinant(const glm::vec4 &a, const glm::vec4 &b, const glm::vec4 &c) {
  glm::vec4 ab = glm::vec4(b);
  glm::vec4 ac = glm::vec4(c);

  ab -= a;
  ac -= a;

  return ab.y * ac.x - ab.x * ac.y;
}


_raster::Rasterizer::Rasterizer(int width, int height) {
  this->width = width;
  this->height = height;

  int depth_buffer_size = width * height;

  depth_buffer = depth_buffer_allocator.allocate(depth_buffer_size);

  for (int i = 0; i < depth_buffer_size; ++i) {
    depth_buffer[i] = INFINITY;
  }

  framebuffer = GenImageColor(width, height, BLACK);
  texture = LoadTextureFromImage(framebuffer);
  SetTextureFilter(texture, TEXTURE_FILTER_POINT);
}


inline bool is_left_or_top_edge(const glm::vec4 &start, const glm::vec4 &end) {
  glm::vec4 edge = end;
  edge -= start;

  return (edge.y > 0) || (edge.y == 0 && edge.x < 0);
}


glm::vec4 gen_frag_color_from_vertex_color(const glm::vec3 &frag_coord,
                                           const glm::vec4 &a_color,
                                           const glm::vec4 &b_color,
                                           const glm::vec4 &c_color) {
  glm::vec4 lambda = glm::vec4(frag_coord, 0.0f);
  glm::vec4 color = a_color * lambda.x + b_color * lambda.y + c_color * lambda.z;

  float color_a = glm::max(0, glm::min(255, (int)glm::floor(color.x * 256.0f)));
  float color_b = glm::max(0, glm::min(255, (int)glm::floor(color.y * 256.0f)));
  float color_c = glm::max(0, glm::min(255, (int)glm::floor(color.z * 256.0f)));

  return glm::vec4(color_a, color_b, color_c, 255.0);
}


void _raster::Rasterizer::raster_triangle(const _vertex::Primitive &triangle) {
  glm::vec4 a = triangle.triangle_coords[0];
  glm::vec4 b = triangle.triangle_coords[1];
  glm::vec4 c = triangle.triangle_coords[2];

  const float xmin = glm::fmin(a.x, b.x, c.x);
  const float ymin = glm::fmin(a.y, b.y, c.y);
  const float xmax = glm::fmax(a.x, b.x, c.x);
  const float ymax = glm::fmax(a.y, b.y, c.y);

  for (float y = ymin; y <= ymax; ++y) {
    for (float x = xmin; x <= xmax; ++x) {
      glm::vec4 pixel = glm::vec4(x, y, 0.0f, 0.0f);

      float area = determinant(a, b, c);
      bool clockwise = false;

      if (area < 0) {
        std::swap(b, c);
        area = -area;
        clockwise = true;
      }

      float w0 = determinant(b, c, pixel);
      float w1 = determinant(c, a, pixel);
      float w2 = determinant(a, b, pixel);

      if (is_left_or_top_edge(b, c)) {
        --w0;
      }

      if (is_left_or_top_edge(c, a)) {
        --w1;
      }

      if (is_left_or_top_edge(a, b)) {
        --w2;
      }

      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        glm::vec3 bary_coords = glm::vec3((w0 + 1) / area,(w1 + 1) / area, (w2 + 1) / area);
        float frag_z = (1 / a.z) * bary_coords.x + (1 / b.z) * bary_coords.y + (1 / c.z) * bary_coords.z;
        frag_z = 1 / frag_z;
        glm::vec4 frag_color;

        if (!clockwise) {
          frag_color = gen_frag_color_from_vertex_color(
            bary_coords,
            triangle.triangle_colors[0],
            triangle.triangle_colors[2],
            triangle.triangle_colors[1]
          );
        } else {
          frag_color = gen_frag_color_from_vertex_color(
            bary_coords,
            triangle.triangle_colors[0],
            triangle.triangle_colors[1],
            triangle.triangle_colors[2]
          );
        }

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
  UpdateTexture(texture, framebuffer.data); 
  DrawTexture(texture, 0, 0, WHITE);
}


void _raster::Rasterizer::clear_framebuffer() {
  ImageClearBackground(&framebuffer, BLACK);
}


void _raster::Rasterizer::draw_primitives(uint8_t primitive_type, std::vector<_vertex::Primitive> &primitives) {
  BeginDrawing();
  clear_framebuffer();
  int depth_buffer_size = width * height;

  for (int i = 0; i < depth_buffer_size; ++i) {
    depth_buffer[i] = INFINITY;
  }

  for (const _vertex::Primitive &primitive : primitives) {
    raster_triangle(primitive);
  }

  primitives.clear();
  draw_framebuffer();
  EndDrawing();
}
