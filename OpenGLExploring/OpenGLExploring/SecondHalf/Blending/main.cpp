#include "../../Model.hpp"
#include "../../shader.hpp"
#include "../../camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <map>


void scrollCallback( GLFWwindow* window, double xpos, double ypos );
void mouseCallback( GLFWwindow* window, double xposIn, double yposIn );
void processInput( GLFWwindow* window );
unsigned int loadTexture( char const* path );


const int windowWidth = 1920, windowHeight = 1080;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Camera vars
Camera camera( glm::vec3( 0.f, 0.f, 3.f ) );
bool firstMouse = true;
float lastX = windowWidth / 2.f, lastY = windowHeight / 2.f;

int main()
{

#pragma region SetUp

	GLFWwindow* window;

	/* Initialize the library */
	if ( !glfwInit() )
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow( windowWidth, windowHeight, "Blending", NULL, NULL );
	if ( !window ) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent( window );

	if ( glewInit() != GLEW_OK ) {
		std::cout << "Glew failed to init!" << std::endl;
		return -1;
	}

	stbi_set_flip_vertically_on_load( true );

	// Use z-buffer
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );

	std::cout << glGetString( GL_VERSION ) << std::endl;

#pragma endregion

#pragma region TransparencyObjects

	float transparentVertices[] = {
		// pos				 // tex
		0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  1.0f,
	};

	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays( 1, &transparentVAO );
	glGenBuffers( 1, &transparentVBO );
	glBindVertexArray( transparentVAO );
	glBindBuffer( GL_ARRAY_BUFFER, transparentVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( transparentVertices ), transparentVertices, GL_STATIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* ) 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* ) ( 3 * sizeof( float ) ) );

	glBindVertexArray( 0 );

	unsigned int transparentTexture = loadTexture( "Assets/Images/grass.png" );

	std::vector<glm::vec3> vegetation
	{
		glm::vec3( -1.5f, 0.0f, -0.48f ),
		glm::vec3( 1.5f, 0.0f, 0.51f ),
		glm::vec3( 0.0f, 0.0f, 0.7f ),
		glm::vec3( -0.3f, 0.0f, -2.3f ),
		glm::vec3( 0.5f, 0.0f, -0.6f )
	};

#pragma endregion

#pragma region BlendingObjects

	float blendingVertices[] = {
		// pos				 // tex
		0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  1.0f,
	};

	unsigned int blendingVAO, blendingVBO;
	glGenVertexArrays( 1, &blendingVAO );
	glGenBuffers( 1, &blendingVBO );
	glBindVertexArray( blendingVAO );
	glBindBuffer( GL_ARRAY_BUFFER, blendingVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( blendingVertices ), blendingVertices, GL_STATIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* ) 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* ) ( 3 * sizeof( float ) ) );

	glBindVertexArray( 0 );

	unsigned int blendingTexture = loadTexture( "Assets/Images/blending_transparent_window.png" );

	std::vector<glm::vec3> windows
	{
		glm::vec3( -1.35f, 0.0f, -0.65f ),
		glm::vec3( 1.42f, 0.0f, 0.66f ),
		glm::vec3( 0.12f, 0.0f, 0.59f ),
		glm::vec3( -0.44f, 0.0f, -2.18f ),
		glm::vec3( 0.63f, 0.0f, -0.52f )
	};

#pragma endregion

#pragma region ShaderGen

	Shader discardShader( "SecondHalf/Blending/Shaders/vertexShader.glsl", "SecondHalf/Blending/Shaders/discardFragmentShader.glsl" );
	discardShader.setInt( "u_Texture", 0 );

	Shader blendShader( "SecondHalf/Blending/Shaders/vertexShader.glsl", "SecondHalf/Blending/Shaders/blendFragmentShader.glsl" );
	discardShader.setInt( "u_Texture", 1 );

#pragma endregion

	// input
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouseCallback );
	glfwSetScrollCallback( window, scrollCallback );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	while ( !glfwWindowShouldClose( window ) ) {
		glClearColor( .12f, .12f, .12f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		// Keep track of time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Input
		processInput( window );

#pragma region DefiningMatrices

		// Bind the shader for objects
		discardShader.use();

		// View matrix
		glm::mat4 view = camera.GetViewMatrix();
		// Projection matrix
		int screenWidth, screenHeight;
		glfwGetWindowSize( window, &screenWidth, &screenHeight );
		glm::mat4 projection = glm::perspective( glm::radians( camera.Zoom ), screenWidth / ( float ) screenHeight, .1f, 100.f );

		discardShader.setMatrix4( "u_Projection", projection );
		discardShader.setMatrix4( "u_View", view );

		glm::mat4 model = glm::mat4( 1.0f );


		glActiveTexture( GL_TEXTURE0 );
		glBindVertexArray( transparentVAO );
		glBindTexture( GL_TEXTURE_2D, transparentTexture );

		for ( unsigned int i = 0; i < vegetation.size(); i++ )
		{
			model = glm::mat4( 1.0f );
			model = glm::translate( model, vegetation[ i ] );
			discardShader.setMatrix4( "u_Model", model );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
		}


		blendShader.use();

		blendShader.setMatrix4( "u_Projection", projection );
		blendShader.setMatrix4( "u_View", view );

		glActiveTexture( GL_TEXTURE1 );
		glBindVertexArray( blendingVAO );
		glBindTexture( GL_TEXTURE_2D, blendingTexture );

		std::map<float, glm::vec3> sorted;
		for ( unsigned int i = 0; i < windows.size(); i++ ) {
			float distance = glm::length( camera.Position - windows[ i ] );
			sorted[ distance ] = windows[ i ];
		}

		for ( std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it !=
			sorted.rend(); ++it ) {
			model = glm::mat4( 1.0f );
			model = glm::translate( model, it->second );
			blendShader.setMatrix4( "u_Model", model );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
		}

#pragma endregion

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void scrollCallback( GLFWwindow* window, double xpos, double ypos )
{
	camera.ProcessMouseScroll( static_cast< float >( ypos ) );
}

void mouseCallback( GLFWwindow* window, double xposIn, double yposIn )
{
	float xpos = static_cast< float >( xposIn );
	float ypos = static_cast< float >( yposIn );

	if ( firstMouse ) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement( xoffset, yoffset );
}

void processInput( GLFWwindow* window )
{
	if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
		glfwSetWindowShouldClose( window, true );
	}

	if ( glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS )
		camera.ProcessKeyboard( MOVE_FAST, deltaTime );
	else
		camera.ProcessKeyboard( SLOW_DOWN, deltaTime );

	if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		camera.ProcessKeyboard( FORWARD, deltaTime );
	if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
		camera.ProcessKeyboard( BACKWARD, deltaTime );
	if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
		camera.ProcessKeyboard( LEFT, deltaTime );
	if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
		camera.ProcessKeyboard( RIGHT, deltaTime );
	if ( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS )
		camera.ProcessKeyboard( UP, deltaTime );
	if ( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )
		camera.ProcessKeyboard( DOWN, deltaTime );
}

unsigned int loadTexture( char const* path )
{
	unsigned int textureID;
	glGenTextures( 1, &textureID );

	int width, height, nrComponents;
	unsigned char* data = stbi_load( path, &width, &height, &nrComponents, 0 );
	if ( data ) {
		GLenum format;
		if ( nrComponents == 1 )
			format = GL_RED;
		else if ( nrComponents == 3 )
			format = GL_RGB;
		else if ( nrComponents == 4 )
			format = GL_RGBA;

		glBindTexture( GL_TEXTURE_2D, textureID );
		glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		stbi_image_free( data );
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free( data );
	}

	return textureID;
}