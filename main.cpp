// Autor: Ognjen Zekovic
// Opis: Svemirski brod sa kamerom koja ga prati, svemirska stanica, zvezdano polje

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
GLuint loadTexture(const char* path);
void generateCube(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 color);
void generatePyramid(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 color);
void generateQuad(std::vector<float>& vertices, std::vector<unsigned int>& indices);
void generateStars(std::vector<float>& vertices, int numStars);

glm::vec3 shipPosition(0.0f, 0.0f, -5.0f);
float shipRotationY = 0.0f;
// povecati po potrebi laptopa
const float shipSpeed = 0.001f;
const float rotationSpeed = 0.05f;
float timeStamp = 0.0f;

int main(void)
{



    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 800;
    unsigned int wHeight = 600;
    const char wTitle[] = "Svemirski Brod";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ GEOMETRIJA +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int starShader = createShader("star.vert", "star.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    // trup
    std::vector<float> cubeVertices;
    std::vector<unsigned int> cubeIndices;
    generateCube(cubeVertices, cubeIndices, glm::vec3(0.7f, 0.7f, 0.9f));

    // nos
    std::vector<float> pyramidVertices;
    std::vector<unsigned int> pyramidIndices;
    generatePyramid(pyramidVertices, pyramidIndices, glm::vec3(0.9f, 0.9f, 1.0f));

    // stanica
    std::vector<float> stationVertices;
    std::vector<unsigned int> stationIndices;
    generateCube(stationVertices, stationIndices, glm::vec3(0.8f, 0.3f, 0.3f));

    // zvezde
    std::vector<float> starVertices;
    generateStars(starVertices, 200); // 200 komada

    // potpis
    std::vector<float> quadVertices;
    std::vector<unsigned int> quadIndices;
    generateQuad(quadVertices, quadIndices);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ TEKSTURE +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int signatureTexture = loadTexture("signature.png");

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    // trup
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), cubeIndices.data(), GL_STATIC_DRAW);

    unsigned int stride = 7 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // nos
    unsigned int pyramidVAO, pyramidVBO, pyramidEBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glGenBuffers(1, &pyramidEBO);

    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, pyramidVertices.size() * sizeof(float), pyramidVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices.size() * sizeof(unsigned int), pyramidIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // stanica
    unsigned int stationVAO, stationVBO, stationEBO;
    glGenVertexArrays(1, &stationVAO);
    glGenBuffers(1, &stationVBO);
    glGenBuffers(1, &stationEBO);

    glBindVertexArray(stationVAO);
    glBindBuffer(GL_ARRAY_BUFFER, stationVBO);
    glBufferData(GL_ARRAY_BUFFER, stationVertices.size() * sizeof(float), stationVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stationEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, stationIndices.size() * sizeof(unsigned int), stationIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // zvezde
    unsigned int starVAO, starVBO;
    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);

    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, starVertices.size() * sizeof(float), starVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // potpis
    unsigned int quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(unsigned int), quadIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            UNIFORME            +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");

    unsigned int starViewLoc = glGetUniformLocation(starShader, "uV");
    unsigned int starProjectionLoc = glGetUniformLocation(starShader, "uP");

    unsigned int quadPosLoc = glGetUniformLocation(textureShader, "uPos");

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP +++++++++++++++++++++++++++++++++++++++++++++++++

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        // za prosecni laptop 0.016
        timeStamp += 0.00016f;

        // izlaz
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // kontrole
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            shipPosition.x += sin(glm::radians(shipRotationY)) * shipSpeed;
            shipPosition.z += cos(glm::radians(shipRotationY)) * shipSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            shipPosition.x -= sin(glm::radians(shipRotationY)) * shipSpeed;
            shipPosition.z -= cos(glm::radians(shipRotationY)) * shipSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            shipRotationY += rotationSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            shipRotationY -= rotationSpeed;
        }

        // kamera
        glm::vec3 cameraOffset(-2.0f * sin(glm::radians(shipRotationY)), 1.5f, -2.0f * cos(glm::radians(shipRotationY)));
        glm::vec3 cameraPos = shipPosition + cameraOffset;
        glm::mat4 view = glm::lookAt(cameraPos, shipPosition, glm::vec3(0.0f, 1.0f, 0.0f));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // zvezde
        glUseProgram(starShader);
        glUniformMatrix4fv(starProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(starViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glBindVertexArray(starVAO);
        glDrawArrays(GL_POINTS, 0, 200);

            // objekti
        glUseProgram(unifiedShader);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // stanica
        glm::mat4 stationModel = glm::mat4(1.0f);
        stationModel = glm::translate(stationModel, glm::vec3(0.0f, -1.5f, 0.0f));
        stationModel = glm::rotate(stationModel, timeStamp * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        stationModel = glm::scale(stationModel, glm::vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stationModel));
        glBindVertexArray(stationVAO);
        glDrawElements(GL_TRIANGLES, stationIndices.size(), GL_UNSIGNED_INT, 0);

        // trup
        glm::mat4 shipBodyModel = glm::mat4(1.0f);
        shipBodyModel = glm::translate(shipBodyModel, shipPosition);
        shipBodyModel = glm::rotate(shipBodyModel, glm::radians(shipRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        shipBodyModel = glm::scale(shipBodyModel, glm::vec3(0.3f, 0.2f, 0.6f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(shipBodyModel));
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

        //nos
        glm::mat4 shipNoseModel = glm::mat4(1.0f);
        shipNoseModel = glm::translate(shipNoseModel, shipPosition);
        shipNoseModel = glm::rotate(shipNoseModel, glm::radians(shipRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        shipNoseModel = glm::translate(shipNoseModel, glm::vec3(0.0f, 0.0f, 0.4f));
        shipNoseModel = glm::scale(shipNoseModel, glm::vec3(0.2f, 0.2f, 0.4f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(shipNoseModel));
        glBindVertexArray(pyramidVAO);
        glDrawElements(GL_TRIANGLES, pyramidIndices.size(), GL_UNSIGNED_INT, 0);

        // potpis
        glDisable(GL_DEPTH_TEST);
        glUseProgram(textureShader);
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, signatureTexture);
        glUniform2f(quadPosLoc, 0.7f, -0.8f); // Donji desni ugao
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ CLEANUP +++++++++++++++++++++++++++++++++++++++++++++++++

    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteBuffers(1, &pyramidEBO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &stationVBO);
    glDeleteBuffers(1, &stationEBO);
    glDeleteVertexArrays(1, &stationVAO);
    glDeleteBuffers(1, &starVBO);
    glDeleteVertexArrays(1, &starVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(unifiedShader);
    glDeleteProgram(starShader);
    glDeleteProgram(textureShader);

    glfwTerminate();
    return 0;
}

GLuint loadTexture(const char* path) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 4) {
            format = GL_RGBA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return texture;
    }
    else {
        std::cerr << "Failed to load texture" << std::endl;
        stbi_image_free(data);
        return 0;
    }
}

void generateCube(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 color)
{
    vertices = {
        // front
        -0.5f, -0.5f,  0.5f,  color.r, color.g, color.b, 1.0f,
         0.5f, -0.5f,  0.5f,  color.r, color.g, color.b, 1.0f,
         0.5f,  0.5f,  0.5f,  color.r * 1.2f, color.g * 1.2f, color.b * 1.2f, 1.0f,
        -0.5f,  0.5f,  0.5f,  color.r * 1.2f, color.g * 1.2f, color.b * 1.2f, 1.0f,

        // back
        -0.5f, -0.5f, -0.5f,  color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f,
         0.5f, -0.5f, -0.5f,  color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f,
         0.5f,  0.5f, -0.5f,  color.r * 0.8f, color.g * 0.8f, color.b * 0.8f, 1.0f,
        -0.5f,  0.5f, -0.5f,  color.r * 0.8f, color.g * 0.8f, color.b * 0.8f, 1.0f
    };

    indices = {
        // front
        0, 1, 2,   2, 3, 0,
        // back
        4, 6, 5,   6, 4, 7,
        // left
        3, 7, 0,   0, 7, 4,
        // right
        1, 5, 6,   6, 2, 1,
        // top
        3, 2, 6,   6, 7, 3,
        // bottom
        0, 1, 5,   5, 4, 0
    };
}

void generatePyramid(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 color)
{
    vertices = {
        // osnova
        -0.5f, -0.5f, -0.5f,  color.r * 0.7f, color.g * 0.7f, color.b * 0.7f, 1.0f,
         0.5f, -0.5f, -0.5f,  color.r * 0.7f, color.g * 0.7f, color.b * 0.7f, 1.0f,
         0.5f, -0.5f,  0.5f,  color.r * 0.7f, color.g * 0.7f, color.b * 0.7f, 1.0f,
        -0.5f, -0.5f,  0.5f,  color.r * 0.7f, color.g * 0.7f, color.b * 0.7f, 1.0f,
        // vrh
         0.0f,  0.5f,  0.0f,  color.r, color.g, color.b, 1.0f
    };

    indices = {
        // osnova
        0, 1, 2,   2, 3, 0,
        // stranice
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
}

void generateQuad(std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    vertices = {
        // pozicije       // tex
        -0.2f, 0.4f,      0.0f, 0.0f,
        -0.2f, 0.0f,      0.0f, 1.0f,
         0.2f, 0.0f,      1.0f, 1.0f,
         0.2f, 0.4f,      1.0f, 0.0f
    };

    indices = {
        0, 1, 3,
        1, 2, 3
    };
}

void generateStars(std::vector<float>& vertices, int numStars)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-50.0f, 50.0f);

    vertices.clear();
    for (int i = 0; i < numStars; i++)
    {
        vertices.push_back(dis(gen)); // x
        vertices.push_back(dis(gen)); // y
        vertices.push_back(dis(gen)); // z
    }
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}