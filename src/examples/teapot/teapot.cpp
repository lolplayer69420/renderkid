// TODO: Subir esto a github y arreglar la iluminacion y las lineas entre triangulos
// TODO: Añadir estadisticas (FPS, tiempo por cuadro, vertices cargados) y clipping
#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#define MODEL_PATH "src/examples/teapot/teapot.obj"


#include "tinyobjloader/tiny_obj_loader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <renderkid.hpp>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>

#define COLOR glm::vec3(0.0f, 0.0f, 1.0f)
#define SPEED 0.01f

struct Vertex {
  glm::vec3 coords;
  glm::vec3 color = COLOR;
  glm::vec3 normal;
};


class Mesh {
  public:
    Mesh(const std::vector<Vertex> vertices) {
      this->vertices = vertices;
      setup_mesh();
    }

    void draw_mesh() const {
      render::use_vertex_buffer(vertex_buffer);
      render::draw(TRIANGLES);
    }
  private:
    std::vector<Vertex> vertices;
    uint8_t vertex_buffer;
    void setup_mesh();
};


class Model {
  public:
    Model(const char *path) {
      load_model(path);
    }

    void draw_model() {
      for (const Mesh &mesh : meshes) {
        mesh.draw_mesh();
      }
    }
  private:
    std::vector<Mesh> meshes;
    void load_model(const char *path);
    Mesh process_shape(const tinyobj::shape_t &shape, const tinyobj::attrib_t &attrib);
};


void Model::load_model(const char *path) {
  tinyobj::ObjReaderConfig reader_config;

  reader_config.triangulate = true;

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "Error while loading object: " << reader.Error() << std::endl;
    }

    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "ObjReader Warning: " << reader.Warning() << "\n";
  }

  const tinyobj::attrib_t &attrib = reader.GetAttrib();
  const std::vector<tinyobj::shape_t> &shapes = reader.GetShapes();

  for (const tinyobj::shape_t &shape : shapes) {
    meshes.push_back(process_shape(shape, attrib));
  }
}


Mesh Model::process_shape(const tinyobj::shape_t &shape, const tinyobj::attrib_t &attrib) {
  size_t offset = 0;
  std::vector<Vertex> mesh_vertices;
  tinyobj::mesh_t mesh = shape.mesh;
  const std::vector<tinyobj::index_t>& indices = mesh.indices;
  const std::vector<int>& material_ids = mesh.material_ids;

  for (size_t i = 0; i < material_ids.size(); ++i) {
    for (size_t j = 0; j < 3; ++j) {
      Vertex vertex;
      tinyobj::index_t index = mesh.indices[offset + j];

      vertex.coords.x = attrib.vertices[3 * index.vertex_index];
      vertex.coords.y = attrib.vertices[3 * index.vertex_index + 1];
      vertex.coords.z = attrib.vertices[3 * index.vertex_index + 2];

      if (index.normal_index >= 0) {
        vertex.normal.x = attrib.normals[3 * index.normal_index];
        vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
        vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
      }

      mesh_vertices.push_back(vertex);
    }

    offset += 3;
  }

  return Mesh(mesh_vertices);
}


void Mesh::setup_mesh() {
  vertex_buffer = render::create_vertex_buffer();
  render::use_vertex_buffer(vertex_buffer);

  render::add_vertex_attribute(VERTEX_ATTRIBUTE, 0);
  render::add_vertex_attribute(COLOR_RGB_ATTRIBUTE, 3);
  render::add_vertex_attribute(NORM_VECTOR_ATTRIBUTE, 6);

  render::load_data_into_vertex_buffer((float*)&vertices[0], (vertices.size() * sizeof(Vertex)) / sizeof(float));
  render::set_vertex_buffer_brightness(4.0f);
}


int main() {
  render::init_window(800, 600);
  render::set_viewport(0, 0, 800, 600);

  Model teapot_model = Model(MODEL_PATH);
  glm::vec4 light_position = glm::vec4(0.0f, 1.0f, 2.0f, 1.0f);
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
  glm::mat4 view = glm::lookAt(glm::vec3(0.5f, -0.5f, 6.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  render::set_perspective_projection(glm::radians(40.0f), 5.0f, 5.0f, 0.5f, 20.0f);
  render::set_model_matrix(model);
  render::set_view_matrix(view);

  render::set_ambient_strength(0.5f);
  render::set_specular_strength(0.5f);
  render::set_view_position(glm::vec3(1.0f, 0.5f, 0.0f));
  uint8_t light = render::create_light(glm::vec3(light_position.x, light_position.y, light_position.z),
                                       glm::vec3(1.0f));

  while (true) {
    model = glm::rotate(model, SPEED, glm::vec3(0.0f, 1.0f, 0.0f));
    render::set_model_matrix(model);
    teapot_model.draw_model();
  }
}
