#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main( void )
{
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
	float positions[ 6 ] = {
		//	  x		 y
			-0.5f, -0.5f, // Vertex 01
			 0.5f, -0.5f, // Vertex 02
			 0.0f,  0.5f  // Vertex 03
	};

	// Insruct OpenGL to generate 1 buffer in the VRAM
	// and then set the location of that buffer to our local variable which actually represents a pointer to that buffer
	unsigned int buff;
	glGenBuffers( 1, &buff );

	// Instruct OpenGL to bind(select) a buffer we provide
	// Remember: this step is NECESSARY to do before we try to manipulate the buffer in any way
	//			 This is because OpenGL operates like a state machine, therefore we must 'set the right state' so that we can then utilize it
	glBindBuffer( GL_ARRAY_BUFFER, buff );

	// Instruct OpenGL to add the given data to the BOUND buffer
	// 
	// SIZE: the amount of memory that buffer is going to store
	// DATA: the array we set to the buffer
	// USAGE: with this we give a hint to OpenGL how to deal with that data:
	//		  Providing GL_STATIC_DRAW instructs that we have the intention of setting the data only once and then only reading it;
	//		  However, that does not mean that if we should modify it later on, we are forbidden to do so; no - we can, but it may be slower due to the way we have instructed OpenGL to interpret that data initially
	glBufferData( GL_ARRAY_BUFFER, 6 * sizeof( float ), positions, GL_STATIC_DRAW );

	// Instruct OpenGL to define an attribute from a vertex from the currently bound buffer so that we can later utilize it in our shader
	// A Vertex is comprised of Attributes. An example of attributes could be 'Position', 'Color', 'Normal', etc.
	//
	// INDEX: the index which we will then use to access this attribute in our shader
	// SIZE: the count of elements in that attribute: 1, 2, 3 or 4 - in case of 'Position', it is 2 since it is a vec2
	// STRIDE: the actual byte size of that attribute
	// POINTER: the position in bytes of that attribute inside the vertex
	//			Example: if after 'xy-pos' attribute we have another attribute that is 'color' for ex, the position of the 'color' attribute would be '8' since we would have to jump 2 * 'float_size' amount of space in memory to get there
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 2, 0 );
	// Instruct OpenGL to enable an attribute of given index from a vertex from a currently bound buffer
	glEnableVertexAttribArray( 0 );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) )
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );

		glDrawArrays( GL_TRIANGLES, 0, 3 );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}