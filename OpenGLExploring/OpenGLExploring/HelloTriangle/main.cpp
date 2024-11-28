#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int compileShader( unsigned int type, const std::string& source )
{
	// Creates the shader and returns its memory address
	unsigned int id = glCreateShader( type );

	// Copy source into a local variable to prevent original variable going out of scope and us ending up trying to access invalid memory
	std::string srcStr = source;
	// Get the memory location of the first character of the string
	const char* src = srcStr.c_str();
	// Sets the source of the shader by passing by location of the first character and nullptr for null-terminated string to instrunct to go the end of the source
	glShaderSource( id, 1, &src, nullptr );
	// Compiles the shader
	glCompileShader( id );

	// Get the result of the compilation process
	int result;
	glGetShaderiv( id, GL_COMPILE_STATUS, &result );
	if ( result == GL_FALSE ) // If it has failed
	{
		// Get the length of the error message
		int length;
		glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
		// Dynamically allocate a char array on the stack and then pass it to retrieve the error message
		char* message = ( char* ) _malloca( length * sizeof( char ) );
		glGetShaderInfoLog( id, length, &length, message );

		// Log type of shader and error message
		std::cout << "Failed to compile " <<
			( type == GL_VERTEX_SHADER ? "vertex" : "fragment" )
			<< " shader!" << std::endl;
		std::cout << message << std::endl;

		// Frees memory - opposite of glCreateShader()
		glDeleteShader( id );

		return 0;
	}

	return id;
}

static unsigned int createShader( const std::string& vertexShader, const std::string& fragmentShader )
{
	// Creates a program for shaders to be attatched to
	unsigned int program = glCreateProgram();

	// Compile the actual shaders from the source code we provided
	unsigned int vs = compileShader( GL_VERTEX_SHADER, vertexShader );
	unsigned int fs = compileShader( GL_FRAGMENT_SHADER, fragmentShader );

	// For following code, check the docs for more details:
	// 
	// When linking the shaders into a program, it links the output of each shader to the inputs of the next shader.
	// This is also where you'll get LINKING ERRORS if your outputs and inputs do not match!!!
	// 
	// Attaches shaders to the program
	glAttachShader( program, vs );
	glAttachShader( program, fs );
	// Links and validates program
	glLinkProgram( program );
	glValidateProgram( program );

	// Frees memory after having attatched shaders to a program - opposite of glCreateShader()
	glDeleteShader( vs );
	glDeleteShader( fs );

	return program;
}

int main( void )
{
	GLFWwindow* window;

	/* Initialize the library */
	if ( !glfwInit() )
		return -1;
	/*glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );*/

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
		//	  x		 y
			-0.5f, -0.5f, // Vertex 01
			 0.5f, -0.5f, // Vertex 02
			 0.0f,  0.5f, // Vertex 03
			//-0.5f,  0.5f  // Vertex 04
	};


	// Instruct OpenGL to generate 1 VAO
	unsigned int vao1;
	glGenVertexArrays( 1, &vao1 );
	// then binds it so that for the next VBO, it register the buffer's attributes so that when switching from things to draw,
	// you would not have to redefine the attributes for each shape/object but only bind its VAO.
	glBindVertexArray( vao1 );

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
	// STRIDE: the byte size distance to the start of the same attribute in the next vertex
	// POINTER: the position in bytes of that attribute inside the vertex
	//			Example: if after 'xy-pos' attribute we have another attribute that is 'color' for ex, the position of the 'color' attribute would be '8' since we would have to jump 2 * 'float_size' amount of space in memory to get there
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 2, 0 );
	// Instruct OpenGL to enable an attribute of given index from a vertex from a currently bound buffer
	glEnableVertexAttribArray( 0 );


	// Create a second triangle so that in run-time I can swith between the two

	float positions2[] = {
		//	  x		 y
			 0.0f, -0.5f, // Vertex 01
			 0.5f,  0.5f, // Vertex 02
			-0.5f,  0.5f, // Vertex 03
			//-0.5f,  0.1f  // Vertex 04
	};

	unsigned int vao2;
	glGenVertexArrays( 1, &vao2 );
	glBindVertexArray( vao2 );

	unsigned int buff2;
	glGenBuffers( 1, &buff2 );
	glBindBuffer( GL_ARRAY_BUFFER, buff2 );
	glBufferData( GL_ARRAY_BUFFER, 6 * sizeof( float ), positions2, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ), ( void* ) 0 );
	glEnableVertexAttribArray( 0 );


	// Index buffer:
	// Both triangles utilize the same index buffer

	unsigned int indices[] = {
		0, 1, 2,
		/*2, 3, 0*/
	};


	unsigned int ibo;
	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof( unsigned int ), indices, GL_STATIC_DRAW );


	// switch to first VAO to set it up properly with the index buffer (like the second one above)
	// to prevent undefined behavior
	glBindVertexArray( vao1 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );



	// Define shaders (BAD way)
	const std::string vs = R"glsl(
#version 330 core

layout(location = 0) in vec4 position;

void main(){
   gl_Position = position;
}
)glsl";
	const std::string fs = R"glsl(
#version 330 core

out vec4 color;

void main(){
   color = vec4(1.0, 1.0, 0.0, 1.0);
}
)glsl";

	// Create(compile) the shaders we defined and get back the program id to which the shaders have been attached
	unsigned int shader = createShader( vs, fs );
	// BIND(select) the shader program for OpenGL to use
	glUseProgram( shader );


	bool shouldDisplayFirst = true;

	int framesToShow = 30;
	int counter = 1;
	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) )
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );

		if ( counter > framesToShow )
		{
			// Swap shapes
			glBindVertexArray( shouldDisplayFirst ? vao2 : vao1 );

			// Usually you would also swap index buffers but since we use the same one for both triangles it is not necessary

			shouldDisplayFirst = !shouldDisplayFirst;
			counter = 0;
		}
		glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );
		//glDrawArrays( GL_TRIANGLES, 0, 3 );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();

		counter++;
	}

	// Free memory - opposite of glCreateProgram()
	glDeleteProgram( shader );

	glfwTerminate();
	return 0;
}