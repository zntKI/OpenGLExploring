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
unsigned int loadTexture (const char* path);

const unsigned int windowWidth = 1920, windowHeight = 1080;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Camera vars
Camera camera( glm::vec3( 0.f, 0.f, 3.f ) );
bool firstMouse = true;
float lastX = windowWidth / 2.f, lastY = windowHeight / 2.f;

int main( void )
{
#pragma region SetUp

	srand( time( 0 ) );

	GLFWwindow* window;

	/* Initialize the library */
	if ( !glfwInit() )
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow( windowWidth, windowHeight, "Hello", NULL, NULL );
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

#pragma endregion

#pragma region TextureGen

	//stbi_set_flip_vertically_on_load( true );
	
	unsigned int diffTexture = loadTexture ("Assets/Images/brickwall.jpg");
	unsigned int normalTexture = loadTexture ("Assets/Images/brickwall_normal.jpg");

#pragma endregion

#pragma region ObjectGen

	// Vertices
	float vertices[] = {
		//	  position			normals			   texture coor
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
	};

	unsigned int VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	unsigned int VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*)0);
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*)(3 * sizeof (float)));
	glEnableVertexAttribArray (2);
	glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*)(6 * sizeof (float)));

#pragma endregion

#pragma region ShaderGen

	Shader normalShader( "SecondHalf/NormalMapping/Shaders/normalVertexShader.glsl", "SecondHalf/NormalMapping/Shaders/normalFragmentShader.glsl" );

	normalShader.use();
	normalShader.setInt( "u_TexDiffuse", 0 );
	normalShader.setInt( "u_TexNormal", 1 );

#pragma endregion

	// Use z-buffer
	glEnable( GL_DEPTH_TEST );

	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouseCallback );
	glfwSetScrollCallback( window, scrollCallback );

	glm::vec3 lightPos (0.f, 0.f, 2.f);

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window ) )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		// Keep track of time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Input
		processInput( window );


		// Rendering
		normalShader.use();

		glm::mat4 model = glm::mat4 (1.f);
		normalShader.setMatrix4 ("u_Model", model);

		glm::mat4 view = camera.GetViewMatrix ();
		normalShader.setMatrix4 ("u_View", view);

		int screenWidth, screenHeight;
		glfwGetWindowSize (window, &screenWidth, &screenHeight);
		glm::mat4 projection = glm::perspective (glm::radians (camera.Zoom), screenWidth / (float)screenHeight, .1f, 100.f);
		normalShader.setMatrix4 ("u_Proj", projection);

		normalShader.setVec3 ("u_LightPos", lightPos);
		normalShader.setVec3 ("u_ViewPos", camera.Position);

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, diffTexture);
		glActiveTexture (GL_TEXTURE1);
		glBindTexture (GL_TEXTURE_2D, normalTexture);

		glBindVertexArray( VAO );
		glDrawArrays (GL_TRIANGLES, 0, 6);

		glfwSwapBuffers( window );
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

unsigned int loadTexture (char const* path)
{
	unsigned int textureID;
	glGenTextures (1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load (path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture (GL_TEXTURE_2D, textureID);
		glTexImage2D (GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap (GL_TEXTURE_2D);

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free (data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free (data);
	}

	return textureID;
}