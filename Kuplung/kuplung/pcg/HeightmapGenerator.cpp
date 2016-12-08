//
//  HeightmapGenerator.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/21/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

// http://www.chadvernon.com/blog/resources/directx9/terrain-generation-with-a-heightmap/
// http://www.rastertek.com/tertut02.html

#include "HeightmapGenerator.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>

void HeightmapGenerator::initPosition() {
    this->position_x1 = 0.0f;
    this->position_x2 = 1.0f;
    this->position_y1 = 0.0f;
    this->position_y2 = 1.0f;

    this->Setting_Octaves = 3;
    this->Setting_Frequency = 2.0f;
    this->Setting_Persistence = 0.5f;
    this->Setting_ColorTerrain = true;
    this->Setting_TerrainType = 0;

    this->Setting_OffsetHorizontal = 0.0f;
    this->Setting_OffsetVertical = 0.0f;
    this->Setting_ScaleCoeficient = 10.0f;
    this->Setting_HeightCoeficient = 10.0f;

    this->Setting_Seed = 1;
}

void HeightmapGenerator::generateTerrain(std::string assetsFolder, const int width, const int height) {
    this->assetsFolder = assetsFolder;
    this->width = width;
    this->height = height;

    this->position_x1 += this->Setting_OffsetHorizontal;
    this->position_x2 += this->Setting_OffsetHorizontal;
    this->position_y1 += this->Setting_OffsetVertical;
    this->position_y2 += this->Setting_OffsetVertical;

    module::Perlin perlinNoiser;

    perlinNoiser.SetOctaveCount(this->Setting_Octaves);
    perlinNoiser.SetFrequency(double(this->Setting_Frequency));
    perlinNoiser.SetPersistence(double(this->Setting_Persistence));
    if (this->Setting_SeedRandom)
        this->Setting_Seed = rand();
    perlinNoiser.SetSeed(this->Setting_Seed);

    // heightmap
    if (this->Setting_TerrainType != GeometryTerrainType_Sphere) {
        utils::NoiseMapBuilderPlane heightMapBuilder;
        heightMapBuilder.SetSourceModule(perlinNoiser);
        heightMapBuilder.SetDestNoiseMap(this->heightMap);
        heightMapBuilder.SetDestSize(this->width, this->height);
        heightMapBuilder.SetBounds(double(this->position_x1), double(this->position_x2), double(this->position_y1), double(this->position_y2));
        heightMapBuilder.Build();
    }
    else {
        utils::NoiseMapBuilderSphere heightMapBuilder;
        heightMapBuilder.SetSourceModule(perlinNoiser);
        heightMapBuilder.SetDestNoiseMap(this->heightMap);
        heightMapBuilder.SetDestSize(this->width, this->height);
        heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);
        heightMapBuilder.Build();
    }

    // render
    utils::RendererImage renderer;
    renderer.SetSourceNoiseMap(this->heightMap);
    renderer.SetDestImage(this->image);
    if (this->Setting_ColorTerrain) {
        renderer.ClearGradient();
        renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 128, 255)); // deeps
        renderer.AddGradientPoint(-0.2500, utils::Color(0, 0, 255, 255)); // shallow
        renderer.AddGradientPoint(0.0000, utils::Color(0, 128, 255, 255)); // shore
        renderer.AddGradientPoint(0.0625, utils::Color(240, 240, 64, 255)); // sand
        renderer.AddGradientPoint(0.1250, utils::Color(32, 160, 0, 255)); // grass
        renderer.AddGradientPoint(0.3750, utils::Color(224, 224, 0, 255)); // dirt
        renderer.AddGradientPoint(0.7500, utils::Color(128, 128, 128, 255)); // rock
        renderer.AddGradientPoint(1.0000, utils::Color(255, 255, 255, 255)); // snow
    }
    renderer.Render();

    std::string filename;
    if (Settings::Instance()->Terrain_HeightmapImageHistory) {
        time_t t = time(0);
        struct tm * now = localtime(&t);

        int year = now->tm_year + 1900;
        int month = now->tm_mon + 1;
        int day = now->tm_mday;
        int hour = now->tm_hour;
        int minute = now->tm_min;
        int seconds = now->tm_sec;

        std::string fileSuffix = std::to_string(year) + std::to_string(month) + std::to_string(day) +
                                 std::to_string(hour) + std::to_string(minute) + std::to_string(seconds);
        filename = "terrain_heightmap_" + fileSuffix + ".bmp";
    }
    else
        filename = "terrain_heightmap.bmp";
    this->heightmapImage = assetsFolder + "/" + filename;

    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("Generating terrain [O = %i, F = %f, P = %f] = %s",
                                                                        this->Setting_Octaves,
                                                                        double(this->Setting_Frequency),
                                                                        double(this->Setting_Persistence),
                                                                        this->heightmapImage.c_str()));

#ifdef _WIN32
#else
    boost::replace_all(assetsFolder, "Kuplung.app/Contents/Resources", "");
#endif
    utils::WriterBMP writer;
    writer.SetSourceImage(this->image);
    writer.SetDestFilename(this->heightmapImage);
    writer.WriteDestFile();

    switch (this->Setting_TerrainType) {
        case GeometryTerrainType_Cubic:
            this->generatePlaneGeometryCubic();
            break;
        case GeometryTerrainType_Sphere:
            this->generateSphereGeometry();
            break;
        default:
            this->generatePlaneGeometrySmooth();
            break;
    }
}

void HeightmapGenerator::generatePlaneGeometrySmooth() {
    // BIG thanks to http://stackoverflow.com/a/10114636/69897 !!!!
    unsigned int heightmapHeight = this->heightMap.GetHeight();
    unsigned int heightmapWidth = this->heightMap.GetWidth();

    this->vertices.clear();
    this->uvs.clear();
    this->indices.clear();
    this->colors.clear();

    float worldCenter = -1.0f * heightmapWidth / 2.0f;

    const float rr = 1.0f / float(heightmapHeight - 1);
    const float ss = 1.0f / float(heightmapWidth - 1);

    float p_x, p_y, p_z;
    glm::vec3 position, color;
    glm::vec2 uv;
    utils::Color c;
    for (unsigned int y=0; y<heightmapHeight; ++y) {
        for (unsigned int x=0; x<heightmapWidth; ++x) {
            p_x = x + worldCenter;
            p_y = y + worldCenter;
            p_z = this->heightMap.GetValue(x, y) * this->Setting_HeightCoeficient;
            position = glm::vec3(p_x, p_y, p_z) / this->Setting_ScaleCoeficient;
            uv = glm::vec2(x * ss, 1.0f - y * rr);

            c = this->image.GetValue(x, y);
            color = glm::vec3(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f);

            this->vertices.push_back(position);
            this->uvs.push_back(uv);
            this->normals.push_back(glm::vec3(0, 1, 0));
            this->colors.push_back(color);
        }
    }

    glm::vec3 v1, v2, v3, normal;
    for (unsigned int y=0; y<(heightmapHeight - 1); ++y) {
        for (unsigned int x=0; x<(heightmapWidth - 1); ++x) {
            unsigned int start = y * heightmapWidth + x;
            this->indices.push_back(start);
            this->indices.push_back(start + 1);
            this->indices.push_back(start + heightmapWidth);
            v1 = this->vertices[start];
            v2 = this->vertices[start + 1];
            v3 = this->vertices[start + heightmapWidth];
            normal = glm::cross(v2 - v1, v3 - v1);
            this->normals[start] = normal;
            this->normals[start + 1] = normal;
            this->normals[start + heightmapWidth] = normal;

            this->indices.push_back(start + 1);
            this->indices.push_back(start + 1 + heightmapWidth);
            this->indices.push_back(start + heightmapWidth);
            v1 = this->vertices[start];
            v2 = this->vertices[start + heightmapWidth];
            v3 = this->vertices[start + 1 + heightmapWidth];
            normal = glm::cross(v2 - v1, v3 - v1);
            this->normals[start] = normal;
            this->normals[start + heightmapWidth] = normal;
            this->normals[start + 1 + heightmapWidth] = normal;
        }
    }

    this->generateMeshModel();
}

void HeightmapGenerator::generateSphereGeometry() {
    unsigned int heightmapHeight = this->heightMap.GetHeight();
    unsigned int heightmapWidth = this->heightMap.GetWidth();

    this->vertices.clear();
    this->uvs.clear();
    this->normals.clear();
    this->colors.clear();
    this->indices.clear();

    std::string grapher = "";

    const float rr = 1.0f / float(heightmapHeight - 1);
    const float ss = 1.0f / float(heightmapWidth - 1);

    static const double pi = glm::pi<double>();
    static const double pi_2 = glm::half_pi<double>();

    float hmValue;
    float p_x, p_y, p_z;
    glm::vec3 position, color;
    glm::vec2 uv;
    utils::Color c;
    for (unsigned int y=0; y<heightmapHeight; ++y) {
        for (unsigned int x=0; x<heightmapWidth; ++x) {
            hmValue = this->heightMap.GetValue(x, y);
            p_x = float(cos(2 * pi * x * ss) * sin(pi * y * rr));
            p_y = float(sin(-pi_2 + pi * y * rr));
            p_z = float(sin(2 * pi * x * ss) * sin(pi * y * rr));
            position = glm::vec3(p_x, p_y, p_z) * this->Setting_ScaleCoeficient;
            position += glm::normalize(position) * hmValue;

            if (Settings::Instance()->logDebugInfo)
                grapher += Settings::Instance()->string_format("%f,%f,%f\n", position.x, position.y, position.z);

            uv = glm::vec2(x * 1.0f / heightmapWidth, y * 1.0f / heightmapHeight);
            c = this->image.GetValue(x, y);
            color = glm::vec3(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f);

            this->vertices.push_back(position);
            this->uvs.push_back(uv);
            this->normals.push_back(glm::vec3(0, 1, 0));
            this->colors.push_back(color);
        }
    }

    glm::vec3 v1, v2, v3, normal;
    for (unsigned int y=0; y<(heightmapHeight - 1); ++y) {
        for (unsigned int x=0; x<(heightmapWidth - 1); ++x) {
            unsigned int start = y * heightmapWidth + x;
            this->indices.push_back(start);
            this->indices.push_back(start + 1);
            this->indices.push_back(start + heightmapWidth);
            v1 = this->vertices[start];
            v2 = this->vertices[start + 1];
            v3 = this->vertices[start + heightmapWidth];
            normal = glm::cross(v2 - v1, v3 - v1);
            this->normals[start] = normal;
            this->normals[start + 1] = normal;
            this->normals[start + heightmapWidth] = normal;

            this->indices.push_back(start + 1);
            this->indices.push_back(start + 1 + heightmapWidth);
            this->indices.push_back(start + heightmapWidth);
            v1 = this->vertices[start];
            v2 = this->vertices[start + heightmapWidth];
            v3 = this->vertices[start + 1 + heightmapWidth];
            normal = glm::cross(v2 - v1, v3 - v1);
            this->normals[start] = normal;
            this->normals[start + heightmapWidth] = normal;
            this->normals[start + 1 + heightmapWidth] = normal;
        }
    }

    this->generateMeshModel();

    if (Settings::Instance()->logDebugInfo) {
        std::ofstream out(this->assetsFolder + "/terrain.txt");
        out << grapher;
        out.close();
    }
}

void HeightmapGenerator::generatePlaneGeometryCubic() {
    int heightmapHeight = this->heightMap.GetHeight();
    int heightmapWidth = this->heightMap.GetWidth();

    this->vertices.clear();
    this->uvs.clear();
    this->normals.clear();
    this->colors.clear();
    this->indices.clear();

    std::string grapher = "";

    const float rr = 1.0f / float(heightmapHeight - 1);
    const float ss = 1.0f / float(heightmapWidth - 1);

    unsigned int vertIndex = 0;
    float worldCenter = -1.0f * heightmapWidth / 2.0f;
    glm::vec3 v0, v1, v2, v3, v4, v5, v10, v11, n, n2, n3, color;
    glm::vec2 uv, uv2, uv3;
    float hmValue, hmValue2, hmValue3;
    utils::Color c;

    for (int y=0; y<(heightmapHeight - 1) * 3; ++y) {
        for (int x=0; x<heightmapWidth - 1; ++x) {
            hmValue = this->heightMap.GetValue(x, y) * this->Setting_HeightCoeficient;
            hmValue2 = this->heightMap.GetValue(x + 1, y) * this->Setting_HeightCoeficient;
            hmValue3 = this->heightMap.GetValue(x, y + 1) * this->Setting_HeightCoeficient;

            c = this->image.GetValue(x, y);
            color = glm::vec3(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f);
            uv = glm::vec2(glm::clamp(float(x), 0.0f, 1.0f), glm::clamp(float(y), 0.0f, 1.0f));
            uv = glm::vec2(x * ss, y * rr);
            uv2 = glm::vec2((x + 1) * ss, y * rr);
            uv3 = glm::vec2(x * ss, (y + 1) * rr);

            // counter clockwise direction
            //
            //   ^
            //   |  11 --- 10 --- 12 ---- 13 --- 14
            //   |   |    / |    / |    / |    / |
            //   |   |   /  |   /  |   /  |   /  |
            //   |   |  /   |  /   |  /   |  /   |
            //   |   | /    | /    | /    | /    |
            //   |   3 ---- 2 ---- 5 ---- 7 ---- 9
            //   |   |    / |    / |    / |    / |
            //   |   |   /  |   /  |   /  |   /  |
            //   |   |  /   |  /   |  /   |  /   |
            //   |   | /    | /    | /    | /    |
            //   Y   0 ---- 1 ---- 4 ---- 6 ---- 8
            //   |
            //   0---X---------------------------------->
            //

            v0 = glm::vec3(x + worldCenter, y + worldCenter, hmValue);
            v1 = glm::vec3(x + worldCenter + 1, y + worldCenter, hmValue);
            v2 = glm::vec3(x + worldCenter + 1, y + worldCenter + 1, hmValue);
            v3 = glm::vec3(x + worldCenter, y + worldCenter + 1, hmValue);
            v4 = glm::vec3(x + worldCenter + 1, y + worldCenter, hmValue2);
            v5 = glm::vec3(x + worldCenter + 1, y + worldCenter + 1, hmValue2);
            v10 = glm::vec3(x + worldCenter + 1, y + worldCenter + 1, hmValue3);
            v11 = glm::vec3(x + worldCenter, y + worldCenter + 1, hmValue3);
            n = glm::cross(v1 - v0, v2 - v0);
            n2 = glm::cross(v4 - v1, v5 - v1);
            n3 = glm::cross(v10 - v3, v11 - v3);

    // triangle 1
            this->vertices.push_back(v0 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v1 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v2 / this->Setting_ScaleCoeficient);

            this->uvs.push_back(uv);
            this->uvs.push_back(uv);
            this->uvs.push_back(uv);

            this->normals.push_back(n);
            this->normals.push_back(n);
            this->normals.push_back(n);

            this->indices.push_back(vertIndex);

            vertIndex += 1;

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

            if (Settings::Instance()->logDebugInfo)
                grapher += Settings::Instance()->string_format(" %f,%f,%f;%f,%f,%f;%f,%f,%f \n",
                       v0.x, v0.y, v0.z,
                       v1.x, v1.y, v1.z,
                       v2.x, v2.y, v2.z);

    // triangle 2
            this->vertices.push_back(v0 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v2 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v3 / this->Setting_ScaleCoeficient);

            this->uvs.push_back(uv);
            this->uvs.push_back(uv);
            this->uvs.push_back(uv);

            this->normals.push_back(n);
            this->normals.push_back(n);
            this->normals.push_back(n);

            this->indices.push_back(vertIndex);

            vertIndex += 1;

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

    // connecting triangle 1 - right
            this->vertices.push_back(v1 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v4 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v5 / this->Setting_ScaleCoeficient);

            this->uvs.push_back(uv2);
            this->uvs.push_back(uv2);
            this->uvs.push_back(uv2);

            this->normals.push_back(n2);
            this->normals.push_back(n2);
            this->normals.push_back(n2);

            this->indices.push_back(vertIndex);

            vertIndex += 1;

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

    // connecting triangle 2 - right
            this->vertices.push_back(v1 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v5 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v2 / this->Setting_ScaleCoeficient);

            this->uvs.push_back(uv2);
            this->uvs.push_back(uv2);
            this->uvs.push_back(uv2);

            this->normals.push_back(n2);
            this->normals.push_back(n2);
            this->normals.push_back(n2);

            this->indices.push_back(vertIndex);

            vertIndex += 1;

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

    // connecting triangle 1 - top
            this->vertices.push_back(v3 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v2 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v10 / this->Setting_ScaleCoeficient);

            this->uvs.push_back(uv3);
            this->uvs.push_back(uv3);
            this->uvs.push_back(uv3);

            this->normals.push_back(n3);
            this->normals.push_back(n3);
            this->normals.push_back(n3);

            this->indices.push_back(vertIndex);

            vertIndex += 1;

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

    // connecting triangle 2 - top
            this->vertices.push_back(v3 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v10 / this->Setting_ScaleCoeficient);
            this->vertices.push_back(v11 / this->Setting_ScaleCoeficient);

            this->uvs.push_back(uv3);
            this->uvs.push_back(uv3);
            this->uvs.push_back(uv3);

            this->normals.push_back(n3);
            this->normals.push_back(n3);
            this->normals.push_back(n3);

            this->indices.push_back(vertIndex);

            vertIndex += 1;

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);
        }
    }

    this->generateMeshModel();

    if (Settings::Instance()->logDebugInfo) {
        std::ofstream out(this->assetsFolder + "/terrain.txt");
        out << grapher;
        out.close();
    }
}

void HeightmapGenerator::generateMeshModel() {
    this->modelTerrain = {};
    this->modelTerrain.vertices.clear();
    this->modelTerrain.texture_coordinates.clear();
    this->modelTerrain.normals.clear();
    this->modelTerrain.indices.clear();

    this->modelTerrain.ID = 1;
    this->modelTerrain.MaterialTitle = "MaterialTerrain";
    this->modelTerrain.ModelTitle = "Terrain";

    this->modelTerrain.vertices = this->vertices;
    this->modelTerrain.texture_coordinates = this->uvs;
    this->modelTerrain.normals = this->normals;
    this->modelTerrain.indices = this->indices;

    this->modelTerrain.countVertices = int(this->vertices.size());
    this->modelTerrain.countTextureCoordinates = int(this->uvs.size());
    this->modelTerrain.countNormals = int(this->normals.size());
    this->modelTerrain.countIndices = int(this->indices.size());

    MeshModelMaterial material = {};
    material.MaterialID = 1;
    material.MaterialTitle = "MaterialTerrain";
    material.AmbientColor = glm::vec3(0.7f);
    material.DiffuseColor = glm::vec3(0.7f);
    material.SpecularExp = 99.0f;
    material.IlluminationMode = 2;
    material.OpticalDensity = 1.0f;
    material.Transparency = 1.0f;
    this->modelTerrain.ModelMaterial = material;
}
