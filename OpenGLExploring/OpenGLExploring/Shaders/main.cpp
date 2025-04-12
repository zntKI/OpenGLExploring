#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../shader.hpp"

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

	// Vertices
	float positions[] = {
		//	  position		  color
		//	  x		 y	  red  green  blue
			-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Vertex 01
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 02
			 0.0f,  0.5f, 0.0f, 0.0f, 1.0f  // Vertex 03
	};

	unsigned int VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );
	
	unsigned int VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, 15 * sizeof( float ), positions, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, ( void* ) 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, ( void* ) ( sizeof( float ) * 2 ) );
	glEnableVertexAttribArray( 1 );



	unsigned int indices[] = {
		0, 1, 2
	};

	unsigned int ibo;
	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof( unsigned int ), indices, GL_STATIC_DRAW );


	Shader ourShader( "Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl" );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) )
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );

		/*float timeValue = glfwGetTime();
		float greenValue = ( rand() % 255 ) / 255.f;
		float redValue = ( rand() % 255 ) / 255.f;
		float blueValue = ( rand() % 255 ) / 255.f;
		int vertexColorLocation = glGetUniformLocation( shader, "ourColor" );
		glUniform4f( vertexColorLocation, greenValue, redValue, blueValue, 1.f );*/

		glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}