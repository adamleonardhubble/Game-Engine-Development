/** \file assimpLoader.h
*/
#pragma once

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "components/meshComponent.h"
#include "systems/log.h"
#include "rendering/model.h"

namespace Engine
{
	/**
	\class AssimpLoader
	\brief Class that loads model data with assimp and then gets the required data for displaying the model
	*/
	class AssimpLoader
	{
	private:
		//! Function to process a node on the model
		static void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<Model> model)
		{
			std::string parentName = "Null";
			if (node->mParent != nullptr) parentName = node->mParent->mName.C_Str();
			//if (node->mNumMeshes > 0) LOG_INFO("MESHED NODE: {0} PARENT: {1}", node->mName.C_Str(), parentName);
			//if (node->mNumMeshes == 0) LOG_INFO("UNMESHED NODE: {0} PARENT: {1}", node->mName.C_Str(), parentName);

			aiMatrix4x4* transform = &node->mTransformation;

			/*LOG_INFO("TRANSFORM");
			LOG_INFO("{0} {1} {2} {3}", transform->a1, transform->a2, transform->a3, transform->a4);
			LOG_INFO("{0} {1} {2} {3}", transform->b1, transform->b2, transform->b3, transform->b4);
			LOG_INFO("{0} {1} {2} {3}", transform->c1, transform->c2, transform->c3, transform->c4);
			LOG_INFO("{0} {1} {2} {3}", transform->d1, transform->d2, transform->d3, transform->d4);*/

			// process all the node's meshes
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				processMesh(mesh, scene, model);
			}

			//  Process child nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene, model);
			}
		}

		//! Function to process the mesh
		static void processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Model> model)
		{
			// Find vertex properties
			bool hasPositions = mesh->HasPositions();
			bool hasNormals = mesh->HasNormals();
			bool hasTangents = mesh->HasTangentsAndBitangents();
			unsigned int numColourChannels = mesh->GetNumColorChannels();
			unsigned int numUVChannels = mesh->GetNumUVChannels();

			//LOG_INFO("VERTICES");

			model->verticesSize = mesh->mNumVertices * 14;
			model->vertices = new float[model->verticesSize];

			model->posVerticesSize = mesh->mNumVertices * 3;
			model->posVertices = new float[model->posVerticesSize];

			// Iterate through vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				// Get vertex data
				glm::vec3 position, normal, tangent, biTangent;
				std::vector<glm::vec4> colours(numColourChannels);
				std::vector<glm::vec2> texCoords(numUVChannels);

				if (hasPositions) position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				if (hasNormals) normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				if (hasTangents)
				{
					tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
					biTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
				}

				for (unsigned int j = 0; j < numColourChannels; j++)
				{
					colours[j].r = mesh->mColors[j][i].r;
					colours[j].g = mesh->mColors[j][i].g;
					colours[j].b = mesh->mColors[j][i].b;
					colours[j].a = mesh->mColors[j][i].a;
				}

				for (unsigned int j = 0; j < numUVChannels; j++)
				{
					texCoords[j] = glm::vec2(mesh->mTextureCoords[j][i].x, mesh->mTextureCoords[j][i].y);
				}

				// Log part - assume postion, normal and UV coords
				//LOG_INFO("P x:{0}, y:{1}, z:{2}, N x:{3}, y:{4}, z:{5}, T u:{6}, v{7}", position.x, position.y, position.z, normal.x, normal.y, normal.z, texCoords[0].x, texCoords[0].y);


				model->vertices[i * 14] = position.x;
				model->vertices[(i * 14) + 1] = position.y;
				model->vertices[(i * 14) + 2] = position.z;
				model->vertices[(i * 14) + 3] = normal.x;
				model->vertices[(i * 14) + 4] = normal.y;
				model->vertices[(i * 14) + 5] = normal.z;
				model->vertices[(i * 14) + 6] = texCoords[0].x;
				model->vertices[(i * 14) + 7] = texCoords[0].y;
				model->vertices[(i * 14) + 8] = tangent.x;
				model->vertices[(i * 14) + 9] = tangent.y;
				model->vertices[(i * 14) + 10] = tangent.z;
				model->vertices[(i * 14) + 11] = biTangent.x;
				model->vertices[(i * 14) + 12] = biTangent.y;
				model->vertices[(i * 14) + 13] = biTangent.z;

				model->posVertices[i * 3] = position.x;
				model->posVertices[(i * 3) + 1] = position.y;
				model->posVertices[(i * 3) + 2] = position.z;
			}

			//LOG_INFO("INDICES");

			model->indicesSize = mesh->mNumFaces * 3;
			model->indices = new unsigned int[model->indicesSize];

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++) unsigned int index = face.mIndices[j];

				// Log part - assume all faces are trinalge and therefore ahve 3 indices
				//LOG_INFO("Face {0}: {1} {2} {3}", i, face.mIndices[0], face.mIndices[1], face.mIndices[2]);


				model->indices[i * 3] = face.mIndices[0];
				model->indices[(i * 3) + 1] = face.mIndices[1];
				model->indices[(i * 3) + 2] = face.mIndices[2];
			}

			std::vector<aiTextureType> types = {
				aiTextureType_NONE,
				aiTextureType_DIFFUSE,
				aiTextureType_SPECULAR,
				aiTextureType_AMBIENT,
				aiTextureType_EMISSIVE,
				aiTextureType_HEIGHT,
				aiTextureType_NORMALS,
				aiTextureType_SHININESS,
				aiTextureType_OPACITY,
				aiTextureType_DISPLACEMENT,
				aiTextureType_LIGHTMAP,
				aiTextureType_REFLECTION,
				aiTextureType_BASE_COLOR,
				aiTextureType_NORMAL_CAMERA,
				aiTextureType_EMISSION_COLOR,
				aiTextureType_METALNESS,
				aiTextureType_DIFFUSE_ROUGHNESS,
				aiTextureType_AMBIENT_OCCLUSION,
				aiTextureType_UNKNOWN
			};


			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			for (auto type : types)
			{
				for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
				{
					aiString str;
					material->GetTexture(type, i, &str);
					//LOG_INFO("Texture type:{0} filepath:{1}", type, str.C_Str());
				}

			}


			/*aiString stringValue;
			int intValue;
			float floatValue;
			aiColor3D colorValue;
			if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, stringValue)) LOG_INFO("Material name: {0}", stringValue.C_Str());

			if (AI_SUCCESS == material->Get(AI_MATKEY_SHADING_MODEL, intValue))
			{
				if (intValue == aiShadingMode_Flat) LOG_INFO("Material shading model: Flat shading");
				if (intValue == aiShadingMode_Gouraud) LOG_INFO("Material shading model: Gouraud");
				if (intValue == aiShadingMode_Phong) LOG_INFO("Material shading model: Phong");
				if (intValue == aiShadingMode_Blinn) LOG_INFO("Material shading model: Blinn");
				if (intValue == aiShadingMode_Toon) LOG_INFO("Material shading model: Toon");
				if (intValue == aiShadingMode_OrenNayar) LOG_INFO("Material shading model: Oren Nayar");
				if (intValue == aiShadingMode_Minnaert) LOG_INFO("Material shading model: Minnaert");
				if (intValue == aiShadingMode_CookTorrance) LOG_INFO("Material shading model: Cook Torrance");
				if (intValue == aiShadingMode_Fresnel) LOG_INFO("Material shading model: Fresnel");
				if (intValue == aiShadingMode_NoShading) LOG_INFO("Material shading model: No shading");
			}

			if (AI_SUCCESS == material->Get(AI_MATKEY_ENABLE_WIREFRAME, intValue))
			{
				if (intValue == 0) LOG_INFO("Material wireframe: false");
				else if (intValue == 1) LOG_INFO("Material wireframe: true");
				else LOG_INFO("Material wireframe: unknown");
			}


			if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, floatValue)) LOG_INFO("Material shininess: {0}", floatValue);
			if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, floatValue)) LOG_INFO("Material shininess strength: {0}", floatValue);
			if (AI_SUCCESS == material->Get(AI_MATKEY_REFLECTIVITY, floatValue)) LOG_INFO("Material reflectivity: {0}", floatValue);
			if (AI_SUCCESS == material->Get(AI_MATKEY_REFRACTI, floatValue)) LOG_INFO("Material refraction index: {0}", floatValue);
			if (AI_SUCCESS == material->Get(AI_MATKEY_TRANSPARENCYFACTOR, floatValue)) LOG_INFO("Material transprancy factor: {0}", floatValue);
			if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, floatValue)) LOG_INFO("Material opacity: {0}", floatValue);

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, colorValue)) LOG_INFO("Material ambient colour: {0}, {1}, {2}", colorValue.r, colorValue.g, colorValue.b);
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, colorValue)) LOG_INFO("Material diffuse colour: {0}, {1}, {2}", colorValue.r, colorValue.g, colorValue.b);
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, colorValue)) LOG_INFO("Material specular colour: {0}, {1}, {2}", colorValue.r, colorValue.g, colorValue.b);
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, colorValue)) LOG_INFO("Material emissive colour: {0}, {1}, {2}", colorValue.r, colorValue.g, colorValue.b);
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_REFLECTIVE, colorValue)) LOG_INFO("Material reflective colour: {0}, {1}, {2}", colorValue.r, colorValue.g, colorValue.b);
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_TRANSPARENT, colorValue)) LOG_INFO("Material tranparent colour: {0}, {1}, {2}", colorValue.r, colorValue.g, colorValue.b);*/
		}
	public:
		//! Function to load a model
		/*!
		\param res A pointer to the resource manager
		\param filepath The file location of the model being loaded
		\param model A pointer to the model that the vertex data will be saved in
		*/
		static void loadModel(const std::string& filepath, std::shared_ptr<Model> model)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filepath, aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_Triangulate);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				LOG_ERROR("Cannot load: {0}, ASSIMP Error {1}", filepath, importer.GetErrorString());
				return;
			}
			else
				processNode(scene->mRootNode, scene, model);
		}
	};
}