#pragma once

#include <iostream>

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

};