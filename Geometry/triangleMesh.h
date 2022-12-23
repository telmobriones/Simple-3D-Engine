// -*-C++-*-
#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <list>
#include "trfm3D.h"
#include "material.h"
#include "bbox.h"
#include "glm.h"

class TriangleMesh {

public:

	enum type_t {
		trm = 1, // no tex coords, no bump
		texcoords = 2, // tex coords
		bump = 4 // bump mapping texture associated
	};

	/**
	 * Create a list of triangle meshes from a wavefront (.obj) file
	 *
	 * Leave the result in list 'l'
	 */
	static void CreateTMeshObj(const std::string & DirName, const std::string & FileName,
							   std::list<TriangleMesh *> & l);

	TriangleMesh();
	~TriangleMesh();

	// Material
	void assignMaterial(Material *front, Material *back);

	// Add points, normals, texcoords
	int addPoint(const Vector3 & P); // Add a vertex to the mesh. Return the index of the added vertex.
	int addNormal(const Vector3 & N); // Add a normal to the mesh. Return the index of the added normal.
	// Add a tex coord. NOTE: only first 2 coords of st are actually used
	int addTexCoord(const Vector3 & st);
	// Add triangle (with previously added vertex/normnal/tcoords)
	// Add a triangle giving vertices. Normal are calculated.
	void addTriangle(int P0, int P1, int P2);
	// Add a triangle giving vertices and normals.
	void addTriangle(int P0, int P1, int P2,
					 int N0, int N1, int N2);
	// Add a triangle giving vertices and tex coordinates.
	void addTriangleNoNormal(int P0, int P1, int P2,
							 int T0, int T1, int T2);
	// Add a triangle giving vertices, normals and texture coordinates.
	void addTriangle(int P0, int P1, int P2,
					 int N0, int N1, int N2,
					 int T0, int T1, int T2);

	// Retieve indices
	int *vIdx(size_t i); // get vertex indices for triangle i
	int *nIdx(size_t i); // get normal indices for triangle i
	int *tIdx(size_t i); // get tex. coordinate indices for triangle i
	int *tgtIdx(size_t i); // get tangent indicess for triangle i
	int *btgtIdx(size_t i); // get bitangent indicess for triangle i
	// same thing, const version
	const int *vIdx(size_t i) const; // get vertex indices for triangle i
	const int *nIdx(size_t i) const; // get normal indices for triangle i
	const int *tIdx(size_t i) const; // get tex. coordinate indices for triangle i
	const int *tgtIdx(size_t i) const; // get tangent indicess for triangle i
	const int *btgtIdx(size_t i) const; // get bitangent indicess for triangle i

	// Retieve coordinates
	float *vCoords(size_t idx); // get coordinates of vertex i
	float *nCoords(size_t idx); // get coordinates of normal i
	float *texCoords(size_t idx); // get coordinates of texcoord i
	float *tgtCoords(size_t idx); // get coordinates of texcoord i
	float *btgtCoords(size_t idx); // get coordinates of texcoord i
	// same thing, const version
	const float *vCoords(size_t idx) const; // get coordinates of vertex i
	const float *nCoords(size_t idx) const; // get coordinates of normal i
	const float *texCoords(size_t idx) const; // get coordinates of texcoord i
	const float *tgtCoords(size_t idx) const; // get coordinates of texcoord i
	const float *btgtCoords(size_t idx) const; // get coordinates of texcoord i

	// Stats
	size_t numTriangles() const; // number of triangles
	size_t numVertices() const; // number of (distinct) vertices
	size_t numNormals() const; // number of (distinct) normals
	size_t numTexCoords() const; // number of (distinct) tex coords.
	size_t numTangents() const; // number of texture tangents
	size_t numBitangents() const; // number of texture bitangents

	// Recalculate functions
	void renormalize(); // Normalize all normals
	void setFaceted(); // Recalculate all normals to create a faceted mesh
	void setSmooth(); // Recalculate all normals to create a smooth mesh
	int removeDoubles(); // Merge vertices sharing position (a-la blender function)

	// change BBox to include Tmesh vertices
	void includeBBox(BBox * box) const;
	void includeBBox(BBox & box) const;

	// apply a trfm to all vertices
	void applyTrfm(const Trfm3D * trfm);

	const Material *getMaterial(bool front = true) const;
	Material *getMaterial(bool front = true);
	void setMaterial(Material *, bool front = true);

	void print() const;

	friend class TriangleMeshGL;

private:

	// Create a triangle mesh given a glm model
	TriangleMesh(int numtriangles, GLuint *triangles, GLMmodel *model,
				 Material *front, Material *back);
	TriangleMesh(const TriangleMesh & o);
	TriangleMesh & operator=(const TriangleMesh & o);

	void tangentTMesh(); // Calculate TBN

	//! one material (not owned)
	type_t m_type;
	Material *m_materialFront;
	Material *m_materialBack;
	bool m_hasTex; // Whether tmesh has texture
	bool m_isTransp;  // Whether the material is transparent

	std::vector<float> m_vCoords;      // Vertex coordinates. Number of distinct vertices: m_vCoords.size()/3 */
	std::vector<float> m_nCoords;      // Normal coordinates (x, y, z).
	std::vector<float> m_texCoords;    // Texture coordinates (x, y).
	std::vector<float> m_tgtCoords;    // Tangent coordinates (x, y, z)
	std::vector<float> m_btgtCoords;   // Bitangent coordinates  (x, y, z)

	std::vector<int> m_vIndices;   // Triangle vertex indices. Number of triangles: m_vIndices.size()/3
	std::vector<int> m_nIndices;   // Triangle normal indices.
	std::vector<int> m_texIndices; // Triangle texture indices.
	std::vector<int> m_tgtIndices;  // Triangle tangent indices.
	std::vector<int> m_btgtIndices; // Triangle bitangent indices.

	bool     m_vbo_uptodate; // whether the VAO/VBO are up-to-date
	// VBO
	GLuint  m_vbo_id; // Vertex Buffer Object id
	// VAO
	GLuint  m_vao_id; // Vertex Array Object
};
