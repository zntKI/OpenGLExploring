#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	// program ID
	unsigned int ID;

	Shader( const char* vertexPath, const char* fragmentPath );
	~Shader();
	
	void use();

	void setBool( const std::string& name, bool value ) const;
	void setInt( const std::string& name, int value ) const;
	void setFloat( const std::string& name, float value ) const;

	/// <summary>
	/// DON'T USE
	/// </summary>
	void setVec3( const std::string& name, glm::vec3 vec ) const;
	void setVec3( const std::string& name, float x, float y, float z ) const;
	void setMatrix4( const std::string& name, glm::mat4 matrix ) const;

};