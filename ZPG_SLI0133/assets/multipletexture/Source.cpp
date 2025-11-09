//Include GLEW
#include <GL/glew.h>
//Include GLFW
#include <GLFW/glfw3.h> 

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ShaderLoader.h"

using namespace std;

const float triangle [48] = {
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,

    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f
};


const char* vertex_shader =
"#version 330\n"
"layout(location = 0) in vec3 vp;"
"layout(location = 1) in vec3 vc;"
"layout(location = 2) in vec2 vt; "
"uniform mat4 modelMatrix;"
"out vec2 uv;"
""
"void main () {"
"    gl_Position = modelMatrix * vec4 (vp, 1.0);"
"    uv=vt;"
"}";

const char* fragment_shader =
"#version 330\n"
"out vec4 frag_colour;"
"uniform sampler2D textureUnitID;"
"in vec2 uv;"
"void main () {"
"     frag_colour = texture(textureUnitID, uv);"
"}";



static void error_callback(int error, const char* description) { 
    fputs(description, stderr); 
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(800, 600, "ZPG", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float)height; //perspective matrix
    glViewport(0, 0, width, height);

    //Vertex Array Object (VAO)
    GLuint VBO = 0;
    glGenBuffers(1, &VBO); // generate the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), &triangle[0], GL_STATIC_DRAW);

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO); //generate the VAO
    glBindVertexArray(VAO); //bind the VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //enable vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(sizeof(float)*3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(sizeof(float) * 6));

       
    //create and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    glLinkProgram(shaderProgram);
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }






    //Texture one in texture unit 0
    glActiveTexture(GL_TEXTURE0);
    int text_width, text_height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("wooden_fence.png", &text_width, &text_height, &channels, 4);
    if (!data) {
        std::cerr << "Error loading texture: wooden_fence.png" << std::endl;
    }
    GLuint tex1;
    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_width, text_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);


    //Texture two in texture unit 1
    glActiveTexture(GL_TEXTURE1);
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("grass.png", &text_width, &text_height, &channels, 4);
    if (!data) {
        std::cerr << "Error loading texture: grass.png" << std::endl;
    }
    GLuint tex2;
    glGenTextures(1, &tex2);
    glBindTexture(GL_TEXTURE_2D, tex2);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_width, text_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    glm::mat4 M = glm::mat4(1.0f);
    
    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);
    GLint idTU = glGetUniformLocation(shaderProgram, "textureUnitID");
   
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glUniform1i(idTU, 0); // set TU 0
        M = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
        M = glm::rotate(M, (float)glfwGetTime() * 5.f, glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(0.5f, 0.5f, 0.5f));
        GLint idModelTransform = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(idModelTransform, 1, GL_FALSE, &M[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(VAO);
        glUniform1i(idTU, 1); // set TU 1
        M = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
        M = glm::rotate(M, -(float)glfwGetTime() * 5.f, glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(0.5f, 0.5f, 0.5f));
        idModelTransform = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(idModelTransform, 1, GL_FALSE, &M[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6); 

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}