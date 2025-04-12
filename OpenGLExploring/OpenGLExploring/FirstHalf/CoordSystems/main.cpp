#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../shader.hpp"


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
	//float vertices[] = {
	//	//	  position			color			   texture coor
	//	//	  x		 y	   z	red  green  blue    s      r
	//		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f, // Vertex 01
	//		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f, // Vertex 02
	//		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, // Vertex 03
	//		-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f,  0.0f,  1.0f, // Vertex 04
	//};

	float vertices[] = {
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
	};


	unsigned int VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	unsigned int VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// position
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, ( void* ) 0 );
	glEnableVertexAttribArray( 0 );
	//// color
	//glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) ( sizeof( float ) * 3 ) );
	//glEnableVertexAttribArray( 1 );
	// texture coordinates
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, ( void* ) ( sizeof( float ) * 3 ) );
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

	Shader ourShader( "FirstHalf/CoordSystems/vertexShader.glsl", "FirstHalf/CoordSystems/fragmentShader.glsl" );

	ourShader.use();
	ourShader.setInt( "texture1", 0 );
	ourShader.setInt( "texture2", 1 );

#pragma endregion

	//// Model matrix
	//glm::mat4 model( 1.f );
	//model = glm::rotate( model, glm::radians( -55.f ), glm::vec3( 1.f, 0.f, 0.f ) );

	// View matrix
	glm::mat4 view( 1.f );
	view = glm::translate( view, glm::vec3( 0.f, 0.f, -3.f ) );

	// Projection matrix
	int screenWidth, screenHeight;
	glfwGetWindowSize( window, &screenWidth, &screenHeight );
	glm::mat4 projection = glm::perspective( glm::radians( 45.f ), screenWidth / ( float ) screenHeight, .1f, 100.f );

	//ourShader.setMatrix4( "model", model );
	ourShader.setMatrix4( "view", view );
	ourShader.setMatrix4( "projection", projection );


	// Usually set within the rendering loop but in our case that is not necessary since our program is static and does not require that
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture1 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, texture2 );

	glm::vec3 cubePositions[] = {
glm::vec3( 0.0f, 0.0f, 0.0f ),
glm::vec3( 2.0f, 5.0f, -15.0f ),
glm::vec3( -1.5f, -2.2f, -2.5f ),
glm::vec3( -3.8f, -2.0f, -12.3f ),
glm::vec3( 2.4f, -0.4f, -3.5f ),
glm::vec3( -1.7f, 3.0f, -7.5f ),
glm::vec3( 1.3f, -2.0f, -2.5f ),
glm::vec3( 1.5f, 2.0f, -2.5f ),
glm::vec3( 1.5f, 0.2f, -1.5f ),
glm::vec3( -1.3f, 1.0f, -1.5f )
	};

	// Use z-buffer
	glEnable( GL_DEPTH_TEST );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) )
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glBindVertexArray( VAO );

		for ( unsigned int i = 0; i < 10; i++ )
		{
			glm::mat4 model = glm::mat4( 1.f );
			model = glm::translate( model, cubePositions[ i ] );

			float angle = 20.f * i;
			if ( i % 3 == 0 )
			{
				angle = glfwGetTime() * 25.0f;
			}

			model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.f, .3f, .5f ) );

			ourShader.setMatrix4( "model", model );

			glDrawArrays( GL_TRIANGLES, 0, 36 );
		}

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}