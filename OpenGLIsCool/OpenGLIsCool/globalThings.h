#pragma once

#include <vector>
#include "globalOpenGLStuff.h"
#include "cMeshObject.h"
#include "cVAOManager.h"
#include "LightManager/cLightManager.h"
#include "BasicTextureManager/cBasicTextureManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void LoadAllTheModels(unsigned int program, cVAOManager* pVAOManager);

extern std::vector< cMeshObject* > g_pVecObjects;
extern cLightManager* g_pLightManager;
extern cBasicTextureManager* g_pBasicTextureManager;
extern int g_iSelectedModel;
extern int g_iSelectedLight;
extern glm::vec3 g_cameraEye;