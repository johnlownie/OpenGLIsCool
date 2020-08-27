#include "globalThings.h"

#include <stdio.h>
#include <iostream>

// void LoadAllTheModels(unsigned int program, cVAOManager* pTheVAOManager, std::string modelsToLoad);

void LoadAllTheModels(unsigned int program, cVAOManager* pTheVAOManager)
{
	sModelDrawInfo mdoIsoSphere;
	if (!pTheVAOManager->LoadModelIntoVAO("assets/models/ISO_Shphere_flat_4div_xyz_n_rgba_uv.ply", mdoIsoSphere, program))
	{
		std::cout << "ERROR: " << pTheVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdoBirdOfPrey;
	if (!pTheVAOManager->LoadModelIntoVAO("assets/models/BirdOfPrey_132386_Verts_Centred.ply", mdoBirdOfPrey, program))
	{
		std::cout << "ERROR: " << pTheVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdoTorpedo;
	if (!pTheVAOManager->LoadModelIntoVAO("assets/models/TearDropBullet_inverted_normals_xyz_n_rgba_uv.ply", mdoTorpedo, program))
	{
		std::cout << "ERROR: " << pTheVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdoEnterprise;
	if (!pTheVAOManager->LoadModelIntoVAO("assets/models/USS_Enterprise_183289_Verts.ply", mdoEnterprise, program))
	{
		std::cout << "ERROR: " << pTheVAOManager->getLastError() << std::endl;
	}

	cMeshObject* pIsoSphere = new cMeshObject();
	pIsoSphere->meshName = "assets/models/ISO_Shphere_flat_4div_xyz_n_rgba_uv.ply";
	pIsoSphere->scale = 1.0f;
	pIsoSphere->colourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pIsoSphere->isWireFrame = true;
	pIsoSphere->isVisible = false;
	pIsoSphere->friendlyName = "DebugSphere";
	::g_pVecObjects.push_back(pIsoSphere);

	// Add to list of things to draw
	cMeshObject* pBirdOfPrey = new cMeshObject();
	pBirdOfPrey->meshName = "assets/models/BirdOfPrey_132386_Verts_Centred.ply";
	pBirdOfPrey->scale = 1.0f;
	pBirdOfPrey->colourRGBA = glm::vec4(0.95f, 0.95f, 0.95f, 1.0f);
	pBirdOfPrey->friendlyName = "BirdOfPrey";
//	::g_pVecObjects.push_back(pBirdOfPrey);

	cMeshObject* pTorpedo = new cMeshObject();
	pTorpedo->meshName = "assets/models/TearDropBullet_inverted_normals_xyz_n_rgba_uv.ply";
	pTorpedo->scale = 1.0f;
	pTorpedo->colourRGBA = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
	pTorpedo->friendlyName = "Torpedo";
	//::g_pVecObjects.push_back(pTorpedo);

	cMeshObject* pEnterprise = new cMeshObject();
	pEnterprise->meshName = "assets/models/00_Star Wars USS_Enterprise_183289_Verts.ply";
	pEnterprise->position = glm::vec3(4.0f, 10.0f, 15.0f);
	pEnterprise->scale = 1.0f;
	pEnterprise->colourRGBA = glm::vec4(0.91f, 0.80f, 0.85f, 1.0f);
	pEnterprise->isWireFrame = true;
	pEnterprise->friendlyName = "Enterprise";
	::g_pVecObjects.push_back(pEnterprise);

	return;
}