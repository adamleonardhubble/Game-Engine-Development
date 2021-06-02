#pragma once

namespace Engine
{
	class Model
	{
	public:
		//! Constructor
		Model() : vertices(nullptr), indices(nullptr), verticesSize(0), indicesSize(0) {}
		//! Destructor
		~Model() 
		{
			delete vertices;
			vertices = nullptr;
			delete indices;
			indices = nullptr;
			delete posVertices;
			posVertices = nullptr;
		}

		float* vertices; //!< Pointer to the verices of the model being loaded
		unsigned int* indices; //!< Pointer to the indices of the model being loaded
		unsigned int verticesSize; //!< The number of values of the vertices being loaded
		unsigned int indicesSize; //!< The number of values of the indices being loaded

		float* posVertices;
		unsigned int posVerticesSize;
	};
}