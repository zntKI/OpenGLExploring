#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// TODO: make the colors rotate around the corners of the triangle

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


	// Define shaders (BAD way)
	const std::string vs = R"glsl(
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

out vec3 ourColor;
	
void main(){
   gl_Position = position;

   ourColor = color;
}
)glsl";
	const std::string fs = R"glsl(
#version 330 core

in vec3 ourColor;

out vec4 outColor;

void main(){
   outColor = vec4(ourColor, 1.0);
}
)glsl";

	unsigned int shader = createShader( vs, fs );
	glUseProgram( shader );


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

	glDeleteProgram( shader );

	glfwTerminate();
	return 0;
}