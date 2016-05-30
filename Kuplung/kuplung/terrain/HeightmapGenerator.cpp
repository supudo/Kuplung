//
//  HeightmapGenerator.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/21/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//


// http://www.chadvernon.com/blog/resources/directx9/terrain-generation-with-a-heightmap/

#include "HeightmapGenerator.hpp"
#include "noise.h"
#include "kuplung/utilities/libnoise/noiseutils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/algorithm/string/replace.hpp>

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
}

void HeightmapGenerator::initSettings() {
    this->Setting_Octaves = 1;
    this->Setting_Frequency = 1.0f;
    this->Setting_Persistence = 1.0f;
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
    //heightMapBuilder.SetDestSize(Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    heightMapBuilder.SetDestSize(140, 140);
    heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
    //heightMapBuilder.SetBounds(this->position_x1, this->position_x2, this->position_y1, this->position_y2);
    heightMapBuilder.Build();

    // render
    utils::RendererImage renderer;
    utils::Image image;
    renderer.SetSourceNoiseMap(heightMap);
    renderer.SetDestImage(image);
//    renderer.ClearGradient();
//    renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 128, 255)); // deeps
//    renderer.AddGradientPoint(-0.2500, utils::Color(0, 0, 255, 255)); // shallow
//    renderer.AddGradientPoint(0.0000, utils::Color(0, 128, 255, 255)); // shore
//    renderer.AddGradientPoint(0.0625, utils::Color(240, 240, 64, 255)); // sand
//    renderer.AddGradientPoint(0.1250, utils::Color(32, 160, 0, 255)); // grass
//    renderer.AddGradientPoint(0.3750, utils::Color(224, 224, 0, 255)); // dirt
//    renderer.AddGradientPoint(0.7500, utils::Color(128, 128, 128, 255)); // rock
//    renderer.AddGradientPoint(1.0000, utils::Color(255, 255, 255, 255)); // snow
    renderer.Render();

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
    std::string filename = "terrain_heightmap_" + fileSuffix + ".bmp";
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

    this->vertices = {};
    this->colors = {};
    this->indices = {};

    int vertIndex = 0;
    for (int y=0; y<heightmapHeight; ++y) {
        for (int x=0; x<heightmapWidth; ++x) {

            float hmValue = heightMap.GetValue(x, y);

            this->vertices.push_back(x);
            this->vertices.push_back(y * hmValue + x);
            this->vertices.push_back(y);

            float color = -0.15f + (hmValue / 256.0f);

            this->colors.push_back(0.0);
            this->colors.push_back(0.0);
            this->colors.push_back(color);
            this->colors.push_back(1.0);

            this->indices.push_back(vertIndex);
            vertIndex += 1;

        }
    }
}
