/** \file debugLayer.cpp
*/

#include "engine_pch.h"

#ifdef NG_DEBUG

#include "platform/openGL/imGui/debugLayer.h"

#include "core/application.h"

#include "systems/timer.h"

#include <Windows.h>

namespace Engine
{
	DebugLayer::DebugLayer(bool editor, const std::string& name) : ImGuiLayer(name), m_bEditor(editor)
	{
		for (auto& iterator : Timer::s_debugProcessTimer) // For all elements within the debug process timer map, do the following
		{
			m_processesMap.insert(std::make_pair(iterator.first, std::make_pair("", ""))); // Insert the name of every process recorded within the debug process timer map (for initialisation)
		}
	}

	void DebugLayer::onUpdate(float timestep)
	{
		m_fTimePassed += timestep;

		if (m_fTimePassed >= m_fUpdateFrequency)
		{
			FPSprofiling(timestep);	// Perform FPS profiling-related functionality

			for (auto& iterator : m_processesMap) // For all elements within the processes map, do the following
			{
				std::string currentTime; std::string averageTime; // Create strings, used to store the returned current and average process time values, in text format

				ProcessProfiling(iterator.first, currentTime, averageTime); // Function call, per process being measured

				m_processesMap[iterator.first].first = currentTime;	// Set the current time to execute the process, to the returned current time value
				m_processesMap[iterator.first].second = averageTime; // Set the average time to execute the process, to the returned average time value
			}

			m_fTimePassed = 0;
		}
	}

	void DebugLayer::drawLayer() // Update the layer
	{
		if (!m_bEditor)
			BeginFrame(); // Update IMGui

		#pragma region Performance Profiler
		
		ImGui::Begin("Performance Profiler"); // Set the IMGui window title, create a IMGui window instance
		if (ImGui::CollapsingHeader("Frames Per Second")) // If the collapsable header titled 'Frames Per Second' is pressed, do the following
		{
			ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

			ImGui::PlotLines("", m_currentFrameGraphValues, IM_ARRAYSIZE(m_currentFrameGraphValues), NULL, m_sCurrentFPSText.c_str(), -1.0f, m_fCurrentFPSGraphHeight, ImVec2(0, 100)); // Set an IMGui line graph object, to the FPS count of the current '0.1' seconds overtime, for the range specified

			ImGui::PlotLines("", m_averageFrameGraphValues, IM_ARRAYSIZE(m_averageFrameGraphValues), NULL, m_sAverageFPSText.c_str(), -1.0f, m_fAverageFPSGraphHeight, ImVec2(0, 100)); // Set an IMGui line graph object, to the average FPS count of the current '0.1' seconds overtime, for the range specified
			ImGui::SliderInt("", &m_iSampleSize, 0, 100, "Sample Size: %d"); // Set an IMGui integer slider object, to the sample size of the average FPS count

			ImGui::ProgressBar(m_fFrameDropAmount * 0.01, ImVec2(0, 50), m_sDroppedFramesText.c_str()); // Set an IMGui progress bar object, to the frame drop amount 
		
			ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
		}

		if (ImGui::CollapsingHeader("Processes")) // If the collapsable header titled 'Process' is pressed, do the following
		{
			ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

			if (ImGui::CollapsingHeader("Application Processes")) // If the collapsable header titled 'Application Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Event Handling")) // If the collapsable header titled 'Event Handling' is pressed, do the following
				{
					ImGui::Text(m_processesMap["EventHandling"].first.c_str()); // Set an IMGui text object, to the time taken to handle events (all events)
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["EventHandling"].second.c_str()); // Set an IMGui text object to the average time taken to handle events (all events)
				}

				if (ImGui::CollapsingHeader("Application Update")) // If the collapsable header titled 'Application Update' is pressed, do the following
				{
					ImGui::Text(m_processesMap["ApplicationUpdate"].first.c_str()); // Set an IMGui text object, to the time taken to update the application window
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["ApplicationUpdate"].second.c_str()); // Set an IMGui text object to the average time taken to update the application window
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}

			if (ImGui::CollapsingHeader("Render Processes")) // If the collapsable header titled 'Render Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Basic Renderer Submit")) // If the collapsable header titled 'Basic Renderer Submit' is pressed, do the following
				{
					ImGui::Text(m_processesMap["BasicRendererSubmit"].first.c_str()); // Set an IMGui text object, to the time taken to submit a draw call using the basic renderer 
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["BasicRendererSubmit"].second.c_str()); // Set an IMGui text object to the average time taken to submit a draw call using the basic renderer 
				}

				if (ImGui::CollapsingHeader("Text Renderer Submit")) // If the collapsable header titled 'Text Renderer Submit' is pressed, do the following
				{
					ImGui::Text(m_processesMap["TextRendererSubmit"].first.c_str()); // Set an IMGui text object, to the time taken to submit a draw call using the text renderer 
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["TextRendererSubmit"].second.c_str()); // Set an IMGui text object to the average time taken to submit a draw call using the text renderer 
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}

			if (ImGui::CollapsingHeader("Shader Processes")) // If the collapsable header titled 'Shader Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Shader Parse Single Source")) // If the collapsable header titled 'Shader Parse Single Source' is pressed, do the following
				{
					ImGui::Text(m_processesMap["ParseShaderSourceSingle"].first.c_str()); // Set an IMGui text object, to the time taken to parse a single source of shader code
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["ParseShaderSourceSingle"].second.c_str()); // Set an IMGui text object to the average time taken to parse a single source of shader code
				}

				if (ImGui::CollapsingHeader("Shader Compile and Link")) // If the collapsable header titled 'Shader Compile and Link' is pressed, do the following
				{
					ImGui::Text(m_processesMap["CompileAndLinkShader"].first.c_str()); // Set an IMGui text object, to the time taken to compile and link the vertex and fragment shader data
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["CompileAndLinkShader"].second.c_str()); // Set an IMGui text object to the average time taken to compile and link the vertex and fragment shader data
				}

				if (ImGui::CollapsingHeader("Identify Shader Uniforms")) // If the collapsable header titled 'Identify Shader Uniforms' is pressed, do the following
				{
					ImGui::Text(m_processesMap["IdentifyShaderUniforms"].first.c_str()); // Set an IMGui text object, to the time taken to identify shader uniform variables
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["IdentifyShaderUniforms"].second.c_str()); // Set an IMGui text object to the average time taken to identify shader uniform variables
				}

				if (ImGui::CollapsingHeader("Store Shader Uniforms")) // If the collapsable header titled 'Store Shader Uniforms' is pressed, do the following
				{
					ImGui::Text(m_processesMap["StoreShaderUniforms"].first.c_str()); // Set an IMGui text object, to the time taken to store shader uniform variables
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["StoreShaderUniforms"].second.c_str()); // Set an IMGui text object to the average time taken to store shader uniform variables
				}

				if (ImGui::CollapsingHeader("Initialise Shader Single Source")) // If the collapsable header titled 'Initialise Shader Single Source' is pressed, do the following
				{
					ImGui::Text(m_processesMap["InitialiseShaderSingleSource"].first.c_str()); // Set an IMGui text object, to the time taken to intialise shader instances
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["InitialiseShaderSingleSource"].second.c_str()); // Set an IMGui text object to the average time taken to intialise shader instances
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}
			
			if (ImGui::CollapsingHeader("Uniform Buffer Processes")) // If the collapsable header titled 'Uniform Buffer Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Initialise Uniform Buffers")) // If the collapsable header titled 'Initialise Uniform Buffers' is pressed, do the following
				{
					ImGui::Text(m_processesMap["InitialiseUniformBuffers"].first.c_str()); // Set an IMGui text object, to the time taken to intialise the uniform buffer objects
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["InitialiseUniformBuffers"].second.c_str()); // Set an IMGui text object to the average time taken to intialise the uniform buffer objects
				}

				if (ImGui::CollapsingHeader("Attach Uniform Shader Block")) // If the collapsable header titled 'Attach Uniform Shader Block' is pressed, do the following
				{
					ImGui::Text(m_processesMap["AttachUniformShaderBlock"].first.c_str()); // Set an IMGui text object, to the time taken to bind the uniform blocks
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["AttachUniformShaderBlock"].second.c_str()); // Set an IMGui text object to the average time taken to bind the uniform blocks
				}

				if (ImGui::CollapsingHeader("Set Uniform Buffer Data")) // If the collapsable header titled 'Set Uniform Buffer Data' is pressed, do the following
				{
					ImGui::Text(m_processesMap["SetUniformBufferData"].first.c_str()); // Set an IMGui text object, to the time taken to set uniform buffer data
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["SetUniformBufferData"].second.c_str()); // Set an IMGui text object to the average time taken to set uniform buffer data
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}

			if (ImGui::CollapsingHeader("Index Buffer Processes")) // If the collapsable header titled 'Index Buffer Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Initialise Index Buffers")) // If the collapsable header titled 'Initialise Index Buffers' is pressed, do the following
				{
					ImGui::Text(m_processesMap["InitialiseIndexBuffers"].first.c_str()); // Set an IMGui text object, to the time taken to intialise the index buffer objects
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["InitialiseIndexBuffers"].second.c_str()); // Set an IMGui text object to the average time taken to intialise the index buffer objects
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}

			if (ImGui::CollapsingHeader("Vertex Buffer Processes")) // If the collapsable header titled 'Index Buffer Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Initialise Vertex Buffers")) // If the collapsable header titled 'Initialise Vertex Buffers' is pressed, do the following
				{
					ImGui::Text(m_processesMap["InitialiseVertexBuffers"].first.c_str()); // Set an IMGui text object, to the time taken to intialise the vertex buffer objects
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["InitialiseVertexBuffers"].second.c_str()); // Set an IMGui text object to the average time taken to intialise the vertex buffer objects
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}

			if (ImGui::CollapsingHeader("Vertex Array Processes")) // If the collapsable header titled 'Vertex Array Processes' is pressed, do the following
			{
				ImGui::Indent(22.0f); // Indent the following IMGui objects by '22'

				if (ImGui::CollapsingHeader("Set Vertex Buffer Data")) // If the collapsable header titled 'Set Vertex Buffer Data' is pressed, do the following
				{
					ImGui::Text(m_processesMap["SetVertexBufferData"].first.c_str()); // Set an IMGui text object, to the time taken to set the vertex buffer data
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["SetVertexBufferData"].second.c_str()); // Set an IMGui text object to the average time taken to set the vertex buffer data
				}

				if (ImGui::CollapsingHeader("Set Index Buffer Data")) // If the collapsable header titled 'Set Index Buffer Data' is pressed, do the following
				{
					ImGui::Text(m_processesMap["SetIndexBufferData"].first.c_str()); // Set an IMGui text object, to the time taken to set the index buffer data
					ImGui::SameLine(); // Set the following IMGui objects to be positioned on the same line 
					ImGui::Text(m_processesMap["SetIndexBufferData"].second.c_str()); // Set an IMGui text object to the average time taken to set the index buffer data
				}

				ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
			}

			ImGui::Unindent(22.0f); // Unindent the following IMGui objects by '22'
		}

		ImGui::End(); // Enclose the function calls of the current IMGui window instance, used if want to create another window instance with 'Begin()'
		#pragma endregion

		if (!m_bEditor)
			EndFrame(); // Configure IMGui render properties
	}

	void DebugLayer::FPSprofiling(float timestep) // Calculate FPS profiling variables
	{
		m_fTimer += timestep; // Add the timestep to the time counter

		m_iTotalFramesPassed++; // Increment the total number of frames passed since application entry

		#pragma region Sample Size Management
		if (m_vAverageFPS.size() > m_iSampleSize) // If the the size of the vector container is larger than 'm_iSampleSize', do the following
		{
			for (int i = 0; i <= m_vAverageFPS.size(); i++) // For the size of the vector container, do the following
			{
				m_vAverageFPS.pop_back(); // Pop the last element in the vector container, until the vector container is no loner larger than the sample size
			}
		}
		#pragma endregion

		#pragma region Current and Average FPS
		m_sTimestep = timestep * 1000.0f; // Set 'm_sTimestep' to the timestep divided by '1000' (convert to milliseconds)

		float FPS = (1 / timestep); // Calculate the FPS count of the current frame

		m_sFPSCurrent = FPS; // Set 'm_sFPSCurrent' to the current FPS count

		std::vector<float>::iterator iteratorFPS = m_vAverageFPS.begin(); // Vector container iterator, set to the first element in the vector container

		m_vAverageFPS.insert(iteratorFPS, FPS); // Insert the current FPS into the first element in the vector container

		for (int i = 0; i < m_vAverageFPS.size(); i++) // For the size of the vector container, do the following
		{
			m_fAccumulatedFPS += m_vAverageFPS.at(i); // Add and equal the FPS count at every element in the vector container 
		}

		float averageFPS = m_fAccumulatedFPS / m_vAverageFPS.size(); // Set 'averageFPS' to the average FPS count divided by the size of the vector

		m_sFPSAverage = averageFPS; // Set 'm_sFPSAverage' to the average FPS count

		m_fAccumulatedFPS = 0.0f;	// Reset the average FPS
		#pragma endregion

		#pragma region Dropped Frames Count
		if (m_vAverageFPS.at(0) <= 0.1f * m_sFPSAverage)	// If the current FPS count is '90%' smaller than the current average FPS count, do the following
		{
			m_iDroppedFramesCount++; // Increment the number of frames dropped
		}

		m_fFrameDropAmount = (float)m_iDroppedFramesCount / (float)m_iTotalFramesPassed * 100; // Set 'm_fFrameDropAmount' to the percentage of frames dropped against the number of frames passed since runtime
		
		m_sDroppedFramesText = "Dropped Frames [" + std::to_string(m_iDroppedFramesCount) + "] \t Drop Percentage [" + std::to_string(m_fFrameDropAmount) + "] %"; // Set 'm_sDroppedFramesText' to the text format of the frame drop count and amount
		
		#pragma endregion

		#pragma region Update FPS Profiling Gradually
		if (m_fTimer >= 0.1f) // If the time counter is larger than or equal to '0.1' (seconds), do the following
		{
			m_sCurrentFPSText = "Current FPS [" + std::to_string(m_sFPSCurrent) + "] \t Time Passed [" + std::to_string(m_sTimestep) + "] ms"; // Set 'm_sCurrentFPSText' to the text format of the current FPS count and timestep
			m_sAverageFPSText = "Average FPS [" + std::to_string(m_sFPSAverage) + "]";	// Set 'm_sAverageFPSText' to the text format of the current average FPS count

			#pragma region FPS Line Graphs
			m_fCurrentFPSCount = m_sFPSCurrent; // Set 'm_fCurrentFPSCount' to the most recent FPS count calculated
			m_fCurrentAverageFPSCount = m_sFPSAverage;	// Set 'm_fCurrentAverageFPSCount' to the most recent average FPS count calculated

			m_currentFrameGraphValues[m_iGraphIncrementer] = m_fCurrentFPSCount; // Set each element in the array to the most recent frame count, for every iteration
			m_averageFrameGraphValues[m_iGraphIncrementer] = m_fCurrentAverageFPSCount; // Set each element in the array to the most recent average frame count, for every iteration

			if (m_iGraphIncrementer < 99) // If the number of elements in the arrays is smaller than the arrays size, do the following
			{
				m_iGraphIncrementer++; // Increment the graph element incrementer to store the next FPS counts, in the next available elements, in the arrays
			}
			else // Else if the number of elements in the arrays are equal to the size of the arrays, do the following 
			{
				for (int i = 0; i < 99; i++) // For all elements in each array, do the following
				{
					m_currentFrameGraphValues[i] = m_currentFrameGraphValues[i + 1]; // Shift the FPS counts for all of the previously stored elements, to their next corresponding element in the array
					m_averageFrameGraphValues[i] = m_averageFrameGraphValues[i + 1]; // Shift the average FPS counts for all of the previously stored elements, to their next corresponding element in the array
				}
			}

			m_fCurrentFPSGraphHeight = m_fCurrentFPSCount * 1.5f; // Set 'm_fCurrentFPSGraphHeight'	to the current FPS count mutiplied by '1.5' to cater for spikes in difference
			m_fAverageFPSGraphHeight = m_fCurrentAverageFPSCount * 1.5f; // Set 'm_fCurrentAverageFPSCount'	to the current FPS count mutiplied by '1.5' to cater for spikes in difference
			#pragma endregion											

			m_fTimer = 0.0f; // Reset the time counter
		}
		#pragma endregion
	}

	void DebugLayer::ProcessProfiling(std::string processName, std::string& currentTime, std::string& averageTime) // Calculate process profiling variables
	{
		if (Timer::s_debugProcessTimer[processName].size() > 0)	// If the processes vector container has an element, do the following
		{
			float timeAccumulated = 0.0f; // Create and set 'timeAccumulated' to '0'

			currentTime = "Current Time [" + std::to_string(Timer::s_debugProcessTimer[processName].at(0)) + "] ms"; // Set the current time string passed, to the value of the current element in the container

			for (int i = 0; i < Timer::s_debugProcessTimer[processName].size(); i++) // For the size of the vector container, do the following
			{
				timeAccumulated += Timer::s_debugProcessTimer[processName].at(i); // Add and equal the vector containers elements (durations)
			}

			averageTime = "\t Average Time [" + std::to_string(timeAccumulated / Timer::s_debugProcessTimer[processName].size()) + "] ms"; // Set the average time string passed, to the value of the time accumulated divded by the size of the container

			timeAccumulated = 0.0f;	// Reset the time accumulated value
		}
	}

}

#endif