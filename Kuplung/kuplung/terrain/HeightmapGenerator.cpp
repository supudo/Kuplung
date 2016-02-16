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
}

void HeightmapGenerator::generateTerrain(std::string assetsFolder, int screenWidth, int screenHeight, double offsetHorizontal, double offsetVertical) {
    this->position_x1 += offsetHorizontal;
    this->position_x2 += offsetHorizontal;
    this->position_y1 += offsetVertical;
    this->position_y2 += offsetVertical;

    module::Perlin perlinNoiser;

    // heightmap
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    heightMapBuilder.SetSourceModule(perlinNoiser);
    heightMapBuilder.SetDestNoiseMap(heightMap);
    //heightMapBuilder.SetDestSize(screenWidth, screenHeight);
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

    // write image
    boost::replace_all(assetsFolder, "Kuplung.app/Contents/Resources", "");
    utils::WriterBMP writer;
    writer.SetSourceImage(image);
    writer.SetDestFilename(assetsFolder + "/terrain_heightmap.bmp");
    writer.WriteDestFile();
    this->heightmapImage = assetsFolder + "/terrain_heightmap.bmp";

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
