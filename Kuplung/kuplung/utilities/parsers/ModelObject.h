//
//  ModelObject.h
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/settings/Settings.h"

#ifndef ModelObject_h
#  define ModelObject_h

#  include <glm/glm.hpp>
#  include <string>
#  include <vector>

typedef enum objMaterialImageType {
  objMaterialImageType_Ambient,
  objMaterialImageType_Diffuse,
  objMaterialImageType_Specular,
  objMaterialImageType_SpecularExp,
  objMaterialImageType_Dissolve,
  objMaterialImageType_Bump,
  objMaterialImageType_Displacement
} objMaterialImageType;

struct MeshMaterialTextureImage {
  int Width, Height;
  bool UseTexture;
  char mmtiPad[7]; // not-sure yet
  std::string Filename;
  std::string Image;
  std::vector<std::string> Commands;
};

struct MeshModelMaterial {
  int MaterialID;
  float SpecularExp;
  std::string MaterialTitle;

  glm::vec3 AmbientColor, DiffuseColor, SpecularColor, EmissionColor;
  float Transparency;
  unsigned int IlluminationMode;
  float OpticalDensity;

  MeshMaterialTextureImage TextureAmbient;
  MeshMaterialTextureImage TextureDiffuse;
  MeshMaterialTextureImage TextureSpecular;
  MeshMaterialTextureImage TextureSpecularExp;
  MeshMaterialTextureImage TextureDissolve;
  MeshMaterialTextureImage TextureBump;
  MeshMaterialTextureImage TextureDisplacement;
};

struct MeshModel {
  int ID;
  FBEntity File;
  std::string ModelTitle, MaterialTitle;
  int countVertices, countTextureCoordinates, countNormals, countIndices;

  MeshModelMaterial ModelMaterial;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> texture_coordinates;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;

  const unsigned int dataCountVertices() const noexcept {
    return countVertices * sizeof(glm::vec3);
  }
  const unsigned int dataCountTextureCoordinates() const noexcept {
    return countTextureCoordinates * sizeof(glm::vec2);
  }
  const unsigned int dataCountNormals() const noexcept {
    return countNormals * sizeof(glm::vec3);
  }
  const unsigned int dataCountIndices() const noexcept {
    return countIndices;
  }
  const unsigned int dataCount() const noexcept {
    return dataCountVertices() + dataCountTextureCoordinates() + dataCountNormals() + dataCountIndices();
  }
  const unsigned int dataCountBillboard(const bool includeColors) const noexcept {
    return dataCountVertices() + (includeColors ? dataCountVertices() : 0) + dataCountIndices();
  }
};

void static Kuplung_printObjModels(const std::vector<MeshModel>& models, bool byIndices) {
  for (size_t i = 0; i < models.size(); i++) {
    MeshModel m = models[i];
    printf("model.ID = %i\n", m.ID);
    printf("model.countIndices = %i (%i)\n", m.countIndices, ((*std::max_element(m.indices.begin(), m.indices.end())) + 1));
    printf("model.countNormals = %i (%i)\n", m.countNormals, (m.countNormals * 3));
    printf("model.countTextureCoordinates = %i (%i)\n", m.countTextureCoordinates, (m.countTextureCoordinates * 2));
    printf("model.countVertices = %i (%i)\n", m.countVertices, (m.countVertices * 3));
    printf("model.MaterialTitle = %s\n", m.MaterialTitle.c_str());
    printf("model.ModelTitle = %s\n", m.ModelTitle.c_str());

    if (byIndices) {
      printf("m.geometry :\n");
      for (size_t j = 0; j < m.indices.size(); j++) {
        const size_t idx = m.indices[j];
        std::string geom = Settings::Instance()->string_format("index = %i ---> ", idx);
        const glm::vec3 vert = m.vertices[idx];
        const glm::vec2 tc = m.texture_coordinates[idx];
        const glm::vec3 n = m.normals[idx];
        geom += Settings::Instance()->string_format("vertex = [%g, %g, %g]", vert.x, vert.y, vert.z);
        geom += Settings::Instance()->string_format(", uv = [%g, %g]", tc.x, tc.y);
        geom += Settings::Instance()->string_format(", normal = [%g, %g, %g]", n.x, n.y, n.z);
        //printf("%s\n", geom.c_str());
      }
    }
    else {
      std::string verts;
      for (size_t j = 0; j < m.vertices.size(); j++) {
        const glm::vec3 v = m.vertices[j];
        verts += Settings::Instance()->string_format("[%g, %g, %g], ", v.x, v.y, v.z);
      }
      printf("m.vertices : %s\n", verts.c_str());

      std::string uvs;
      for (size_t j = 0; j < m.texture_coordinates.size(); j++) {
        uvs += Settings::Instance()->string_format("[%g, %g], ", m.texture_coordinates[j].x, m.texture_coordinates[j].y);
      }
      printf("m.texture_coordinates : %s\n", uvs.c_str());

      std::string normals;
      for (size_t j = 0; j < m.normals.size(); j++) {
        const glm::vec3 n = m.normals[j];
        normals += Settings::Instance()->string_format("[%f, %f, %f], ", n.x, n.y, n.z);
      }
      printf("m.normals : %s\n", normals.c_str());

      std::string indices;
      for (size_t j = 0; j < m.indices.size(); j++) {
        indices += Settings::Instance()->string_format("%i, ", m.indices[j]);
      }
      printf("m.indices : %s\n", indices.c_str());
    }

    printf("model.ModelMaterial.MaterialID = %i\n", m.ModelMaterial.MaterialID);
    printf("model.ModelMaterial.MaterialTitle = %s\n", m.ModelMaterial.MaterialTitle.c_str());

    printf("model.ModelMaterial.AmbientColor = %g, %g, %g\n", m.ModelMaterial.AmbientColor.r, m.ModelMaterial.AmbientColor.g, m.ModelMaterial.AmbientColor.b);
    printf("model.ModelMaterial.DiffuseColor = %g, %g, %g\n", m.ModelMaterial.DiffuseColor.r, m.ModelMaterial.DiffuseColor.g, m.ModelMaterial.DiffuseColor.b);
    printf("model.ModelMaterial.SpecularColor = %g, %g, %g\n", m.ModelMaterial.SpecularColor.r, m.ModelMaterial.SpecularColor.g, m.ModelMaterial.SpecularColor.b);
    printf("model.ModelMaterial.EmissionColor = %g, %g, %g\n", m.ModelMaterial.EmissionColor.r, m.ModelMaterial.EmissionColor.g, m.ModelMaterial.EmissionColor.b);

    printf("model.ModelMaterial.SpecularExp = %g\n", m.ModelMaterial.SpecularExp);
    printf("model.ModelMaterial.Transparency = %g\n", m.ModelMaterial.Transparency);
    printf("model.ModelMaterial.OpticalDensity = %g\n", m.ModelMaterial.OpticalDensity);
    printf("model.ModelMaterial.IlluminationMode = %u\n", m.ModelMaterial.IlluminationMode);

    printf("model.ModelMaterial.textures_ambient.Filename = %s\n", m.ModelMaterial.TextureAmbient.Filename.c_str());
    printf("model.ModelMaterial.textures_diffuse.Filename = %s\n", m.ModelMaterial.TextureDiffuse.Filename.c_str());
    printf("model.ModelMaterial.textures_specular.Filename = %s\n", m.ModelMaterial.TextureSpecular.Filename.c_str());
    printf("model.ModelMaterial.textures_specularExp.Filename = %s\n", m.ModelMaterial.TextureSpecularExp.Filename.c_str());
    printf("model.ModelMaterial.textures_dissolve.Filename = %s\n", m.ModelMaterial.TextureDissolve.Filename.c_str());
    printf("model.ModelMaterial.textures_bump.Filename = %s\n", m.ModelMaterial.TextureBump.Filename.c_str());
    printf("model.ModelMaterial.textures_displacement.Filename = %s\n", m.ModelMaterial.TextureDisplacement.Filename.c_str());
  }
  printf("\n--------\n");
}

#endif /* ModelObject_h */
