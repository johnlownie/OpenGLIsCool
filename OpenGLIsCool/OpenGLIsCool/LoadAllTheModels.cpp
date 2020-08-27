#include "globalThings.h"

#include <stdio.h>
#include <iostream>
#include "BasicTextureManager/cBasicTextureManager.h"

extern cBasicTextureManager* g_pBasicTextureManager;

// void LoadAllTheModels(unsigned int program, cVAOManager* pVAOManager, std::string modelsToLoad);

void LoadAllTheModels(unsigned int program, cVAOManager* pVAOManager)
{
	std::vector<sModelDrawInfo> vecDrawInfos;

	// Setup the texture manager
	::g_pBasicTextureManager->SetBasePath("assets/textures");

	// Load the textures
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("cobblestone.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("fauci.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("DarkGrey.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("IslandHeightMap.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("StoneTexture.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("WaterSurfaceTexture.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("SandTexture.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("pool-water-caustic.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("HeyThereTexture_Mask_or_discard.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("PolkaDotMask.bmp", true);
	::g_pBasicTextureManager->Create2DTextureFromBMPFile("SpidermanUV_square.bmp", true);

	vecDrawInfos.push_back(sModelDrawInfo("ISO_Shphere_flat_4div_xyz_n_rgba_uv.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("00_terrain_xyz_n_rgba_uv.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("00_FLAT_terrain_xyz_n_rgba_uv.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("00_atat_xyz_n_rgba_uv.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("00_Star Wars Snowspeeder_xyz_n_rgba_uv_2.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("0_review_X-Wing_Attack_xyz_n_rgba_uv.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("0_review_y-wing_xyz_n_rgba_uv_2.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("0_review_Isoshphere_xyz_n_rgba_uv_InvertedNormals.ply"));

	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_body.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_head.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Hips.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Left_arm.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Left_hand.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Left_leg.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Right_arm.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Right_hand.ply"));
	vecDrawInfos.push_back(sModelDrawInfo("legospiderman_Right_leg.ply"));

	if (!pVAOManager->LoadModelsIntoVAO(vecDrawInfos, program))
	{
		std::cout << "There were errors loading the models" << std::endl;
	}

	// Add to list of things to draw
	cMeshObject* pIsoSphere = new cMeshObject();
	pIsoSphere->meshName = "ISO_Shphere_flat_4div_xyz_n_rgba_uv.ply";
	pIsoSphere->scale = 2.0f;
	pIsoSphere->diffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pIsoSphere->isWireFrame = true;
	pIsoSphere->isVisible = false;
	pIsoSphere->friendlyName = "DebugSphere";

	cMeshObject* pTerrain = new cMeshObject();
	pTerrain->meshName = "00_terrain_xyz_n_rgba_uv.ply";
	pTerrain->position.y = -20.0f;
	pTerrain->scale = 2.0f;
	pTerrain->diffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pTerrain->specularRGB_Power = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pTerrain->isWireFrame = false;
	pTerrain->friendlyName = "Seafloor";
	pTerrain->textureNames[0] = "pool-water-caustic.bmp";
	pTerrain->textureNames[1] = "SandTexture.bmp";
	pTerrain->textureNames[2] = "";
	pTerrain->textureNames[3] = "";
	pTerrain->textureRatios[0] = 0.5f;
	pTerrain->textureRatios[1] = 0.5f;
	pTerrain->textureRatios[2] = 0.0f;
	pTerrain->textureRatios[3] = 0.0f;

	cMeshObject* pWaterSurface = new cMeshObject();
	pWaterSurface->meshName = "00_FLAT_terrain_xyz_n_rgba_uv.ply";
	pWaterSurface->position.y = 15.0f;
	pWaterSurface->scale = 2.0f;
	pWaterSurface->diffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 0.7f);
	pWaterSurface->specularRGB_Power = glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f);
	pWaterSurface->isWireFrame = false;
	pWaterSurface->friendlyName = "WaterSurface";
	pWaterSurface->textureNames[0] = "WaterSurfaceTexture.bmp";
	pWaterSurface->textureRatios[0] = 1.0f;
	pWaterSurface->discardTexture = "PolkaDotMask.bmp";

	cMeshObject* pATAT = new cMeshObject();
	pATAT->meshName = "00_atat_xyz_n_rgba_uv.ply";
	pATAT->position.x = 4.0f;
	pATAT->scale = 1.0f;
	pATAT->diffuseRGBA = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
	pATAT->friendlyName = "ATAT";
	pATAT->textureNames[0] = "DarkGrey.bmp";
	pATAT->textureRatios[0] = 1.0f;

	cMeshObject* pSnowspeeder = new cMeshObject();
	pSnowspeeder->meshName = "00_Star Wars Snowspeeder_xyz_n_rgba_uv_2.ply";
	pSnowspeeder->position = glm::vec3(4.0f, 10.0f, 15.0f);
	pSnowspeeder->scale = 1.0f;
	pSnowspeeder->orientation.y = glm::radians(180.0f);
	pSnowspeeder->diffuseRGBA = glm::vec4(0.91f, 0.80f, 0.85f, 1.0f);
	pSnowspeeder->friendlyName = "Snowspeeder";

	cMeshObject* pXWing = new cMeshObject();
	pXWing->meshName = "0_review_X-Wing_Attack_xyz_n_rgba_uv.ply";
	pXWing->position = glm::vec3(-3.5f, 6.0f, 20.0f);
	pXWing->scale = 1.0f;
	pXWing->orientation.x = glm::radians(90.0f);
	pXWing->orientation.y = glm::radians(180.0f);
	pXWing->diffuseRGBA = glm::vec4(0.91f, 0.85f, 0.85f, 1.0f);
	pXWing->friendlyName = "X-Wing";

	cMeshObject* pYWing = new cMeshObject();
	pYWing->meshName = "0_review_y-wing_xyz_n_rgba_uv_2.ply";
	pYWing->position = glm::vec3(-10.0f, 20.0f, 15.0f);
	pYWing->scale = 1.0f;
	pYWing->orientation.x = glm::radians(-90.0f);
	pYWing->diffuseRGBA = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
	pYWing->friendlyName = "Y-Wing";

	cMeshObject* pIsoSphereInverted = new cMeshObject();
	pIsoSphereInverted->meshName = "0_review_Isoshphere_xyz_n_rgba_uv_InvertedNormals.ply";
	pIsoSphereInverted->position = glm::vec3(-10.0f, 20.0f, -5.0f);
	pIsoSphereInverted->scale = 1.0f;
	pIsoSphereInverted->diffuseRGBA = glm::vec4(0.91f, 0.80f, 0.85f, 1.0f);
	pIsoSphereInverted->orientation.x = glm::radians(-90.0f);

	cMeshObject* pSpidermanBody = new cMeshObject();
	pSpidermanBody->meshName = "legospiderman_body.ply";
	pSpidermanBody->orientation.x = glm::radians(-90.0f);
	pSpidermanBody->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanBody->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanHead = new cMeshObject();
	pSpidermanHead->meshName = "legospiderman_head.ply";
	pSpidermanHead->orientation.x = glm::radians(-90.0f);
	pSpidermanHead->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanHead->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanHips = new cMeshObject();
	pSpidermanHips->meshName = "legospiderman_Hips.ply";
	pSpidermanHips->orientation.x = glm::radians(-90.0f);
	pSpidermanHead->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanHead->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanLeftArm = new cMeshObject();
	pSpidermanLeftArm->meshName = "legospiderman_Left_arm.ply";
	pSpidermanLeftArm->orientation.x = glm::radians(-90.0f);
	pSpidermanLeftArm->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanLeftArm->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanLeftHand = new cMeshObject();
	pSpidermanLeftHand->meshName = "legospiderman_Left_hand.ply";
	pSpidermanLeftHand->orientation.x = glm::radians(-90.0f);
	pSpidermanLeftHand->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanLeftHand->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanLeftLeg = new cMeshObject();
	pSpidermanLeftLeg->meshName = "legospiderman_Left_leg.ply";
	pSpidermanLeftLeg->orientation.x = glm::radians(-90.0f);
	pSpidermanLeftLeg->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanLeftLeg->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanRightArm = new cMeshObject();
	pSpidermanRightArm->meshName = "legospiderman_Right_arm.ply";
	pSpidermanRightArm->orientation.x = glm::radians(-90.0f);
	pSpidermanRightArm->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanRightArm->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanRightHand = new cMeshObject();
	pSpidermanRightHand->meshName = "legospiderman_Right_hand.ply";
	pSpidermanRightHand->orientation.x = glm::radians(-90.0f);
	pSpidermanRightHand->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanRightHand->textureRatios[0] = 1.0f;

	cMeshObject* pSpidermanRightLeg = new cMeshObject();
	pSpidermanRightLeg->meshName = "legospiderman_Right_leg.ply";
	pSpidermanRightLeg->orientation.x = glm::radians(-90.0f);
	pSpidermanRightLeg->textureNames[0] = "SpidermanUV_square.bmp";
	pSpidermanRightLeg->textureRatios[0] = 1.0f;

	// Opaque Objects
	::g_pVecObjects.push_back(pTerrain);
	::g_pVecObjects.push_back(pIsoSphere);
	::g_pVecObjects.push_back(pATAT);
	::g_pVecObjects.push_back(pSnowspeeder);
	::g_pVecObjects.push_back(pXWing);
	::g_pVecObjects.push_back(pYWing);
	::g_pVecObjects.push_back(pIsoSphereInverted);

	::g_pVecObjects.push_back(pSpidermanBody);
	::g_pVecObjects.push_back(pSpidermanHead);
	::g_pVecObjects.push_back(pSpidermanHips);
	::g_pVecObjects.push_back(pSpidermanLeftArm);
	::g_pVecObjects.push_back(pSpidermanLeftHand);
	::g_pVecObjects.push_back(pSpidermanLeftLeg);
	::g_pVecObjects.push_back(pSpidermanRightArm);
	::g_pVecObjects.push_back(pSpidermanRightHand);
	::g_pVecObjects.push_back(pSpidermanRightLeg);

	// Transparent Objects
	::g_pVecObjects.push_back(pWaterSurface);

	return;
}