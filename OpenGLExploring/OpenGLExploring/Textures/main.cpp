#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include "../shader.hpp"

#include <stb_image.h>

// TODO: make the colors rotate around the corners of the triangle

int main( void )
{
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


	// Generating textures:

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


	Shader ourShader( "Textures/vertexShader.glsl", "Textures/fragmentShader.glsl" );

	ourShader.use();
	ourShader.setInt( "texture1", 0 );
	ourShader.setInt( "texture2", 1 );

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

		glBindVertexArray( VAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();

		float mixAmount;
		glGetUniformfv( ourShader.ID, glGetUniformLocation( ourShader.ID, "mixAmount" ), &mixAmount );

		if ( glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS )
			mixAmount += 0.01;
		else if ( glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS )
			mixAmount -= 0.01;
		mixAmount = std::max( 0.f, std::min( 1.f, mixAmount ) );

		ourShader.setFloat( "mixAmount", mixAmount );
	}

	glfwTerminate();
	return 0;
}