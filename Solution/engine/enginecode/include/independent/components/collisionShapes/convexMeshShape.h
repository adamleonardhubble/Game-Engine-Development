/** \file convexMeshShape.h
*/
#pragma once

#include "components/collisionComponent.h"

#include "rendering/model.h"

namespace Engine
{
	/**
	\class ConvexMeshCollisionComponent
	\brief A convex mesh shape collision component for the physics
	*/
	class ConvexMeshCollisionComponent : public CollisionComponent
	{
	private:
		std::shared_ptr<Model> m_pModel; //!< The model

		rp3d::PolygonVertexArray::PolygonFace* m_pPolygonFaces;
		rp3d::PolygonVertexArray* m_pPolygonVertexArray;
		rp3d::PolyhedronMesh* m_pPolyhedronMesh;
	public:
		//! Constructor
		/*!
		\param mass The mass of the collider
		\param model The model to use
		\param scale The scale of the collider
		\param category The collision category the collider is in
		\param canCollideWith The collision categories the collider can collide with
		*/
		ConvexMeshCollisionComponent(float mass, std::shared_ptr<Model> model, glm::vec3 scale, CollisionCategory category, unsigned short canCollideWith) :
			CollisionComponent(mass, category, canCollideWith), m_pModel(model)
		{
			int numFaces = m_pModel->indicesSize / 3;

			m_pPolygonFaces = new rp3d::PolygonVertexArray::PolygonFace[numFaces];
			rp3d::PolygonVertexArray::PolygonFace* face = m_pPolygonFaces;
			for (int i = 0; i < numFaces; i++) 
			{
				face->indexBase = i * 3;

				face->nbVertices = 3;
				face++;
			}

			m_pPolygonVertexArray = new rp3d::PolygonVertexArray(
				m_pModel->posVerticesSize / 3,
				m_pModel->posVertices, 3 * sizeof(float),
				m_pModel->indices, sizeof(int), numFaces, m_pPolygonFaces,
				rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
				rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

			m_pPolyhedronMesh = new rp3d::PolyhedronMesh(m_pPolygonVertexArray);

			m_pShape = new rp3d::ConvexMeshShape(m_pPolyhedronMesh, rp3d::Vector3(scale.x, scale.y, scale.z));

			m_componentType = ComponentType::ConvexMeshCollider; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::ConvexMeshCollider; }
	};
}