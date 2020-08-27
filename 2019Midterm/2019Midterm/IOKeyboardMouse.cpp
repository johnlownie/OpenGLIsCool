#include "globalThings.h"

#include <stdio.h>
#include <iostream>
#include <sstream>

#include "globalOpenGLStuff.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::stringstream ssTitle;
    const float CAMERASPEED = 1.0f;
    const float LIGHTSTEP = 0.1f;

    if (mods == GLFW_RELEASE)
    {
        // Move the camera
        if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_cameraEye.x -= CAMERASPEED;
        if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_cameraEye.x += CAMERASPEED;
        if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_cameraEye.y -= CAMERASPEED;
        if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_cameraEye.y += CAMERASPEED;
        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_cameraEye.z -= CAMERASPEED;
        if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_cameraEye.z += CAMERASPEED;

        // Change camera target to selected model
        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            ::g_cameraTarget.x = ::g_pVecObjects[::g_iSelectedModel]->position.x;
            ::g_cameraTarget.y = ::g_pVecObjects[::g_iSelectedModel]->position.y;
            ::g_cameraTarget.z = ::g_pVecObjects[::g_iSelectedModel]->position.z + 100.0f;
        }

        ssTitle << "Camera Target: " << ::g_cameraTarget.x << ", " << ::g_cameraTarget.y << ", " << ::g_cameraTarget.z
            << " - Pos: " << ::g_cameraEye.x << ", "
            << ::g_cameraEye.y << ", "
            << ::g_cameraEye.z;
    }

    // First light manipulations
    if (mods == GLFW_MOD_SHIFT)
    {
        // Select a model
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            ::g_pVecObjects[::g_iSelectedModel]->isWireFrame = false;
            ::g_iSelectedModel--;
            if (::g_iSelectedModel < 0) { ::g_iSelectedModel = g_pVecObjects.size() - 1; }
            ::g_pVecObjects[::g_iSelectedModel]->isWireFrame = true;
        }

        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            ::g_pVecObjects[::g_iSelectedModel]->isWireFrame = false;
            ::g_iSelectedModel++;
            if (::g_iSelectedModel == g_pVecObjects.size()) { ::g_iSelectedModel = 0; }
            ::g_pVecObjects[::g_iSelectedModel]->isWireFrame = true;
        }

        // Move the selected object
        if (key == GLFW_KEY_A && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->position.x -= 1;
        if (key == GLFW_KEY_D && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->position.x += 1;
        if (key == GLFW_KEY_Q && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->position.y -= 1;
        if (key == GLFW_KEY_E && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->position.y += 1;
        if (key == GLFW_KEY_W && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->position.z -= 1;
        if (key == GLFW_KEY_S && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->position.z += 1;

        // Rotate the selected object
        if (key == GLFW_KEY_K && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->orientation.z += glm::radians(CAMERASPEED);
        if (key == GLFW_KEY_L && action == GLFW_PRESS) ::g_pVecObjects[::g_iSelectedModel]->orientation.z -= glm::radians(CAMERASPEED);

        // Update the window title
        ssTitle << "Model: " << ::g_pVecObjects[::g_iSelectedModel]->friendlyName << "  - Pos: "
            << ::g_pVecObjects[::g_iSelectedModel]->position.x << ", "
            << ::g_pVecObjects[::g_iSelectedModel]->position.y << ", "
            << ::g_pVecObjects[::g_iSelectedModel]->position.z << " - Orientation: "
            << ::g_pVecObjects[::g_iSelectedModel]->orientation.x << ",  "
            << ::g_pVecObjects[::g_iSelectedModel]->orientation.y << ",  "
            << ::g_pVecObjects[::g_iSelectedModel]->orientation.z;
    }

    // Light manipulations
    if (mods == GLFW_MOD_CONTROL)
    {
        // Select a light
        if (key == GLFW_KEY_LEFT  && action == GLFW_PRESS) ::g_iSelectedLight--;
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) ::g_iSelectedLight++;
            
        if (::g_iSelectedLight < 0) ::g_iSelectedLight = ::g_pLightManager->NUMBER_OF_LIGHTS - 1;
        if (::g_iSelectedLight == ::g_pLightManager->NUMBER_OF_LIGHTS) ::g_iSelectedLight = 0;

        // Move the light
        if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].position.x -= LIGHTSTEP;
        if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].position.x += LIGHTSTEP;
        if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].position.z -= LIGHTSTEP;
        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].position.z += LIGHTSTEP;
        if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].position.y -= LIGHTSTEP;
        if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].position.y += LIGHTSTEP;

        // Adjust lighting attenuation
        if (key == GLFW_KEY_1) ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y *= 0.99f;    // Linear
        if (key == GLFW_KEY_2) ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y *= 1.01f;    // Linear
        if (key == GLFW_KEY_3) ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z *= 0.99f;    // Quardatic
        if (key == GLFW_KEY_4) ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z *= 1.01f;    // Quadratic

        // Toggle light wireframe
        if (key == GLFW_KEY_J && action == GLFW_PRESS) ::g_pLightManager->vecLights[::g_iSelectedLight].ToggleWireframe();

        // Turn debug spheres on/off
        if (key == GLFW_KEY_K && action == GLFW_PRESS) ::g_pLightManager->vecLights[::g_iSelectedLight].ToggleDebugSpheres();

        // Turn light on/off
        if (key == GLFW_KEY_L && action == GLFW_PRESS) ::g_pLightManager->vecLights[::g_iSelectedLight].ToggleLight();

        // Update the window title
        ssTitle << "Light: " << ::g_pLightManager->vecLights[::g_iSelectedLight].friendlyName << " : "
            << ::g_pLightManager->vecLights[::g_iSelectedLight].position.x << ", "
            << ::g_pLightManager->vecLights[::g_iSelectedLight].position.y << ", "
            << ::g_pLightManager->vecLights[::g_iSelectedLight].position.z << "  "
            << "Line:" << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y << "  "
            << "Quad:" << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z << " - " << g_pLightManager->vecLights[::g_iSelectedLight].showWireframe;
    }

    std::cout << "Light: " << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y << ", " << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z << " - ";
    std::cout << "Cam: " << ::g_cameraEye.x << ", " << ::g_cameraEye.y << ", " << ::g_cameraEye.z << std::endl;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    std::string sTitleText = ssTitle.str();
    glfwSetWindowTitle(window, sTitleText.c_str());

    return;
}
