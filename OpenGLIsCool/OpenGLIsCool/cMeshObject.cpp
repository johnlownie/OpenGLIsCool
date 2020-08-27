#include "cMeshObject.h"

unsigned int cMeshObject::s_NextID = 1001;

cMeshObject::cMeshObject()
{
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->diffuseRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->specularRGB_Power = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->orientation = glm::vec3(0.0f, 0.0f, 0.0f);

	this->scale = 1.0f;
	this->isWireFrame = false;
	this->isVisible = true;

	this->friendlyName = "???";

	for (unsigned int i = 0; i != cMeshObject::NUMBER_OF_TEXTURES; i++)
	{
		this->textureNames[i] = "???";
		this->textureRatios[i] = 0.0f;
	}

	this->discardTexture = "";

	this->m_ID = cMeshObject::s_NextID;
	cMeshObject::s_NextID++;

	return;
}

cMeshObject::~cMeshObject() {}

unsigned int cMeshObject::getUniqueID(void)
{
	return this->m_ID;
}