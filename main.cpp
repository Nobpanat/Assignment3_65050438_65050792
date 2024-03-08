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

glm::vec3 lightColor = glm::vec3(1.0f,1.0f,1.0f);
glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 0.0f);


void CreateOBJ() 
{ 
    Mesh *obj1 = new Mesh(); 
    bool loaded = obj1->CreateMeshFromOBJ("Models/rocket4.obj"); 
    if (loaded) meshList.push_back(obj1); 
    else  std::cout<<"Failed to load model"<<std::endl; 

    Mesh *light = new Mesh(); 
    light->CreateMeshFromOBJ("Models/cube.obj");
    meshListL.push_back(light);


    Mesh *obj2 = new Mesh(); 
    loaded = obj2->CreateMeshFromOBJ("Models/moon.obj"); 
    if (loaded) meshList.push_back(obj2); 
    else  std::cout<<"Failed to load model"<<std::endl; 

    Mesh *obj3 = new Mesh(); 
    loaded = obj3->CreateMeshFromOBJ("Models/earth.obj"); 
    if (loaded) meshList.push_back(obj3); 
    else  std::cout<<"Failed to load model"<<std::endl; 

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
float yaw = -90.0f, pitch = 0.0f;// -90 to Z
void checkMouse(){
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

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0; // uint is unsigned int deafult is 0 because
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
    //------------------------------------
    unsigned int texture; 
    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture); 
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("Textures/8k_moon.jpg", &width, &height, &nrChannels,0);
    stbi_set_flip_vertically_on_load(true);

    if (data)
    {
        // bind image with texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    // Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // draw here

        glm::vec3 pyramidPositions[] =
            {
                glm::vec3(0.0f, 0.0f, -4.0f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),
                glm::vec3(-1.3f, 1.0f, -1.5f)};
        uniformView = shaderList[0]->GetUniformLocation("view");
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float speed = 5.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw))* cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw))* cos(glm::radians(pitch));
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
        // cameraPos.y = 0;

        checkMouse();
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        glm::mat4 cameraPosMat(1.0f);
        cameraPosMat[3][0] = -cameraPos.x;
        cameraPosMat[3][1] = -cameraPos.y;
        cameraPosMat[3][2] = -cameraPos.z;

        glm::mat4 cameraRotateMat(1.0f);
        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0.0f); // column first
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0.0f);
        cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0.0f);
        // view = cameraRotateMat * cameraPosMat;
        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
        // view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        //light

        glUniform3fv(shaderList[0]->GetUniformLocation("lightColor"),1,(GLfloat*)&(lightColor));
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);
        
        // Object

        for (int i = 0; i < 1; i++)
        {

            glm::mat4 model(1.0f);
            model = glm::translate(model, pyramidPositions[i]);
            // model = glm::rotate(model, glm::radians(2.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

            meshList[0]->RenderMesh();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,texture);
        }
        uniformView = shaderListL[0]->GetUniformLocation("view");
        shaderListL[0]->UseShader();
        uniformModel = shaderListL[0]->GetUniformLocation("model");
        uniformProjection = shaderListL[0]->GetUniformLocation("projection");
        glm::mat4 model(1.0f);
        model = glm::translate(model,glm::vec3(5.0f, 5.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        meshListL[0]->RenderMesh();
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
