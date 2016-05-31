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
    this->position_x1 = 6.0;
    this->position_x2 = 10.0;
    this->position_y1 = 1.0;
    this->position_y2 = 5.0;

    this->Setting_Octaves = 6;
    this->Setting_Frequency = 1.0f;
    this->Setting_Persistence = 0.5f;
    this->Setting_ColorTerrain = false;
}

void HeightmapGenerator::initSettings() {
    this->Setting_Octaves = 1;
    this->Setting_Frequency = 1.0f;
    this->Setting_Persistence = 1.0f;
    this->Setting_ColorTerrain = false;
}

void HeightmapGenerator::generateTerrain(std::string assetsFolder, double offsetHorizontal, double offsetVertical) {
    this->position_x1 += offsetHorizontal;
    this->position_x2 += offsetHorizontal;
    this->position_y1 += offsetVertical;
    this->position_y2 += offsetVertical;

    module::Perlin perlinNoiser;

    perlinNoiser.SetOctaveCount(this->Setting_Octaves);
    perlinNoiser.SetFrequency((double)this->Setting_Frequency);
    perlinNoiser.SetPersistence((double)this->Setting_Persistence);

    // heightmap
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    heightMapBuilder.SetSourceModule(perlinNoiser);
    heightMapBuilder.SetDestNoiseMap(heightMap);
    heightMapBuilder.SetDestSize(20, 20);
    heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
//    heightMapBuilder.SetBounds(this->position_x1, this->position_x2, this->position_y1, this->position_y2);
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
                                                                        (double)this->Setting_Frequency,
                                                                        (double)this->Setting_Persistence,
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
    this->normals.clear();
    this->colors.clear();
    this->indices.clear();

    std::string grapher = "";

    this->modelTerrain = {};

    this->modelTerrain.countIndices = 0;
    this->modelTerrain.countNormals = 0;
    this->modelTerrain.countTextureCoordinates = 0;
    this->modelTerrain.countVertices = 0;
    this->modelTerrain.ID = 1;
    this->modelTerrain.MaterialTitle = "MaterialTerrain";
    this->modelTerrain.ModelTitle = "Terrain";

    static const double pi = glm::pi<double>();
    static const double pi_2 = glm::half_pi<double>();

    const float rr = 1.0f / float(heightmapHeight - 1);
    const float ss = 1.0f / float(heightmapWidth - 1);

    /*
    // sphere generation
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

    int vertIndex = 0;
    for (int y=0; y<heightmapHeight; ++y) {
        for (int x=0; x<heightmapWidth; ++x) {
            float hmValue = heightMap.GetValue(x, y) * 10;

            utils::Color c = image.GetValue(x, y);
            glm::vec3 color = glm::vec3(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f);
            glm::vec2 uv = glm::vec2(glm::clamp((float)x, 0.0f, 1.0f), glm::clamp((float)y, 0.0f, 1.0f));

    // triangle 1
            glm::vec3 v1 = glm::vec3(x, y, hmValue);
            glm::vec3 v2 = glm::vec3(x + 1, y, hmValue);
            glm::vec3 v3 = glm::vec3(x + 1, y + 1, hmValue);
            this->vertices.push_back(v1 / 10.0f);
            this->vertices.push_back(v2 / 10.0f);
            this->vertices.push_back(v3 / 10.0f);
            this->modelTerrain.countVertices += 3;
            this->modelTerrain.vertices.push_back(v1 / 10.0f);
            this->modelTerrain.vertices.push_back(v2 / 10.0f);
            this->modelTerrain.vertices.push_back(v3 / 10.0f);

            uv = glm::vec2(x * ss, y * rr);
            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.countTextureCoordinates += 3;

            glm::vec3 n1 = glm::cross(v2 - v1, v3 - v1);
            this->normals.push_back(n1);
            this->normals.push_back(n1);
            this->normals.push_back(n1);
            this->modelTerrain.countNormals += 3;
            this->modelTerrain.normals.push_back(n1);
            this->modelTerrain.normals.push_back(n1);
            this->modelTerrain.normals.push_back(n1);

            this->indices.push_back(vertIndex);
            vertIndex += 1;
            this->modelTerrain.countIndices += 1;
            this->modelTerrain.indices.push_back(vertIndex);

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

    // triangle 2
            glm::vec3 v4 = glm::vec3(x, y, hmValue);
            glm::vec3 v5 = glm::vec3(x, y + 1, hmValue);
            glm::vec3 v6 = glm::vec3(x + 1, y + 1, hmValue);
            this->vertices.push_back(v4 / 10.0f);
            this->vertices.push_back(v6 / 10.0f);
            this->vertices.push_back(v5 / 10.0f);
            this->modelTerrain.countVertices += 3;
            this->modelTerrain.vertices.push_back(v4 / 10.0f);
            this->modelTerrain.vertices.push_back(v5 / 10.0f);
            this->modelTerrain.vertices.push_back(v6 / 10.0f);

            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.texture_coordinates.push_back(uv);
            this->modelTerrain.countTextureCoordinates += 3;

            glm::vec3 n2 = glm::cross(v5 - v4, v6 - v4);
            this->normals.push_back(n2);
            this->normals.push_back(n2);
            this->normals.push_back(n2);
            this->modelTerrain.countNormals += 3;
            this->modelTerrain.normals.push_back(n2);
            this->modelTerrain.normals.push_back(n2);
            this->modelTerrain.normals.push_back(n2);

            this->indices.push_back(vertIndex);
            vertIndex += 1;
            this->modelTerrain.countIndices += 1;
            this->modelTerrain.indices.push_back(vertIndex);

            this->colors.push_back(color);
            this->colors.push_back(color);
            this->colors.push_back(color);

            grapher += Settings::Instance()->string_format(" %f,%f,%f;%f,%f,%f;%f,%f,%f \n",
                   v1.x, v1.y, v1.z,
                   v3.x, v2.y, v2.z,
                   v2.x, v3.y, v3.z);

            grapher += Settings::Instance()->string_format(" %f,%f,%f;%f,%f,%f;%f,%f,%f \n",
                   v4.x, v4.y, v4.z,
                   v5.x, v5.y, v5.z,
                   v6.x, v6.y, v6.z);
        }
    }

    MeshModelMaterial material;
    material.MaterialID = 1;
    material.MaterialTitle = "MaterialTerrain";
    material.AmbientColor = glm::vec3(0.7f);
    material.DiffuseColor = glm::vec3(0.7f);
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

    std::ofstream out(assetsFolder + "/terrain.txt");
    out << grapher;
    out.close();
}
