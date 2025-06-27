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
	window = glfwCreateWindow( windowWidth, windowHeight, "Instancing", NULL, NULL );
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
	glDepthFunc( GL_LESS );

	std::cout << glGetString( GL_VERSION ) << std::endl;

#pragma endregion

	Model planet( "Assets/Models/Planet/planet.obj" );
	Model rock( "Assets/Models/Rock/rock.obj" );


	unsigned int amount = 10000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[ amount ];
	srand( glfwGetTime() );
	float radius = 100.0f;
	float offset = 25.f;

	for ( unsigned int i = 0; i < amount; i++ )
	{
		glm::mat4 model = glm::mat4( 1.0f );

		// 1. translation: displace along circle with radius [-offset, offset]
		float angle = ( float ) i / ( float ) amount * 360.0f;

		float displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
		float x = sin( angle ) * radius + displacement;

		displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller than x/z

		displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
		float z = cos( angle ) * radius + displacement;

		model = glm::translate( model, glm::vec3( x, y, z ) );


		// 2. scale: scale between 0.05 and 0.25f
		float scale = ( rand() % 20 ) / 100.0f + 0.05;
		model = glm::scale( model, glm::vec3( scale ) );


		// 3. rotation: add random rotation around a (semi)random rotation axis
		float rotAngle = ( rand() % 360 );
		model = glm::rotate( model, rotAngle, glm::vec3( 0.4f, 0.6f, 0.8f ) );


		modelMatrices[ i ] = model;
	}


	unsigned int instanceVBO;
	glGenBuffers( 1, &instanceVBO );
	glBindBuffer( GL_ARRAY_BUFFER, instanceVBO );
	glBufferData( GL_ARRAY_BUFFER, amount * sizeof( glm::mat4 ), &modelMatrices[ 0 ], GL_STATIC_DRAW );

	for ( unsigned int i = 0; i < rock.meshes.size(); i++ )
	{
		unsigned int VAO = rock.meshes[ i ].VAO;
		glBindVertexArray( VAO );

		std::size_t v4s = sizeof( glm::vec4 );

		glEnableVertexAttribArray( 3 );
		glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 4 * v4s, ( void* ) 0 );
		glVertexAttribDivisor( 3, 1 );

		glEnableVertexAttribArray( 4 );
		glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, 4 * v4s, ( void* ) ( 1 * v4s ) );
		glVertexAttribDivisor( 4, 1 );

		glEnableVertexAttribArray( 5 );
		glVertexAttribPointer( 5, 4, GL_FLOAT, GL_FALSE, 4 * v4s, ( void* ) ( 2 * v4s ) );
		glVertexAttribDivisor( 5, 1 );

		glEnableVertexAttribArray( 6 );
		glVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE, 4 * v4s, ( void* ) ( 3 * v4s ) );
		glVertexAttribDivisor( 6, 1 );

		glBindVertexArray( 0 );
	}

#pragma region ShaderGen

	// planet shader
	Shader planetShader( "SecondHalf/Instancing/Shaders/planetVertexShader.glsl", "SecondHalf/Instancing/Shaders/fragmentShader.glsl" );

	planetShader.use();

	// Material
	planetShader.setFloat( "u_Material.shininess", 32.f );

	// Directional light
	planetShader.setVec3( "u_DirectionalLight.direction", glm::vec3( -.2f, -1.f, -.3f ) );
	planetShader.setVec3( "u_DirectionalLight.ambient", glm::vec3( 0.05f, 0.05f, 0.05f ) );
	planetShader.setVec3( "u_DirectionalLight.diffuse", glm::vec3( 0.4f, 0.4f, 0.4f ) );
	planetShader.setVec3( "u_DirectionalLight.specular", glm::vec3( 0.5f, 0.5f, 0.5f ) );


	// rock shader
	Shader rockShader( "SecondHalf/Instancing/Shaders/rockVertexShader.glsl", "SecondHalf/Instancing/Shaders/fragmentShader.glsl" );

	// Material
	rockShader.setFloat( "u_Material.shininess", 32.f );

	// Directional light
	rockShader.setVec3( "u_DirectionalLight.direction", glm::vec3( -.2f, -1.f, -.3f ) );
	rockShader.setVec3( "u_DirectionalLight.ambient", glm::vec3( 0.05f, 0.05f, 0.05f ) );
	rockShader.setVec3( "u_DirectionalLight.diffuse", glm::vec3( 0.4f, 0.4f, 0.4f ) );
	rockShader.setVec3( "u_DirectionalLight.specular", glm::vec3( 0.5f, 0.5f, 0.5f ) );

#pragma endregion

	// input
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouseCallback );
	glfwSetScrollCallback( window, scrollCallback );

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
		planetShader.use();

		// View matrix
		glm::mat4 view = camera.GetViewMatrix();
		// Projection matrix
		int screenWidth, screenHeight;
		glfwGetWindowSize( window, &screenWidth, &screenHeight );
		glm::mat4 projection = glm::perspective( glm::radians( camera.Zoom ), screenWidth / ( float ) screenHeight, .1f, 100.f );

		planetShader.setMatrix4( "u_Projection", projection );
		planetShader.setMatrix4( "u_View", view );
		planetShader.setVec3( "u_ViewPos", camera.Position );

		glm::mat4 model = glm::mat4( 1.0f );
		model = glm::translate( model, glm::vec3( 0.0f, -3.0f, 0.0f ) );
		model = glm::scale( model, glm::vec3( 4.f, 4.f, 4.f ) );	// it's a bit too big for our scene, so scale it down
		planetShader.setMatrix4( "u_Model", model );

		// Calculate normal matrix once in cpp code, instead of doing it for every vertex for performance reasons
		glm::mat3 normalMatrix = glm::mat3( model );
		normalMatrix = glm::transpose( normalMatrix );
		normalMatrix = glm::inverse( normalMatrix );
		planetShader.setMatrix3( "u_NormalMatrix", normalMatrix );

		planet.Draw( planetShader );


		rockShader.use();

		rockShader.setMatrix4( "u_Projection", projection );
		rockShader.setMatrix4( "u_View", view );
		rockShader.setVec3( "u_ViewPos", camera.Position );

		for ( unsigned int i = 0; i < rock.meshes.size(); i++ )
		{
			glBindVertexArray( rock.meshes[ i ].VAO );
			glDrawElementsInstanced( GL_TRIANGLES, rock.meshes[ i ].indices.size(), GL_UNSIGNED_INT, 0, amount );

			rock.Draw( rockShader );
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