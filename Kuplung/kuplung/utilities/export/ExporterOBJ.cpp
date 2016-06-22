//
//  ExporterOBJ.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ExporterOBJ.hpp"
#include <fstream>
#include <glm/gtx/matrix_decompose.hpp>

ExporterOBJ::~ExporterOBJ() {
    this->destroy();
}

void ExporterOBJ::destroy() {
}

void ExporterOBJ::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
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

std::string ExporterOBJ::exportMesh(ModelFace *face) {
    MeshModel model = face->meshModel;
    std::string meshData = "";
    std::string v(""), vt(""), vn(""), f("");

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(face->matrixModel, scale, rotation, translation, skew, perspective);

    this->funcProgress(0.0f);
    int totalProgress = int(model.indices.size()) * 2;
    float progressCounter = 0.0f;

    meshData += this->nlDelimiter;
    meshData += "o " + model.ModelTitle + this->nlDelimiter;
    for (size_t j=0; j<model.indices.size(); j++) {
        int idx = model.indices[j];
        glm::vec3 vertex = model.vertices[idx];
        vertex += glm::vec3(face->positionX->point, face->positionY->point, face->positionZ->point);
        vertex = vertex * rotation;
        vertex = vertex * scale;

        glm::vec2 texture_coordinate;
        if (model.texture_coordinates.size() > 0)
            texture_coordinate = model.texture_coordinates[idx];
        glm::vec3 normal = model.normals[idx];

        if (find(this->uniqueVertices.begin(), this->uniqueVertices.end(), vertex) == this->uniqueVertices.end()) {
            this->uniqueVertices.push_back(vertex);
            v += "v " + std::to_string(vertex.x) + " " + std::to_string(vertex.y) + " " + std::to_string(vertex.z) + this->nlDelimiter;
        }

        if (model.texture_coordinates.size() && find(this->uniqueTextureCoordinates.begin(), this->uniqueTextureCoordinates.end(), texture_coordinate) == this->uniqueTextureCoordinates.end()) {
            this->uniqueTextureCoordinates.push_back(texture_coordinate);
            vt += "vt " + std::to_string(texture_coordinate.x) + " " + std::to_string(texture_coordinate.y) + this->nlDelimiter;
        }

        if (find(this->uniqueNormals.begin(), this->uniqueNormals.end(), normal) == this->uniqueNormals.end()) {
            this->uniqueNormals.push_back(normal);
            vn += "vn " + std::to_string(normal.x) + " " + std::to_string(normal.y) + " " + std::to_string(normal.z) + this->nlDelimiter;
        }

        progressCounter += 1;

        float progress = (progressCounter / float(totalProgress)) * 100.0f;
        this->funcProgress(progress);
    }

    meshData += v;
    meshData += vt;
    meshData += vn;

    std::string triangleFace = "";
    for (size_t k=0, vCounter = 1; k<model.indices.size(); k++, vCounter++) {
        int j = model.indices[(int)k];

        glm::vec3 vertex = model.vertices[j];
        vertex += glm::vec3(face->positionX->point, face->positionY->point, face->positionZ->point);
        vertex = vertex * rotation;
        vertex = vertex * scale;

        long v = find(this->uniqueVertices.begin(), this->uniqueVertices.end(), vertex) - this->uniqueVertices.begin() + 1;
        long vn = find(this->uniqueNormals.begin(), this->uniqueNormals.end(), model.normals[j]) - this->uniqueNormals.begin() + 1;

        long vt = -1;
        if (model.texture_coordinates.size() > 0)
            vt = find(this->uniqueTextureCoordinates.begin(), this->uniqueTextureCoordinates.end(), model.texture_coordinates[j]) - this->uniqueTextureCoordinates.begin();

        triangleFace += " " + std::to_string(v) + "/" + (vt > -1 ? std::to_string(vt + 1) : "") + "/" + std::to_string(vn);

        if (vCounter % 3 == 0) {
            f += "f" + triangleFace + this->nlDelimiter;
            triangleFace = "";
        }

        progressCounter += 1;

        float progress = (progressCounter / float(totalProgress)) * 100.0f;
        this->funcProgress(progress);
    }

    meshData += "usemtl " + model.MaterialTitle + this->nlDelimiter;
    meshData += "s off" + this->nlDelimiter;
    meshData += f;

    return meshData;
}

void ExporterOBJ::exportGeometry(std::vector<ModelFace*> faces) {
    std::string fileContents = "# Kuplung v1.0 OBJ File Export" + this->nlDelimiter;
    fileContents += "# http://www.github.com/supudo/kuplung/" + this->nlDelimiter;
    fileContents += "mtllib " + this->exportFile.title + ".mtl" + this->nlDelimiter;

    this->uniqueVertices.clear();
    this->uniqueTextureCoordinates.clear();
    this->uniqueNormals.clear();
    this->uniqueVertices.clear();
    this->uniqueTextureCoordinates.clear();
    this->uniqueNormals.clear();
    this->vCounter = 1;
    this->vtCounter = 1;
    this->vnCounter = 1;

    for (size_t i=0; i<faces.size(); i++) {
        fileContents += this->exportMesh(faces[i]);
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
