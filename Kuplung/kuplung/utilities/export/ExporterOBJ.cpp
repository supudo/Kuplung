//
//  ExporterOBJ.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ExporterOBJ.hpp"
#include <fstream>

ExporterOBJ::~ExporterOBJ() {
    this->destroy();
}

void ExporterOBJ::destroy() {
}

void ExporterOBJ::init() {
#ifdef _WIN32
    this->nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    this->nlDelimiter = "\r";
#else
    this->nlDelimiter = "\n";
#endif
}

void ExporterOBJ::exportToFile(FBEntity file, std::vector<ModelFace*> faces) {
    this->exportFile = file;
    this->exportGeometry(faces);
    this->exportMaterials(faces);
}

int ExporterOBJ::findInMap3(std::map<int, glm::vec3> m, glm::vec3 v) {
    std::map<int, glm::vec3>::const_iterator it;
    int c = 0;
    for (it = m.begin(); it != m.end(); ++it) {
        if (it->second == v)
            return c;
        c += 1;
    }
    return -1;
}

int ExporterOBJ::findInMap2(std::map<int, glm::vec2> m, glm::vec2 v) {
    std::map<int, glm::vec2>::const_iterator it;
    int c = 0;
    for (it = m.begin(); it != m.end(); ++it) {
        if (it->second == v)
            return c;
        c += 1;
    }
    return -1;
}

void ExporterOBJ::exportGeometry(std::vector<ModelFace*> faces) {
    std::string fileContents = "# Kuplung v1.0 OBJ File Export" + this->nlDelimiter;
    fileContents += "# http://www.github.com/supudo/kuplung/" + this->nlDelimiter;
    fileContents += "mtllib " + this->exportFile.title + ".mtl" + this->nlDelimiter;

    std::string v(""), vt(""), vn(""), f("");

    std::map<int, glm::vec3> uniqueVertices;
    std::map<int, glm::vec2> uniqueTextureCoordinates;
    std::map<int, glm::vec3> uniqueNormals;
    int vCounter = 1, vtCounter = 1, vnCounter = 1;

    for (size_t i=0; i<faces.size(); i++) {
        MeshModel model = faces[i]->meshModel;
        fileContents += this->nlDelimiter;
        fileContents += "o " + model.ModelTitle + this->nlDelimiter;
        for (size_t j=0; j<model.indices.size(); j++) {
            int idx = model.indices[j];
            glm::vec3 vertex = model.vertices[idx];
            glm::vec2 texture_coordinate;
            if (model.texture_coordinates.size() > 0)
                texture_coordinate = model.texture_coordinates[idx];
            glm::vec3 normal = model.normals[idx];

            if (this->findInMap3(uniqueVertices, vertex) == -1) {
                uniqueVertices[vCounter] = vertex;
                vCounter += 1;
                v += "v " + std::to_string(vertex.x) + " " + std::to_string(vertex.y) + " " + std::to_string(vertex.z) + this->nlDelimiter;
            }

            if (model.texture_coordinates.size() > 0 && this->findInMap2(uniqueTextureCoordinates, texture_coordinate) == -1) {
                uniqueTextureCoordinates[vtCounter] = texture_coordinate;
                vtCounter += 1;
                vt += "vt " + std::to_string(texture_coordinate.x) + " " + std::to_string(texture_coordinate.y) + this->nlDelimiter;
            }

            if (this->findInMap3(uniqueNormals, normal) == -1) {
                uniqueNormals[vnCounter] = normal;
                vnCounter += 1;
                vn += "vn " + std::to_string(normal.x) + " " + std::to_string(normal.y) + " " + std::to_string(normal.z) + this->nlDelimiter;
            }
        }

        fileContents += v;
        fileContents += vt;
        fileContents += vn;
    }

    for (size_t i=0; i<faces.size(); i++) {
        MeshModel model = faces[i]->meshModel;
        std::string triangleFace = "";
        for (size_t k=0, vCounter = 1; k<model.indices.size(); k++, vCounter++) {
            int j = model.indices[(int)k];

            int v = this->findInMap3(uniqueVertices, model.vertices[j]) + 1;
            int vn = this->findInMap3(uniqueNormals, model.normals[j]) + 1;

            int vt = -1;
            if (model.texture_coordinates.size() > 0)
                vt = this->findInMap2(uniqueTextureCoordinates, model.texture_coordinates[j]);

            triangleFace += " " + std::to_string(v) + "/" + (vt > -1 ? std::to_string(vt + 1) : "") + "/" + std::to_string(vn);

            if (vCounter % 3 == 0) {
                f += "f" + triangleFace + this->nlDelimiter;
                triangleFace = "";
            }
        }

        fileContents += "usemtl " + model.MaterialTitle + this->nlDelimiter;
        fileContents += "s off" + this->nlDelimiter;
        fileContents += f;
    }
    fileContents += this->nlDelimiter;

    if (fileContents != "") {
        time_t t = time(0);
        struct tm * now = localtime(&t);

        int year = now->tm_year + 1900;
        int month = now->tm_mon + 1;
        int day = now->tm_mday;
        int hour = now->tm_hour;
        int minute = now->tm_min;
        int seconds = now->tm_sec;

        std::string fileSuffix = "_" +
                                 std::to_string(year) + std::to_string(month) + std::to_string(day) +
                                 std::to_string(hour) + std::to_string(minute) + std::to_string(seconds);

        fileSuffix = "";
        this->saveFile(fileContents, this->exportFile.path + "/" + this->exportFile.title + fileSuffix + ".obj");
    }
}

void ExporterOBJ::exportMaterials(std::vector<ModelFace*> faces) {
    std::map<std::string, std::string> materials;
    for (int i=0; i<(int)faces.size(); i++) {
        MeshModelMaterial mat = faces[i]->meshModel.ModelMaterial;
        if (materials[mat.MaterialTitle] == "") {
            materials[mat.MaterialTitle] = this->nlDelimiter;
            materials[mat.MaterialTitle] += "newmtl " + mat.MaterialTitle + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("Ns %f", mat.SpecularExp) + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("Ka %f %f %f", mat.AmbientColor.r, mat.AmbientColor.g, mat.AmbientColor.b) + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("Kd %f %f %f", mat.DiffuseColor.r, mat.DiffuseColor.g, mat.DiffuseColor.b) + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("Ks %f %f %f", mat.SpecularColor.r, mat.SpecularColor.g, mat.SpecularColor.b) + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("Ke %f %f %f", mat.EmissionColor.r, mat.EmissionColor.g, mat.EmissionColor.b) + this->nlDelimiter;
            if (mat.OpticalDensity >= 0.0f)
                materials[mat.MaterialTitle] += Settings::Instance()->string_format("Ni %f", mat.OpticalDensity) + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("d %f", mat.Transparency) + this->nlDelimiter;
            materials[mat.MaterialTitle] += Settings::Instance()->string_format("illum %i", mat.IlluminationMode) + this->nlDelimiter;

            if (mat.TextureAmbient.Image != "")
                materials[mat.MaterialTitle] += "map_Ka " + mat.TextureAmbient.Image + this->nlDelimiter;
            if (mat.TextureDiffuse.Image != "")
                materials[mat.MaterialTitle] += "map_Kd " + mat.TextureDiffuse.Image + this->nlDelimiter;
            if (mat.TextureDissolve.Image != "")
                materials[mat.MaterialTitle] += "map_d " + mat.TextureDissolve.Image + this->nlDelimiter;
            if (mat.TextureBump.Image != "")
                materials[mat.MaterialTitle] += "map_Bump " + mat.TextureBump.Image + this->nlDelimiter;
            if (mat.TextureDisplacement.Image != "")
                materials[mat.MaterialTitle] += "disp " + mat.TextureDisplacement.Image + this->nlDelimiter;
            if (mat.TextureSpecular.Image != "")
                materials[mat.MaterialTitle] += "map_Ks " + mat.TextureSpecular.Image + this->nlDelimiter;
            if (mat.TextureSpecularExp.Image != "")
                materials[mat.MaterialTitle] += "map_Ns " + mat.TextureSpecularExp.Image + this->nlDelimiter;
        }
    }

    std::string fileContents = "# Kuplung MTL File" + this->nlDelimiter;
    fileContents += "# Material Count: " + std::to_string((int)materials.size()) + this->nlDelimiter;
    fileContents += "# http://www.github.com/supudo/kuplung/" + this->nlDelimiter;

    for (std::map<std::string, std::string>::iterator iter = materials.begin(); iter != materials.end(); ++iter) {
        fileContents += iter->second;
    }

    fileContents += this->nlDelimiter;

    if (fileContents != "")
        this->saveFile(fileContents, this->exportFile.path + "/" + this->exportFile.title + ".mtl");
}

void ExporterOBJ::saveFile(std::string fileContents, std::string fileName) {
//    printf("--------------------------------------------------------\n");
//    printf("%s\n", fileName.c_str());
//    printf("%s\n", fileContents.c_str());
//    printf("--------------------------------------------------------\n");

    std::ofstream out(fileName);
    out << fileContents;
    out.close();
}
