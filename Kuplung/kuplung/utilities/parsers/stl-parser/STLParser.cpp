//
//  STLParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "STLParser.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>

STLParser::STLParser() {
  this->Setting_Axis_Forward = 0;
  this->Setting_Axis_Up = 0;
}

STLParser::~STLParser() {}

void STLParser::init(const std::function<void(float)>& doProgress) {
  this->doProgress = doProgress;
  this->Setting_Axis_Forward = 4;
  this->Setting_Axis_Up = 5;
  this->parserUtils = std::make_unique<ParserUtils>();
}

std::vector<MeshModel> STLParser::parse(const FBEntity& file, const std::vector<std::string>& settings) {
  this->models = {};

  if (settings.size() > 0 && !settings[0].empty())
    this->Setting_Axis_Forward = std::stoi(settings[0]);
  if (settings.size() > 1 && !settings[1].empty())
    this->Setting_Axis_Up = std::stoi(settings[1]);

  unsigned int fileSize;

  FILE* fp = fopen(file.path.c_str(), "rb");
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  char* stl_data_buffer = new char[fileSize + 1];
  fseek(fp, 0, SEEK_SET);
  fread(stl_data_buffer, sizeof(char), fileSize, fp);
  fclose(fp);
  stl_data_buffer[fileSize] = '\0';

  bool result = false;
  if (this->isBinarySTL(stl_data_buffer, fileSize))
    result = this->loadBinaryFile(file, fileSize);
  else if (this->isAsciiSTL(stl_data_buffer, fileSize))
    result = this->loadAsciiFile(file);
  else
    Settings::Instance()->funcDoLog("[STLParser] Error occured - cannot determing file type (ascii/binary)!");

  if (!result)
    Settings::Instance()->funcDoLog("[STLParser] Error occured - parsing failed! See errors logged above.");

  delete[] stl_data_buffer;

  return this->models;
}

bool STLParser::isBinarySTL(const char* buffer, unsigned int fileSize) {
  if (fileSize < 84)
    return false;
  const uint32_t faceCount = *reinterpret_cast<const uint32_t*>(buffer + 80);
  const uint32_t expectedBinaryFileSize = faceCount * 50 + 84;
  return expectedBinaryFileSize == fileSize;
}

bool STLParser::isAsciiSTL(const char* buffer, unsigned int fileSize) {
  if (this->isBinarySTL(buffer, fileSize))
    return false;

  const char* bufferEnd = buffer + fileSize;

  if (!this->skipSpaces(&buffer))
    return false;

  if (buffer + 5 >= bufferEnd)
    return false;

  bool isASCII(strncmp(buffer, "solid", 5) == 0);
  if (isASCII) {
    // A lot of importers are write solid even if the file is binary. So we have to check for ASCII-characters.
    if (fileSize >= 500) {
      isASCII = true;
      for (unsigned int i = 0; i < 500; i++) {
        if (buffer[i] > 127) {
          isASCII = false;
          break;
        }
      }
    }
  }
  return isASCII;
}

template <class char_t>
bool STLParser::skipSpaces(const char_t** inout) {
  return this->skipSpaces<char_t>(*inout, inout);
}

template <class char_t>
bool STLParser::skipSpaces(const char_t* in, const char_t** out) {
  while (*in == char_t(' ') || *in == char_t('\t'))
    ++in;
  *out = in;
  return !this->isLineEnd<char_t>(*in);
}

template <class char_t>
bool STLParser::isLineEnd(char_t in) {
  return (in == char_t('\r') || in == char_t('\n') || in == char_t('\0') || in == char_t('\f'));
}

glm::vec3 STLParser::parsePoint(std::ifstream& s) {
  float x = this->parseFloat(s);
  float y = this->parseFloat(s);
  float z = this->parseFloat(s);
  return glm::vec3(x, y, z);
}

float STLParser::parseFloat(std::ifstream& s) {
  char f_buf[sizeof(float)];
  s.read(f_buf, 4);
  float* fptr = (float*)f_buf;
  return *fptr;
}

bool STLParser::loadBinaryFile(const FBEntity& file, unsigned int fileSize) {
  Settings::Instance()->funcDoLog("[STLParser] Parsing binary STL file...");
  bool result = true;

  if (fileSize < 84) {
    Settings::Instance()->funcDoLog("[STLParser] Error occured - header is too small!");
    return false;
  }

  int currentModelID = 0;
  MeshModel entityModel;
  entityModel.File = file;
  entityModel.ID = currentModelID;
  entityModel.ModelTitle = file.title;
  entityModel.countVertices = 0;
  entityModel.countTextureCoordinates = 0;
  entityModel.countNormals = 0;
  entityModel.countIndices = 0;

  std::ifstream stl_file_data(file.path.c_str(), std::ios::in | std::ios::binary);

  char header_info[80] = "";
  char n_triangles[4];
  stl_file_data.read(header_info, 80);
  stl_file_data.read(n_triangles, 4);
  //std::string h(header_info);

  unsigned int indicesCounter = 0;
  unsigned int* r = (unsigned int*)n_triangles;
  unsigned int num_triangles = *r;
  int progressCounter = 0;
  unsigned int progressTotal = num_triangles;
  for (unsigned int i = 0; i < num_triangles; i++) {
    glm::vec3 normal = this->parsePoint(stl_file_data);
    glm::vec3 v1 = this->parsePoint(stl_file_data);
    glm::vec3 v2 = this->parsePoint(stl_file_data);
    glm::vec3 v3 = this->parsePoint(stl_file_data);

    normal = this->parserUtils->fixVectorAxis(normal, this->Setting_Axis_Forward, this->Setting_Axis_Up);
    v1 = this->parserUtils->fixVectorAxis(v1, this->Setting_Axis_Forward, this->Setting_Axis_Up);
    v2 = this->parserUtils->fixVectorAxis(v2, this->Setting_Axis_Forward, this->Setting_Axis_Up);
    v3 = this->parserUtils->fixVectorAxis(v3, this->Setting_Axis_Forward, this->Setting_Axis_Up);

    entityModel.normals.push_back(normal);
    entityModel.vertices.push_back(v1);
    entityModel.texture_coordinates.push_back(glm::vec2(0.0f));
    entityModel.indices.push_back(indicesCounter);
    indicesCounter += 1;

    entityModel.normals.push_back(normal);
    entityModel.vertices.push_back(v2);
    entityModel.texture_coordinates.push_back(glm::vec2(0.0f));
    entityModel.indices.push_back(indicesCounter);
    indicesCounter += 1;

    entityModel.normals.push_back(normal);
    entityModel.vertices.push_back(v3);
    entityModel.texture_coordinates.push_back(glm::vec2(0.0f));
    entityModel.indices.push_back(indicesCounter);
    indicesCounter += 1;

    char attrs[2];
    stl_file_data.read(attrs, 2);

    progressCounter += 1;
    float progress = (float(progressCounter) / float(progressTotal)) * 100.0f;
    this->doProgress(progress);
  }

  MeshModelMaterial entityMaterial = {};
  entityMaterial.MaterialID = 1;
  entityMaterial.MaterialTitle.clear();
  entityMaterial.DiffuseColor = glm::vec3(0.7, 0.7, 0.7);
  entityMaterial.Transparency = 1.0f;
  entityMaterial.IlluminationMode = 2;
  entityModel.ModelMaterial = entityMaterial;

  entityModel.countIndices = int(entityModel.indices.size());
  entityModel.countNormals = int(entityModel.normals.size());
  entityModel.countVertices = int(entityModel.vertices.size());

  currentModelID += 1;
  this->models.push_back(entityModel);

  stl_file_data.close();

  return result;
}

bool STLParser::loadAsciiFile(const FBEntity& file) {
  Settings::Instance()->funcDoLog("[STLParser] Parsing ascii STL file...");

  bool result = true;

  int currentModelID = 0;
  MeshModel entityModel;
  entityModel.File = file;
  entityModel.ID = currentModelID;
  entityModel.ModelTitle = file.title;
  entityModel.countVertices = 0;
  entityModel.countTextureCoordinates = 0;
  entityModel.countNormals = 0;
  entityModel.countIndices = 0;

  std::ifstream stl_file_data(file.path.c_str());
  if (!stl_file_data.is_open()) {
    Settings::Instance()->funcDoLog("[STLParser] Cannot open .stl file - " + file.path + "!");
    stl_file_data.close();
    return {};
  }

  unsigned int indicesCounter = 0;
  std::string singleLine;
  glm::vec3 normal;
  while (std::getline(stl_file_data, singleLine)) {
    if (boost::starts_with(singleLine, "facet")) {
      boost::replace_first(singleLine, "facet normal ", "");
      std::stringstream valReader(singleLine);
      valReader >> normal.x >> normal.y >> normal.z;
    }
    else if (boost::starts_with(singleLine, "vertex")) {
      boost::replace_first(singleLine, "vertex ", "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      entityModel.vertices.push_back(v);
      entityModel.normals.push_back(normal);
      entityModel.indices.push_back(indicesCounter);
      indicesCounter += 1;
    }
  }
  stl_file_data.close();

  MeshModelMaterial entityMaterial = {};
  entityMaterial.MaterialID = 1;
  entityMaterial.MaterialTitle.clear();
  entityMaterial.DiffuseColor = glm::vec3(0.7, 0.7, 0.7);
  entityMaterial.Transparency = 1.0f;
  entityMaterial.IlluminationMode = 2;
  entityModel.ModelMaterial = entityMaterial;

  entityModel.countIndices = int(entityModel.indices.size());
  entityModel.countNormals = int(entityModel.normals.size());
  entityModel.countVertices = int(entityModel.vertices.size());

  currentModelID += 1;
  this->models.push_back(entityModel);

  return result;
}
