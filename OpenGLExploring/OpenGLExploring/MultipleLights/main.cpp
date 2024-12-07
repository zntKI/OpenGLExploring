#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "../shader.hpp"
#include "../camera.hpp"


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
	window = glfwCreateWindow( windowWidth, windowHeight, "Hello World", NULL, NULL );
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

	// Use z-buffer
	glEnable( GL_DEPTH_TEST );

	std::cout << glGetString( GL_VERSION ) << std::endl;

#pragma endregion

#pragma region TextureGen

	stbi_set_flip_vertically_on_load( true );
	unsigned int diffuseMap = loadTexture( "Assets/Images/container2.png" );
	unsigned int specularMap = loadTexture( "Assets/Images/container2_specular.png" );
	unsigned int emissionMap = loadTexture( "Assets/Images/matrix.jpg" );

#pragma endregion

#pragma region ObjectGen

	float vertices[] = {
		// positions          // normals          // tex coords

		// Front face
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

	   // Back face
	  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
	   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
	  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
	  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

	  // Left face
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	 // Right face
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	 // Down face
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

	// Up face
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
	};

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
	// normals
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) ( sizeof( float ) * 3 ) );
	glEnableVertexAttribArray( 1 );
	// texture coordinates
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) ( sizeof( float ) * 6 ) );
	glEnableVertexAttribArray( 2 );


	// light source
	unsigned int lightVAO;
	glGenVertexArrays( 1, &lightVAO );
	glBindVertexArray( lightVAO );

	glBindBuffer( GL_ARRAY_BUFFER, VBO );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, ( void* ) 0 );
	glEnableVertexAttribArray( 0 );

#pragma endregion

#pragma region ShaderGen

	// object shader
	Shader objectShader( "MultipleLights/Shaders/vertexShader.glsl", "MultipleLights/Shaders/fragmentShader.glsl" );

	// Material
	objectShader.setInt( "material.diffuse", 0 );
	objectShader.setInt( "material.specular", 1 );
	objectShader.setInt( "material.emission", 2 );
	objectShader.setFloat( "material.shininess", 32.f );


	// Lights

	// Directional light
	objectShader.setVec3( "directionalLight.direction", glm::vec3( -.2f, -1.f, -.3f ) );
	objectShader.setVec3( "directionalLight.ambient", glm::vec3( 0.05f, 0.05f, 0.05f ) );
	objectShader.setVec3( "directionalLight.diffuse", glm::vec3( 0.4f, 0.4f, 0.4f ) );
	objectShader.setVec3( "directionalLight.specular", glm::vec3( 0.5f, 0.5f, 0.5f ) );

	// Point lightS
	glm::vec3 pointLightPositions[] = {
glm::vec3( 0.7f, 0.2f, 2.0f ),
glm::vec3( 2.3f, -3.3f, -4.0f ),
glm::vec3( -4.0f, 2.0f, -12.0f ),
glm::vec3( 0.0f, 0.0f, -3.0f )
	};

	// Common
	glm::vec3 ambientColor( 0.05f, 0.05f, 0.05f );
	glm::vec3 diffuseColor( 0.8f, 0.8f, 0.8f );
	glm::vec3 specularColor( 1.f, 1.f, 1.f );

	// point light 1
	objectShader.setVec3( "pointLights[0].position", pointLightPositions[ 0 ] );
	objectShader.setVec3( "pointLights[0].ambient", ambientColor );
	objectShader.setVec3( "pointLights[0].diffuse", diffuseColor );
	objectShader.setVec3( "pointLights[0].specular", specularColor );
	objectShader.setFloat( "pointLights[0].constant", 1.0f );
	objectShader.setFloat( "pointLights[0].linear", 0.09f );
	objectShader.setFloat( "pointLights[0].quadratic", 0.032f );
	// point light 2
	objectShader.setVec3( "pointLights[1].position", pointLightPositions[ 1 ] );
	objectShader.setVec3( "pointLights[1].ambient", ambientColor );
	objectShader.setVec3( "pointLights[1].diffuse", diffuseColor );
	objectShader.setVec3( "pointLights[1].specular", specularColor );
	objectShader.setFloat( "pointLights[1].constant", 1.0f );
	objectShader.setFloat( "pointLights[1].linear", 0.09f );
	objectShader.setFloat( "pointLights[1].quadratic", 0.032f );
	// point light 3
	objectShader.setVec3( "pointLights[2].position", pointLightPositions[ 2 ] );
	objectShader.setVec3( "pointLights[2].ambient", ambientColor );
	objectShader.setVec3( "pointLights[2].diffuse", diffuseColor );
	objectShader.setVec3( "pointLights[2].specular", specularColor );
	objectShader.setFloat( "pointLights[2].constant", 1.0f );
	objectShader.setFloat( "pointLights[2].linear", 0.09f );
	objectShader.setFloat( "pointLights[2].quadratic", 0.032f );
	// point light 4
	objectShader.setVec3( "pointLights[3].position", pointLightPositions[ 3 ] );
	objectShader.setVec3( "pointLights[3].ambient", ambientColor );
	objectShader.setVec3( "pointLights[3].diffuse", diffuseColor );
	objectShader.setVec3( "pointLights[3].specular", specularColor );
	objectShader.setFloat( "pointLights[3].constant", 1.0f );
	objectShader.setFloat( "pointLights[3].linear", 0.09f );
	objectShader.setFloat( "pointLights[3].quadratic", 0.032f );


	// Spot light
	objectShader.setVec3( "spotLight.position", camera.Position );
	objectShader.setVec3( "spotLight.direction", camera.Front );
	objectShader.setFloat( "spotLight.cutOffInner", glm::cos( glm::radians( 12.5f ) ) );
	objectShader.setFloat( "spotLight.cutOffOuter", glm::cos( glm::radians( 17.5f ) ) );
	objectShader.setVec3( "spotLight.ambient", glm::vec3( 0.f ) );
	objectShader.setVec3( "spotLight.diffuse", glm::vec3( 1.f ) );
	objectShader.setVec3( "spotLight.specular", glm::vec3( 1.f ) );
	objectShader.setFloat( "spotLight.constant", 1.f );
	objectShader.setFloat( "spotLight.linear", .09f );
	objectShader.setFloat( "spotLight.quadratic", .032f );


	// light shader
	Shader lightShader( "MultipleLights/Shaders/lightVertexShader.glsl", "MultipleLights/Shaders/lightFragmentShader.glsl" );

	lightShader.setVec3( "light.ambient", ambientColor );
	lightShader.setVec3( "light.diffuse", diffuseColor );

#pragma endregion

	// Usually set within the rendering loop but in our case that is not necessary since our program is static and does not require that
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, diffuseMap );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, specularMap );
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, emissionMap );

	// input
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouseCallback );
	glfwSetScrollCallback( window, scrollCallback );

	while ( !glfwWindowShouldClose( window ) )
	{
		glClearColor( .12f, .12f, .12f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		// Keep track of time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Input
		processInput( window );


		// Rendering

		// Common Transformations

#pragma region DefiningMatrices

		// Model matrix
		glm::mat4 model = glm::mat4( 1.f );
		// View matrix
		glm::mat4 view = camera.GetViewMatrix();
		// Projection matrix
		int screenWidth, screenHeight;
		glfwGetWindowSize( window, &screenWidth, &screenHeight );
		glm::mat4 projection = glm::perspective( glm::radians( camera.Zoom ), screenWidth / ( float ) screenHeight, .1f, 100.f );

#pragma endregion


#pragma region ObjectProcessing

		// Bind the shader for objects
		objectShader.use();

		objectShader.setVec3( "spotLight.position", camera.Position );
		objectShader.setVec3( "spotLight.direction", camera.Front );

		// Set View matrix
		objectShader.setMatrix4( "u_view", view );

		// Set Projection matrix
		objectShader.setMatrix4( "u_projection", projection );

		// Set the look direction
		objectShader.setVec3( "u_viewPos", camera.Position );

		// Bind VAO for objects
		glBindVertexArray( VAO );

		// Model displacement
		for ( unsigned int i = 0; i < 10; i++ )
		{
			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::translate( model, cubePositions[ i ] );
			float angle = 20.0f * i;
			model = glm::rotate( model, glm::radians( angle ),
				glm::vec3( 1.0f, 0.3f, 0.5f ) );
			objectShader.setMatrix4( "u_model", model );

			glDrawArrays( GL_TRIANGLES, 0, 36 );
		}

#pragma endregion

#pragma region LightProcessing

		// Bind the shader for objects
		lightShader.use();

		// Set View matrix
		lightShader.setMatrix4( "u_view", view );

		// Set Projection matrix
		lightShader.setMatrix4( "u_projection", projection );

		// Bind VAO for objects
		glBindVertexArray( lightVAO );

		// Model displacement
		for ( unsigned int i = 0; i < 4; i++ )
		{
			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::translate( model, pointLightPositions[ i ] );
			model = glm::scale( model, glm::vec3( .2f ) );
			lightShader.setMatrix4( "u_model", model );

			glDrawArrays( GL_TRIANGLES, 0, 36 );
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
	if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
	{
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
	if ( data )
	{
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

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		stbi_image_free( data );
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free( data );
	}

	return textureID;
}