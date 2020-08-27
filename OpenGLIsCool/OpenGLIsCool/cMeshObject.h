#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class cMeshObject
{
public:
	cMeshObject();
	~cMeshObject();
	std::string meshName;
	glm::vec3 position;
	glm::vec4 diffuseRGBA;
	glm::vec4 specularRGB_Power;
	glm::vec3 orientation;
	float scale;
	bool isWireFrame;
	bool isVisible;

	std::string friendlyName;

	static const unsigned int NUMBER_OF_TEXTURES = 4;
	std::string textureNames[NUMBER_OF_TEXTURES];
	float textureRatios[NUMBER_OF_TEXTURES];

	std::string discardTexture;

	unsigned int getUniqueID(void);

private:
	unsigned int m_ID;
	static unsigned int s_NextID;
};

