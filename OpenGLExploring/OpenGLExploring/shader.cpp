#include "shader.hpp"

#include <fstream>
#include <sstream>

#include <GL/glew.h>

Shader::Shader( const char* vertexPath, const char* fragmentPath )
{
	// 1. read code from file

	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
	fShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );

	try
	{
		vShaderFile.open( vertexPath );
		fShaderFile.open( fragmentPath );

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch ( std::ifstream::failure e )
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSSFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	// 2. create and compile shaders

	unsigned int vertex, fragment;
	int success;
	char infoLog[ 512 ];

	vertex = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex, 1, &vShaderCode, nullptr );
	glCompileShader( vertex );

	glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
	if ( success == GL_FALSE )
	{
		glGetShaderInfoLog( vertex, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}

	fragment = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment, 1, &fShaderCode, nullptr );
	glCompileShader( fragment );

	glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
	if ( success == GL_FALSE )
	{
		glGetShaderInfoLog( fragment, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}


	// 3. shader program

	this->ID = glCreateProgram();
	glAttachShader( this->ID, vertex );
	glAttachShader( this->ID, fragment );
	glLinkProgram( this->ID );

	// print linking errors if any
	glGetProgramiv( this->ID, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( this->ID, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
			infoLog << std::endl;
	}

	this->use();


	// 4. delete shaders
	glDeleteShader( vertex );
	glDeleteShader( fragment );
}

Shader::~Shader()
{
	glDeleteProgram( this->ID );
}

void Shader::use()
{
	glUseProgram( this->ID );
}

void Shader::setBool( const std::string& name, bool value ) const
{
	glUniform1i( glGetUniformLocation( this->ID, name.c_str() ), ( int ) value );
}

void Shader::setInt( const std::string& name, int value ) const
{
	glUniform1i( glGetUniformLocation( this->ID, name.c_str() ), value );
}

void Shader::setFloat( const std::string& name, float value ) const
{
	glUniform1f( glGetUniformLocation( this->ID, name.c_str() ), value );
}

void Shader::setVec2( const std::string& name, glm::vec2 vec ) const
{
	glUniform2fv( glGetUniformLocation( this->ID, name.c_str() ), 1, glm::value_ptr( vec ) );
}

void Shader::setVec3( const std::string& name, glm::vec3 vec ) const
{
	glUniform3fv( glGetUniformLocation( this->ID, name.c_str() ), 1, glm::value_ptr(vec));
}

void Shader::setVec3( const std::string& name, float x, float y, float z ) const
{
	glUniform3f( glGetUniformLocation( this->ID, name.c_str() ), x, y, z );
}

void Shader::setVec4( const std::string& name, glm::vec4 vec ) const
{
	glUniform4fv( glGetUniformLocation( this->ID, name.c_str() ), 1, glm::value_ptr( vec ) );
}

void Shader::setMatrix4( const std::string& name, glm::mat4 matrix ) const
{
	glUniformMatrix4fv( glGetUniformLocation( this->ID, name.c_str() ), 1, GL_FALSE, glm::value_ptr( matrix ) );
}

void Shader::setMatrix3( const std::string& name, glm::mat3 matrix ) const
{
	glUniformMatrix3fv( glGetUniformLocation( this->ID, name.c_str() ), 1, GL_FALSE, glm::value_ptr( matrix ) );
}