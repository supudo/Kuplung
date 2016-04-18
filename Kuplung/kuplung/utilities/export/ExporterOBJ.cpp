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
}

void ExporterOBJ::exportToFile(FBEntity file, std::vector<ModelFace*> faces) {
#ifdef _WIN32
    this->nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    this->nlDelimiter = "\r";
#else
    this->nlDelimiter = "\n";
#endif
    this->exportFile = file;
    this->exportGeometry(faces);
    this->exportMaterials(faces);
}

void ExporterOBJ::exportGeometry(std::vector<ModelFace*> faces) {
    std::string fileContents = "# Kuplung v1.0 OBJ File Export" + this->nlDelimiter;
    fileContents += "# http://www.github.com/supudo/kuplung/" + this->nlDelimiter;
    fileContents += "mtllib " + this->exportFile.title + ".mtl" + this->nlDelimiter;

    std::string objFaces = "";
    int faceCounter = 1;
    int normalsCounter = 1;

    for (int i=0; i<(int)faces.size(); i++) {
        ModelFace *mmf = faces[i];
        fileContents += "o " + mmf->oFace.ModelTitle + nlDelimiter;
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec4 tp01 = mmf->matrixProjection * mmf->matrixCamera * mmf->matrixModel * glm::vec4(mmf->oFace.vectors_vertices[j], 1.0);
                glm::vec4 tp02 = mmf->matrixProjection * mmf->matrixCamera * mmf->matrixModel * glm::vec4(mmf->oFace.vectors_vertices[j - 1], 1.0);
                glm::vec4 tp03 = mmf->matrixProjection * mmf->matrixCamera * mmf->matrixModel * glm::vec4(mmf->oFace.vectors_vertices[j - 2], 1.0);
                glm::vec3 tp1 = glm::vec3(tp01.x, -tp01.z, tp01.y);
                glm::vec3 tp2 = glm::vec3(tp02.x, -tp02.z, tp02.y);
                glm::vec3 tp3 = glm::vec3(tp03.x, -tp03.z, tp03.y);

                std::string singleFace;
                fileContents += Settings::Instance()->string_format("v %f %f %f", tp1.x, tp1.y, tp1.z) + this->nlDelimiter;
                singleFace += std::to_string(faceCounter) + "//" + std::to_string(normalsCounter) + " "; faceCounter += 1;
                fileContents += Settings::Instance()->string_format("v %f %f %f", tp2.x, tp2.y, tp2.z) + this->nlDelimiter;
                singleFace += std::to_string(faceCounter) + "//" + std::to_string(normalsCounter) + " "; faceCounter += 1;
                fileContents += Settings::Instance()->string_format("v %f %f %f", tp3.x, tp3.y, tp3.z) + this->nlDelimiter;
                singleFace += std::to_string(faceCounter) + "//" + std::to_string(normalsCounter) + " "; faceCounter += 1;
                objFaces += "f " + singleFace + this->nlDelimiter;
            }
        }
        for (size_t j=0; j<mmf->oFace.vectors_normals.size(); j++) {
            glm::vec3 n = mmf->oFace.vectors_normals[j];
            fileContents += Settings::Instance()->string_format("vn %f %f %f", n.x, n.y, n.z) + this->nlDelimiter;
            normalsCounter += 1;
        }
        fileContents += "usemtl " + mmf->oFace.materialID + this->nlDelimiter;
        fileContents += "s off" + this->nlDelimiter;
        fileContents += objFaces;
    }
    fileContents += this->nlDelimiter;

    if (fileContents != "")
        this->saveFile(fileContents, this->exportFile.path + "/" + this->exportFile.title + ".obj");
}

void ExporterOBJ::exportMaterials(std::vector<ModelFace*> faces) {
    int materialCount = 0;
    std::string fileContents = "# Kuplung MTL File" + this->nlDelimiter;
    fileContents += "# Material Count: " + std::to_string(materialCount) + this->nlDelimiter;
    fileContents += "# http://www.github.com/supudo/kuplung/" + this->nlDelimiter;
    fileContents += this->nlDelimiter;

    for (int i=0; i<(int)faces.size(); i++) {
        ModelFace *mmf = faces[i];
        objMaterial mat = mmf->oFace.faceMaterial;
        fileContents += "newmtl " + mat.materialID + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("Ns %f", mat.specularExp) + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("Ka %f %f %f", mat.ambient.r, mat.ambient.g, mat.ambient.b) + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("Kd %f %f %f", mat.diffuse.r, mat.diffuse.g, mat.diffuse.b) + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("Ks %f %f %f", mat.specular.r, mat.specular.g, mat.specular.b) + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("Ke %f %f %f", mat.emission.r, mat.emission.g, mat.emission.b) + this->nlDelimiter;
        if (mat.opticalDensity >= 0.0f)
            fileContents += Settings::Instance()->string_format("Ni %f", mat.opticalDensity) + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("d %f", mat.transparency) + this->nlDelimiter;
        fileContents += Settings::Instance()->string_format("illum %i", mat.illumination) + this->nlDelimiter;

        if (mat.textures_ambient.image != "")
            fileContents += "map_Ka " + mat.textures_ambient.image + this->nlDelimiter;
        if (mat.textures_diffuse.image != "")
            fileContents += "map_Kd " + mat.textures_diffuse.image + this->nlDelimiter;
        if (mat.textures_dissolve.image != "")
            fileContents += "map_d " + mat.textures_dissolve.image + this->nlDelimiter;
        if (mat.textures_bump.image != "")
            fileContents += "map_Bump " + mat.textures_bump.image + this->nlDelimiter;
        if (mat.textures_displacement.image != "")
            fileContents += "disp " + mat.textures_displacement.image + this->nlDelimiter;
        if (mat.textures_specular.image != "")
            fileContents += "map_Ks " + mat.textures_specular.image + this->nlDelimiter;
        if (mat.textures_specularExp.image != "")
            fileContents += "map_Ns " + mat.textures_specularExp.image + this->nlDelimiter;
    }
    fileContents += this->nlDelimiter;

    if (fileContents != "")
        this->saveFile(fileContents, this->exportFile.path + "/" + this->exportFile.title + ".mtl");
}

void ExporterOBJ::saveFile(std::string fileContents, std::string fileName) {
    printf("--------------------------------------------------------\n");
    printf("%s\n", fileName.c_str());
    printf("%s\n", fileContents.c_str());
    printf("--------------------------------------------------------\n");

    std::ofstream out(fileName);
    out << fileContents;
    out.close();
}
