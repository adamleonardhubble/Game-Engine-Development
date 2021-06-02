/** \file debugLayer.h
*/

#pragma once // Compile once

#ifdef NG_DEBUG

#include "imGuiLayer.h"

#include "systems/log.h"

namespace Engine
{
	/**
	\class DebugLayer
	\brief 
	*/
	class DebugLayer : public ImGuiLayer
	{
	private:
		float m_fUpdateFrequency = 1.f / 100.f; //!< The frequency to output the data
		float m_fTimePassed = 0; //!< The time since last outputting the data
		bool m_bEditor; //!< If being run in the editor
	public:
		#pragma region FPS Profiling Variables
		float m_fTimer = 0.0f; //!< Float, representing the time counter (time passed)
		float m_sFPSCurrent = 0.0f;	//!< Float, representing the FPS integer count
		float m_sFPSAverage = 0.0f; //!< Float, representing the average FPS integer count
		float m_sTimestep = 0.0f; //!< Float, representing the time passed between each frames
		float m_fAccumulatedFPS = 0; //!< Float, representing the FPS count accumulated over the set amount of frames
		int m_iSampleSize = 10; //!< Integer, representing the sample size that the average FPS count considers
		float m_fCurrentFPSCount = 0.0f; //!< Float, representing the current FPS count
		float m_fCurrentAverageFPSCount = 0.0f; //!< Float, representing the current averaged FPS count

		std::string m_sCurrentFPSText = "";	//!< String, representing the text label on the current FPS line graph (current FPS) 
		std::string m_sTimestepText = ""; //!< String, representing the text label on the current FPS line graph (time passed)

		std::vector<float> m_vAverageFPS; //!< Vector container, used to store FPS counts and calculate average FPS counts

		float m_currentFrameGraphValues[100] = {}; //!< Array, used to store and retrieve the FPS counts for the corresponding line graph
		float m_averageFrameGraphValues[100] = {}; //!< Array, used to store and retrieve the average FPS counts for the corresponding line graph
		int m_iGraphIncrementer = 0; //!< Integer, representing the current graph plot line
		float m_fCurrentFPSGraphHeight = 0.0f; //!< Float, representing the current FPS count, used to control the height of the current FPS line graph
		float m_fAverageFPSGraphHeight = 0.0f; //!< Float, representing the average FPS count, used to control the height of the average FPS line graph

		std::string m_sAverageFPSText = "";	//!< String, representing the text label on the average FPS line graph

		int m_iDroppedFramesCount = 0; //!< Integer, representing the number of frames dropped during application runtime
		int m_iTotalFramesPassed = 0; //!< Integer representing the number of frames passed since application entry
		float m_fFrameDropAmount = 0.0f; //!< Float representing the percentage of frames dropped comparitively to the total number of frames passed during runtime

		std::string m_sDroppedFramesText = ""; //!< String, representing the text label on the dropped frames progress bar
		#pragma endregion

		#pragma region Process Profiling Variables
		std::map<std::string, std::pair<std::string, std::string>> m_processesMap; //!< Map, storing the timed processes name and its current and average execution times in text format
		#pragma endregion

		//! Constructor
		/*!
		\paran editor If the editor is being run
		\param name The name of the layer
		*/
		DebugLayer(bool editor, const std::string& name = "IMGuiLayer");

		//! Function called when the layer is added
		void onAttach() override {}

		//! Function called when the layer is removed
		void onDetach() override {}

		//! Function to update the layer every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;

		void drawObjects() override {}

		//! Function to draw the texture that the layer is rendered to
		void drawLayer() override;

		//! Calculate FPS profiling variables
		/*!
		\param timestep A float representing the time passed since the last frame
		*/
		void FPSprofiling(float timestep);

		//! Calculate process profiling variables
	    /*!
	    \param processName A string representing the name of the process being measured
	    \param currentTime A reference to a string representing the current time taken to execute a process
	    \param averageTime A reference to a string representing the average time taken to execute a process
	    \param sampleSize An integer representing the sample size considered for averaging process time
	    */
		void ProcessProfiling(std::string processName, std::string& currentTime, std::string& averageTime);
	};
}

#endif