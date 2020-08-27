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
#include <sstream>
#include <algorithm>

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
cVAOManager* g_pVAOManager = NULL;
cLightManager* g_pLightManager = NULL;
cBasicTextureManager* g_pBasicTextureManager = NULL;

std::vector<cMeshObject*> g_pVecObjects;
std::vector<cMeshObject*> g_pOpaqueObjects;
std::vector<cMeshObject*> g_pTransparentObjects;

// Light stuff
int g_iSelectedModel = 0;
int g_iSelectedLight = 0;

// Texture stuff
static glm::vec2 heightMapTextureOffset = glm::vec2(0.0f, 0.0f);
static float textureRotationAngle = 0.0f;

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

// For sorting transparent objects
class cIsCloserFunctor
{
public:
    cIsCloserFunctor(glm::vec3 cameraEye)
    {
        this->m_CameraEye = cameraEye;
    }

    bool operator() (cMeshObject* pA, cMeshObject* pB)
    {
        float distA = glm::distance(pA->position, this->m_CameraEye);
        float distB = glm::distance(pB->position, this->m_CameraEye);

        return distA > distB;
    }

private:
    glm::vec3 m_CameraEye;
};

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
        delete ::g_pVAOManager;

        exit(EXIT_FAILURE);
    }

    // Print out some texture stats
    GLint texture_units = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

    GLint combined_texture_unit_count = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combined_texture_unit_count);

    std::cout << "Texture Units: " << texture_units << " - Combined Texture Unit Count: " << combined_texture_unit_count << std::endl;

    program = ::g_pShaderManager->getIDFromFriendlyName("SimpleShaderProg");

    // Create the managers
    ::g_pVAOManager = new cVAOManager();
    ::g_pBasicTextureManager = new cBasicTextureManager();
    ::g_pLightManager = new cLightManager();

    // Set up the managers
    LoadAllTheModels(program, ::g_pVAOManager);
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

    // Left engine light for Y-Wing
    ::g_pLightManager->vecLights[1].position = glm::vec4(-13.6f, 19.7f, 20.3f, 1.0f);
    ::g_pLightManager->vecLights[1].param1.x = 0.0f;   // Point Light
    ::g_pLightManager->vecLights[1].atten.x = 0.0f;    // Constant
    ::g_pLightManager->vecLights[1].atten.y = 0.0150705f;    // Linear
    ::g_pLightManager->vecLights[1].atten.z = 0.400047f;    // Quadraic
    ::g_pLightManager->vecLights[1].diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[1].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[1].param2.x = 1.0f;   // On
    ::g_pLightManager->vecLights[0].friendlyName = "Y-Wing Left";

    // Right engine light for Y-Wing
    ::g_pLightManager->vecLights[2].position = glm::vec4(-6.8f, 19.7f, 20.3f, 1.0f);
    ::g_pLightManager->vecLights[2].param1.x = 0.0f;   // Point Light
    ::g_pLightManager->vecLights[2].atten.x = 0.0f;    // Constant
    ::g_pLightManager->vecLights[2].atten.y = 0.0150705f;    // Linear
    ::g_pLightManager->vecLights[2].atten.z = 0.400047f;    // Quadraic
    ::g_pLightManager->vecLights[2].diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[2].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[2].param2.x = 1.0f;   // On
    ::g_pLightManager->vecLights[0].friendlyName = "Y-Wing Right";

    // Moon (Inverted Normal Sphere)
    ::g_pLightManager->vecLights[3].position = glm::vec4(-10.0f, 20.0f, -5.0f, 1.0f);
    ::g_pLightManager->vecLights[3].param1.x = 0.0f;   // Point Light
    ::g_pLightManager->vecLights[3].atten.x = 0.0f;    // Constant
    ::g_pLightManager->vecLights[3].atten.y = 0.0001f;    // Linear
    ::g_pLightManager->vecLights[3].atten.z = 0.0470901;    // Quadraic
    ::g_pLightManager->vecLights[3].diffuse = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[3].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[3].param2.x = 1.0f;   // On
    ::g_pLightManager->vecLights[0].friendlyName = "Orb";

    // Engine lights for the X-wing
    ::g_pLightManager->vecLights[4].position = glm::vec4(-4.2f, 5.0f, 24.0f, 1.0f);
    ::g_pLightManager->vecLights[4].param1.x = 0.0f;   // Point light
    ::g_pLightManager->vecLights[4].atten.x = 0.0f;     // Constant
    ::g_pLightManager->vecLights[4].atten.y = 0.00001f;    // Linear
    ::g_pLightManager->vecLights[4].atten.z = 3.71354f;    // Quadratic
    ::g_pLightManager->vecLights[4].diffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[4].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[4].param2.x = 1.0f;        // 1.0 for on (0.0 for off)

    ::g_pLightManager->vecLights[5].position = glm::vec4(-2.7f, 5.0f, 24.0f, 1.0f);
    ::g_pLightManager->vecLights[5].param1.x = 0.0f;   // Point light
    ::g_pLightManager->vecLights[5].atten.x = 0.0f;     // Constant
    ::g_pLightManager->vecLights[5].atten.y = 0.00001f;    // Linear
    ::g_pLightManager->vecLights[5].atten.z = 3.71354f;    // Quadratic
    ::g_pLightManager->vecLights[5].diffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[5].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[5].param2.x = 1.0f;        // 1.0 for on (0.0 for off)

    ::g_pLightManager->vecLights[6].position = glm::vec4(-4.2f, 6.7f, 24.0f, 1.0f);
    ::g_pLightManager->vecLights[6].param1.x = 0.0f;   // Point light
    ::g_pLightManager->vecLights[6].atten.x = 0.0f;     // Constant
    ::g_pLightManager->vecLights[6].atten.y = 0.00001f;    // Linear
    ::g_pLightManager->vecLights[6].atten.z = 3.71354f;    // Quadratic
    ::g_pLightManager->vecLights[6].diffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[6].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[6].param2.x = 1.0f;        // 1.0 for on (0.0 for off)

    ::g_pLightManager->vecLights[7].position = glm::vec4(-2.7f, 6.7f, 24.0f, 1.0f);
    ::g_pLightManager->vecLights[7].param1.x = 0.0f;   // Point light
    ::g_pLightManager->vecLights[7].atten.x = 0.0f;     // Constant
    ::g_pLightManager->vecLights[7].atten.y = 0.00001f;    // Linear
    ::g_pLightManager->vecLights[7].atten.z = 3.71354f;    // Quadratic
    ::g_pLightManager->vecLights[7].diffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[7].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[7].param2.x = 1.0f;        // 1.0 for on (0.0 for off)

    ::g_pLightManager->vecLights[8].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[8].param1.x = 0.0f;   // Point light
    ::g_pLightManager->vecLights[8].atten.x = 0.0f;     // Constant
    ::g_pLightManager->vecLights[8].atten.y = 0.00001f;    // Linear
    ::g_pLightManager->vecLights[8].atten.z = 0.21354f;    // Quadratic
    ::g_pLightManager->vecLights[8].diffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[8].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[8].param2.x = 0.0f;        // 1.0 for on (0.0 for off)

    ::g_pLightManager->vecLights[9].position = glm::vec4(0.0f, 20.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[9].param1.x = 1.0f;   // Spot light
    ::g_pLightManager->vecLights[9].atten.x = 0.0f;     // Constant
    ::g_pLightManager->vecLights[9].atten.y = 0.00001f;    // Linear
    ::g_pLightManager->vecLights[9].atten.z = 0.002f;    // Quadratic
    ::g_pLightManager->vecLights[9].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[9].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->vecLights[9].param2.x = 1.0f;        // 1.0 for on (0.0 for off)

    ::g_pLightManager->vecLights[9].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pLightManager->vecLights[9].param1.y = 15.0f;   // y = inner angle
    ::g_pLightManager->vecLights[9].param1.z = 30.0f;   // z = outer angle

    // 
    ::g_pOpaqueObjects.reserve(::g_pVecObjects.size());
    ::g_pTransparentObjects.reserve(::g_pVecObjects.size());

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

        GLint eyeLocation_UniLoc = glGetUniformLocation(program, "eyeLocation");
        glUniform4f(eyeLocation_UniLoc, ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);

        // Pass the lighting info to the shader
        ::g_pLightManager->CopyLightValuesToShader();

        // *****************************************
        // Update anything per frame
        // *****************************************
        
        // Flicker the X-wing engine lights
        for (unsigned int i = 4; i < 8; i++)
        {
            ::g_pLightManager->vecLights[i].atten.x = 0.0f;
            ::g_pLightManager->vecLights[i].atten.y = 0.00001f;
            ::g_pLightManager->vecLights[i].atten.z = 3.71354f;
            ::g_pLightManager->vecLights[i].atten.w = 1.0f;

            float rPerterb = ((float)rand() / (RAND_MAX));
            ::g_pLightManager->vecLights[i].atten.y += rPerterb;
        }

        // Point spotlight at the AT-AT
        cMeshObject* pATAT = findObjectByName("ATAT");
        glm::vec3 spotDirection = pATAT->position - glm::vec3(::g_pLightManager->vecLights[9].position);
        spotDirection = glm::normalize(spotDirection);
        ::g_pLightManager->vecLights[9].direction = glm::vec4(spotDirection, 1.0f);

        // *****************************************
        // Draw the entire scene of objects
        // *****************************************
        ::g_pOpaqueObjects.clear();
        ::g_pTransparentObjects.clear();

        for (std::vector<cMeshObject*>::iterator it_pCurMesh = ::g_pVecObjects.begin(); it_pCurMesh != ::g_pVecObjects.end(); it_pCurMesh++)
        {
            cMeshObject* pCurMesh = *it_pCurMesh;

            if (pCurMesh->diffuseRGBA.a < 1.0f)
            {
                ::g_pTransparentObjects.push_back(pCurMesh);
            }
            else
            {
                ::g_pOpaqueObjects.push_back(pCurMesh);

            }
        }

        // Draw opaque objects
        for (std::vector<cMeshObject*>::iterator it_pCurMesh = ::g_pOpaqueObjects.begin(); it_pCurMesh != ::g_pOpaqueObjects.end(); it_pCurMesh++)
        {
            DrawObject(*it_pCurMesh, program, matView, matProjection);
        }

        // Sort the transparent objects farthest to closest from camera
        std::sort(::g_pTransparentObjects.rbegin(), ::g_pTransparentObjects.rend(), cIsCloserFunctor(::g_cameraEye));

        // Draw transparent objects
        for (std::vector<cMeshObject*>::iterator it_pCurMesh = ::g_pTransparentObjects.begin(); it_pCurMesh != ::g_pTransparentObjects.end(); it_pCurMesh++)
        {
            DrawObject(*it_pCurMesh, program, matView, matProjection);
        }

        cMeshObject* pDebugSphere = findObjectByName("DebugSphere");
        pDebugSphere->diffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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

            pDebugSphere->diffuseRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            pDebugSphere->scale = dist05;
            DrawObject(pDebugSphere, program, matView, matProjection);

            pDebugSphere->diffuseRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            pDebugSphere->scale = dist25;
            DrawObject(pDebugSphere, program, matView, matProjection);

            pDebugSphere->diffuseRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            pDebugSphere->scale = dist50;
            DrawObject(pDebugSphere, program, matView, matProjection);

            pDebugSphere->diffuseRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            pDebugSphere->scale = dist75;
            DrawObject(pDebugSphere, program, matView, matProjection);
            pDebugSphere->isVisible = false;
        }

        // Update the window title
        std::stringstream ssTitle;
        ssTitle << "Light # " << ::g_iSelectedLight << " : "
            << ::g_pLightManager->vecLights[::g_iSelectedLight].position.x << ", "
            << ::g_pLightManager->vecLights[::g_iSelectedLight].position.y << ", "
            << ::g_pLightManager->vecLights[::g_iSelectedLight].position.z << "  "
            << "Line:" << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y << "  "
            << "Quad:" << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z << " - " << g_pLightManager->vecLights[::g_iSelectedLight].showWireframe;

        if (::g_pLightManager->vecLights[::g_iSelectedLight].param1.x == 1.0f) {
            ssTitle << " " << "Inner: " << ::g_pLightManager->vecLights[::g_iSelectedLight].param1.y
                << " - Outer: " << ::g_pLightManager->vecLights[::g_iSelectedLight].param1.z;
        }

        std::string sTitleText = ssTitle.str();

        glfwSetWindowTitle(window, sTitleText.c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete ::g_pShaderManager;
    delete ::g_pVAOManager;
    delete ::g_pLightManager;
    delete ::g_pBasicTextureManager;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void DrawObject(cMeshObject* pCurMesh, GLuint program, glm::mat4& matView, glm::mat4& matProjection)
{
    if (!pCurMesh->isVisible) { return; }

    // Textures
    GLint texture0A_UniLoc = glGetUniformLocation(program, "texture0A");
    GLint texture0B_UniLoc = glGetUniformLocation(program, "texture0B");
    GLint texture0C_UniLoc = glGetUniformLocation(program, "texture0C");
    GLint texture0D_UniLoc = glGetUniformLocation(program, "texture0D");

    GLuint texture_ID0 = ::g_pBasicTextureManager->getTextureIDFromName(pCurMesh->textureNames[0]);
    GLuint texture_ID1 = ::g_pBasicTextureManager->getTextureIDFromName(pCurMesh->textureNames[1]);
    GLuint texture_ID2 = ::g_pBasicTextureManager->getTextureIDFromName(pCurMesh->textureNames[2]);
    GLuint texture_ID3 = ::g_pBasicTextureManager->getTextureIDFromName(pCurMesh->textureNames[3]);

    // Choose texture units for the textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_ID0);
    glUniform1i(texture0A_UniLoc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_ID1);
    glUniform1i(texture0B_UniLoc, 1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_ID2);
    glUniform1i(texture0C_UniLoc, 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_ID3);
    glUniform1i(texture0D_UniLoc, 3);

    // Height map in vertext shader
    GLint bUseHeightMap_ID = glGetUniformLocation(program, "bUseHeightMap");

    // Hack
    if (pCurMesh->friendlyName == "HeightTerrain")
    {
        glUniform1f(bUseHeightMap_ID, (float)GL_TRUE);
        
        GLint textureHeightMap_ID = glGetUniformLocation(program, "textureHeightMap");
        GLuint texture_ID30 = ::g_pBasicTextureManager->getTextureIDFromName(pCurMesh->textureNames[0]);

        heightMapTextureOffset.x += 0.001f;

        glActiveTexture(GL_TEXTURE30);
        glBindTexture(GL_TEXTURE_2D, texture_ID30);
        glUniform1i(textureHeightMap_ID, 30);

        textureRotationAngle += glm::radians(0.1f);
        glm::mat4 matRotTexture = glm::rotate(glm::mat4(1.0f), textureRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        GLint matTextRotation_UniLoc = glGetUniformLocation(program, "matTextRotation");
        glUniformMatrix4fv(matTextRotation_UniLoc, 1, GL_FALSE, glm::value_ptr(matRotTexture));

        GLint textureOffsetVS_ID = glGetUniformLocation(program, "textureOffsetVS");
        glUniform2f(textureOffsetVS_ID, heightMapTextureOffset.x, heightMapTextureOffset.y);
    }
    else
    {
        glUniform1f(bUseHeightMap_ID, (float)GL_FALSE);
    }

    // Discard texture
    GLint bUseDiscardTexture_ID = glGetUniformLocation(program, "bUseDiscardTexture");

    if (pCurMesh->discardTexture != "")
    {
        glUniform1f(bUseDiscardTexture_ID, (float)GL_TRUE);

        GLint textureDiscard_LocID = glGetUniformLocation(program, "textureDiscard");
        GLuint texture_ID16 = ::g_pBasicTextureManager->getTextureIDFromName(pCurMesh->discardTexture);

        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_2D, texture_ID16);
        glUniform1i(textureDiscard_LocID, 16);
    }
    else
    {
        glUniform1f(bUseDiscardTexture_ID, (float)GL_FALSE);
    }

    // Set texture ratios
    GLint textureRatios_ID = glGetUniformLocation(program, "textureRatios");
    glUniform4f(textureRatios_ID, pCurMesh->textureRatios[0], pCurMesh->textureRatios[1], pCurMesh->textureRatios[2], pCurMesh->textureRatios[3]);

    // Get the locations for the "uniform variables"
    GLint diffuseColourRGBA_LocID = glGetUniformLocation(program, "diffuseColourRGBA");
    GLint specularColour_LocID = glGetUniformLocation(program, "specularColour");

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

    // Alpha transparency using the blend function
    if (pCurMesh->diffuseRGBA.a < 1.0f)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the uniform color info
    glUniform4f(diffuseColourRGBA_LocID, pCurMesh->diffuseRGBA.r, pCurMesh->diffuseRGBA.g, pCurMesh->diffuseRGBA.b, pCurMesh->diffuseRGBA.a);
    glUniform4f(specularColour_LocID, pCurMesh->specularRGB_Power.r, pCurMesh->specularRGB_Power.g, pCurMesh->specularRGB_Power.b, pCurMesh->specularRGB_Power.w);

    sModelDrawInfo mdoModelToDraw;
    if (::g_pVAOManager->FindDrawInfoByModelName(pCurMesh->meshName, mdoModelToDraw))
    {
        glBindVertexArray(mdoModelToDraw.VAO_ID);
        glDrawElements(GL_TRIANGLES, mdoModelToDraw.numberOfIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    return;
}