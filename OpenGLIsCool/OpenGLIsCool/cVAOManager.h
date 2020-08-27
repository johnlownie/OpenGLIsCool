#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>
#include <vector>

// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVert_xyzw_rbga_UVx2
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float r, g, b, a;
	float u1, v1, u2, v2;


	sVert_xyzw_rbga_UVx2()
	{
		this->x = this->y = this->z = this->w = 0.0f;
		this->nx = this->ny = this->nz = this->nw = 0.0f;
		this->r = this->g = this->b = this->a = 0.0f;
		this->u1 = this->v1 = this->u2 = this->v2 = 0.0f;

		return;
	}
};

struct sModelDrawInfo
{
	sModelDrawInfo();
	sModelDrawInfo(std::string filename);

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// Extents: largest and smallest values for bounding/hit box
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float centreX, centreY, centreZ;
	float deltaX, deltaY, deltaZ;

	float maxExtent;
	float scaleForUnitBB;

	sVert_xyzw_rbga_UVx2* pVertices;
	unsigned int* pIndices;
};

struct sModelModifiers
{
	float scale;
	float offsetX, offsetY, offsetZ;

	sModelModifiers()
	{
		this->scale = 1.0f;
		this->offsetX = this->offsetY = this->offsetZ = 0.0f;
	}
};
class cVAOManager
{
public:

	cVAOManager(void);
	~cVAOManager(void);

	bool LoadModelIntoVAO(std::string fileName, sModelDrawInfo &drawInfo, unsigned int shaderProgramID);
	bool LoadModelIntoVAO(std::string fileName, sModelDrawInfo &drawInfo, sModelModifiers modelModifiers, unsigned int shaderProgramID);
	bool LoadModelsIntoVAO(std::vector<sModelDrawInfo> &vecDrawInfos, unsigned int shaderProgramId);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename, sModelDrawInfo &drawInfo);

	std::string getLastError(bool bAndClear = true);

	sModelDrawInfo getModelFileInfo(std::string filename);
	void ScaleLoadedModel(float scale);
	void CenterAtOrigin(void);
	void SetBaseLoadPath(std::string fullPathName);
	std::string GetBaseLoadPath(void);

private:

	std::string basepath;

	std::map< std::string /*model name*/, sModelDrawInfo /* info needed to draw*/ > m_map_ModelName_to_VAOID;

	// Loads the ply model file into a temporary array
	bool m_LoadTheModel( std::string fileName, sModelDrawInfo &drawInfo);

	std::string m_lastErrorString;
	void m_AppendTextToLastError(std::string text, bool addNewLineBefore = true);
};

#endif
