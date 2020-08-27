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
	glm::vec4 colourRGBA;
	glm::vec3 orientation;
	float scale;
	bool isWireFrame;
	bool isVisible;

	std::string friendlyName;

	unsigned int getUniqueID(void);

private:
	unsigned int m_ID;
	static unsigned int s_NextID;
};

