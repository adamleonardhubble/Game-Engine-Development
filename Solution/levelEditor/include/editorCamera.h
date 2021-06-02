/** \file editorCamera.h
*/
#pragma once

/**
\class EditorCamera
\brief Camera conreoller for the camera that will be used in the level editor
*/
class EditorCamera : public Engine::CameraController
{
private:
	std::shared_ptr<Engine::PerspectiveEulerCamera3D> m_camera; //!< Pointer to the camera
	glm::mat4 m_view; //!< The cameras view, for calculations
	glm::vec3 m_position; //!< The cameras position, for calculations
	glm::vec3 m_forward; //!< The cameras forward axis
	glm::vec3 m_up; //!< The cameras up axis
	glm::vec3 m_right; //!< The cameras right axis
	glm::vec3 m_worldUp; //!< The world up axis

	float m_fYaw; //!< The cameras yaw
	float m_fPitch; //!< The cameras pitch
	float m_fTranslationSpeed = 0.5f; //!< The speed the camera will move
	float m_fRotationSpeed = 4.f; //!< The speed the camera will rotate
	glm::vec2 m_lastMousePosition; //!< The previous mouse position, for using the mouse to rotate the camera

	bool m_bLeftPressed = false; //!< If the left mouse button is pressed
	bool m_bRightPressed = false; //!< If the right mouse button is pressed

	//! Function to update the cameras view
	void updateView();
public:
	//! Constructor
	/*!
	\param update If the camera can be updated when the level loads
	*/
	EditorCamera() {}

	//! Function to initialize the controller
	/*!
	\param fov The field of view
	\param aspectRatio The cameras aspect ratio
	\param nearClip Closest something can be drawn
	\param farClip Furthest something can be drawn
	\param position The position for the camera to start in
	\param rotation The yaw and pitch of the camera
	*/
	void init(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation) override;
	//! Function to get the camera
	/*!
	\return A pointer to the camera
	*/
	inline std::shared_ptr<Engine::Camera> getCamera() override { return m_camera; }
	//! Function to update the controller every frame
	/*!
	\param timestep The time since the previous frame
	*/
	void onUpdate(float timestep) override;
	//! Function to handle events
	/*!
	\param e The event
	*/
	void onEvent(Engine::Event& e) override;
	//! Function to handle window resize events
	/*!
	\param e The event
	\return If the event was successful or not
	*/
	bool onResize(Engine::WindowResizeEvent& e) override { return true; }
	//! Function to handle mouse button press events
	/*!
	\param e The event
	\return If the event was successful or not
	*/
	bool onMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
	//! Function to handle mouse button release events
	/*!
	\param e The event
	\return If the event was successful or not
	*/
	bool onMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
	//! Function to handle mouse scroll events
	/*!
	\param e The event
	\return If the event was successful or not
	*/
	bool onMouseScrolled(Engine::MouseScrolledEvent& e);

	//! Function to get the position of the camera
	/*!
	\return The position of the camera
	*/
	glm::vec3 getPosition() override;
	//! Function to get the yaw and the pitch of the camera
	/*!
	\return The yaw and the pitch of the camera
	*/
	glm::vec2 getYawPitch() override;
};