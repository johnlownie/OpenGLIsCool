#include "globalOpenGLStuff.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "globalThings.h"
#include "cShaderManager.h"
#include "cVAOManager.h"
#include "cMeshObject.h"
#include "LightManager/cLightManager.h"

// Function signature
void DrawObject(cMeshObject* pCurMesh, GLuint program, glm::mat4& matView, glm::mat4& matProjection);

// Camera Stuff
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, 100.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

cShaderManager* g_pShaderManager = NULL;
cVAOManager* g_pTheVAOManager = NULL;
cLightManager* g_pLightManager = NULL;

std::vector<cMeshObject*> g_pVecObjects;
int g_iSelectedModel = 0;
int g_iSelectedLight = 0;

cMeshObject* findObjectByName(std::string friendlyNameToFind)
{
    for (std::vector<cMeshObject*>::iterator it_pCO = ::g_pVecObjects.begin(); it_pCO != ::g_pVecObjects.end(); it_pCO++)
    {
        cMeshObject* pCO = *it_pCO;

        if (pCO->friendlyName == friendlyNameToFind)
        {
            return pCO;
        }
    }

    return NULL;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
    std::cout << "About to start..." << std::endl;

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cout << "Can't init GLFW.  Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1536, 1024, "Long Live COBOL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    ::g_pShaderManager = new cShaderManager();

    cShaderManager::cShader vertShader;
    cShaderManager::cShader fragShader;

    vertShader.fileName = "simpleVertex.glsl";
    fragShader.fileName = "simpleFragment.glsl";

    if (!::g_pShaderManager->createProgramFromFile("SimpleShaderProg", vertShader, fragShader))
    {
        std::cout << "ERROR: can't make shader program: " << ::g_pShaderManager->getLastError() << std::endl;
        delete ::g_pShaderManager;
        delete ::g_pTheVAOManager;

        exit(EXIT_FAILURE);
    }

    program = ::g_pShaderManager->getIDFromFriendlyName("SimpleShaderProg");

    ::g_pTheVAOManager = new cVAOManager();
    LoadAllTheModels(program, ::g_pTheVAOManager);

    ::g_pLightManager = new cLightManager();
    ::g_pLightManager->LoadUniformLocations(program);

    // Sun
    ::g_pLightManager->vecLights[0].position = glm::vec4(34.0f, 87.0f, 47.0f, 1.0f);
    ::g_pLightManager->vecLights[0].param1.x = 0.0f;   // Point Light
    ::g_pLightManager->vecLights[0].atten.x = 0.0f;    // Constant
    ::g_pLightManager->vecLights[0].atten.y = 0.0150704719f;    // Linear
    ::g_pLightManager->vecLights[0].atten.z = 2.42356309e-05f;    // Quadraic
    ::g_pLightManager->vecLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[0].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[0].param2.x = 1.0f;   // On
    ::g_pLightManager->vecLights[0].friendlyName = "Environment";

    std::cout << "We're all set!  Buckle up!" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glm::mat4 matView, matProjection;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);

        matProjection = glm::perspective(0.6f, ratio, 0.1f, 10000.0f);

        matView = glm::mat4(1.0f);
        matView = glm::lookAt(::g_cameraEye, ::g_cameraTarget, ::g_upVector);

        // Pass the lighting info to the shader
        ::g_pLightManager->CopyLightValuesToShader();

        // *****************************************
        // Draw the entire scene of objects
        // *****************************************
        for (std::vector<cMeshObject*>::iterator it_pCurMesh = ::g_pVecObjects.begin(); it_pCurMesh != ::g_pVecObjects.end(); it_pCurMesh++)
        {
            cMeshObject* pCurMesh = *it_pCurMesh;

            DrawObject(pCurMesh, program, matView, matProjection);
        }

        cMeshObject* pDebugSphere = findObjectByName("DebugSphere");
        pDebugSphere->colourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        pDebugSphere->scale = 1.0f;

        for (int i = 0; i < ::g_pLightManager->NUMBER_OF_LIGHTS; i++)
        {
            if (!::g_pLightManager->vecLights[i].isOn()) continue;

            pDebugSphere->isVisible = ::g_pLightManager->vecLights[i].showWireframe;
            pDebugSphere->position = ::g_pLightManager->vecLights[i].position;
            DrawObject(pDebugSphere, program, matView, matProjection);
            pDebugSphere->isVisible = false;
        }

        if (::g_pLightManager->vecLights[::g_iSelectedLight].isOn() && ::g_pLightManager->vecLights[::g_iSelectedLight].showDebugSpheres)
        {
            cLightHelper myLightHelper;
            float dist05 = myLightHelper.calcApproxDistFromAtten(0.05f, 0.01f, 1000000, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.x, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z, 50);
            float dist25 = myLightHelper.calcApproxDistFromAtten(0.25f, 0.01f, 1000000, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.x, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z, 50);
            float dist50 = myLightHelper.calcApproxDistFromAtten(0.50f, 0.01f, 1000000, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.x, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z, 50);
            float dist75 = myLightHelper.calcApproxDistFromAtten(0.75f, 0.01f, 1000000, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.x, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y, ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z, 50);

            // Draw spheres to match the brightness for first light
            pDebugSphere->position = ::g_pLightManager->vecLights[::g_iSelectedLight].position;
            pDebugSphere->isVisible = true;

            pDebugSphere->colourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            pDebugSphere->scale = dist05;
            DrawObject(pDebugSphere, program, matView, matProjection);

            pDebugSphere->colourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            pDebugSphere->scale = dist25;
            DrawObject(pDebugSphere, program, matView, matProjection);

            pDebugSphere->colourRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            pDebugSphere->scale = dist50;
            DrawObject(pDebugSphere, program, matView, matProjection);

            pDebugSphere->colourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            pDebugSphere->scale = dist75;
            DrawObject(pDebugSphere, program, matView, matProjection);
            pDebugSphere->isVisible = false;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete ::g_pShaderManager;
    delete ::g_pTheVAOManager;

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void DrawObject(cMeshObject* pCurMesh, GLuint program, glm::mat4& matView, glm::mat4& matProjection)
{
    if (!pCurMesh->isVisible) { return; }

    // Get the locations for the "uniform variables"
    GLint diffuseColour_LocID = glGetUniformLocation(program, "diffuseColour");

    GLint vpos_location = glGetAttribLocation(program, "vPos");
    GLint vcol_location = glGetAttribLocation(program, "vCol");
    GLint matModel_LocID = glGetUniformLocation(program, "matModel");
    GLint matView_LocID = glGetUniformLocation(program, "matView");
    GLint matProj_LocID = glGetUniformLocation(program, "matProj");

    GLint hasNoLighting_LocID = glGetUniformLocation(program, "hasNoLighting");

    glm::mat4 matModel = glm::mat4(1.0f);
    glm::mat4 matScale = glm::mat4(1.0f);

    // **************************************
    // Model Transformations
    // **************************************
    glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(pCurMesh->position.x, pCurMesh->position.y, pCurMesh->position.z));
    matModel = matModel * matTranslation;

    glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f), pCurMesh->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f), pCurMesh->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f), pCurMesh->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matModel = matModel * matRotateX * matRotateY * matRotateZ;

    // Set up a scaling matrix
    matScale = glm::scale(glm::mat4(1.0f), glm::vec3(pCurMesh->scale, pCurMesh->scale, pCurMesh->scale));
    matModel = matModel * matScale;

    glUseProgram(program);
    if (pCurMesh->isWireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1f(hasNoLighting_LocID, (float)GL_TRUE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform1f(hasNoLighting_LocID, (float)GL_FALSE);
    }
    glUniformMatrix4fv(matModel_LocID, 1, GL_FALSE, glm::value_ptr(matModel));
    glUniformMatrix4fv(matView_LocID, 1, GL_FALSE, glm::value_ptr(matView));
    glUniformMatrix4fv(matProj_LocID, 1, GL_FALSE, glm::value_ptr(matProjection));

    // Set the uniform color info
    glUniform3f(diffuseColour_LocID, pCurMesh->colourRGBA.r, pCurMesh->colourRGBA.g, pCurMesh->colourRGBA.b);

    sModelDrawInfo mdoModelToDraw;
    if (::g_pTheVAOManager->FindDrawInfoByModelName(pCurMesh->meshName, mdoModelToDraw))
    {
        glBindVertexArray(mdoModelToDraw.VAO_ID);
        glDrawElements(GL_TRIANGLES, mdoModelToDraw.numberOfIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    return;
}