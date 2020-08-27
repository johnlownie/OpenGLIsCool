#pragma once

#include <vector>
#include "globalOpenGLStuff.h"
#include "cMeshObject.h"
#include "cVAOManager.h"
#include "LightManager/cLightManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void LoadAllTheModels(unsigned int program, cVAOManager* pTheVAOManager);

extern std::vector< cMeshObject* > g_pVecObjects;
extern cLightManager* g_pLightManager;
extern int g_iSelectedModel;
extern int g_iSelectedLight;
extern glm::vec3 g_cameraEye;
extern glm::vec3 g_cameraTarget;