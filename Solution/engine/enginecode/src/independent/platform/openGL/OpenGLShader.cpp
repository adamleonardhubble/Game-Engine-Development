/** \file OpenGLShader.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/OpenGLShader.h"
#include "platform/openGL/OpenGLUniformElement.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <fstream>
#include <sstream>

namespace Engine
{
	//! Used to get the ShaderDataType from the shader file string
	/*!
	\param glslSrc The string being checked
	\return The shader data type
	*/
	static ShaderDataType GLSLStrToSDT(const std::string& glslSrc)
	{
		ShaderDataType                 result = ShaderDataType::None;
									   
		if (glslSrc == "int")          result = ShaderDataType::Int;
		if (glslSrc == "ivec2")        result = ShaderDataType::Int2;
		if (glslSrc == "ivec3")        result = ShaderDataType::Int3;
		if (glslSrc == "ivec4")        result = ShaderDataType::Int4;
		if (glslSrc == "float")        result = ShaderDataType::Float;
		if (glslSrc == "vec2")         result = ShaderDataType::Float2;
		if (glslSrc == "vec3")         result = ShaderDataType::Float3;
		if (glslSrc == "vec4")         result = ShaderDataType::Float4;
		if (glslSrc == "mat3")         result = ShaderDataType::Mat3;
		if (glslSrc == "mat4")         result = ShaderDataType::Mat4;
		if (glslSrc == "bool")         result = ShaderDataType::Bool;
		if (glslSrc == "sampler2D")    result = ShaderDataType::Sampler2D;
		if (glslSrc == "samplerCube")  result = ShaderDataType::SamplerCube;

		return result;
	}


	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("InitialiseShaderSingleSource"); // Start 'InitialiseShaderSingleSource' timer
#endif

		// Create the shaders
		m_iVertShader = glCreateShader(GL_VERTEX_SHADER);
		m_iFragShader = glCreateShader(GL_FRAGMENT_SHADER);

		m_bMakingUniformBuffer = false;

		parseSource(filepath); // Parse the file

		storeUniformLocations(); // Store the uniform locations

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("InitialiseShaderSingleSource"); // Stop 'InitialiseShaderSingleSource' timer
#endif
	}

	OpenGLShader::OpenGLShader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	{
		// Create the shaders
		m_iVertShader = glCreateShader(GL_VERTEX_SHADER);
		m_iFragShader = glCreateShader(GL_FRAGMENT_SHADER);

		m_bMakingUniformBuffer = false;

		parseSource(vertexFilepath, fragmentFilepath); // Parse the file

		storeUniformLocations(); // Store the uniform locations
	}

	OpenGLShader::~OpenGLShader()
	{
		m_uniformLayout.clear(); // Empty the uniform layout map

		glDeleteProgram(m_iShaderID); // Free memory
	}

	void OpenGLShader::bind()
	{
		glUseProgram(m_iShaderID); 
	}

	void OpenGLShader::unbind()
	{
		glUseProgram(0);
	}

	bool OpenGLShader::uploadData(const std::string& name, void* data)
	{
		auto it = m_uniformLayout.find(name); // Create and set a uniform map iterator to find the passed name of a shader uniform in the uniform map

		if (it != m_uniformLayout.end()) // If the iterator has not reached the end of the uniform map, do the following
		{
			// Find the uniform object with the correspending name in the map and call the function to upload the data
			m_uniformLayout.find(name)->second->uniformUpload(data);

			return true; // Return 'true'
		}
		else // Else if uniform layout map does not contain the shader uniform passed, do the following
		{
			return false; // Return 'false'
		}

	}

	bool OpenGLShader::uploadData(const UniformLayout& uniforms)
	{
		m_uniformLayout = uniforms; // Set the uniform layout to the one passed in

		return true; // Return 'true'
	}

	void OpenGLShader::parseSource(const std::string& filepath)
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("ParseShaderSourceSingle");	// Start 'ParseShaderSourceSingle' timer
#endif

		std::fstream handle(filepath, std::ios::in);
		enum { NONE = -1, VERTEX = 0, FRAGMENT } region; // For the region in the shader file

		// If the file is not open
		if (!handle.is_open()) 
			LOG_CRITICAL("Could not open shader file '{0}'", filepath); // Log a critical error

		std::string line; // For the line being got
		std::string src[2]; // One for vertex and one for fragment shader

		// Get a line from the file, while there is a line to be got
		while (getline(handle, line))
		{
			// If in the vertex region set the region and skip to the start of the loop again
			if (line.find("#region Vertex") != std::string::npos) { region = VERTEX; continue; }
			// If in the fragment region set the region and skip to the start of the loop again
			if (line.find("#region Fragment") != std::string::npos) { region = FRAGMENT; continue; }
			// If in the geometry region do nothing
			if (line.find("#region Geometry") != std::string::npos) { region = NONE; continue; }
			// If in the tessalation region do nothing
			if (line.find("#region Tessalation") != std::string::npos) { region = NONE; continue; }

			// If in the vertex region and 'in ' is in the line
			if (region == VERTEX && line.find("in ") != std::string::npos)
			{
				extractBufferLayout(line); // Get the buffer layout
			}

			// If in a region
			if (region != NONE)
			{
				// If 'uniform' is in the line or making a uniform buffer layout
				if (line.find("uniform") != std::string::npos || m_bMakingUniformBuffer)
				{
					// If 'layout' is in the line or making a uniform buffer layout
					if (line.find("layout") != std::string::npos || m_bMakingUniformBuffer)
					{
						// Call the identifUniform function passing in true
						identifyUniform(line, true);
					}
					else
						identifyUniform(line); // Call the identifUniform function, will have defaule false
				}
				// Add the line to the region string
				src[region] += (line + "\n");
			}
		}
		
		handle.close(); // Close the file

		compileAndLink(src[VERTEX], src[FRAGMENT]); // Compile and link the shader

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("ParseShaderSourceSingle"); // Stop 'ParseShaderSourceSingle' timer
#endif
	}

	void OpenGLShader::parseSource(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	{
		// Parsing the vertex shader file
		std::fstream vertexHandle(vertexFilepath, std::ios::in);
		enum { NONE = -1, VERTEX = 0, FRAGMENT }; // For the shader file being read

		// If the file is not open
		if (!vertexHandle.is_open())
			LOG_CRITICAL("Could not open shader file '{0}'", vertexFilepath); // Log a critical error

		std::string line; // For the line being got
		std::string src[2]; // One for vertex and one for fragment shader

		// Get a line from the file, while there is a line to be got
		while (getline(vertexHandle, line))
		{
			// If 'in ' is in the line
			if (line.find("in ") != std::string::npos)
			{
				extractBufferLayout(line); // Get the buffer layout
			}
			// If 'uniform' is in the line or making a uniform buffer layout
			if (line.find("uniform") != std::string::npos || m_bMakingUniformBuffer)
			{
				// If 'layout' is in the line or making a uniform buffer layout
				if (line.find("layout") != std::string::npos || m_bMakingUniformBuffer)
				{
					// Call the identifUniform function passing in true
					identifyUniform(line, true);
				}
				else
					identifyUniform(line); // Call the identifUniform function, will have defaule false
			}
			// Add the line to the vertex string
			src[VERTEX] += (line + "\n");
		}
		// Close the file
		vertexHandle.close();

		// Parsing the fragment shader file
		std::fstream fragmentHandle(fragmentFilepath, std::ios::in);

		// If the file is not open
		if (!fragmentHandle.is_open())
			LOG_CRITICAL("Could not open shader file '{0}'", fragmentFilepath); // Log a critical error

		// Get a line from the file, while there is a line to be got
		while (getline(fragmentHandle, line))
		{
			// If 'uniform' is in the line or making a uniform buffer layout
			if (line.find("uniform") != std::string::npos || m_bMakingUniformBuffer)
			{
				// If 'layout' is in the line or making a uniform buffer layout
				if (line.find("layout") != std::string::npos || m_bMakingUniformBuffer)
				{
					// Call the identifUniform function passing in true
					identifyUniform(line, true);
				}
				else
					identifyUniform(line); // Call the identifUniform function, will have defaule false
			}
			// Add the line to the fragment string
			src[FRAGMENT] += (line + "\n");
		}
		// Close the file
		fragmentHandle.close();

		compileAndLink(src[VERTEX], src[FRAGMENT]); // Compile and link the shader
	}

	void OpenGLShader::extractBufferLayout(const std::string& line)
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("ExtractShaderBufferLayout"); // Start 'ExtractShaderBufferLayout' timer
#endif

		std::string type; // The type

		std::stringstream iss(line);

		// Go through the line
		while (iss >> type)
		{
			// If type is currently 'in'
			if (type == "in")
			{
				// The next word is the type of the uniform so set type to the next word
				iss >> type;
				break; // Leave the loop
			}
		}
		// Add the element to the buffer layout as a ShaderDataType
		m_bufferLayout.addElement(GLSLStrToSDT(type));

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("ExtractShaderBufferLayout"); // Stop 'ExtractShaderBufferLayout' timer
#endif
	}

	void OpenGLShader::identifyUniform(const std::string& line, bool block)
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("IdentifyShaderUniforms"); // Start 'IdentifyShaderUniforms' timer
#endif

		std::string type; // The type
		std::string name; // The name

		std::stringstream iss(line);

		// Go through the line
		while (iss >> type)
		{
			// If 'uniform' is in the line
			if (line.find("uniform") != std::string::npos)
			{
				// If type is currently 'uniform'
				if (type == "uniform")
				{
					// If not in a uniform block
					if (!block)
					{
						// The next word is the type of the uniform so set type to the next word
						iss >> type;
						// The next word is the name of the uniform so set name to the next word
						iss >> name;
						// Remove the ; from the name
						name.erase(std::remove(name.begin(), name.end(), ';'), name.end());

						// Add The name and a new uniform object with the name and type to the map
						m_uniformLayout.insert(std::make_pair(name, new OpenGLUniformElement(name, GLSLStrToSDT(type))));

						break; // Leave the while loop
					}
					else // If in a uniform block
					{
						// If m_bMakingUniformBuffer is false
						if (!m_bMakingUniformBuffer)
						{
							// The next word is the name of the uniform block so set name to the next word
							iss >> name;
							m_sUniformBufferName = name; // Set the stored name to name as the name of the current uniform block

							UniformBufferLayouts::iterator it;
							it = m_uniformBufferLayouts.find(m_sUniformBufferName);
							if (it == m_uniformBufferLayouts.end())
							{
								// Add a uniform buffer layout to the map with its name
								m_uniformBufferLayouts.insert(std::make_pair(m_sUniformBufferName, new UniformBufferLayout()));
								m_bMakingUniformBuffer = true; // Is now making a uniform buffer
							}

							break; // Leave the while loop
						}
					}
				}
			}
			else if (block) // If in a uniform block
			{
				if (type == "{") // If line starts with a {
				{
					break; // Leave the while loop
				}
				else if (type == "};") // If line starts with };
				{
					// Finished making the uniform buffer
					m_bMakingUniformBuffer = false;
					break; // Leave the while loop
				}
				else // If line starts with anything else
				{
					// If the line starts with a tab
					if (type.find("\t") != std::string::npos)
						type.erase(std::remove(type.begin(), type.end(), '\t'), type.end()); // Remove the '\t' from the type

					// Add the uniform element to the layout with the name of the block currently being parsed
					m_uniformBufferLayouts.at(m_sUniformBufferName)->addElement(GLSLStrToSDT(type));
					break; // Leave the while loop
				}
			}
		}

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("IdentifyShaderUniforms"); // Stop 'IdentifyShaderUniforms' timer
#endif
	}

	void OpenGLShader::storeUniformLocations()
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("StoreShaderUniforms"); // Start 'StoreShaderUniforms' timer
#endif

		// For each thing in the uniform layout map
		for (auto& it : m_uniformLayout)
		{
			// Get the type of the uniform
			switch (it.second->getType())
			{
			case ShaderDataType::Int:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					GLint valueInt;
					valueInt = *(GLint*)data; // Get the data from the void pointer
					glUniform1i(location, valueInt); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Int2:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const int* addri;
					addri = (const int*)data; // Get the data from the void pointer
					glUniform2iv(location, 1, addri); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Int3:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const int* addri;
					addri = (const int*)data; // Get the data from the void pointer
					glUniform3iv(location, 1, addri); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Int4:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const int* addri;
					addri = (const int*)data; // Get the data from the void pointer
					glUniform4iv(location, 1, addri); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Float:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					GLfloat valueFloat;
					valueFloat = *(float*)data; // Get the data from the void pointer
					glUniform1f(location, valueFloat); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Float2:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const float* addrf;
					addrf = (const float*)data; // Get the data from the void pointer
					glUniform2fv(location, 1, addrf); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Float3:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const float* addrf;
					addrf = (const float*)data; // Get the data from the void pointer
					glUniform3fv(location, 1, addrf); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Float4:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const float* addrf;
					addrf = (const float*)data; // Get the data from the void pointer
					glUniform4fv(location, 1, addrf); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Mat3:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const float* addrf;
					addrf = (const float*)data; // Get the data from the void pointer
					glUniformMatrix3fv(location, 1, GL_FALSE, addrf); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Mat4:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					const float* addrf;
					addrf = (const float*)data; // Get the data from the void pointer
					glUniformMatrix4fv(location, 1, GL_FALSE, addrf); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Bool:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					GLint valueInt;
					valueInt = *(bool*)data; // Get the data from the void pointer
					glUniform1i(location, valueInt); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::Sampler2D:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
				{
					GLint valueInt;
					valueInt = *(int*)data; // Get the data from the void pointer
					glUniform1i(location, valueInt); // Upload the data
					return true;
				});
				break;
			case ShaderDataType::SamplerCube:
				// Save the location in the object and set the objects upload data function
				it.second->setLocationAndFunction(m_iShaderID,
					[](GLuint location, void* data)
					{
						GLint valueInt;
						valueInt = *(int*)data; // Get the data from the void pointer
						glUniform1i(location, valueInt); // Upload the data
						return true;
					});
				break;
			}
		}

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("StoreShaderUniforms"); // Stop 'StoreShaderUniforms' timer
#endif
	}

	void OpenGLShader::compileAndLink(const std::string& vert, const std::string& frag)
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("CompileAndLinkShader"); // Start 'CompileAndLinkShader' timer
#endif

		m_pcVertSrc = vert.c_str(); // Set the stored vertex shader to the string passed in 

		// Set the vertex shader source and compile it
		glShaderSource(m_iVertShader, 1, &m_pcVertSrc, 0);
		glCompileShader(m_iVertShader);

		GLint m_isCompiled = 0; // To check if it compiled successfully
		glGetShaderiv(m_iVertShader, GL_COMPILE_STATUS, &m_isCompiled); // Check if compiled
		// If not compiled
		if (m_isCompiled == GL_FALSE)
		{
			GLint maxLength = 0; // To get the log length
			glGetShaderiv(m_iVertShader, GL_INFO_LOG_LENGTH, &maxLength); // Get the log length (number of chars)

			std::vector<GLchar> infoLog(maxLength); // To store the log
			glGetShaderInfoLog(m_iVertShader, maxLength, &maxLength, &infoLog[0]); // Get the log
			// Output the log
			LOG_ERROR("Shader compile error: {0}", std::string(infoLog.begin(), infoLog.end()));

			glDeleteShader(m_iVertShader); // Delete the shader
			return; // Leave the function
		}

		m_pcFragSrc = frag.c_str(); // Set the stored fragment shader to the string passed in 

		// Set the fragment shader source and compile it
		glShaderSource(m_iFragShader, 1, &m_pcFragSrc, 0);
		glCompileShader(m_iFragShader);

		glGetShaderiv(m_iFragShader, GL_COMPILE_STATUS, &m_isCompiled); // Check if compiled
		// If not compiled
		if (m_isCompiled == GL_FALSE)
		{
			// To get the log length
			GLint maxLength = 0;
			glGetShaderiv(m_iFragShader, GL_INFO_LOG_LENGTH, &maxLength); // Get the log length (number of chars)

			std::vector<GLchar> infoLog(maxLength); // To store the log
			glGetShaderInfoLog(m_iFragShader, maxLength, &maxLength, &infoLog[0]); // Get the log
			// Output the log
			LOG_ERROR("Shader compile error: {0}", std::string(infoLog.begin(), infoLog.end()));

			// Delete the shaders
			glDeleteShader(m_iFragShader);
			glDeleteShader(m_iVertShader);

			return; // Leave the function
		}

		// Create the program, attach the shaders and link the program
		m_iShaderID = glCreateProgram();
		glAttachShader(m_iShaderID, m_iVertShader);
		glAttachShader(m_iShaderID, m_iFragShader);
		glLinkProgram(m_iShaderID);

		GLint m_isLinked = 0; // To check if it linked successfully
		glGetProgramiv(m_iShaderID, GL_LINK_STATUS, (int*)&m_isLinked); // Check if it linked successfully
		// If it didn't link
		if (m_isLinked == GL_FALSE)
		{
			// To get the log length
			GLint maxLength = 0;
			glGetProgramiv(m_iShaderID, GL_INFO_LOG_LENGTH, &maxLength); // Get the log length (number of chars)

			std::vector<GLchar> infoLog(maxLength); // To store the log
			glGetProgramInfoLog(m_iShaderID, maxLength, &maxLength, &infoLog[0]); // Get the log
			// Output the log
			LOG_ERROR("Shader linking error: {0}", std::string(infoLog.begin(), infoLog.end()));

			// Delete the program and the shaders
			glDeleteProgram(m_iShaderID);
			glDeleteShader(m_iVertShader);
			glDeleteShader(m_iFragShader);

			return; // Leave the function
		}
		// Detach the shaders
		glDetachShader(m_iShaderID, m_iVertShader);
		glDetachShader(m_iShaderID, m_iFragShader);

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("CompileAndLinkShader"); // Stop 'CompileAndLinkShader' timer
#endif
	}
}