#include "globalThings.h"

#include <stdio.h>
#include <iostream>
#include "globalOpenGLStuff.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
    }

    // First model manipulations
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

        
        if (key == GLFW_KEY_J && action == GLFW_PRESS) ::g_pLightManager->vecLights[::g_iSelectedLight].ToggleWireframe();     // Toggle light wireframe
        if (key == GLFW_KEY_K && action == GLFW_PRESS) ::g_pLightManager->vecLights[::g_iSelectedLight].ToggleDebugSpheres();  // Turn debug spheres on/off
        if (key == GLFW_KEY_L && action == GLFW_PRESS) ::g_pLightManager->vecLights[::g_iSelectedLight].ToggleLight();         // Turn light on/off

        if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].param1.y -= 0.1f;     // Inner angle of spotlight
        if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].param1.y += 0.1f;     // 
        if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].param1.z -= 0.1f;     // Outer angle of spotlight
        if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT)) ::g_pLightManager->vecLights[::g_iSelectedLight].param1.z += 0.1f;     // 
    }

    std::cout << "Light: " << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.y << ", " << ::g_pLightManager->vecLights[::g_iSelectedLight].atten.z << " - ";
    std::cout << "Cam: " << ::g_cameraEye.x << ", " << ::g_cameraEye.y << ", " << ::g_cameraEye.z << std::endl;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    return;
}
