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
    this->gridSize = gridSize / 10;

    this->vertices.clear();
    this->normals.clear();
    this->uvs.clear();
    this->indices.clear();
    this->colors.clear();

    this->fileParser = new FileModelManager();
    this->fileParser->init(nullptr);

    this->generateFirstHull();
    this->generateMeshModel();
}

void SpaceshipMeshGenerator::generateFirstHull() {
    FBEntity file;
    file.isFile = true;
    file.extension = ".obj";
    file.title = "cube";
    file.path = Settings::Instance()->appFolder() + "/shapes/cube.obj";
    MeshModel hull = this->fileParser->parse(file, FileBrowser_ParserType_Own2)[0];

    this->vertices = hull.vertices;
    this->normals = hull.normals;
    this->uvs = hull.texture_coordinates;
    this->indices = hull.indices;

    for (size_t i=0; i<this->vertices.size(); i++) {
        this->colors.push_back(glm::vec3(0.8));
    }
}

void SpaceshipMeshGenerator::generateFirstHull2() {
    this->vertices.push_back(glm::vec3(-1.0,  1.0, -1.0));
    this->vertices.push_back(glm::vec3( 1.0,  1.0, -1.0));
    this->vertices.push_back(glm::vec3( 1.0, -1.0, -1.0));
    this->vertices.push_back(glm::vec3(-1.0, -1.0, -1.0));
    this->vertices.push_back(glm::vec3(-1.0,  1.0,  1.0));
    this->vertices.push_back(glm::vec3( 1.0,  1.0,  1.0));
    this->vertices.push_back(glm::vec3( 1.0, -1.0,  1.0));
    this->vertices.push_back(glm::vec3(-1.0, -1.0,  1.0));

    glm::vec3 norms[] = {
        glm::vec3( 0.0,  0.0, -1.0),
        glm::vec3( 0.0,  0.0,  1.0),
        glm::vec3( 0.0,  1.0,  0.0),
        glm::vec3( 1.0,  0.0,  0.0),
        glm::vec3( 0.0, -1.0,  0.0),
        glm::vec3(-1.0,  0.0,  0.0)
    };

    this->indices.push_back(1); this->normals.push_back(norms[0]);
    this->indices.push_back(2); this->normals.push_back(norms[0]);
    this->indices.push_back(3); this->normals.push_back(norms[0]);

    this->indices.push_back(7); this->normals.push_back(norms[1]);
    this->indices.push_back(6); this->normals.push_back(norms[1]);
    this->indices.push_back(5); this->normals.push_back(norms[1]);

    this->indices.push_back(4); this->normals.push_back(norms[2]);
    this->indices.push_back(5); this->normals.push_back(norms[2]);
    this->indices.push_back(1); this->normals.push_back(norms[2]);

    this->indices.push_back(5); this->normals.push_back(norms[3]);
    this->indices.push_back(6); this->normals.push_back(norms[3]);
    this->indices.push_back(2); this->normals.push_back(norms[3]);

    this->indices.push_back(2); this->normals.push_back(norms[4]);
    this->indices.push_back(6); this->normals.push_back(norms[4]);
    this->indices.push_back(7); this->normals.push_back(norms[4]);

    this->indices.push_back(0); this->normals.push_back(norms[5]);
    this->indices.push_back(3); this->normals.push_back(norms[5]);
    this->indices.push_back(7); this->normals.push_back(norms[5]);

    this->indices.push_back(0); this->normals.push_back(norms[0]);
    this->indices.push_back(1); this->normals.push_back(norms[0]);
    this->indices.push_back(3); this->normals.push_back(norms[0]);

    this->indices.push_back(4); this->normals.push_back(norms[1]);
    this->indices.push_back(7); this->normals.push_back(norms[1]);
    this->indices.push_back(5); this->normals.push_back(norms[1]);

    this->indices.push_back(0); this->normals.push_back(norms[2]);
    this->indices.push_back(4); this->normals.push_back(norms[2]);
    this->indices.push_back(1); this->normals.push_back(norms[2]);

    this->indices.push_back(1); this->normals.push_back(norms[3]);
    this->indices.push_back(5); this->normals.push_back(norms[3]);
    this->indices.push_back(2); this->normals.push_back(norms[3]);

    this->indices.push_back(3); this->normals.push_back(norms[4]);
    this->indices.push_back(2); this->normals.push_back(norms[4]);
    this->indices.push_back(7); this->normals.push_back(norms[4]);

    this->indices.push_back(4); this->normals.push_back(norms[5]);
    this->indices.push_back(0); this->normals.push_back(norms[5]);
    this->indices.push_back(7); this->normals.push_back(norms[5]);

    for (size_t i=0; i<this->indices.size(); i++) {
        this->colors.push_back(glm::vec3(0.8));
    }
}

void SpaceshipMeshGenerator::generateFirstHull3() {
    int x = this->getRandomValue(-1.0 * this->gridSize, this->gridSize);
    int y = this->getRandomValue(-1.0 * this->gridSize, this->gridSize);
    int z = this->getRandomValue(-1.0 * this->gridSize, this->gridSize);
    int extrude_size = this->getRandomValue(-1.0 * this->gridSize, this->gridSize, false);

    float px = float(x);
    float py = float(y);
    float pz = float(z);

    // side 1
    glm::vec3 v0 = glm::vec3(px, py, pz);
    glm::vec3 v1 = glm::vec3(px, py + 1, pz);
    glm::vec3 v2 = glm::vec3(px, py, pz - 1);
    glm::vec3 v3 = glm::vec3(px, py + 1, pz - 1);

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
    glm::vec3 v4 = v0 + glm::vec3(extrude_size, 0, 0);
    glm::vec3 v5 = v1 + glm::vec3(extrude_size, 0, 0);
    glm::vec3 v6 = v2 + glm::vec3(extrude_size, 0, 0);
    glm::vec3 v7 = v3 + glm::vec3(extrude_size, 0, 0);

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
//        float r = this->getRandomValue(-1.0 * this->gridSize, this->gridSize);
//        float g = this->getRandomValue(-1.0 * this->gridSize, this->gridSize);
//        float b = this->getRandomValue(-1.0 * this->gridSize, this->gridSize);
//        this->colors.push_back(glm::vec3(r, g, b));
        this->colors.push_back(glm::vec3(0.8));
    }
}

int SpaceshipMeshGenerator::getRandomValue(float valueMin, float valueMax, bool zeroIsValid) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> distr(valueMin, valueMax);

    int v = int(distr(rng));
    if (zeroIsValid)
        return v;
    else {
        if (v == 0)
            return this->getRandomValue(valueMin, valueMax, zeroIsValid);
        else
            return v;
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