/*
Author: Brack Harmon

*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

unsigned int textures[7];

// Vertex Shader Source Code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
})";

// Fragment Shader Source Code
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
})";

// Callback function to handle window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// This function is called once to initialize the scene and OpenGL
static void init() {
    // Initialize time.
    glfwSetTime(0.0);

    // Set background color.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Load Textures
    textures[0] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/Background.png");
    textures[1] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/Roll.png");
    textures[2] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/Indicator.png");
    textures[3] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/Pitch.png");
    textures[4] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/SkyGround.png");
    textures[5] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/TopBottom.png");
    textures[6] = loadTexture("C:/Users/Brack/RedBird/Assessment/Altitude-Indicator/AltitudeIndicator/Rescources/Stationary.png");


    //GLSL::checkError(GET_FILE_LINE);
}

void render(unsigned int* shaderProgram, unsigned int* VAO) {
    // Transformation: Rotate over time
    glm::mat4 transform = glm::mat4(1.0f);
    float angle = (float)glfwGetTime() * glm::radians(50.0f);
    transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));

    // Use Shader Program
    glUseProgram(*shaderProgram);
    int transformLoc = glGetUniformLocation(*shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(glGetUniformLocation(*shaderProgram, "texture1"), 0);

    // Draw Quad
    glBindVertexArray(*VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Function to load texture
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture Wrapping and Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load Image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip image
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {
        GLenum format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture\n";
    }

    stbi_image_free(data);
    return textureID;
}

int main() {

    // Initialize GLFW library
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Altitude Indicator", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Define Vertex Data
    float vertices[] = {
        // Positions       // Texture Coords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    // Setup Buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Compile and Link Shader Program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Enable blending to allow transparent pixels to be rendered
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Initialize scene
    init();

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        render(&shaderProgram, &VAO);

        // Swap Buffers and Poll Events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
