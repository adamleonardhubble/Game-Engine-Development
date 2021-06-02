/** \file CGO.h
\brief Contains classes for components, component messages and gameobjects
*/
#pragma once

#include "events/event.h"

#include "systems/log.h"

namespace Engine
{
	/**
	\class ComponentMessageType
	\brief Enum for all of the different types of messages
	*/
	enum class ComponentMessageType
	{
		None = 0,
		PositionIntegrate, PositionSet, RelativePositionSet, RotationSet, ScaleSet,
		VelocityIntegrate, VelocitySetLinear, VelocitySetAngular,
		AccelerationSet,
		KeyPressed, KeyReleased, MouseButton, MouseScroll, MouseMoved,
		UniformSet, TextureSet,
		RigidBody, CollisionDetected,
		ModelSet,
		TransformSend
	};

	/**
	\class ComponentMessage
	\brief Used to send any type of data between the components of a gameobject
	*/
	class ComponentMessage
	{
	public:
		//! Constructor
		/*!
		\param type The message type
		\param data The data being sent
		*/
		ComponentMessage(ComponentMessageType type, void* data) : 
			m_msgType(type), m_msgData(data) {}

		ComponentMessageType m_msgType; //!< The type of the message
		void* m_msgData; //!< The message data
	};

	/**
	\class MessageDispatcher
	\brief Dispatches messages to the correct receive functions
	*/
	class MessageDispatcher
	{
		using ReceiveFunc = std::function<void(void*)>; //!< ReceiveFunc is a function which returns nothing and takes a void pointer
	private:
		const ComponentMessage& m_message; //!< The message
	public:
		//! Constructor
		/*!
		\param msg The message
		*/
		MessageDispatcher(const ComponentMessage& msg) : m_message(msg) {}

		//! Function to check if the message type matches and to call the receive function if it does
		bool dispatch(ComponentMessageType type, const ReceiveFunc& func)
		{
			// Iff the message type matches the one passed in
			if (m_message.m_msgType == type)
			{
				// Call the function
				func(m_message.m_msgData);
				return true;
			}
			return
				false;
		}
	};

	class GameObject;

	/**
	\class ComponentType
	\brief The types of components, used to get components from an object
	*/
	enum class ComponentType
	{
		None = 0, 
		Fixed3DCamera, FPSCamera, Free3DCamera,
		BoxCollider, CapsuleCollider, ConvexMeshCollider, SphereCollider,
		BallSocketJoint, FixedJoint, HingeJoint, SliderJoint,
		Light, Material, Mesh, Position, Text, Texture, RigidBody, LuaScript,
		Oscilate, Player, Rotate, TextureSwitch, Velocity
	};

	/**
	\class Component
	\brief Base class for all components to be put on a gameobject
	*/
	class Component
	{
	protected:
		GameObject* m_pOwner = nullptr; //!< Pointer to the gameobject it is on

		ComponentType m_componentType = ComponentType::None; //!< The type of component

		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		virtual void sendMessage(const ComponentMessage& msg) {}
	public:
		//! Function called when the component is attached to a gameobject
		/*!
		\param owner Pointer to the gameobject the component is being attached to
		*/
		virtual void onAttach(GameObject* owner) { m_pOwner = owner; }
		//! Function called when the component is detached from a gameobject
		virtual void onDetach() { m_pOwner = nullptr; }
		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		virtual void onUpdate(float timestep) {}
		//! Function to handle events
		/*!
		\param e The event
		*/
		virtual void onEvent(Event& e) {}
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		virtual void receiveMessage(const ComponentMessage& msg) = 0;

		//! Function to get the object the component is on
		/*!
		\return A pointer to the object
		*/
		GameObject* getObject() { return m_pOwner; }

		//! Function to get the type of component it is
		/*!
		\return The type
		*/
		virtual ComponentType getType() { return m_componentType; }
	};

	/**
	\class GameObject
	\brief A thing in the game world
	*/
	class GameObject
	{
	private:
		std::string m_name; //!< The name of the object
		std::string m_tag; //!< The tag given to the object
		int m_iIndex; //!< The object index value
		std::vector<std::shared_ptr<Component>> m_components; //!< A container of shared pointers to all the objects components
	public:
		//! Default constructor
		GameObject() {}
		//! Constructor
		/*!
		\param name The name of the object
		\param tag The objects tag
		\param i The objects index value
		*/
		GameObject(const std::string& name, const std::string& tag, int i) : m_name(name), m_tag(tag), m_iIndex(i)
		{

		}

		//! Function to update the object every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep)
		{
			// Call onUpdate on each component
			for (auto& comp : m_components)
				comp->onUpdate(timestep);
		}
		//! Function to handle events
		/*!
		\param e The event
		*/
		void onEvent(Event& e)
		{
			// Call onEvent on each component
			for (auto& comp : m_components)
				comp->onEvent(e);
		}
		//! Function to add a new component
		/*!
		\param comp A pointer to the component
		*/
		void addComponent(const std::shared_ptr<Component> comp)
		{
			// Add the component
			m_components.push_back(comp);
			m_components.back()->onAttach(this); // Call onAttach
		}
		//! Function called when removing a component
		/*!
		\param iter The component being removed
		*/
		void removeComponent(std::vector<std::shared_ptr<Component>>::iterator iter) 
		{
			// If the component being deleted is part of the game object
			if (iter != m_components.end())
			{
				// Delete the component
				iter = m_components.erase(iter);
			}
		}

		//! Function to get a component
		/*!
		\return An iterator on the component in the vector
		*/
		template <typename G>
		std::vector<std::shared_ptr<Component>>::iterator getComponent()
		{
			// Make an iterator
			auto result = m_components.end();
			// Check each component and if the type matches, return it
			for (auto it = m_components.begin(); it != m_components.end(); ++it)
			{
				//if (typeid(decltype(*(it->get()))).hash_code() == typeid(G).hash_code())
					//return it;
				if ((*it)->getType() == G::getStaticComponentType())
				{
					return it;
				}
			}
			// Return components.end if the component doesn't exist
			return result;
		}

		//! Used to set an iterator to the start of the vector
		/*!
		\return An iterator at the start of the vector
		*/
		inline std::vector<std::shared_ptr<Component>>::iterator begin() { return m_components.begin(); }
		//! Used to set an iterator to the end of the vector
		/*!
		\return An iterator at the end of the vector
		*/
		inline std::vector<std::shared_ptr<Component>>::iterator end() { return m_components.end(); }

		//! Used to get the name of the object
		/*!
		\return The name of the object
		*/
		inline std::string& name() { return m_name; }
		//! Used to get the tag of the object
		/*!
		\return The tag of the object
		*/
		inline std::string& tag() { return m_tag; }
		//! Used to get the objects index value
		/*!
		\return The objects index value
		*/
		inline int& index() { return m_iIndex; }
	};
}
