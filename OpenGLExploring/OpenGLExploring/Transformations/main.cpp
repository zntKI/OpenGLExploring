#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.hpp"


// TODO: make the colors rotate around the corners of the triangle

int main( void )
{
#pragma region SetUp

	srand( time( 0 ) );

	GLFWwindow* window;

	/* Initialize the library */
	if ( !glfwInit() )
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
	if ( !window )
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent( window );

	if ( glewInit() != GLEW_OK )
	{
		std::cout << "Glew failed to init!" << std::endl;
		return -1;
	}

	std::cout << glGetString( GL_VERSION ) << std::endl;

#pragma endregion

#pragma region TextureGen

	// Generate an array of textures (in our case only 1) and stores them in the var
	unsigned int texture1;
	glGenTextures( 1, &texture1 );
	glBindTexture( GL_TEXTURE_2D, texture1 );

	// set the texture wrapping/filtering options (on currently bound texture)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	int tWidth, tHeight, tNrChannels;

	stbi_set_flip_vertically_on_load( true );
	unsigned char* data = stbi_load( "Assets/Images/container.jpg", &tWidth, &tHeight,
		&tNrChannels, 0 );

	if ( data )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free( data );



	unsigned int texture2;
	glGenTextures( 1, &texture2 );
	glBindTexture( GL_TEXTURE_2D, texture2 );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	data = stbi_load( "Assets/Images/awesomeface.png", &tWidth, &tHeight,
		&tNrChannels, 0 );

	if ( data )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free( data );

#pragma endregion

#pragma region ObjectGen

	// Vertices
	float vertices[] = {
		//	  position			color			   texture coor
		//	  x		 y	   z	red  green  blue    s      r
			 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f, // Vertex 01
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f, // Vertex 02
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, // Vertex 03
			-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f,  0.0f,  1.0f, // Vertex 04
	};

	unsigned int VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	unsigned int VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// position
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) 0 );
	glEnableVertexAttribArray( 0 );
	// color
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) ( sizeof( float ) * 3 ) );
	glEnableVertexAttribArray( 1 );
	// texture coordinates
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) ( sizeof( float ) * 6 ) );
	glEnableVertexAttribArray( 2 );



	unsigned int indices[] = {
		2, 1, 0,
		0, 3, 2
	};

	unsigned int ibo;
	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

#pragma endregion

#pragma region ShaderGen

	Shader ourShader( "Transformations/vertexShader.glsl", "Transformations/fragmentShader.glsl" );

	ourShader.use();
	ourShader.setInt( "texture1", 0 );
	ourShader.setInt( "texture2", 1 );

#pragma endregion


	// Create an identity matrix
	glm::mat4 trans( 1.f );
	// Apply rotation around a (normalized) axys
	trans = glm::rotate( trans, glm::radians( 90.f ), glm::vec3( 0.f, 0.f, 1.f ) );
	// First apply the scaling, then the rotation (right-to-left) since OpenGL is column-oriented
	trans = glm::scale( trans, glm::vec3( .5f, .5f, .5f ) );


	ourShader.setMatrix4( "transform", trans );


	// Usually set within the rendering loop but in our case that is not necessary since our program is static and does not require that
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture1 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, texture2 );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) )
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );

		/*trans = glm::mat4( 1.f );
		trans = glm::rotate( trans, ( float ) glfwGetTime(), glm::vec3( 0.f, 0.f, 1.f ) );
		trans = glm::translate( trans, glm::vec3( .5f, -.5f, 0.f ) );*/

		trans = glm::mat4( 1.f );
		float time = ( float ) glfwGetTime();
		trans = glm::translate( trans, glm::vec3( cos( time ) / 2.f, sin( time ) / 2.f, 0.f ) );
		trans = glm::scale( trans, glm::vec3( cos( time ), sin( time ), 0.f ) );
		trans = glm::rotate( trans, time, glm::vec3( 0.f, 0.f, -1.f ) );

		ourShader.setMatrix4( "transform", trans );

		glBindVertexArray( VAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}