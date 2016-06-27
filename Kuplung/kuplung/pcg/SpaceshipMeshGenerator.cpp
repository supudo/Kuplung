//
//  SpaceshipMeshGenerator.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/21/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SpaceshipMeshGenerator.hpp"
#include <random>

SpaceshipMeshGenerator::~SpaceshipMeshGenerator() {
}

void SpaceshipMeshGenerator::generate(int gridSize) {
    this->vertices.clear();
    this->normals.clear();
    this->uvs.clear();
    this->indices.clear();
    this->colors.clear();

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0, gridSize);

    unsigned int x = dist6(rng);
    unsigned int y = dist6(rng);
    unsigned int z = dist6(rng);

    float px = float(x) / 10;
    float py = float(y) / 10;
    float pz = float(z) / 10;

    //
    //   y
    //   ^
    //   |   0 -- 1
    //   |   |   /|
    //   |   |  / |
    //   |   | /  |
    //   |   2 ---3
    //   |
    //   -----------> x
    //
    glm::vec3 v1 = glm::vec3(px, py, pz);
    glm::vec3 v2 = glm::vec3(px + 1, py, pz);
    glm::vec3 v3 = glm::vec3(px, py - 1, pz);
    glm::vec3 v4 = glm::vec3(px + 1, py - 1, pz);

    this->vertices.push_back(v1);
    this->vertices.push_back(v2);
    this->vertices.push_back(v3);
    this->vertices.push_back(v4);

    this->uvs.push_back(glm::vec2(0));

    this->normals.push_back(glm::cross(v2 - v1, v3 - v1));
    this->normals.push_back(glm::cross(v3 - v2, v4 - v2));

    this->indices.push_back(0);
    this->indices.push_back(1);
    this->indices.push_back(2);
    this->indices.push_back(1);
    this->indices.push_back(2);
    this->indices.push_back(3);

    for (size_t i=0; i<this->vertices.size(); i++) {
        this->colors.push_back(glm::vec3(1.0, 0.0, 0.0));
    }

    this->generateMeshModel();
}

void SpaceshipMeshGenerator::generateMeshModel() {
    this->modelSpaceship = {};
    this->modelSpaceship.vertices.clear();
    this->modelSpaceship.texture_coordinates.clear();
    this->modelSpaceship.normals.clear();
    this->modelSpaceship.indices.clear();

    this->modelSpaceship.ID = 1;
    this->modelSpaceship.MaterialTitle = "MaterialSpaceship";
    this->modelSpaceship.ModelTitle = "Spaceship";

    this->modelSpaceship.vertices = this->vertices;
    this->modelSpaceship.texture_coordinates = this->uvs;
    this->modelSpaceship.normals = this->normals;
    this->modelSpaceship.indices = this->indices;

    this->modelSpaceship.countVertices = int(this->vertices.size());
    this->modelSpaceship.countTextureCoordinates = int(this->uvs.size());
    this->modelSpaceship.countNormals = int(this->normals.size());
    this->modelSpaceship.countIndices = int(this->indices.size());

    MeshModelMaterial material;
    material.MaterialID = 1;
    material.MaterialTitle = "MaterialTerrain";
    material.AmbientColor = glm::vec3(0.7f);
    material.DiffuseColor = glm::vec3(0.7f);
    material.SpecularExp = 99.0f;
    material.IlluminationMode = 2;
    material.OpticalDensity = 1.0;
    material.Transparency = 1.0f;
    MeshMaterialTextureImage textureDiffuse;
    textureDiffuse.Image = "";
    textureDiffuse.Filename = "";
    textureDiffuse.Width = 0;
    textureDiffuse.Height = 0;
    textureDiffuse.UseTexture = true;
    material.TextureDiffuse = textureDiffuse;
    this->modelSpaceship.ModelMaterial = material;
}
