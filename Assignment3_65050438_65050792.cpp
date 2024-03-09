#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm> // For std::clamp
#include "Libs/stb_image.h"

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader *> shaderList;
std::vector<Mesh *> meshListL;
std::vector<Shader *> shaderListL;

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char *fShader = "Shaders/shader.frag";

// Vertex Shader
static const char *vLightShader = "Shaders/lightShader.vert";

// Fragment Shader
static const char *fLightShader = "Shaders/lightShader.frag";

glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 0.0f);


void CreateOBJ()
{
    // model rocket
    Mesh *obj1 = new Mesh();
    bool loaded = obj1->CreateMeshFromOBJ("Models/rocket.obj");
    if (loaded)
        meshList.push_back(obj1);
    else
        std::cout << "Failed to load model" << std::endl;

    // light
    Mesh *light = new Mesh();
    light->CreateMeshFromOBJ("Models/cube.obj");
    meshListL.push_back(light);

    // model moon
    Mesh *obj2 = new Mesh();
    loaded = obj2->CreateMeshFromOBJ("Models/circle.obj");
    if (loaded)
        meshList.push_back(obj2);
    else
        std::cout << "Failed to load model" << std::endl;

    // model earth
    Mesh *obj3 = new Mesh();
    loaded = obj3->CreateMeshFromOBJ("Models/circle.obj");
    if (loaded)
        meshList.push_back(obj3);
    else
        std::cout << "Failed to load model" << std::endl;

    // model star 
    Mesh *obj4 = new Mesh();
    loaded = obj4->CreateMeshFromOBJ("Models/star.obj");
    if (loaded)
        meshList.push_back(obj4);
    else
        std::cout << "Failed to load model" << std::endl;
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    Shader *shader2 = new Shader();
    shader2->CreateFromFiles(vLightShader, fLightShader);
    shaderListL.push_back(shader2);
}
float yaw = -90.0f, pitch = 0.0f; 
void checkMouse()
{
    float xOffset, yOffset;

    double xPos, yPos;

    glfwGetCursorPos(mainWindow.getWindow(), &xPos, &yPos);

    static float lastX = xPos;
    static float lastY = yPos;

    xOffset = xPos - lastX;
    yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    float sensitivity = 0.3f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;
    pitch = std::clamp(pitch, -89.0f, 89.0f);
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0; 
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

    //------------------------------------

    glm::mat4 view(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

    float lastFrame = static_cast<float>(glfwGetTime());
    //------------------------------------ texture

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // ตั้งค่าการกระจายและการกระจายของ texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // texture rocket
    int width, height, nrChannels;
    unsigned char *data = stbi_load("Textures/rocketImg.png", &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(true);

    if (data)
    {
        // ผูกภาพกับ texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
   

    // texture moon
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // ตั้งค่าการกระจายและการกระจายของ texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width2, height2, nrChannels2;
    unsigned char *data2 = stbi_load("Textures/8k_moon.jpg", &width2, &height2, &nrChannels2, 0);
    stbi_set_flip_vertically_on_load(true);

    if (data2)
    {
        // ผูกภาพกับ texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data2);

    // texture earth
    unsigned int texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);

    // ตั้งค่าการกระจายและการกระจายของ texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width3, height3, nrChannels3;
    unsigned char *data3 = stbi_load("Textures/8k_earth.jpg", &width3, &height3, &nrChannels3, 0);
    stbi_set_flip_vertically_on_load(true);

    if (data3)
    {
        // ผูกภาพกับ texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data3);

    // textuer star

    unsigned int texture4;
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);

    // ตั้งค่าการกระจายและการกระจายของ texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width4, height4, nrChannels4;
    unsigned char *data4 = stbi_load("Textures/2k_stars.jpg", &width4, &height4, &nrChannels4, 0);
    stbi_set_flip_vertically_on_load(true);

    if (data4)
    {
        // ผูกภาพกับ texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width4, height4, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data4);

    while (!mainWindow.getShouldClose())
    {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        // position of model
        glm::vec3 Positions[] =
            {
                glm::vec3(0.0f, 5.0f, -4.0f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(0.0f, 0.0f, 0.0f)
            };
        uniformView = shaderList[0]->GetUniformLocation("view");
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float speed = 5.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraDirection = glm::normalize(direction);

        glfwPollEvents();
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += (cameraDirection)*deltaTime * speed;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= (cameraDirection)*deltaTime * speed;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos -= (cameraRight)*deltaTime * speed;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += (cameraRight)*deltaTime * speed;
        }
        // กด Enter กลับมาตำแหน่งแรก
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
            yaw = -90.0f;
            pitch = 0.0f;
            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraDirection = glm::normalize(direction);
        }
        

        checkMouse();
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        glm::mat4 cameraPosMat(1.0f);
        cameraPosMat[3][0] = -cameraPos.x;
        cameraPosMat[3][1] = -cameraPos.y;
        cameraPosMat[3][2] = -cameraPos.z;

        glm::mat4 cameraRotateMat(1.0f);
        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0.0f); 
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0.0f);
        cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0.0f);
       
        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
        

        // light

        glUniform3fv(shaderList[0]->GetUniformLocation("lightColor"), 1, (GLfloat *)&(lightColor));
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);

        // Object

        for (int i = 0; i < meshList.size(); i++)
        {
            // ตั้งค่าโมเดล
            glm::mat4 model(1.0f);

            if(i == 1){
                model = glm::translate(model, Positions[i]);
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.7f));
            }
            else{
                model = glm::translate(model, Positions[i]);
                model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));

            }

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

            // ผูก texture
            glActiveTexture(GL_TEXTURE0);

            // rocket
            if (i == 0)
            {
                // continue;
                glBindTexture(GL_TEXTURE_2D, texture);
            }

            // moon
            else if (i == 1)
            {
                glBindTexture(GL_TEXTURE_2D, texture2);
            }

            // earth
            else if (i == 2)
            {
                glBindTexture(GL_TEXTURE_2D, texture3);
            }
            // star
            else if (i == 3)
            {
                glBindTexture(GL_TEXTURE_2D, texture4);
            }
            // เรนเดอร์โมเดล
            meshList[i]->RenderMesh();
        }

        uniformView = shaderListL[0]->GetUniformLocation("view");
        shaderListL[0]->UseShader();
        uniformModel = shaderListL[0]->GetUniformLocation("model");
        uniformProjection = shaderListL[0]->GetUniformLocation("projection");
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 5.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        meshListL[0]->RenderMesh();
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
