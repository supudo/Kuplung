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
	j["scene"] = this->defaultSceneName;
	//j["scenes"] = this->exportScenes(faces);
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

	nlohmann::json jBufferViews;
	int totalBufferLength = 0, meshCounter = 0, nodeCounter = 0;
	std::string bufferData;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		MeshModelMaterial mat = model.ModelMaterial;
		std::string modelTitleLocal = model.ModelTitle + "_" + std::to_string(meshCounter);

		// ---------------------------------------------------------
		// scenes
		j["scenes"][this->defaultSceneName]["nodes"].push_back(nodeCounter);

		// ---------------------------------------------------------
		// nodes
		j["nodes"].push_back({
			{ "mesh", meshCounter },
			{ "name", modelTitleLocal + meshNameSuffix },
			{ "translation", { face.rotateX->point, face.rotateY->point, face.rotateZ->point } }
		});
		
		// ---------------------------------------------------------
		// materials
		j["materials"][model.MaterialTitle]["value"]["ambient"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j["materials"][model.MaterialTitle]["value"]["diffuse"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j["materials"][model.MaterialTitle]["value"]["specular"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j["materials"][model.MaterialTitle]["value"]["emission"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j["materials"][model.MaterialTitle]["value"]["shininess"] = 0.0f;

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
		// meshes
		nlohmann::json jMeshPrimitives;
		if (model.texture_coordinates.size() > 0) {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", model.MaterialTitle },
				{ "indices", modelTitleLocal + meshAccessorSuffix_Indices },
				{ "attributes",
					{
						{ "POSITION", modelTitleLocal + meshAccessorSuffix_Vertices },
						{ "NORMAL", modelTitleLocal + meshAccessorSuffix_Normals },
						{ "TEXCOORD_0", modelTitleLocal + meshAccessorSuffix_UVs }
					}
				}
			};
		}
		else {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", model.MaterialTitle },
				{ "indices", modelTitleLocal + meshAccessorSuffix_Indices },
				{ "attributes",
					{
						{ "POSITION", modelTitleLocal + meshAccessorSuffix_Vertices },
						{ "NORMAL", modelTitleLocal + meshAccessorSuffix_Normals }
					}
				}
			};
		}
		j["meshes"].push_back({ { "name", modelTitleLocal + meshNameSuffix }, { "primitives", jMeshPrimitives } });

		// ---------------------------------------------------------
		// buffers
		int base64_size_indices = model.indices.size() * sizeof(float);
		int base64_size_vertices = model.vertices.size() * 3 * sizeof(float);
		int base64_size_normals = model.normals.size() * 3 * sizeof(float);
		int base64_size_textureCoordinates = model.texture_coordinates.size() * 2 * sizeof(float);
		std::string base64_indices = base64_encode(reinterpret_cast<unsigned char const *>(model.indices.data()), base64_size_indices);
		std::string base64_vertices = base64_encode(reinterpret_cast<unsigned char const *>(model.vertices.data()), base64_size_vertices);
		std::string base64_normals = base64_encode(reinterpret_cast<unsigned char const *>(model.normals.data()), base64_size_normals);
		std::string base64_textureCoordinates = base64_encode(reinterpret_cast<unsigned char const *>(model.texture_coordinates.data()), base64_size_textureCoordinates);

		// ---------------------------------------------------------
		// buffer views
		// indices
		jBufferViews[modelTitleLocal + bufferViewSuffix + suffix_Indices] = {
			{ "buffer", 0 },
			{ "byteOffset", totalBufferLength},
			{ "byteLength", base64_size_indices },
			{ "target", "34963" }
		};
		// vertices
		jBufferViews[modelTitleLocal + bufferViewSuffix + suffix_Vertices] = {
			{ "buffer", 0 },
			{ "byteOffset", totalBufferLength + base64_size_indices },
			{ "byteLength", base64_size_vertices },
			{ "target", "34962" }
		};
		// normals
		jBufferViews[modelTitleLocal + bufferViewSuffix + suffix_Normals] = {
			{ "buffer", 0 },
			{ "byteOffset", totalBufferLength + (base64_size_indices + base64_size_vertices) },
			{ "byteLength", base64_size_normals },
			{ "target", "34962" }
		};
		// uvs
		if (model.texture_coordinates.size() > 0)
			jBufferViews[modelTitleLocal + bufferViewSuffix + suffix_Normals] = {
				{ "buffer", 0 },
				{ "byteOffset", totalBufferLength + (base64_size_indices + base64_size_vertices + base64_size_normals) },
				{ "byteLength", base64_size_textureCoordinates },
				{ "target", "34962" }
			};

		// ---------------------------------------------------------
		// accessors
		nlohmann::json jAccessors;
		// indices
		jAccessors[modelTitleLocal + meshAccessorSuffix_Indices] = {
			{ "bufferView", modelTitleLocal + bufferViewSuffix + suffix_Indices },
			{ "byteOffset", 0 },
			{ "byteStride", 0 },
			{ "componentType", 5125 },
			{ "count", model.indices.size() },
			{ "type", "SCALAR" }
		};
		// vertices
		jAccessors[modelTitleLocal + meshAccessorSuffix_Vertices] = {
			{ "bufferView", modelTitleLocal + bufferViewSuffix + suffix_Vertices },
			{ "byteOffset", 0 },
			{ "byteStride", 0 },
			{ "componentType", 5126 },
			{ "count", model.vertices.size() * 3 },
			{ "type", "VEC3" }
		};
		// normals
		jAccessors[modelTitleLocal + meshAccessorSuffix_Normals] = {
			{ "bufferView", modelTitleLocal + bufferViewSuffix + suffix_Normals },
			{ "byteOffset", 0 },
			{ "byteStride", 0 },
			{ "componentType", 5126 },
			{ "count", model.normals.size() * 3 },
			{ "type", "VEC3" }
		};
		// uvs
		if (model.texture_coordinates.size() > 0)
			jAccessors[modelTitleLocal + meshAccessorSuffix_UVs] = {
				{ "bufferView", modelTitleLocal + bufferViewSuffix + suffix_UVs },
				{ "byteOffset", 0 },
				{ "byteStride", 0 },
				{ "componentType", 5126 },
				{ "count", model.texture_coordinates.size() * 2 },
				{ "type", "VEC2" }
			};
		j["accessors"].push_back(jAccessors);

		// ---------------------------------------------------------
		// misc
		totalBufferLength += base64_size_indices + base64_size_vertices + base64_size_normals + base64_size_textureCoordinates;
		bufferData += base64_indices + base64_vertices + base64_normals + base64_textureCoordinates;

		if (!this->BufferInExternalFile) {
			j["buffers"]["type"] = "arraybuffer";
			j["buffers"]["byteLength"] = totalBufferLength;
			j["buffers"]["uri"] = "data:application/octet-stream;base64," + bufferData;
		}

		meshCounter += 1;
		nodeCounter += 1;
	}

	j["bufferViews"] = jBufferViews;

	if (this->BufferInExternalFile) {
		j["buffers"]["byteLength"] = totalBufferLength;
		j["buffers"]["uri"] = this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin";
		if (!this->saveBufferFile(bufferData))
			Settings::Instance()->funcDoLog("[Kuplung] Error occured while writing the BIN glTF file!");
	}

    //j["nodes"] = this->exportNodes(faces);
    //j["meshes"] = this->exportMeshes(faces);
    //j["accessors"] = this->exportAccessors(faces);
    //j["materials"] = this->exportMaterials(faces);
    //j["textures"] = this->exportTextures(faces);
    //j["images"] = this->exportImages(faces);
    //j["samplers"] = this->exportSamplers(faces);
    //j["bufferViews"] = this->exportBufferViews(faces);
    //j["buffers"] = this->exportBuffers(faces, file);

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

nlohmann::json ExporterGLTF::exportNodes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		j[model.ModelTitle][faceIterator - faces.begin()] = model.ModelTitle + "_mesh";
	}
	return j;
}

nlohmann::json ExporterGLTF::exportMeshes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		nlohmann::json jMeshPrimitives;
		if (model.texture_coordinates.size() > 0) {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", model.MaterialTitle },
				{ "indices", model.ModelTitle + "_mesh_accesor_indices" },
				{ "attributes", {
						{ "POSITION", model.ModelTitle + "_mesh_accesor_vertices" },
						{ "NORMAL", model.ModelTitle + "_mesh_accesor_normals" },
						{ "TEXCOORD_0", model.ModelTitle + "_mesh_accesor_uvs" }
					}
				}
			};
		}
		else {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", model.MaterialTitle },
				{ "indices", model.ModelTitle + "_mesh_accesor_indices" },
				{ "attributes", {
						{ "POSITION", model.ModelTitle + "_mesh_accesor_vertices" },
						{ "NORMAL", model.ModelTitle + "_mesh_accesor_normals" }
					}
				}
			};
		}
		j[model.ModelTitle + "_mesh"] = {
			{ "name", model.ModelTitle + "_mesh" },
			{ "primitives", jMeshPrimitives }
		};
	}
	return j;
}

nlohmann::json ExporterGLTF::exportAccessors(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
    return j;
}

nlohmann::json ExporterGLTF::exportMaterials(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		j[model.MaterialTitle]["value"]["ambient"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j[model.MaterialTitle]["value"]["diffuse"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j[model.MaterialTitle]["value"]["specular"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j[model.MaterialTitle]["value"]["emission"] = { 0.0f, 0.0f, 0.0f, 1.0f };
		j[model.MaterialTitle]["value"]["shininess"] = 0.0f;
	}
    return j;
}

nlohmann::json ExporterGLTF::exportTextures(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
    return j;
}

nlohmann::json ExporterGLTF::exportImages(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
    std::vector<ModelFaceBase*>::const_iterator faceIterator;
    int c = 0;
    for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
        MeshModelMaterial mat = (*faceIterator)->meshModel.ModelMaterial;
        if (!mat.TextureAmbient.Image.empty())
            j.push_back(this->copyImage(mat.TextureAmbient.Image));
        if (!mat.TextureBump.Image.empty())
            j.push_back(this->copyImage(mat.TextureBump.Image));
        if (!mat.TextureDiffuse.Image.empty())
            j[c] = this->copyImage(mat.TextureDiffuse.Image);
        if (!mat.TextureDisplacement.Image.empty())
            j.push_back(this->copyImage(mat.TextureDisplacement.Image));
        if (!mat.TextureDissolve.Image.empty())
            j.push_back(this->copyImage(mat.TextureDissolve.Image));
        if (!mat.TextureSpecular.Image.empty())
            j.push_back(this->copyImage(mat.TextureSpecular.Image));
        if (!mat.TextureSpecularExp.Image.empty())
            j.push_back(this->copyImage(mat.TextureSpecularExp.Image));
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

nlohmann::json ExporterGLTF::exportSamplers(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
    return j;
}

nlohmann::json ExporterGLTF::exportBufferViews(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
	}
    return j;
}

nlohmann::json ExporterGLTF::exportBuffers(const std::vector<ModelFaceBase*>& faces, const FBEntity& file) {
    nlohmann::json j;
    std::vector<ModelFaceBase*>::const_iterator faceIterator;
	nlohmann::json jBuffer, jBufferViews;
	int totalBufferLength = 0;
	std::string bufferData;
    for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
        const ModelFaceBase& face = **faceIterator;
        MeshModel model = face.meshModel;
		int base64_size_indices = model.indices.size() * sizeof(float);
        int base64_size_vertices = model.vertices.size() * 3 * sizeof(float);
		int base64_size_normals = model.normals.size() * 3 * sizeof(float);
		int base64_size_textureCoordinates = model.texture_coordinates.size() * 2 * sizeof(float);
		std::string base64_indices = base64_encode(reinterpret_cast<unsigned char const *>(model.indices.data()), base64_size_indices);
        std::string base64_vertices = base64_encode(reinterpret_cast<unsigned char const *>(model.vertices.data()), base64_size_vertices);
		std::string base64_normals = base64_encode(reinterpret_cast<unsigned char const *>(model.normals.data()), base64_size_normals);
		std::string base64_textureCoordinates = base64_encode(reinterpret_cast<unsigned char const *>(model.texture_coordinates.data()), base64_size_textureCoordinates);
		totalBufferLength += base64_size_indices + base64_size_vertices + base64_size_normals + base64_size_textureCoordinates;
		bufferData += base64_indices + base64_vertices + base64_normals + base64_textureCoordinates;
		jBufferViews[model.ModelTitle + "_view_indices"] = {
			{ "buffer", "" }
		};
    }
	jBuffer["type"] = "arraybuffer";
	jBuffer["byteLength"] = totalBufferLength;
	//jBuffer["uri"] = "data:application/octet-stream;base64," + bufferData;
	if (!this->saveBufferFile(bufferData))
		Settings::Instance()->funcDoLog("[Kuplung] Error occured while writing the BIN glTF file!");
	jBuffer["uri"] = this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin";
	j[this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin"] = jBuffer;
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
	if (buffer_length % 4)
		buffer_length += 4 - buffer_length % 4;

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
	uint32_t length = 12 + 8 + buffer_length;
	if (!std::fwrite(&length, 1, 1, f))
		return false;

	// binary buffer chunk
	// - chunk length
	if (!std::fwrite(&buffer_length, 1, 1, f))
		return false;

	// - chunk type
	uint32_t bufferType = 0x004E4942;
	if (!std::fwrite(&bufferType, 1, 1, f))
		return false;

	// - chunkData
	if (!std::fwrite((char*)&buffer, 1, buffer.size(), f))
		return false;

	// padding
	char pad = 0;
	for (auto i = 0; i < buffer_length - buffer_length; i++) {
		if (!fwrite(&pad, 1, 1, f))
			return false;
	}

	std::fclose(f);

	return true;
}

}}}
