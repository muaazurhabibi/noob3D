#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define STB_IMAGE_IMPLEMENTATION
#include "loader/loader.hpp"

// Nuklear configuration
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

// Core Nuklear implementation
#define NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

// GLFW + OpenGL implementation
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear/nuklear_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

int WINDOW_W = 1600, WINDOW_H = 900;

// vertices and indices for light cube
std::vector<Vertex> vertices = {
    // Front face (+Z)
    {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

    // Back face (-Z)
    {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},

    // Left face (-X)
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

    // Right face (+X)
    {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
    {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

    // Bottom face (-Y)
    {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

    // Top face (+Y)
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}
};
std::vector<unsigned int> indices = {
    // Front
     0,  1,  2,
     2,  3,  0,

    // Back
     4,  5,  6,
     6,  7,  4,

    // Left
     8,  9, 10,
    10, 11,  8,

    // Right
    12, 13, 14,
    14, 15, 12,

    // Bottom
    16, 17, 18,
    18, 19, 16,

    // Top
    20, 21, 22,
    22, 23, 20
};

// callback for screen size change
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    WINDOW_W = width;
    WINDOW_H = height;

    glViewport(0, 0, width, height);
}

// some mouse callbacks
bool mouse_look_enabled = false;
bool first_mouse = true;
float lastX = WINDOW_W/2, lastY = WINDOW_H/2;
float yaw = -90, pitch = 0;
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!mouse_look_enabled) return;
    if (first_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }
    
    const float sens = 0.1f;
    float xoff = (xpos - lastX) * sens;
    float yoff = (lastY - ypos) * sens;
    lastY = ypos, lastX = xpos;

    yaw += xoff, pitch += yoff;
    if (pitch > 89)
    {
        pitch = 89;
    }
    if (pitch < -89)
    {
        pitch = -89;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            first_mouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouse_look_enabled = true;
        }
        else if (action == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            first_mouse = true;
            mouse_look_enabled = false;
        }
    }
}

// helper function to load shader file and create shader
unsigned int createShaderFromFile(char *fname, GLenum shad_type)
{
    unsigned int shad = glCreateShader(shad_type);
    FILE *f = fopen(fname, "r");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *src = (char *)malloc(sizeof(char) * (size+1));
    size_t bytesR = fread(src, 1, (size_t)size, f); src[bytesR] = '\0';
    fclose(f);

    glShaderSource(shad, 1, &src, NULL); glCompileShader(shad);

    return shad;
}

// simple function to render gui
void RenderUI(struct nk_context *ctx)
{
    if (nk_begin(ctx, "noob3D",
        nk_rect(50, 50, 300, 200),
        NK_WINDOW_BORDER |
        NK_WINDOW_MOVABLE |
        NK_WINDOW_SCALABLE |
        NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Hello Nuklear!",
                    NK_TEXT_LEFT);

        if (nk_button_label(ctx, "Click Me"))
        {
            
        }

        nk_layout_row_dynamic(ctx, 30, 1);

        static int value = 50;

        nk_property_int(ctx, "Value:",
            0, &value, 100, 1, 1);
    }

    nk_end(ctx);
}

int main()
{
    printf("noob3D Verion Alpha\n");

    // opengl init
    if (!glfwInit())
    {
        printf("Error initializing GLFW\n");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow(WINDOW_W, WINDOW_H, (const char *)"S3DL", NULL, NULL);
    if (!window)
    {
        printf("Error in creating window\n");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initiaze GLEW\n");
    }

    // nuklear init
    struct nk_context *nk_ctx;
    struct nk_glfw glfw = {0};
    struct nk_font_atlas *atlas;
    nk_ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);

    // setting some settings
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    framebufferSizeCallback(window, width, height);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // camera variables
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::mat4(1.0f), proj = glm::mat4(1.0f);
    // calculating projection
    proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_W/(float)WINDOW_H, 0.1f, 100.0f);

    // loading shaders
    unsigned int vShad = createShaderFromFile((char *)"shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int fShad = createShaderFromFile((char *)"shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    unsigned int shadProg = glCreateProgram(), lightShadProg = glCreateProgram();
    glAttachShader(shadProg, vShad); glAttachShader(shadProg, fShad);
    glAttachShader(lightShadProg, vShad); glAttachShader(lightShadProg, fShad);
    glLinkProgram(shadProg);
    glLinkProgram(lightShadProg);
    glDeleteShader(vShad); glDeleteShader(fShad);

    // creating the light source
    glUseProgram(lightShadProg);
    Mesh L = Mesh(vertices, indices, std::vector<Texture>{});
    glm::vec3 lightPos = glm::vec3(2.0f, 4.0f, 2.0f);
    L.model = glm::translate(L.model, lightPos);
    glUniformMatrix4fv(glGetUniformLocation(lightShadProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform1i(glGetUniformLocation(lightShadProg, "isLight"), GL_TRUE);
    L.modelUniform = glGetUniformLocation(lightShadProg, "model");
    L.viewUniform = glGetUniformLocation(lightShadProg, "view");

    // creating the model
    glUseProgram(shadProg);
    Model M = Model((char *)"test_models/AnnaFigure/Anna.dae");
    glUniformMatrix4fv(glGetUniformLocation(shadProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shadProg, "lightPos"), 1, glm::value_ptr(lightPos));
    M.modelUniform = glGetUniformLocation(shadProg, "model");
    M.viewUniform = glGetUniformLocation(shadProg, "view");

    // center and scale the model to viewable scale
    M.model = glm::scale(M.model, glm::vec3(M.ComputeScale(2.0f)));
    M.model = glm::translate(M.model, -M.ComputeCentroid());

    float curTime = 0, lastTime = 0, deltaTime = 0;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        // make new nuklear frame
        nk_glfw3_new_frame(&glfw);
        // why doesnt nuklear do this on its own
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_SCISSOR_TEST);

        // get elapsed time
        curTime = glfwGetTime();
        deltaTime = curTime - lastTime;
        lastTime = curTime;

        // do camera stuff
        camTarget.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        camTarget.y = sin(glm::radians(pitch));
        camTarget.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camTarget = glm::normalize(camTarget);
        
        // get user input
        float camSpeed = 1.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camPos += camSpeed * camTarget;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camPos -= camSpeed * camTarget;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camPos -= glm::normalize(glm::cross(camTarget, camUp)) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camPos += glm::normalize(glm::cross(camTarget, camUp)) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            camPos -= glm::normalize(glm::cross(camTarget, glm::normalize(glm::cross(camTarget, camUp)))) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            camPos += glm::normalize(glm::cross(camTarget, glm::normalize(glm::cross(camTarget, camUp)))) * camSpeed;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        view = glm::lookAt(
            camPos, camPos + camTarget, camUp
        );

        // draw model
        glUseProgram(shadProg);
        glUniformMatrix4fv(M.modelUniform, 1, GL_FALSE, glm::value_ptr(M.model));
        glUniformMatrix4fv(M.viewUniform, 1, GL_FALSE, glm::value_ptr(view));
        M.Draw(shadProg);
        
        // draw light
        glUseProgram(lightShadProg);
        glUniformMatrix4fv(L.modelUniform, 1, GL_FALSE, glm::value_ptr(L.model));
        glUniformMatrix4fv(L.viewUniform, 1, GL_FALSE, glm::value_ptr(view));
        L.Draw(lightShadProg);

        // render the ui
        RenderUI(nk_ctx);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}