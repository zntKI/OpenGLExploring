#include "../../shader.hpp"
#include "../../camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>

void framebuffer_size_callback (GLFWwindow* window, int width, int height);
void mouse_callback (GLFWwindow* window, double xpos, double ypos);
void scroll_callback (GLFWwindow* window, double xoffset, double yoffset);
void processInput (GLFWwindow* window);
unsigned int loadTexture (const char* path, bool gammaCorrection);
void renderQuad ();
void renderCube ();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;

// camera
Camera camera (glm::vec3 (0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main ()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow (SCR_WIDTH, SCR_HEIGHT, "Bloom", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate ();
        return -1;
    }
    glfwMakeContextCurrent (window);
    glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);
    glfwSetCursorPosCallback (window, mouse_callback);
    glfwSetScrollCallback (window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (glewInit () != GLEW_OK)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable (GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader normalShader ("SecondHalf/Bloom/Shaders/normalVertexShader.glsl", "SecondHalf/Bloom/Shaders/normalFragmentShader.glsl");
    Shader lightsShader ("SecondHalf/Bloom/Shaders/normalVertexShader.glsl", "SecondHalf/Bloom/Shaders/lightsFragmentShader.glsl");
    Shader blurShader ("SecondHalf/Bloom/Shaders/blurVertexShader.glsl", "SecondHalf/Bloom/Shaders/blurFragmentShader.glsl");
    Shader finalShader ("SecondHalf/Bloom/Shaders/finalVertexShader.glsl", "SecondHalf/Bloom/Shaders/finalFragmentShader.glsl");


    // load textures
    // -------------
    unsigned int wallTexture = loadTexture ("Assets/Images/wall.jpg", true); // note that we're loading the texture as an SRGB texture
    unsigned int containerTexture = loadTexture ("Assets/Images/container.jpg", true); // note that we're loading the texture as an SRGB texture

    // configure floating point framebuffer
    // ------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers (1, &hdrFBO);
    glBindFramebuffer (GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[2];
    glGenTextures (2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture (GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers (1, &rboDepth);
    glBindRenderbuffer (GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers (2, attachments);

    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer (GL_FRAMEBUFFER, 0);


    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers (2, pingpongFBO);
    glGenTextures (2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer (GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture (GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }


    // lighting info
    // -------------
    // positions
    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back (glm::vec3 (0.0f, 0.5f, 1.5f));
    lightPositions.push_back (glm::vec3 (-4.0f, 0.5f, -3.0f));
    lightPositions.push_back (glm::vec3 (3.0f, 0.5f, 1.0f));
    lightPositions.push_back (glm::vec3 (-.8f, 2.4f, -1.0f));
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.push_back (glm::vec3 (5.0f, 5.0f, 5.0f));
    lightColors.push_back (glm::vec3 (10.0f, 0.0f, 0.0f));
    lightColors.push_back (glm::vec3 (0.0f, 0.0f, 15.0f));
    lightColors.push_back (glm::vec3 (0.0f, 5.0f, 0.0f));

    // shader configuration
    // --------------------
    normalShader.use ();
    normalShader.setInt ("u_DiffuseTexture", 0);
    blurShader.use ();
    blurShader.setInt ("u_Image", 0);
    finalShader.use ();
    finalShader.setInt ("u_Scene", 0);
    finalShader.setInt ("u_BloomBlur", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose (window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime ());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput (window);

        // render
        // ------
        glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer (GL_FRAMEBUFFER, hdrFBO);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective (glm::radians (camera.Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix ();
        normalShader.use ();
        normalShader.setMatrix4 ("u_Projection", projection);
        normalShader.setMatrix4 ("u_View", view);
        glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, wallTexture);
        // set lighting uniforms
        for (unsigned int i = 0; i < lightPositions.size (); i++)
        {
            normalShader.setVec3 ("u_Lights[" + std::to_string (i) + "].Position", lightPositions[i]);
            normalShader.setVec3 ("u_Lights[" + std::to_string (i) + "].Color", lightColors[i]);
        }
        normalShader.setVec3 ("u_ViewPos", camera.Position);
        // create one large cube that acts as the floor
        glm::mat4 model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (0.0f, -1.0f, 25.0));
        model = glm::scale (model, glm::vec3 (12.5f, 0.5f, 12.5f));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        // then create multiple cubes as the scenery
        glBindTexture (GL_TEXTURE_2D, containerTexture);
        model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (0.0f, 1.5f, 0.0));
        model = glm::scale (model, glm::vec3 (0.5f));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (2.0f, 0.0f, 1.0));
        model = glm::scale (model, glm::vec3 (0.5f));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (-1.0f, -1.0f, 2.0));
        model = glm::rotate (model, glm::radians (60.0f), glm::normalize (glm::vec3 (1.0, 0.0, 1.0)));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (0.0f, 2.7f, 4.0));
        model = glm::rotate (model, glm::radians (23.0f), glm::normalize (glm::vec3 (1.0, 0.0, 1.0)));
        model = glm::scale (model, glm::vec3 (1.25));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (-2.0f, 1.0f, -3.0));
        model = glm::rotate (model, glm::radians (124.0f), glm::normalize (glm::vec3 (1.0, 0.0, 1.0)));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        model = glm::mat4 (1.0f);
        model = glm::translate (model, glm::vec3 (-3.0f, 0.0f, 0.0));
        model = glm::scale (model, glm::vec3 (0.5f));
        normalShader.setMatrix4 ("u_Model", model);
        renderCube ();

        // finally show all the light sources as bright cubes
        lightsShader.use ();
        lightsShader.setMatrix4 ("u_Projection", projection);
        lightsShader.setMatrix4 ("u_View", view);

        for (unsigned int i = 0; i < lightColors.size (); i++)
        {
            model = glm::mat4 (1.0f);
            model = glm::translate (model, glm::vec3 (lightPositions[i]));
            model = glm::scale (model, glm::vec3 (0.25f));
            lightsShader.setMatrix4 ("u_Model", model);
            lightsShader.setVec3 ("u_LightColor", lightColors[i]);
            renderCube ();
        }

        glBindFramebuffer (GL_FRAMEBUFFER, 0);


        // 2. blur bright fragments with two-pass Gaussian Blur 
                // --------------------------------------------------
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use ();
        glActiveTexture (GL_TEXTURE0);
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer (GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt ("u_Horizontal", horizontal);
            glBindTexture (GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad ();

            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }

        glBindFramebuffer (GL_FRAMEBUFFER, 0);


        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        finalShader.use ();
        glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture (GL_TEXTURE1);
        glBindTexture (GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        finalShader.setInt ("u_Bloom", bloom);
        finalShader.setFloat ("u_Exposure", exposure);
        renderQuad ();

        std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers (window);
        glfwPollEvents ();
    }

    glfwTerminate ();
    return 0;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube ()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays (1, &cubeVAO);
        glGenBuffers (1, &cubeVBO);
        // fill buffer
        glBindBuffer (GL_ARRAY_BUFFER, cubeVBO);
        glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray (cubeVAO);
        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*)0);
        glEnableVertexAttribArray (1);
        glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*)(3 * sizeof (float)));
        glEnableVertexAttribArray (2);
        glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*)(6 * sizeof (float)));
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glBindVertexArray (0);
    }
    // render Cube
    glBindVertexArray (cubeVAO);
    glDrawArrays (GL_TRIANGLES, 0, 36);
    glBindVertexArray (0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad ()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays (1, &quadVAO);
        glGenBuffers (1, &quadVBO);
        glBindVertexArray (quadVAO);
        glBindBuffer (GL_ARRAY_BUFFER, quadVBO);
        glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)0);
        glEnableVertexAttribArray (1);
        glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)(3 * sizeof (float)));
    }
    glBindVertexArray (quadVAO);
    glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray (0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput (GLFWwindow* window)
{
    if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose (window, true);

    if (glfwGetKey (window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard (FORWARD, deltaTime);
    if (glfwGetKey (window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard (BACKWARD, deltaTime);
    if (glfwGetKey (window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard (LEFT, deltaTime);
    if (glfwGetKey (window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard (RIGHT, deltaTime);

    if (glfwGetKey (window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
    {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey (window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        bloomKeyPressed = false;
    }

    if (glfwGetKey (window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.001f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey (window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.001f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback (GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport (0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback (GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement (xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback (GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll (static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture (char const* path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures (1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load (path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture (GL_TEXTURE_2D, textureID);
        glTexImage2D (GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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