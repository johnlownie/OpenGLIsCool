#include "cMeshObject.h"

unsigned int cMeshObject::s_NextID = 1001;

cMeshObject::cMeshObject()
{
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->colourRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	this->orientation = glm::vec3(0.0f, 0.0f, 0.0f);

	this->scale = 1.0f;
	this->isWireFrame = false;
	this->isVisible = true;

	this->friendlyName = "???";

	this->m_ID = cMeshObject::s_NextID;
	cMeshObject::s_NextID++;

	return;
}

cMeshObject::~cMeshObject() {}

unsigned int cMeshObject::getUniqueID(void)
{
	return this->m_ID;
}