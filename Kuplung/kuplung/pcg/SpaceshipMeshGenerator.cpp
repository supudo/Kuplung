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
    this->gridSize = gridSize;

    this->vertices.clear();
    this->normals.clear();
    this->uvs.clear();
    this->indices.clear();
    this->colors.clear();

    this->generateFirstHull();
    this->generateMeshModel();
}

void SpaceshipMeshGenerator::generateFirstHull() {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> distGridSize(-1.0 * this->gridSize, this->gridSize);
    std::uniform_int_distribution<std::mt19937::result_type> distColor(0, 1);

    int x = int(distGridSize(rng));
    int y = int(distGridSize(rng));
    int z = int(distGridSize(rng));
    int extrude_size = int(distGridSize(rng)) / 10;

    float px = float(x) / 10;
    float py = float(y) / 10;
    float pz = float(z) / 10;

    // side 1
    glm::vec3 v0 = glm::vec3(px, py, pz);
    glm::vec3 v1 = glm::vec3(px + 1, py, pz);
    glm::vec3 v2 = glm::vec3(px, py - 1, pz);
    glm::vec3 v3 = glm::vec3(px + 1, py - 1, pz);

    this->vertices.push_back(v0);
    this->vertices.push_back(v1);
    this->vertices.push_back(v2);
    this->vertices.push_back(v3);

    this->uvs.push_back(glm::vec2(0));

    this->normals.push_back(glm::cross(v1 - v0, v2 - v0));
    this->normals.push_back(glm::cross(v2 - v1, v3 - v1));

    this->indices.push_back(0);
    this->indices.push_back(1);
    this->indices.push_back(2);
    this->indices.push_back(1);
    this->indices.push_back(2);
    this->indices.push_back(3);

    // side 2
    glm::vec3 v4 = v0 + glm::vec3(0, 0, extrude_size);
    glm::vec3 v5 = v1 + glm::vec3(0, 0, extrude_size);
    glm::vec3 v6 = v2 + glm::vec3(0, 0, extrude_size);
    glm::vec3 v7 = v3 + glm::vec3(0, 0, extrude_size);

    this->vertices.push_back(v4);
    this->vertices.push_back(v5);
    this->vertices.push_back(v6);
    this->vertices.push_back(v7);

    this->uvs.push_back(glm::vec2(0));

    this->normals.push_back(glm::cross(v5 - v4, v6 - v4));
    this->normals.push_back(glm::cross(v6 - v5, v7 - v5));

    this->indices.push_back(4);
    this->indices.push_back(5);
    this->indices.push_back(6);
    this->indices.push_back(5);
    this->indices.push_back(6);
    this->indices.push_back(7);

    // connections
    this->uvs.push_back(glm::vec2(0));
    this->normals.push_back(glm::cross(v6 - v2, v4 - v2));
    this->normals.push_back(glm::cross(v4 - v0, v2 - v0));
    this->indices.push_back(6);
    this->indices.push_back(4);
    this->indices.push_back(2);
    this->indices.push_back(4);
    this->indices.push_back(2);
    this->indices.push_back(0);

    this->uvs.push_back(glm::vec2(0));
    this->normals.push_back(glm::cross(v5 - v3, v5 - v3));
    this->normals.push_back(glm::cross(v5 - v1, v3 - v1));
    this->indices.push_back(7);
    this->indices.push_back(5);
    this->indices.push_back(3);
    this->indices.push_back(5);
    this->indices.push_back(3);
    this->indices.push_back(1);

    this->uvs.push_back(glm::vec2(0));
    this->normals.push_back(glm::cross(v7 - v2, v6 - v2));
    this->normals.push_back(glm::cross(v7 - v2, v3 - v2));
    this->indices.push_back(6);
    this->indices.push_back(7);
    this->indices.push_back(2);
    this->indices.push_back(7);
    this->indices.push_back(2);
    this->indices.push_back(3);

    this->uvs.push_back(glm::vec2(0));
    this->normals.push_back(glm::cross(v5 - v0, v4 - v0));
    this->normals.push_back(glm::cross(v5 - v0, v1 - v0));
    this->indices.push_back(4);
    this->indices.push_back(5);
    this->indices.push_back(0);
    this->indices.push_back(5);
    this->indices.push_back(0);
    this->indices.push_back(1);

    // colors
    for (size_t i=0; i<this->vertices.size(); i++) {
        //this->colors.push_back(glm::vec3(distColor(rng), distColor(rng), distColor(rng)));
        this->colors.push_back(glm::vec3(0.8));
    }
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
