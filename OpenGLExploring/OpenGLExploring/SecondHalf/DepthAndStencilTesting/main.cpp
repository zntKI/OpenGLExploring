#include "../../Model.hpp"
#include "../../shader.hpp"
#include "../../camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


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
	window = glfwCreateWindow( windowWidth, windowHeight, "Depth and Stencil testing", NULL, NULL );
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
	glEnable( GL_STENCIL_TEST );

	std::cout << glGetString( GL_VERSION ) << std::endl;

#pragma endregion

	Model backpack( "Assets/Models/Backpack/backpack.obj" );
	Model backpack2( "Assets/Models/Backpack/backpack.obj" );

#pragma region ShaderGen

	// object shader
	Shader objectShader( "SecondHalf/DepthAndStencilTesting/Shaders/vertexShader.glsl", "SecondHalf/DepthAndStencilTesting/Shaders/fragmentShader.glsl" );

	objectShader.use();

	// Material
	objectShader.setFloat( "u_Material.shininess", 32.f );

	// Directional light
	objectShader.setVec3( "u_DirectionalLight.direction", glm::vec3( -.2f, -1.f, -.3f ) );
	objectShader.setVec3( "u_DirectionalLight.ambient", glm::vec3( 0.05f, 0.05f, 0.05f ) );
	objectShader.setVec3( "u_DirectionalLight.diffuse", glm::vec3( 0.4f, 0.4f, 0.4f ) );
	objectShader.setVec3( "u_DirectionalLight.specular", glm::vec3( 0.5f, 0.5f, 0.5f ) );


	Shader colorShader( "SecondHalf/DepthAndStencilTesting/Shaders/colorVertexShader.glsl", "SecondHalf/DepthAndStencilTesting/Shaders/colorFragmentShader.glsl" );

#pragma endregion

	// input
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouseCallback );
	glfwSetScrollCallback( window, scrollCallback );

	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

	while ( !glfwWindowShouldClose( window ) ) {
		glClearColor( .12f, .12f, .12f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );


		// Keep track of time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Input
		processInput( window );

#pragma region DefiningMatrices

		glStencilFunc( GL_ALWAYS, 1, 0xFF );
		glStencilMask( 0xFF );


		// Bind the shader for objects
		objectShader.use();

		// View matrix
		glm::mat4 view = camera.GetViewMatrix();
		// Projection matrix
		int screenWidth, screenHeight;
		glfwGetWindowSize( window, &screenWidth, &screenHeight );
		glm::mat4 projection = glm::perspective( glm::radians( camera.Zoom ), screenWidth / ( float ) screenHeight, .1f, 100.f );

		objectShader.setMatrix4( "u_Projection", projection );
		objectShader.setMatrix4( "u_View", view );
		objectShader.setVec3( "u_ViewPos", camera.Position );

		glm::mat4 model = glm::mat4( 1.0f );
		model = glm::scale( model, glm::vec3( .5f, .5f, .5f ) );	// it's a bit too big for our scene, so scale it down
		objectShader.setMatrix4( "u_Model", model );

		// Calculate normal matrix once in cpp code, instead of doing it for every vertex for performance reasons
		glm::mat3 normalMatrix = glm::mat3( model );
		normalMatrix = glm::transpose( normalMatrix );
		normalMatrix = glm::inverse( normalMatrix );
		objectShader.setMatrix3( "u_NormalMatrix", normalMatrix );

		backpack.Draw( objectShader );

		glm::mat4 model2 = glm::translate( model, glm::vec3( 5.0f, 0.0f, 0.0f ) );
		objectShader.setMatrix4( "u_Model", model2 );

		backpack2.Draw( objectShader );
		

		glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
		glStencilMask( 0xFF );
		glDisable( GL_DEPTH_TEST );


		colorShader.use();

		colorShader.setMatrix4( "u_Projection", projection );
		colorShader.setMatrix4( "u_View", view );
		colorShader.setMatrix4( "u_Model", model );

		backpack.Draw( colorShader );

		colorShader.setMatrix4( "u_Model", model2 );

		backpack2.Draw( colorShader );


		glStencilMask( 0xFF );
		glStencilFunc( GL_ALWAYS, 1, 0xFF );
		glEnable( GL_DEPTH_TEST );

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