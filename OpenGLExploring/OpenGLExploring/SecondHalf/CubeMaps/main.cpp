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


void scrollCallback( GLFWwindow* window, double xpos, double ypos );
void mouseCallback( GLFWwindow* window, double xposIn, double yposIn );
void processInput( GLFWwindow* window );
unsigned int loadCubeMap( std::vector<std::string>& faces );


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
	window = glfwCreateWindow( windowWidth, windowHeight, "Hello World", NULL, NULL );
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

	//stbi_set_flip_vertically_on_load( true );

	// Use z-buffer
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	std::cout << glGetString( GL_VERSION ) << std::endl;

#pragma endregion

	Model backpack( "Assets/Models/Backpack/backpack.obj" );

#pragma region ShaderGen

	// object shader
	Shader objectShader( "SecondHalf/CubeMaps/Shaders/reflectAndRefractVertexShader.glsl", "SecondHalf/CubeMaps/Shaders/refractFragmentShader.glsl" );

	objectShader.use();

	//// Material
	//objectShader.setFloat( "u_Material.shininess", 32.f );

	//// Directional light
	//objectShader.setVec3( "u_DirectionalLight.direction", glm::vec3( -.2f, -1.f, -.3f ) );
	//objectShader.setVec3( "u_DirectionalLight.ambient", glm::vec3( 0.05f, 0.05f, 0.05f ) );
	//objectShader.setVec3( "u_DirectionalLight.diffuse", glm::vec3( 0.4f, 0.4f, 0.4f ) );
	//objectShader.setVec3( "u_DirectionalLight.specular", glm::vec3( 0.5f, 0.5f, 0.5f ) );

	objectShader.setInt( "u_Skybox", 0 );


	Shader skyboxShader( "SecondHalf/CubeMaps/Shaders/skyboxVertexShader.glsl", "SecondHalf/CubeMaps/Shaders/skyboxFragmentShader.glsl" );

	skyboxShader.use();
	skyboxShader.setInt( "u_Skybox", 0 );

#pragma endregion

#pragma region CubeMapGen

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays( 1, &skyboxVAO );
	glBindVertexArray( skyboxVAO );
	glGenBuffers( 1, &skyboxVBO );
	glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void* ) 0 );

	std::vector<std::string> faces
	{
		"Assets/Images/skybox/right.jpg",
		"Assets/Images/skybox/left.jpg",
		"Assets/Images/skybox/top.jpg",
		"Assets/Images/skybox/bottom.jpg",
		"Assets/Images/skybox/front.jpg",
		"Assets/Images/skybox/back.jpg",
	};

	unsigned int cubeMapTex = loadCubeMap(faces);

#pragma endregion

	// input
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouseCallback );
	glfwSetScrollCallback( window, scrollCallback );

	while ( !glfwWindowShouldClose( window ) ) {
		glClearColor( 1.f, 1.f, 1.f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		// Keep track of time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Input
		processInput( window );

#pragma region DefiningMatrices

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
		model = glm::translate( model, glm::vec3( 0.0f, 0.0f, 0.0f ) );
		model = glm::scale( model, glm::vec3( .5f, .5f, .5f ) );	// it's a bit too big for our scene, so scale it down
		objectShader.setMatrix4( "u_Model", model );

		// Calculate normal matrix once in cpp code, instead of doing it for every vertex for performance reasons
		glm::mat3 normalMatrix = glm::mat3( model );
		normalMatrix = glm::transpose( normalMatrix );
		normalMatrix = glm::inverse( normalMatrix );
		objectShader.setMatrix3( "u_NormalMatrix", normalMatrix );
		

		glDepthFunc( GL_LESS );

		backpack.Draw( objectShader );


		glDepthFunc( GL_LEQUAL );

		skyboxShader.use();
		skyboxShader.setMatrix4( "u_Projection", projection );

		skyboxShader.setMatrix4( "u_View", glm::mat4( glm::mat3( view ) ) );

		glBindVertexArray( skyboxVAO );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_CUBE_MAP, cubeMapTex );
		glDrawArrays( GL_TRIANGLES, 0, 36 );

#pragma endregion

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

unsigned int loadCubeMap( std::vector<std::string>& faces )
{
	unsigned int textureID;
	glGenTextures( 1, &textureID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

	int width, height, nrChannels;
	unsigned char* data;
	for ( unsigned int i = 0; i < faces.size(); i++ )
	{
		data = stbi_load( faces[ i ].c_str(), &width, &height, &nrChannels, 0 );

		if ( data )
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );

			stbi_image_free( data );
		}
		else
		{
			std::cout << "Cubemap failed to load at path: " << faces[ i ] << "\n";
			stbi_image_free( data );
		}
	}

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	return textureID;
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