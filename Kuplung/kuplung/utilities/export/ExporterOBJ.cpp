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

void ExporterOBJ::exportGeometry(std::vector<ModelFace*> faces) {
    std::string fileContents = "# Kuplung v1.0 OBJ File Export" + this->nlDelimiter;
    fileContents += "# http://www.github.com/supudo/kuplung/" + this->nlDelimiter;
    fileContents += "mtllib " + this->exportFile.title + ".mtl" + this->nlDelimiter;

    std::string objFaces = "";
    std::string objNormals = "";
    int triangleCounter = 0, pointCounter = 0;
    int indexVertex = 0, indexNormal = 0;
    std::vector<glm::vec3> vectorVertices;
    for (int i=0; i<(int)faces.size(); i++) {
        MeshModel model = faces[i]->meshModel;
        fileContents += "o " + model.ModelTitle + nlDelimiter;
        objFaces = "";
        for (size_t j=0; j<model.vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                float v1 = model.vertices[j].x;
                float v2 = model.vertices[j].y;
                float v3 = model.vertices[j].z;
                fileContents += Settings::Instance()->string_format("v %f %f %f", v1, v2, v3) + this->nlDelimiter;
                vectorVertices.push_back(glm::vec3(v1, v2, v3));
                pointCounter += 1;
                indexVertex += 1;
                if ((triangleCounter + 1) % 3 == 0) {
                    glm::vec3 p1 = vectorVertices[pointCounter];
                    glm::vec3 p2 = vectorVertices[pointCounter - 1];
                    glm::vec3 p3 = vectorVertices[pointCounter - 2];
                    triangleCounter = 0;

                    glm::vec3 n = glm::normalize(glm::cross(p3 - p1, p2 - p1));
                    objNormals += Settings::Instance()->string_format("vn %f %f %f", n.x, n.y, n.z) + this->nlDelimiter;
                    indexNormal += 1;

                    std::string singleFace = "f";
                    singleFace += Settings::Instance()->string_format(" %i//%i", indexVertex - 2, indexNormal);
                    singleFace += Settings::Instance()->string_format(" %i//%i", indexVertex - 1, indexNormal);
                    singleFace += Settings::Instance()->string_format(" %i//%i", indexVertex, indexNormal);
                    objFaces += singleFace + this->nlDelimiter;
                }
                triangleCounter += 1;
            }
        }
        fileContents += objNormals;
        fileContents += "usemtl " + model.MaterialTitle + this->nlDelimiter;
        fileContents += "s off" + this->nlDelimiter;
        fileContents += objFaces;
    }


    // method 2
//    std::string objFaces = "";
//    std::string objNormals = "";
//    int faceCounter = 1;
//    int normalsCounter = 1;

//    for (int i=0; i<(int)faces.size(); i++) {
//        ModelFace *mmf = faces[i];
//        fileContents += "o " + mmf->oFace.ModelTitle + nlDelimiter;

//        for (size_t j=0; j<mmf->oFace.vertices.size(); j++) {
//            if ((j + 1) % 3 == 0 || (mmf->oFace.vertices.size() - j) < 3) {
//                float v1 = mmf->oFace.vertices[j];
//                float v2 = mmf->oFace.vertices[j - 1];
//                float v3 = mmf->oFace.vertices[j - 2];
//                fileContents += Settings::Instance()->string_format("v %f %f %f", v1, v2, v3) + this->nlDelimiter;

//                glm::vec3 n = glm::normalize(glm::cross(v3 - v1, v2 - v1));
//                objNormals += Settings::Instance()->string_format("vn %f %f %f", n.x, n.y, n.z) + this->nlDelimiter;
//                normalsCounter += 1;

//                faceCounter += 1;
//            }
//        }

//        fileContents += "usemtl " + mmf->oFace.materialID + this->nlDelimiter;
//        fileContents += "s off" + this->nlDelimiter;
//        fileContents += objFaces;
//    }



    // method 1
//    for (int i=0; i<(int)faces.size(); i++) {
//        ModelFace *mmf = faces[i];
//        fileContents += "o " + mmf->oFace.ModelTitle + nlDelimiter;
//        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
//            glm::vec3 v = mmf->oFace.vectors_vertices[j];
//            printf("[%i] - %f, %f, %f\n", (int)j, v.x, v.y, v.z);
//            if ((j + 1) % 3 == 0) {
//                glm::vec4 tp01 = mmf->matrixModel * glm::vec4(mmf->oFace.vectors_vertices[j], 1.0);
//                glm::vec4 tp02 = mmf->matrixModel * glm::vec4(mmf->oFace.vectors_vertices[j - 1], 1.0);
//                glm::vec4 tp03 = mmf->matrixModel * glm::vec4(mmf->oFace.vectors_vertices[j - 2], 1.0);
//                printf("-- [%i] - %f, %f, %f\n", (int)j, tp01.x, tp01.y, tp01.z);
//                printf("-- [%i] - %f, %f, %f\n", (int)j, tp02.x, tp02.y, tp02.z);
//                printf("-- [%i] - %f, %f, %f\n", (int)j, tp03.x, tp03.y, tp03.z);
//                glm::vec3 tp1 = glm::vec3(tp01.x, -tp01.z, tp01.y);
//                glm::vec3 tp2 = glm::vec3(tp02.x, -tp02.z, tp02.y);
//                glm::vec3 tp3 = glm::vec3(tp03.x, -tp03.z, tp03.y);

//                std::string singleFace;
//                fileContents += Settings::Instance()->string_format("v %f %f %f", tp1.x, tp1.y, tp1.z) + this->nlDelimiter;
//                singleFace += std::to_string(faceCounter) + "//" + std::to_string(normalsCounter) + " "; faceCounter += 1;
//                fileContents += Settings::Instance()->string_format("v %f %f %f", tp2.x, tp2.y, tp2.z) + this->nlDelimiter;
//                singleFace += std::to_string(faceCounter) + "//" + std::to_string(normalsCounter) + " "; faceCounter += 1;
//                fileContents += Settings::Instance()->string_format("v %f %f %f", tp3.x, tp3.y, tp3.z) + this->nlDelimiter;
//                singleFace += std::to_string(faceCounter) + "//" + std::to_string(normalsCounter) + " "; faceCounter += 1;
//                objFaces += "f " + singleFace + this->nlDelimiter;
//            }
//        }
//        for (size_t j=0; j<mmf->oFace.vectors_normals.size(); j++) {
//            glm::vec3 n = mmf->oFace.vectors_normals[j];
//            fileContents += Settings::Instance()->string_format("vn %f %f %f", n.x, n.y, n.z) + this->nlDelimiter;
//            normalsCounter += 1;
//        }
//        fileContents += "usemtl " + mmf->oFace.materialID + this->nlDelimiter;
//        fileContents += "s off" + this->nlDelimiter;
//        fileContents += objFaces;
//    }
    fileContents += this->nlDelimiter;

    if (fileContents != "")
        this->saveFile(fileContents, this->exportFile.path + "/" + this->exportFile.title + ".obj");
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
