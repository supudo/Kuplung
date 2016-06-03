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
#include "noise.h"
#include "kuplung/utilities/libnoise/noiseutils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>

HeightmapGenerator::~HeightmapGenerator() {
}

void HeightmapGenerator::initPosition() {
    this->position_x1 = 0.0f;
    this->position_x2 = 1.0f;
    this->position_y1 = 0.0f;
    this->position_y2 = 1.0f;

    this->Setting_Octaves = 3;
    this->Setting_Frequency = 2.0f;
    this->Setting_Persistence = 0.5f;
    this->Setting_ColorTerrain = true;

    this->Setting_OffsetHorizontal = 0.0f;
    this->Setting_OffsetVertical = 0.0f;
}

void HeightmapGenerator::generateTerrain(std::string assetsFolder, int width, int height) {
    this->position_x1 += this->Setting_OffsetHorizontal;
    this->position_x2 += this->Setting_OffsetHorizontal;
    this->position_y1 += this->Setting_OffsetVertical;
    this->position_y2 += this->Setting_OffsetVertical;

    module::Perlin perlinNoiser;

    perlinNoiser.SetOctaveCount(this->Setting_Octaves);
    perlinNoiser.SetFrequency(double(this->Setting_Frequency));
    perlinNoiser.SetPersistence(double(this->Setting_Persistence));

    // heightmap
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    heightMapBuilder.SetSourceModule(perlinNoiser);
    heightMapBuilder.SetDestNoiseMap(heightMap);
    heightMapBuilder.SetDestSize(width, height);
    heightMapBuilder.SetBounds(double(this->position_x1), double(this->position_x2), double(this->position_y1), double(this->position_y2));
    heightMapBuilder.Build();

    // render
    utils::RendererImage renderer;
    utils::Image image;
    renderer.SetSourceNoiseMap(heightMap);
    renderer.SetDestImage(image);
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
    writer.SetSourceImage(image);
    writer.SetDestFilename(this->heightmapImage);
    writer.WriteDestFile();

    // vertices, colors, indices

    int heightmapHeight = heightMap.GetHeight();
    int heightmapWidth = heightMap.GetWidth();

    this->vertices.clear();
    this->uvs.clear();
    this->normals.clear();
    this->colors.clear();
    this->indices.clear();

    std::string grapher = "";

    const float rr = 1.0f / float(heightmapHeight - 1);
    const float ss = 1.0f / float(heightmapWidth - 1);

    float balanceCoeficient = 1.0f;
    float divisionCoeficient = 10.0f;
    unsigned int vertIndex = 0;
    float worldCenter = 0;//-1.0f * heightmapWidth / 2.0f;

    glm::vec3 v0, v1, v2, v3, v4, v5, v10, v11, n, n2, n3;
    glm::vec2 uv, uv2, uv3;
    float hmValue, hmValue2, hmValue3;
    for (int y=0; y<heightmapHeight * 3; ++y) {
    //for (int y=0; y<3; ++y) {
        for (int x=0; x<heightmapWidth; ++x) {
            hmValue = heightMap.GetValue(x, y) * 10.0f;
            hmValue2 = heightMap.GetValue(x + 1, y) * 10.0f;
            hmValue3 = heightMap.GetValue(x, y + 1) * 10.0f;

            utils::Color c = image.GetValue(x, y);
            glm::vec3 color = glm::vec3(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f);
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

            v0 = glm::vec3(x + worldCenter, y + worldCenter, hmValue * balanceCoeficient);
            v1 = glm::vec3(x + worldCenter + 1, y + worldCenter, hmValue * balanceCoeficient);
            v2 = glm::vec3(x + worldCenter + 1, y + worldCenter + 1, hmValue);
            v3 = glm::vec3(x + worldCenter, y + worldCenter + 1, hmValue);
            v4 = glm::vec3(x + worldCenter + 1, y + worldCenter, hmValue2 * balanceCoeficient);
            v5 = glm::vec3(x + worldCenter + 1, y + worldCenter + 1, hmValue2);
            v10 = glm::vec3(x + worldCenter + 1, y + worldCenter + 1, hmValue3 * balanceCoeficient);
            v11 = glm::vec3(x + worldCenter, y + worldCenter + 1, hmValue3);
            n = glm::cross(v1 - v0, v2 - v0);
            n2 = glm::cross(v4 - v1, v5 - v1);
            n3 = glm::cross(v10 - v3, v11 - v3);

    // triangle 1
            this->vertices.push_back(v0 / divisionCoeficient);
            this->vertices.push_back(v1 / divisionCoeficient);
            this->vertices.push_back(v2 / divisionCoeficient);

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
            this->vertices.push_back(v0 / divisionCoeficient);
            this->vertices.push_back(v2 / divisionCoeficient);
            this->vertices.push_back(v3 / divisionCoeficient);

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
            this->vertices.push_back(v1 / divisionCoeficient);
            this->vertices.push_back(v4 / divisionCoeficient);
            this->vertices.push_back(v5 / divisionCoeficient);

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
            this->vertices.push_back(v1 / divisionCoeficient);
            this->vertices.push_back(v5 / divisionCoeficient);
            this->vertices.push_back(v2 / divisionCoeficient);

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
            this->vertices.push_back(v3 / divisionCoeficient);
            this->vertices.push_back(v2 / divisionCoeficient);
            this->vertices.push_back(v10 / divisionCoeficient);

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
            this->vertices.push_back(v3 / divisionCoeficient);
            this->vertices.push_back(v10 / divisionCoeficient);
            this->vertices.push_back(v11 / divisionCoeficient);

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
    textureDiffuse.Image = this->heightmapImage;
    textureDiffuse.Filename = this->heightmapImage;
    textureDiffuse.Width = heightmapWidth;
    textureDiffuse.Height = heightmapHeight;
    textureDiffuse.UseTexture = true;
    material.TextureDiffuse = textureDiffuse;
    this->modelTerrain.ModelMaterial = material;

    if (Settings::Instance()->logDebugInfo) {
        std::ofstream out(assetsFolder + "/terrain.txt");
        out << grapher;
        out.close();
    }

    /*
    // sphere generation
    static const double pi = glm::pi<double>();
    static const double pi_2 = glm::half_pi<double>();

    std::vector<float> hmValues;
    int vertIndex = 0;
    for (int y=0; y<heightmapHeight; ++y) {
        for (int x=0; x<heightmapWidth; ++x) {
            float hmValue = heightMap.GetValue(x, y) * 10;
            hmValues.push_back(hmValue);

            utils::Color c = image.GetValue(x, y);
            glm::vec3 color = glm::vec3(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f);
            glm::vec2 uv = glm::vec2(glm::clamp((float)x, 0.0f, 1.0f), glm::clamp((float)y, 0.0f, 1.0f));

            // triangle 1
            glm::vec3 point(
                        float(cos(2 * pi * x * ss) * sin(pi * y * rr)),
                        float(sin(-pi_2 + pi * y * rr)),
                        float(sin(2 * pi * x * ss) * sin(pi * y * rr))
                    );

            this->vertices.push_back(point * hmValue);
            this->modelTerrain.countVertices += 1;
            this->modelTerrain.vertices.push_back(point * hmValue);

            uv = glm::vec2(x * ss, y * rr);
            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.countTextureCoordinates += 1;

            this->normals.push_back(point);
            this->modelTerrain.countNormals += 1;

            this->indices.push_back(vertIndex);
            vertIndex += 1;
            this->modelTerrain.countIndices += 1;
            this->modelTerrain.indices.push_back(vertIndex);

            this->colors.push_back(color);

            grapher += Settings::Instance()->string_format(" %f,%f,%f \n", point.x, point.y, point.z);
        }
    }
    */
}
