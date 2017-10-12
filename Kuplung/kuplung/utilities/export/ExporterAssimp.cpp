//
//  ExporterAssimp.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 13/09/17.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ExporterAssimp.hpp"
#include <fstream>
#include <glm/gtx/matrix_decompose.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace KuplungApp { namespace Utilities { namespace Export {

ExporterAssimp::~ExporterAssimp() {
}

void ExporterAssimp::init(const std::function<void(float)>& doProgress) {
    this->funcProgress = doProgress;
    this->addSuffix = false;
#ifdef _WIN32
    this->nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    this->nlDelimiter = "\r";
#else
    this->nlDelimiter = "\n";
#endif

    this->parserUtils = std::make_unique<ParserUtils>();
	this->exporter = std::make_unique<Assimp::Exporter>();
}

void ExporterAssimp::exportToFile(ImportExportFormats exportFormat, const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects) {
    this->exportFile = file;
    this->objSettings = settings;

	aiScene* scene = new aiScene();
	scene->mRootNode = new aiNode();

	scene->mRootNode->mMeshes = new unsigned int[faces.size()];
	scene->mRootNode->mNumMeshes = faces.size();
	scene->mMeshes = new aiMesh*[faces.size()];
	scene->mNumMeshes = faces.size();

	int counterMesh = 0;
	int counterMaterial = 0;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;

		// materials

		scene->mMaterials = new aiMaterial*[1];
		scene->mMaterials[counterMaterial] = nullptr;
		scene->mNumMaterials = 1;
		scene->mMaterials[counterMaterial] = new aiMaterial();

		scene->mMeshes[counterMesh] = nullptr;
		scene->mMeshes[counterMesh] = new aiMesh();
		scene->mMeshes[counterMesh]->mMaterialIndex = 0;

		scene->mRootNode->mMeshes[counterMesh] = 0;
		auto pMesh = scene->mMeshes[counterMesh];

		// geometry

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> indices;

		// indices
		for (int i = 0; i < model.countIndices; i++) {
			indices.push_back(model.indices[i]);
		}

		// vertices
		for (int i = 0; i < model.vertices.size(); i++) {
			//vertices.push_back(model.vertices[model.indices[i]]);
			vertices.push_back(model.vertices[i]);
		}

		// normals
		for (int i = 0; i < model.normals.size(); i++) {
			//normals.push_back(model.normals[model.indices[i]]);
			normals.push_back(model.normals[i]);
		}

		// UVs
		if (model.texture_coordinates.size() > 0) {
			for (int i = 0; i < model.texture_coordinates.size(); i++) {
				uvs.push_back(model.texture_coordinates[i]);
			}
		}

		const auto& vVertices = vertices;

		pMesh->mVertices = new aiVector3D[vVertices.size()];
		pMesh->mNormals = new aiVector3D[vVertices.size()];
		pMesh->mNumVertices = vVertices.size();

		if (model.countTextureCoordinates > 0) {
			pMesh->mTextureCoords[0] = new aiVector3D[vVertices.size()];
			pMesh->mNumUVComponents[0] = vVertices.size();
		}

		int j = 0;
		for (auto itr = vVertices.begin(); itr != vVertices.end(); ++itr)
		{
			pMesh->mVertices[itr - vVertices.begin()] = aiVector3D(vVertices[j].x, vVertices[j].y, vVertices[j].z);
			pMesh->mNormals[itr - vVertices.begin()] = aiVector3D(normals[j].x, normals[j].y, normals[j].z);
			if (model.countTextureCoordinates > 0)
				pMesh->mTextureCoords[0][itr - vVertices.begin()] = aiVector3D(uvs[j].x, uvs[j].y, 0);
			j++;
		}

		pMesh->mFaces = new aiFace[vVertices.size() / 3];
		pMesh->mNumFaces = (unsigned int)(vVertices.size() / 3);

		for (int i = 0; i < model.countIndices / 3; i++) {
			aiFace &a_face = pMesh->mFaces[i];
			a_face.mIndices = new unsigned int[3];
			a_face.mNumIndices = 3;
			a_face.mIndices[0] = model.indices[i + 0];
			a_face.mIndices[1] = model.indices[i + 1];
			a_face.mIndices[2] = model.indices[i + 2];
		}

		counterMesh += 1;
	}

	this->saveFile(exportFormat, scene);
}

void ExporterAssimp::saveFile(ImportExportFormats exportFormat, aiScene* scene) {
	time_t t = time(0);
	const struct tm * now = localtime(&t);

	const int year = now->tm_year + 1900;
	const int month = now->tm_mon + 1;
	const int day = now->tm_mday;
	const int hour = now->tm_hour;
	const int minute = now->tm_min;
	const int seconds = now->tm_sec;

	std::string fileSuffix = "_" +
		std::to_string(year) + std::to_string(month) + std::to_string(day) +
		std::to_string(hour) + std::to_string(minute) + std::to_string(seconds);

	if (!this->addSuffix)
		fileSuffix.clear();
	std::string filePath = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/"));
	std::string fileName = this->exportFile.title;
	
	std::string eFilename(filePath + "/"), eFormat;
	switch (exportFormat) {
		case ImportExportFormat_OBJ: {
			eFilename += fileName + fileSuffix + ".obj";
			eFormat = "obj";
			break;
		}
		case ImportExportFormat_GLTF: {
			eFilename += fileName + fileSuffix + ".gltf";
			eFormat = "gltf2";
			break;
		}
		case ImportExportFormat_PLY: {
			eFilename += fileName + fileSuffix + ".ply";
			eFormat = "ply";
			break;
		}
		case ImportExportFormat_STL: {
			eFilename += fileName + fileSuffix + ".stl";
			eFormat = "stl";
			break;
		}
	}

	this->exporter->Export(scene, eFormat.c_str(), eFilename.c_str(), aiProcess_FlipUVs);
}

}}}
