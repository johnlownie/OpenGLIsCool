#include "cVAOManager.h"

#include "globalOpenGLStuff.h"

#include <fstream>
#include <vector>
#include <sstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	this->minX = this->minY = this->minZ = 0.0f;
	this->maxX = this->maxY = this->maxZ = 0.0f;
	this->centreX = this->centreY = this->centreZ = 0.0f;
	this->deltaX = this->deltaY = this->deltaZ = 0.0f;
	this->maxExtent = 0.0f;
	this->scaleForUnitBB = 0.0f;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL

	return;
}

sModelDrawInfo::sModelDrawInfo(std::string filename) {
	sModelDrawInfo();

	this->meshName = filename;

	return;
}

cVAOManager::cVAOManager()
{
	this->basepath = "assets/models/";

	return;
}

cVAOManager::~cVAOManager()
{
	return;
}

bool cVAOManager::LoadModelIntoVAO(std::string fileName, sModelDrawInfo& drawInfo, unsigned int shaderProgramID)
{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

	if ( ! this->m_LoadTheModel( fileName, drawInfo ) )
	{
		this->m_AppendTextToLastError( "Didn't load model", true );
		return false;
	}

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	glBufferData( GL_ARRAY_BUFFER, sizeof(sVert_xyzw_rbga_UVx2) * drawInfo.numberOfVertices, (GLvoid*) drawInfo.pVertices, GL_STATIC_DRAW );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * drawInfo.numberOfIndices, (GLvoid*) drawInfo.pIndices, GL_STATIC_DRAW );

	// Set the vertex attributes.
	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");
	GLint vnorm_location = glGetAttribLocation(shaderProgramID, "vNormal");
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");
	GLint vUVx2_location = glGetAttribLocation(shaderProgramID, "vUVx2");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer( vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVert_xyzw_rbga_UVx2), ( void* )offsetof(sVert_xyzw_rbga_UVx2, x));

	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVert_xyzw_rbga_UVx2), ( void* )offsetof(sVert_xyzw_rbga_UVx2, nx));

	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVert_xyzw_rbga_UVx2), (void*)offsetof(sVert_xyzw_rbga_UVx2, r));

	glEnableVertexAttribArray(vUVx2_location);
	glVertexAttribPointer(vUVx2_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVert_xyzw_rbga_UVx2), (void*)offsetof(sVert_xyzw_rbga_UVx2, u1));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vnorm_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vUVx2_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;

	return true;
}

bool cVAOManager::LoadModelsIntoVAO(std::vector<sModelDrawInfo> &vecDrawInfos, unsigned int shaderProgramId)
{
	bool bAllGood = true;

	for (std::vector<sModelDrawInfo>::iterator it = vecDrawInfos.begin(); it != vecDrawInfos.end(); it++)
	{
		if (!this->LoadModelIntoVAO(it->meshName, *it, shaderProgramId))
		{
			bAllGood = false;
		}
	}

	return bAllGood;
}

bool cVAOManager::FindDrawInfoByModelName( std::string filename, sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/, sModelDrawInfo /* info needed to draw*/ >::iterator itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;

	return true;
}

bool cVAOManager::m_LoadTheModel(std::string fileName, sModelDrawInfo &drawInfo)
{
	// Open the file. 
	// Read until we hit the word "vertex"
	// Read until we hit the word "face"
	// Read until we hit the word "end_header"

	std::string fullPathName = this->basepath + fileName;

	std::ifstream thePlyFile(fullPathName.c_str() );
	if ( ! thePlyFile.is_open() )
	{	// Something is wrong...
		std::stringstream ssError;
		ssError << "Can't open >" << fullPathName << "< file." << std::endl;
		this->m_AppendTextToLastError(ssError.str());

		return false;
	}

	std::string temp; 
	while ( thePlyFile >> temp )
	{
		if ( temp == "vertex" ) 
		{
			break;
		}
	}; 
	// read the number of vertices...
	thePlyFile >> drawInfo.numberOfVertices;	//::g_NumberOfVertices;

	while ( thePlyFile >> temp )
	{
		if ( temp == "face" ) 
		{
			break;
		}
	}; 
	// read the number of triangles...
	thePlyFile >> drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles;


	while ( thePlyFile >> temp )
	{
		if ( temp == "end_header" ) 
		{
			break;
		}
	}; 

	// And now, we start reading vertices... Hazzah!
	
	// This is set up to match the ply (3d model) file. 
	// NOT the shader. 
	struct sVertPly
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec4 colour;
		glm::vec2 textUV;
	};

	std::vector<sVertPly> vecTempPlyVerts;

	sVertPly tempVert;
	// Load the vertices...
	for ( unsigned int index = 0; index != drawInfo.numberOfVertices; index++ )
	{
		thePlyFile >> tempVert.pos.x >> tempVert.pos.y >> tempVert.pos.z;

		thePlyFile >> tempVert.normal.x >> tempVert.normal.y >> tempVert.normal.z;
		
		thePlyFile >> tempVert.colour.x >> tempVert.colour.y >> tempVert.colour.z >> tempVert.colour.w;

		thePlyFile >> tempVert.textUV.x >> tempVert.textUV.y;

		// Scale the colour from 0 to 1, instead of 0 to 255
		tempVert.colour.x /= 255.0f;
		tempVert.colour.y /= 255.0f;
		tempVert.colour.z /= 255.0f;
		tempVert.colour.a /= 255.0f;

		// Add too... what? 
		vecTempPlyVerts.push_back(tempVert);
	}

	// Calculate extents of the model, assume vertex 0 is the min and max to start
	drawInfo.minX = drawInfo.maxX = vecTempPlyVerts[0].pos.x;
	drawInfo.minY = drawInfo.maxY = vecTempPlyVerts[0].pos.y;
	drawInfo.minZ = drawInfo.maxZ = vecTempPlyVerts[0].pos.z;

	for (unsigned int i = 0; i != static_cast<unsigned int>(vecTempPlyVerts.size()); i++)
	{
		if (vecTempPlyVerts[i].pos.x < drawInfo.minX) drawInfo.minX = vecTempPlyVerts[i].pos.x;
		if (vecTempPlyVerts[i].pos.y < drawInfo.minY) drawInfo.minY = vecTempPlyVerts[i].pos.y;
		if (vecTempPlyVerts[i].pos.z < drawInfo.minZ) drawInfo.minZ = vecTempPlyVerts[i].pos.z;

		if (vecTempPlyVerts[i].pos.x > drawInfo.maxX) drawInfo.maxX = vecTempPlyVerts[i].pos.x;
		if (vecTempPlyVerts[i].pos.y > drawInfo.maxY) drawInfo.maxY = vecTempPlyVerts[i].pos.y;
		if (vecTempPlyVerts[i].pos.z > drawInfo.maxZ) drawInfo.maxZ = vecTempPlyVerts[i].pos.z;
	}

	drawInfo.deltaX = drawInfo.maxX - drawInfo.minX;
	drawInfo.deltaY = drawInfo.maxY - drawInfo.minY;
	drawInfo.deltaZ = drawInfo.maxZ - drawInfo.minZ;

	drawInfo.centreX = (drawInfo.deltaX / 2.0f) + drawInfo.minX;
	drawInfo.centreY = (drawInfo.deltaY / 2.0f) + drawInfo.minY;
	drawInfo.centreZ = (drawInfo.deltaZ / 2.0f) + drawInfo.minZ;

	drawInfo.maxExtent = drawInfo.deltaX;
	if (drawInfo.deltaY > drawInfo.maxExtent) drawInfo.maxExtent = drawInfo.deltaY;
	if (drawInfo.deltaZ > drawInfo.maxExtent) drawInfo.maxExtent = drawInfo.deltaZ;

	drawInfo.scaleForUnitBB = 1.0f / drawInfo.maxExtent;

	// Create a local vertex array
	// Note: The format the file (ply) is DIFFERENT from this array:
	// - sVertPly was made to match the file format
	// - sVert was made to match the shader vertex attrib format

	drawInfo.pVertices = new sVert_xyzw_rbga_UVx2[drawInfo.numberOfVertices];

	// Optional clear array to zero 
	//memset( drawInfo.pVertices, 0, sizeof(sVert) * drawInfo.numberOfVertices);

	for ( unsigned int index = 0; index != drawInfo.numberOfVertices; index++ )
	{
		drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
		drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
		drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
		drawInfo.pVertices[index].w = 1.0f;

		drawInfo.pVertices[index].nx = vecTempPlyVerts[index].normal.x;
		drawInfo.pVertices[index].ny = vecTempPlyVerts[index].normal.y;
		drawInfo.pVertices[index].nz = vecTempPlyVerts[index].normal.z;
		drawInfo.pVertices[index].nw = 1.0f;

		drawInfo.pVertices[index].r = vecTempPlyVerts[index].colour.r;
		drawInfo.pVertices[index].g = vecTempPlyVerts[index].colour.g;
		drawInfo.pVertices[index].b = vecTempPlyVerts[index].colour.b;
		drawInfo.pVertices[index].a = vecTempPlyVerts[index].colour.a;

		drawInfo.pVertices[index].u1 = vecTempPlyVerts[index].textUV.x;
		drawInfo.pVertices[index].v1 = vecTempPlyVerts[index].textUV.y;
		drawInfo.pVertices[index].u2 = vecTempPlyVerts[index].textUV.x;
		drawInfo.pVertices[index].v2 = vecTempPlyVerts[index].textUV.y;
	}

	struct sTriPly
	{
		unsigned int vindex[3];		// the 3 indices
		sVertPly verts[3];			// The actual vertices
	};

	std::vector<sTriPly> vecTempPlyTriangles;
	sTriPly tempTriangle;
	unsigned int discard = 0;
	for ( unsigned int index = 0; index != drawInfo.numberOfTriangles;	// ::g_NumberOfTriangles; 
		  index++ )
	{
		// 3 5622 5601 5640
		thePlyFile >> discard >> tempTriangle.vindex[0] >> tempTriangle.vindex[1] >> tempTriangle.vindex[2];

		//// Look up the vertex that matches the triangle index values.
		//tempTriangle.verts[0] = vecTempPlyVerts[ tempTriangle.vindex[0] ];
		//tempTriangle.verts[1] = vecTempPlyVerts[ tempTriangle.vindex[1] ];
		//tempTriangle.verts[2] = vecTempPlyVerts[ tempTriangle.vindex[2] ];

		vecTempPlyTriangles.push_back( tempTriangle );
	}

	
	// NOW, we need to put them into the vertex array buffer that 
	//	will be passed to OpenGL. Why? 
	// Because we called glDrawArrays(), that's why. 
	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;
	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	// Optional clear array to zero 
	//memset( drawInfo.pIndices, 0, sizeof(unsigned int) * drawInfo.numberOfIndices);

	// Allocate on the HEAP, so infinite size... 
	// delete pVertices			/// error!
	// delete [] pVertices		/// correct!!
	unsigned int indexBufferIndex = 0;
	for ( unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; triIndex++, indexBufferIndex += 3 )
	{
		sTriPly& curTri = vecTempPlyTriangles[triIndex];

		// This is taking an array of triangles vertex indices 
		// (which is a 2D array of indices, really) and converting
		//  it into a 1D array of indices...)
		drawInfo.pIndices[indexBufferIndex + 0] = curTri.vindex[0];
		drawInfo.pIndices[indexBufferIndex + 1] = curTri.vindex[1];
		drawInfo.pIndices[indexBufferIndex + 2] = curTri.vindex[2];
	}

	return true;
}


std::string cVAOManager::getLastError(bool bAndClear /*=true*/)
{
	std::string theLastError = this->m_lastErrorString;

	if ( bAndClear )
	{
		this->m_lastErrorString = "";
	}

	return theLastError;
}

void cVAOManager::m_AppendTextToLastError(std::string text, bool addNewLineBefore /*=true*/)
{
	std::stringstream ssError;
	ssError << this->m_lastErrorString;

	if ( addNewLineBefore )
	{
		ssError << std::endl;
	}

	ssError << text;

	this->m_lastErrorString = ssError.str();

	return;
}

void cVAOManager::SetBaseLoadPath(std::string basepath)
{
	this->basepath = basepath;

	return;
}

std::string cVAOManager::GetBaseLoadPath(void)
{
	return this->basepath;
}

