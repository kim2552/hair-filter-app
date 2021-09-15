//
// Created by jooho on 2021-09-14.
//

#include "ModelObj.h"

ModelObj::ModelObj(std::string file, std::vector<Texture> textures)
{
    std::vector<Mesh> meshes;
    model = new Model(meshes);		// Initialize the base Model Object

    filename = file;
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;

    reader_config.mtl_search_path = "./";				// Path to material files

    if (!reader.ParseFromFile(filename, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;

        // We create a new mesh per shape
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        int idx_counter = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                glm::vec3 vertex = { vx, vy, vz };

                // Create Bounding Box information
                if (vx > model->originalBb.max.x) { model->originalBb.max.x = vx; }
                if (vx < model->originalBb.min.x) { model->originalBb.min.x = vx; }
                if (vy > model->originalBb.max.y) { model->originalBb.max.y = vy; }
                if (vy < model->originalBb.min.y) { model->originalBb.min.y = vy; }
                if (vz > model->originalBb.max.z) { model->originalBb.max.z = vz; }
                if (vz < model->originalBb.min.z) { model->originalBb.min.z = vz; }

                // Check if `normal_index` is zero or positive. negative = no normal data
                glm::vec3 normal = { 0.0f, 0.0f, 0.0f };
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                    normal = { nx, ny, nz };
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                glm::vec2 texcoord = { 0.0f, 0.0f };
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                    texcoord = { tx, ty };
                }

                // Optional: vertex colors
                tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                glm::vec3 color = { red, green, blue };

                vertices.push_back({
                                           vertex,
                                           normal,
                                           color,
                                           texcoord
                                   });
                indices.push_back(idx_counter++);	// Store the index of the vertex
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }

        model->originalPosition = glm::vec3(model->originalBb.min.x + ((model->originalBb.max.x - model->originalBb.min.x) / 2.0f),
                                            model->originalBb.min.y + ((model->originalBb.max.y - model->originalBb.min.y) / 2.0f),
                                            model->originalBb.min.z + ((model->originalBb.max.z - model->originalBb.min.z) / 2.0f));

        model->meshes.push_back(Mesh(vertices, indices, textures));
    }
}