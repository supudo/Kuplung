//
//  ExporterGLTF.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2017 supudo.net. All rights reserved.
//

#include "ExporterGLTF.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/utilities/cpp-base64/base64.h"
#include <glm/gtx/string_cast.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/endian/conversion.hpp>

namespace KuplungApp { namespace Utilities { namespace Export {

ExporterGLTF::~ExporterGLTF() {
}

void ExporterGLTF::init(const std::function<void(float)>& doProgress) {
    this->funcProgress = doProgress;
    this->addSuffix = false;
#ifdef _WIN32
    this->nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    this->nlDelimiter = "\r";
#else
    this->nlDelimiter = "\n";
#endif

    this->exportFileFolder = "";
    this->parserUtils = std::make_unique<ParserUtils>();
	this->gltfGenerator = "Kuplung (https://github.com/supudo/Kuplung)";
	this->defaultSceneName = "KuplungScene";
	this->defaultMaterialName = "KuplungMaterial";
	this->gltfVersion = "2.0";
	this->BufferInExternalFile = false;
}

void ExporterGLTF::exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects) {
	this->exportFile = file;
	this->objSettings = settings;

	this->prepFolderLocation();

	nlohmann::json j;
	j["asset"] = { { "generator", this->gltfGenerator }, { "version", this->gltfVersion } };
	j["scene"] = 0;
	j["cameras"] = this->exportCameras(managerObjects);

	std::string meshNameSuffix = "_mesh";
	std::string suffix_Indices = "_indices";
	std::string suffix_Vertices = "_vertices";
	std::string suffix_Normals = "_normals";
	std::string suffix_UVs = "_uvs";
	std::string meshAccessorSuffix_Indices = meshNameSuffix + "_accessor" + suffix_Indices;
	std::string meshAccessorSuffix_Vertices = meshNameSuffix + "_accessor" + suffix_Vertices;
	std::string meshAccessorSuffix_Normals = meshNameSuffix + "_accessor" + suffix_Normals;
	std::string meshAccessorSuffix_UVs = meshNameSuffix + "_accessor" + suffix_UVs;
	std::string bufferViewSuffix = "_view";

	nlohmann::json jBufferViews, jAccessors, jBuffers, jMaterials, jNodes, jScenes, jMeshes;
	int totalBufferLength = 0, meshCounter = 0, nodeCounter = 0;
	int currentBufferCounter = 0, currentAccessorCounter = 0, currentMaterialCounter = 0;
	std::string bufferData;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		MeshModelMaterial mat = model.ModelMaterial;
		std::string modelTitleLocal = model.ModelTitle + "_" + std::to_string(meshCounter);

		// ---------------------------------------------------------
		// scenes
		jScenes.push_back(
			{
				{ "nodes", { nodeCounter } }
			}
		);

		// ---------------------------------------------------------
		// nodes
		jNodes.push_back({
			{ "mesh", meshCounter },
			{ "name", modelTitleLocal + meshNameSuffix },
			{ "translation", { face.rotateX->point, face.rotateY->point, face.rotateZ->point } }
		});

		// ---------------------------------------------------------
		// materials
		jMaterials.push_back(
			{
				{ "name", model.MaterialTitle },
				{ "values",
					{
						{ "ambient", { face.materialAmbient->color.r, face.materialAmbient->color.g, face.materialAmbient->color.b, face.Setting_Alpha } },
						{ "diffuse", { face.materialDiffuse->color.r, face.materialDiffuse->color.g, face.materialDiffuse->color.b, face.Setting_Alpha } },
						{ "specular", { face.materialSpecular->color.r, face.materialSpecular->color.g, face.materialSpecular->color.b, face.Setting_Alpha } },
						{ "emission", { face.materialEmission->color.r, face.materialEmission->color.g, face.materialEmission->color.b, face.Setting_Alpha } },
						{ "shininess", 0.0f },
					}
				}
			}
		);

		// ---------------------------------------------------------
		// images
		if (!mat.TextureAmbient.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureAmbient.Image));
		if (!mat.TextureBump.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureBump.Image));
		if (!mat.TextureDiffuse.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureDiffuse.Image));
		if (!mat.TextureDisplacement.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureDisplacement.Image));
		if (!mat.TextureDissolve.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureDissolve.Image));
		if (!mat.TextureSpecular.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureSpecular.Image));
		if (!mat.TextureSpecularExp.Image.empty())
			j["images"].push_back(this->copyImage(mat.TextureSpecularExp.Image));

		// ---------------------------------------------------------
		// buffers
		int base64_size_indices = model.indices.size() * sizeof(unsigned int);
		int base64_size_vertices = model.vertices.size() * sizeof(glm::vec3);
		int base64_size_normals = model.normals.size() * sizeof(glm::vec3);
		int base64_size_textureCoordinates = model.texture_coordinates.size() * sizeof(glm::vec2);
		std::string base64_indices = base64_encode(reinterpret_cast<unsigned char const *>(model.indices.data()), base64_size_indices);
		std::string base64_vertices = base64_encode(reinterpret_cast<unsigned char const *>(model.vertices.data()), base64_size_vertices);
		std::string base64_normals = base64_encode(reinterpret_cast<unsigned char const *>(model.normals.data()), base64_size_normals);
		std::string base64_textureCoordinates = base64_encode(reinterpret_cast<unsigned char const *>(model.texture_coordinates.data()), base64_size_textureCoordinates);

		// ---------------------------------------------------------
		// buffer views
		// indices
		jBufferViews.push_back(
			{
				{ "name", modelTitleLocal + bufferViewSuffix + suffix_Indices },
				{ "buffer", 0 },
				{ "byteOffset", totalBufferLength},
				{ "byteLength", model.indices.size() },
				{ "target", 34963 }
			}
		);
		// vertices
		jBufferViews.push_back(
			{
				{ "name", modelTitleLocal + bufferViewSuffix + suffix_Vertices },
				{ "buffer", 0 },
				{ "byteOffset", totalBufferLength + model.indices.size() + 1 },
				{ "byteLength", model.vertices.size() * 3 * sizeof(float) },
				{ "target", 34962 }
			}
		);
		// normals
		jBufferViews.push_back(
			{
				{ "name", modelTitleLocal + bufferViewSuffix + suffix_Normals },
				{ "buffer", 0 },
				{ "byteOffset", totalBufferLength + model.indices.size() + (model.vertices.size() * 3 * sizeof(float)) + 1 },
				{ "byteLength", model.normals.size() * 3 * sizeof(float) },
				{ "target", 34962 }
			}
		);
		// uvs
		if (model.texture_coordinates.size() > 0)
			jBufferViews.push_back(
				{
					{ "name", modelTitleLocal + bufferViewSuffix + suffix_Normals },
					{ "buffer", 0 },
					{ "byteOffset", totalBufferLength + model.indices.size() + (model.vertices.size() * 3 * sizeof(float)) + (model.normals.size() * 3 * sizeof(float)) + 1 },
					{ "byteLength", model.texture_coordinates.size() },
					{ "target", "34962" }
				}
			);

		// ---------------------------------------------------------
		// accessors
		// indices
		jAccessors.push_back(
			{
				{ "name", modelTitleLocal + meshAccessorSuffix_Indices },
				{ "bufferView", currentBufferCounter },
				{ "byteOffset", 0 },
				{ "componentType", 5125 },
				{ "count", model.indices.size() },
				{ "type", "SCALAR" },
				{ "min", { *std::min_element(model.indices.begin(), model.indices.end()) } },
				{ "max", { *std::max_element(model.indices.begin(), model.indices.end()) } }
			}
		);
		currentBufferCounter += 1;
		// vertices
		float min_vertex_x = (*std::min_element(model.vertices.begin(), model.vertices.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.x < b.x); })).x;
		float min_vertex_y = (*std::min_element(model.vertices.begin(), model.vertices.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.y < b.y); })).y;
		float min_vertex_z = (*std::min_element(model.vertices.begin(), model.vertices.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.z < b.z); })).z;
		float max_vertex_x = (*std::max_element(model.vertices.begin(), model.vertices.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.x > b.x); })).x;
		float max_vertex_y = (*std::max_element(model.vertices.begin(), model.vertices.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.y > b.y); })).y;
		float max_vertex_z = (*std::max_element(model.vertices.begin(), model.vertices.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.z > b.z); })).z;
		jAccessors.push_back(
			{
				{ "name", modelTitleLocal + meshAccessorSuffix_Vertices },
				{ "bufferView", currentBufferCounter },
				{ "byteOffset", 0 },
				{ "componentType", 5126 },
				{ "count", model.vertices.size() },
				{ "type", "VEC3" },
				{ "min", { min_vertex_x, min_vertex_y, min_vertex_z } },
				{ "max", { max_vertex_x, max_vertex_y, max_vertex_z } }
			}
		);
		currentBufferCounter += 1;
		// normals
		float min_normal_x = (*std::min_element(model.normals.begin(), model.normals.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.x < b.x); })).x;
		float min_normal_y = (*std::min_element(model.normals.begin(), model.normals.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.y < b.y); })).y;
		float min_normal_z = (*std::min_element(model.normals.begin(), model.normals.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.z < b.z); })).z;
		float max_normal_x = (*std::max_element(model.normals.begin(), model.normals.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.x > b.x); })).x;
		float max_normal_y = (*std::max_element(model.normals.begin(), model.normals.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.y > b.y); })).y;
		float max_normal_z = (*std::max_element(model.normals.begin(), model.normals.end(), [](const glm::vec3& a, const glm::vec3& b) { return (a.z > b.z); })).z;
		jAccessors.push_back(
			{
				{ "name", modelTitleLocal + meshAccessorSuffix_Normals },
				{ "bufferView", currentBufferCounter },
				{ "byteOffset", 0 },
				{ "componentType", 5126 },
				{ "count", model.normals.size() },
				{ "type", "VEC3" },
				{ "min", { min_normal_x, min_normal_y, min_normal_z } },
				{ "max", { max_normal_x, max_normal_y, max_normal_z } }
			}
		);
		currentBufferCounter += 1;
		// uvs
		if (model.texture_coordinates.size() > 0) {
			float min_uv_s = (*std::min_element(model.texture_coordinates.begin(), model.texture_coordinates.end(), [](const glm::vec2& a, const glm::vec2& b) { return (a.s < b.s); })).s;
			float min_uv_t = (*std::min_element(model.texture_coordinates.begin(), model.texture_coordinates.end(), [](const glm::vec2& a, const glm::vec2& b) { return (a.t < b.t); })).t;
			float max_uv_s = (*std::max_element(model.texture_coordinates.begin(), model.texture_coordinates.end(), [](const glm::vec2& a, const glm::vec2& b) { return (a.s > b.s); })).s;
			float max_uv_t = (*std::max_element(model.texture_coordinates.begin(), model.texture_coordinates.end(), [](const glm::vec2& a, const glm::vec2& b) { return (a.t > b.t); })).t;
			jAccessors.push_back(
				{
					{ "name", modelTitleLocal + meshAccessorSuffix_UVs },
					{ "bufferView", currentBufferCounter },
					{ "byteOffset", 0 },
					{ "componentType", 5126 },
					{ "count", model.texture_coordinates.size() },
					{ "type", "VEC2" },
					{ "min", { min_uv_s, min_uv_t } },
					{ "max", { max_uv_s, max_uv_t } }
				}
			);
			currentBufferCounter += 1;
		}

		// ---------------------------------------------------------
		// misc
		totalBufferLength += model.indices.size();
		totalBufferLength += model.vertices.size() * 3 * sizeof(float);
		totalBufferLength += model.normals.size() * 3 * sizeof(float);
		totalBufferLength += model.texture_coordinates.size() * 2 * sizeof(float);
		totalBufferLength += 1;
		bufferData += base64_indices + base64_vertices + base64_normals + base64_textureCoordinates;

		if (!this->BufferInExternalFile) {
			jBuffers.push_back(
				{
					{ "name", this->defaultSceneName + "_buffer" },
					{ "byteLength", totalBufferLength },
					{ "uri", "data:application/octet-stream;base64," + bufferData }
				}
			);
		}

		// ---------------------------------------------------------
		// meshes
		nlohmann::json jMeshPrimitives;
		if (model.texture_coordinates.size() > 0) {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", currentMaterialCounter },
				{ "indices", currentAccessorCounter + 0 },
				{ "attributes",
				{
					{ "POSITION", currentAccessorCounter + 1 },
					{ "NORMAL", currentAccessorCounter + 2 },
					{ "TEXCOORD_0", currentAccessorCounter + 3 }
				}
				}
			};
			currentAccessorCounter += 3;
		}
		else {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", currentMaterialCounter },
				{ "indices", currentAccessorCounter + 0 },
				{ "attributes",
				{
					{ "POSITION", currentAccessorCounter + 1 },
					{ "NORMAL", currentAccessorCounter + 2 }
				}
				}
			};
			currentAccessorCounter += 2;
		}
		currentMaterialCounter += 1;
		jMeshes.push_back({ { "name", modelTitleLocal + meshNameSuffix },{ "primitives", jMeshPrimitives } });

		meshCounter += 1;
		nodeCounter += 1;
	}

	j["scenes"] = jScenes;
	j["buffers"] = jBuffers;
	j["bufferViews"] = jBufferViews;
	j["accessors"] = jAccessors;
	j["materials"] = jMaterials;
	j["nodes"] = jNodes;
	j["meshes"] = jMeshes;

	if (this->BufferInExternalFile) {
		j["buffers"]["byteLength"] = totalBufferLength;
		j["buffers"]["uri"] = this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin";
		if (!this->saveBufferFile(bufferData))
			Settings::Instance()->funcDoLog("[Kuplung] Error occured while writing the BIN glTF file!");
	}

    if (!this->saveFile(j))
		Settings::Instance()->funcDoLog("[Kuplung] Could not save glTF file!");
}

nlohmann::json ExporterGLTF::exportCameras(std::unique_ptr<ObjectsManager> &managerObjects) {
    nlohmann::json j;
    nlohmann::json viewportCamera;
    viewportCamera["type"] = "perspective";
    viewportCamera["perspective"] = {
        { "aspectRatio", managerObjects->Setting_RatioWidth / managerObjects->Setting_RatioHeight },
        { "yfov", managerObjects->Setting_FOV },
        { "zfar", managerObjects->Setting_PlaneFar },
        { "znear", managerObjects->Setting_PlaneClose }
    };
    j.push_back(viewportCamera);
    return j;
}

nlohmann::json ExporterGLTF::exportScenes(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		j[this->defaultSceneName]["nodes"][faceIterator - faces.begin()] = model.ModelTitle;
	}
    return j;
}

nlohmann::json ExporterGLTF::copyImage(std::string imagePath) {
    nlohmann::json j;
    std::string imageFilename = imagePath.substr(imagePath.find_last_of("\\/"));
    boost::replace_all(imageFilename, "/", "");
    std::string newImagePath = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/")) + "/" + this->exportFile.title + "/" + imageFilename;
    boost::filesystem::copy_file(imagePath, newImagePath, boost::filesystem::copy_option::overwrite_if_exists);
    j["uri"] = imageFilename;
    return j;
}

void ExporterGLTF::prepFolderLocation() {
    std::string folder = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/"));
    std::string newFolder = folder + "/" + this->exportFile.title;
    if (!boost::filesystem::exists(newFolder)) {
        boost::filesystem::path dir(newFolder);
        if (!boost::filesystem::create_directory(dir))
            Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[ExportGLTF] Cannot create destination folder : %s!", newFolder.c_str()));
    }
    this->exportFileFolder = newFolder;
}

bool ExporterGLTF::saveFile(const nlohmann::json& jsonObj) {
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
    std::string filePath = this->exportFileFolder;
    std::string fileName = this->exportFile.title;
    if (boost::algorithm::ends_with(fileName, ".gltf"))
        fileName = fileName.substr(0, fileName.size() - 4);

    std::ofstream out(filePath + "/" + fileName + fileSuffix + ".gltf");
    out << std::setw(4) << jsonObj << std::endl;
    out.close();

	return true;
}

bool ExporterGLTF::saveBufferFile(std::string buffer) {
	std::string file = this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin";

	auto f = fopen(file.c_str(), "wt");
	if (!f) {
		Settings::Instance()->funcDoLog("[Kuplung] Could not create glTF binary file!");
		return false;
	}

	uint32_t buffer_length = buffer.size();

	// 12-byte header
	// - magic
	uint32_t magic = 0x46546C67;
	if (!std::fwrite(&magic, 1, 1, f))
		return false;

	// - version
	uint32_t version = 2;
	if (!std::fwrite(&version, 1, 1, f))
		return false;

	// - length
	uint32_t length = 12 + buffer_length;
	if (!std::fwrite(&length, 1, 1, f))
		return false;

	if (!std::fwrite(&buffer, buffer_length, 1, f))
		return false;

	std::fclose(f);

	return true;
}

}}}
