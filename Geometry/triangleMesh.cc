// If triangle span
// Vertices: v (>2)
// Triangles: v - 2
// Edges: 2v + 1

#include <cassert>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <map>
#include "triangleMesh.h"
#include "glm.h"
#include "tools.h"
#include "materialManager.h"
#include "textureManager.h"

// If triangle span
// Vertices: v (>2)
// Triangles: v - 2
// Edges: 2v + 1

using std::vector;
using std::list;
using std::map;
using std::pair;
using std::string;

// Calculate texture TBN (tangent, bitangent, normal) of a Tmesh
static void tangentTMesh(TriangleMesh *thisMesh);

// normal of a triangle
static Vector3 normalTriangle(const Vector3 & v0, const Vector3 & v1, const Vector3 & v2);

// Tangent and bitangent of a triangle
static void tangentTriangle(const float v0[3], const float v1[3], const float v2[3],
							const float uv0[2], const float uv1[2], const float uv2[2],
							Vector3 & T, Vector3 & B, Vector3 & N);

// semantics of strcmp. 0 if equal.

static int cmp_3f(const float *V1,
				  const float *V2) {

	float aux;
	aux = V1[0] - V2[0];
	if (!distance_is_zero(aux)) return aux > 0 ? 1 : -1;
	aux = V1[1] - V2[1];
	if (!distance_is_zero(aux)) return aux > 0 ? 1 : -1;
	aux = V1[2] - V2[2];
	if (!distance_is_zero(aux)) return aux > 0 ? 1 : -1;
	return 0;
}

static int cmp_2f(const float *V1,
				  const float *V2) {
	float aux;
	aux = V1[0] - V2[0];
	if (!distance_is_zero(aux)) return aux > 0 ? 1 : -1;
	aux = V1[1] - V2[1];
	if (!distance_is_zero(aux)) return aux > 0 ? 1 : -1;
	return 0;
}

TriangleMesh::TriangleMesh() :
	m_type(trm),
	m_materialFront(MaterialManager::instance()->getDefault()),
	m_materialBack(MaterialManager::instance()->getDefault()),
	m_hasTex(false), m_isTransp(false),
	m_vbo_uptodate(true),
	m_vbo_id(0),
	m_vao_id(0) {}

TriangleMesh::~TriangleMesh() {
	// reclaim openGL buffers
	if (m_vbo_id)
		glDeleteBuffers(1, &m_vbo_id);
	if (m_vao_id)
		glDeleteVertexArrays(1, &m_vao_id);
}

void TriangleMesh::assignMaterial(Material *front, Material *back) {
	m_materialFront = front;
	m_materialBack = back;
}

int TriangleMesh::addPoint(const Vector3 & P) {
	int res = m_vCoords.size() / 3;
	m_vCoords.push_back(P[0]);
	m_vCoords.push_back(P[1]);
	m_vCoords.push_back(P[2]);
	return res;
}

int TriangleMesh::addNormal(const Vector3 & N) {
	int res = m_nCoords.size() / 3;
	m_nCoords.push_back(N[0]);
	m_nCoords.push_back(N[1]);
	m_nCoords.push_back(N[2]);
	return res;
}

// Add a tex coord. NOTE: only first 2 coords of st are actually used
int TriangleMesh::addTexCoord(const Vector3 & st) {
	int res = m_texCoords.size() / 2;
	m_texCoords.push_back(st[0]);
	m_texCoords.push_back(st[1]);
	return res;
}

// Add triangle (with previously added vertex/normnal/tcoords)
// Add a triangle giving vertices. Normal are calculated.
// return the index of the newly created triangle.
void TriangleMesh::addTriangle(int P0, int P1, int P2) {
	m_vIndices.push_back(P0);
	m_vIndices.push_back(P1);
	m_vIndices.push_back(P2);
	// Calculate face normal
	Vector3 N = normalTriangle(Vector3(vCoords(P0)),
							   Vector3(vCoords(P1)),
							   Vector3(vCoords(P2)));
	int nidx = addNormal(N);
	m_nIndices.push_back(nidx);
	m_nIndices.push_back(nidx);
	m_nIndices.push_back(nidx);
	m_vbo_uptodate = 0;
}

// Add a triangle giving vertices and normals.
// return the index of the newly created triangle.
void TriangleMesh::addTriangle(int P0, int P1, int P2,
							   int N0, int N1, int N2) {
	m_vIndices.push_back(P0);
	m_vIndices.push_back(P1);
	m_vIndices.push_back(P2);
	m_nIndices.push_back(N0);
	m_nIndices.push_back(N1);
	m_nIndices.push_back(N2);
	m_vbo_uptodate = 0;
}

// Add a triangle giving vertices, normals and tex coords.
// return the index of the newly created triangle.
void TriangleMesh::addTriangle(int P0, int P1, int P2,
							   int N0, int N1, int N2,
							   int T0, int T1, int T2) {
	m_vIndices.push_back(P0);
	m_vIndices.push_back(P1);
	m_vIndices.push_back(P2);
	m_nIndices.push_back(N0);
	m_nIndices.push_back(N1);
	m_nIndices.push_back(N2);
	m_texIndices.push_back(T0);
	m_texIndices.push_back(T1);
	m_texIndices.push_back(T2);
	m_vbo_uptodate = 0;
}

void TriangleMesh::addTriangleNoNormal(int P0, int P1, int P2,
									   int T0, int T1, int T2) {
	m_vIndices.push_back(P0);
	m_vIndices.push_back(P1);
	m_vIndices.push_back(P2);
	// Calculate face normal
	Vector3 N = normalTriangle(Vector3(vCoords(P0)),
							   Vector3(vCoords(P1)),
							   Vector3(vCoords(P2)));
	int nidx = addNormal(N);
	m_nIndices.push_back(nidx);
	m_nIndices.push_back(nidx);
	m_nIndices.push_back(nidx);
	m_texIndices.push_back(T0);
	m_texIndices.push_back(T1);
	m_texIndices.push_back(T2);
	m_vbo_uptodate = 0;
}

///////

// Retieve indices
int *TriangleMesh::vIdx(size_t i) { return &m_vIndices[3 * i ]; } // get vertex indices for triangle i
int *TriangleMesh::nIdx(size_t i) { return &m_nIndices[3 * i ]; } // get normal indices for triangle i
int *TriangleMesh::tIdx(size_t i) { return &m_texIndices[3 * i ]; } // get tex. coordinate indices for triangle i
int *TriangleMesh::tgtIdx(size_t i) { return &m_tgtIndices[3 * i ]; } // get tangent indicess for triangle i
int *TriangleMesh::btgtIdx(size_t i) { return &m_btgtIndices[3 * i ]; } // get bitangent indicess for triangle i

const int *TriangleMesh::vIdx(size_t i) const { return &m_vIndices[3 * i ]; } // get vertex indices for triangle i
const int *TriangleMesh::nIdx(size_t i) const { return &m_nIndices[3 * i ]; } // get normal indices for triangle i
const int *TriangleMesh::tIdx(size_t i) const { return &m_texIndices[3 * i ]; } // get tex. coordinate indices for triang
const int *TriangleMesh::tgtIdx(size_t i) const	{ return &m_tgtIndices[3 * i ]; } // get tangent indicess for triangle i
const int *TriangleMesh::btgtIdx(size_t i) const { return &m_btgtIndices[3 * i ]; } // get bitangent indicess for triangle i

// Retieve coordinates
float *TriangleMesh::vCoords(size_t i) { return &m_vCoords[3 * i]; } // get coordinates of vertex i
float *TriangleMesh::nCoords(size_t i) { return &m_nCoords[3 * i]; } // get coordinates of normal i
float *TriangleMesh::texCoords(size_t i) { return &m_texCoords[2 * i]; } // get coordinates of texcoord i
float *TriangleMesh::tgtCoords(size_t i) { return &m_tgtCoords[3 * i]; } // get coordinates of texcoord i
float *TriangleMesh::btgtCoords(size_t i) { return &m_btgtCoords[3 * i]; } // get coordinates of texcoord i

const float *TriangleMesh::vCoords(size_t i) const { return &m_vCoords[3 * i]; } // get coordinates of vertex i
const float *TriangleMesh::nCoords(size_t i) const { return &m_nCoords[3 * i]; } // get coordinates of normal i
const float *TriangleMesh::texCoords(size_t i) const { return &m_texCoords[2 * i]; } // get coordinates of texcoord i
const float *TriangleMesh::tgtCoords(size_t i) const { return &m_tgtCoords[3 * i]; } // get coordinates of texcoord i
const float *TriangleMesh::btgtCoords(size_t i) const { return &m_btgtCoords[3 * i]; } // get coordinates of texcoord i

/////////////////
// Stats
size_t TriangleMesh::numTriangles() const { return m_vIndices.size() / 3; } // number of triangles
size_t TriangleMesh::numVertices() const { return m_vCoords.size() / 3; } // number of vertices
size_t TriangleMesh::numNormals() const { return m_nCoords.size() / 3; } // number of normals
size_t TriangleMesh::numTexCoords() const { return m_texCoords.size() / 2; } // number of tex coords.
size_t TriangleMesh::numTangents() const { return m_tgtCoords.size() / 3; } // number of texture tangents
size_t TriangleMesh::numBitangents() const { return m_btgtCoords.size() / 3; } // number of texture bitangents

///////

// Construct tmesh from glm struct

struct tmesh_vcoords3_sort_t {
	tmesh_vcoords3_sort_t(const float *vertices) : V(vertices) {};
	bool operator() (int i, int j) {
		int aux = cmp_3f(&V[ 3*i ], &V[ 3*j ]);
		return aux < 0;
	}
	const float *V;
};

struct tmesh_vcoords2_sort_t {
	tmesh_vcoords2_sort_t(const float *vertices) : V(vertices) {};
	bool operator() (int i, int j) {
		int aux = cmp_2f(&V[ 2*i ], &V[ 2*j] );
		return aux < 0;
	}
	const float *V;
};

static void copy_unique_coords_fast(const float * vertices,
									int numvertices, int s,
									vector<float> & new_vertices,
									vector<int> & idxmap) {
	assert(s == 2 || s == 3);
	if(!numvertices) return;
	vector<int>(numvertices, -1).swap(idxmap);
	vector<int> newIdx(numvertices);
	for(int i = 0; i < numvertices; i++) newIdx[i] = i;
	if (s == 2) {
		sort(newIdx.begin(), newIdx.end(), tmesh_vcoords2_sort_t(vertices));
	} else {
		sort(newIdx.begin(), newIdx.end(), tmesh_vcoords3_sort_t(vertices));
	}

	const float *last_vertex = &vertices[newIdx[0] * s];
	// insert first vertex
	int last_index = 0;
	for(int c = 0; c < s; ++c) {
		new_vertices.push_back(*(last_vertex + c));
	}
	idxmap[ newIdx[0] ] = last_index;
	for(int i = 1; i < numvertices; i++) {
		const float *cur_vertex = &vertices[ newIdx[i] * s ];
		int cmp = (s == 2) ? cmp_2f(cur_vertex, last_vertex) : cmp_3f(cur_vertex, last_vertex);
		if(cmp) {
			last_index++;
			for(int c = 0; c < s; ++c) {
				new_vertices.push_back(*(cur_vertex + c));
			}
			last_vertex = cur_vertex;
		}
		idxmap[ newIdx[i] ] = last_index;
	}
}

int TriangleMesh::removeDoubles() {

	vector<float> vCoords_new;
	vector<int> pos_idxmap;
	copy_unique_coords_fast(&m_vCoords[0], m_vCoords.size() / 3, 3,
							vCoords_new, pos_idxmap);
	int res = (m_vCoords.size() - vCoords_new.size()) / 3; // number of vertices removed
	m_vCoords.swap(vCoords_new);
	vector<float>().swap(vCoords_new); // reclaim space
	// create 1:1 normals for each new vertex, by aggregating normals
	// corresponding to old vertices
	vector<float> nCoords_agg(m_vCoords.size(), 0.0); // as many normals as vertices
	for(size_t t = 0, tn = numTriangles(); t != tn; ++t) {
		for (int s = 0; s < 3; ++s) {
			int Pi = m_vIndices[3*t + s];
			int Ni = m_nIndices[3*t + s];
			nCoords_agg[ 3 * pos_idxmap[ Pi ] ] = m_nCoords[ 3 * Ni];
			nCoords_agg[ 3 * pos_idxmap[ Pi ] + 1 ] = m_nCoords[ 3 * Ni + 1];
			nCoords_agg[ 3 * pos_idxmap[ Pi ] + 2 ] = m_nCoords[ 3 * Ni + 2];
		}
	}
	// noralize aggregate normals
	for(int i = 0, m = nCoords_agg.size() / 3; i < m; i++) {
		float *N = &nCoords_agg[3*i];
		Vector3 aux(N);
		aux = aux.normalize();
		N[0] = aux[0];
		N[1] = aux[1];
		N[2] = aux[2];
	}
	//update vIndices and nIndices
	for(size_t i = 0, m = m_vIndices.size(); i < m; ++i) {
		m_vIndices[i] = pos_idxmap[ m_vIndices[i] ];
		m_nIndices[i] = m_vIndices[i];
	}
	// Remove doubles in normals
	vector<float>().swap(m_nCoords);
	vector<int> norm_idxmap;
	copy_unique_coords_fast(&nCoords_agg[0], nCoords_agg.size() / 3, 3,
							m_nCoords, norm_idxmap);
	//last update of nIndices
	for(size_t i = 0, m = m_nIndices.size(); i < m; ++i) {
		m_nIndices[i] = norm_idxmap[ m_nIndices[i] ];
	}
	return res;
}

/*!

  Creates a new triangle mesh given a model previously readed by glm.

  \param n: number of triangles
  \param triangles: array of n triangles
  \param model: glm model (where the actual vertices, normals, textures coordinates are)
  \param front: fronfacing materiak
  \param back: backfacing materiak
  \param type: type of mesh (trm, tex, bump)

  \return a pointer to the new created triangle mesh

*/

// Create a triangle mesh given a glm model
TriangleMesh::TriangleMesh(int numtriangles, GLuint *triangles, GLMmodel *model,
						   Material *front, Material *back)
	: m_materialFront(front), m_materialBack(back)
{
	int type = TriangleMesh::trm;
	if (model->numtexcoords) type |= TriangleMesh::texcoords;
	if (front && front->hasBump()) type |= TriangleMesh::bump;
	if (back && back->hasBump()) type |= TriangleMesh::bump;
	m_type = static_cast<type_t>(type);

	vector<float>(&model->vertices[3], &model->vertices[3] + model->numvertices * 3).swap(m_vCoords);
	m_vIndices.reserve(3 * numtriangles);
	if (model->numnormals) {
		vector<float>(&model->normals[3], &model->normals[3] + model->numnormals * 3).swap(m_nCoords);
		m_nIndices.reserve(3 * numtriangles);
	}
	vector<int> tex_idxmap;
	if (m_type & TriangleMesh::texcoords) {
		copy_unique_coords_fast(&(model->texcoords[2]), model->numtexcoords, 2,
								m_texCoords, tex_idxmap);
		m_texIndices.reserve(3 * numtriangles);
	}
	for (int i = 0; i < numtriangles; ++i) {
		GLMtriangle *T = &(model->triangles[triangles[i]]);
		m_vIndices.push_back( T->vindices[0] - 1 );
		m_vIndices.push_back( T->vindices[1] - 1 );
		m_vIndices.push_back( T->vindices[2] - 1 );
		if (model->numnormals) {
			m_nIndices.push_back( T->nindices[0] - 1 );
			m_nIndices.push_back( T->nindices[1] - 1 );
			m_nIndices.push_back( T->nindices[2] - 1 );
		}
		if (m_type & TriangleMesh::texcoords) {
			m_texIndices.push_back(tex_idxmap[ std::max((int)(T->tindices[0]) - 1, 0) ]);
			m_texIndices.push_back(tex_idxmap[ std::max((int)(T->tindices[1]) - 1, 0) ]);
			m_texIndices.push_back(tex_idxmap[ std::max((int)(T->tindices[2]) - 1, 0) ]);
		}
	}
	if(!model->numnormals) {
		setFaceted();
	}

	// Calculate TBN
	if (m_type & TriangleMesh::bump) {
		tangentTMesh();
	}
	// OpenGL VBO init
	m_vbo_id = 0;
	m_vao_id = 0;
	m_vbo_uptodate = 0;
}

static Material *create_mat(GLMmodel * m, GLMmaterial * mat, const string & DirName, const string & libname) {
	string mtl_fullname = getFilename(DirName, libname);
	Material *newMaterial = MaterialManager::instance()->create(mtl_fullname + "#" + string(mat->name));
	//newMaterial->setAmbient(&mat->ambient[0]);
	newMaterial->setAlpha(mat->diffuse[3]);
	newMaterial->setDiffuse(&mat->diffuse[0]);
	if (mat->specular[0] != 0.0f ||
		mat->specular[1] != 0.0f ||
		mat->specular[2] != 0.0f) {
		newMaterial->setSpecular(&mat->specular[0], mat->shininess);
	}
	TextureManager * texMgr = TextureManager::instance();
	if (mat->texturemap) {
		string image = getFilename(DirName, mat->texturemap);
		newMaterial->setTexture(texMgr->create(mat->texturemap, image));
	}

	if (mat->bumpmap) {
		string image = getFilename(DirName, mat->bumpmap);
		newMaterial->setBumpMap(texMgr->createBumpMap(mat->bumpmap, image));
	}

	if (mat->specularmap) {
		string image = getFilename(DirName, mat->specularmap);
		newMaterial->setSpecularMap(texMgr->createBumpMap(mat->specularmap, image));
	}
	return newMaterial;
}

void TriangleMesh::CreateTMeshObj(const string & DirName, const string & FileName, list<TriangleMesh *> & surfaces) {

	// Given DirName, FileName ("obj/cubes", "cubo.obj")
	//
	// obj_fullname = "obj/cubes/cubo.obj"
	// mtl_fullname = "obj/cubes/cubo.mtl"
	// lib_name = mtl_fullname
	// lib_fname = "cubo.mtl"
	Material *default_mat = MaterialManager::instance()->getDefault();
	string obj_fullname(DirName);
	obj_fullname.append(FileName);
	GLMmodel *m = glmReadOBJ(obj_fullname.c_str());
	GLMgroup *g = &(m->groups[0]);
	while(g) {
		if (g->numtriangles) {
			Material *mat = default_mat;
			if (g->material)
				mat = create_mat(m, &(m->materials[g->material]), DirName, string(m->mtllibname));
			// Create and store the surface (triangleMesh)
			TriangleMesh *surface = new TriangleMesh(g->numtriangles,
													 g->triangles, m, mat, mat);
			surfaces.push_back(surface);
		}
		g = g->next;
	}
	glmDelete(m);
}


static Vector3 unormalTriangle(const Vector3 & v0, const Vector3 & v1, const Vector3 & v2) {
	Vector3 u = v1 - v0;
	Vector3 v = v2 - v0;
	// n = u x v
	return crossVectors(u, v);
}

static Vector3 normalTriangle(const Vector3 & v0, const Vector3 & v1, const Vector3 & v2) {
	Vector3 n = unormalTriangle(v0, v1, v2).normalize();
	return n;
}

// renormalize all mesh normals
void TriangleMesh::renormalize() {
	for(int i = 0, m = numNormals(); i < m; i++) {
		float *N = nCoords(i);
		Vector3 aux(N);
		aux = aux.normalize();
		N[0] = aux[0];
		N[0] = aux[1];
		N[0] = aux[2];
	}
	if (m_type & TriangleMesh::bump) {
		tangentTMesh(); // recalculate TBN
	}
	m_vbo_uptodate = 0;
}

struct bthash_t {
	typedef pair<int, int > key_type;

	bthash_t(TriangleMesh & tmesh) : m_tgtN(0) {
		pair<map<key_type, int>::iterator, bool> P;
		int *Pidx, *Nidx;
		int triang_n = tmesh.numTriangles();
		for(int t = 0; t < triang_n; ++t) {
			Pidx = tmesh.vIdx(t);
			Nidx = tmesh.nIdx(t);
			for(int j = 0; j < 3; j++) {
				key_type key = std::make_pair(Pidx[j], Nidx[j]);
				P = m_hash.insert(std::make_pair(key, m_tgtN));
				if (P.second) // inserted
					m_tgtN++;
			}
		}
	}

	int find(int k1, int k2) {
		map<key_type, int>::iterator it = m_hash.find(std::make_pair(k1, k2));
		if (it == m_hash.end()) return -1;
		return it->second;
	}
	map<key_type, int> m_hash;
	int m_tgtN; // number of tangents
};

static void add_vec(vector<float> &V, int idx, float p[3]) {

	V[idx * 3] = p[0];
	V[idx * 3 + 1] = p[1];
	V[idx * 3 + 2] = p[2];
}

/* // Gram-Schmidt orthogonalize */
// static void gram_schmidt(TriangleMesh *mesh) {

//     float *n;
//     float *t;
//     float *b;

//     int T = NumTrianglesTMesh(mesh);
//     for(int i = 0; i < T; i++) {
//         for (int j = 0; j < 3; j++) {
//             n = GetNormalTMesh(mesh, atVectorI(mesh->nIndices, 3*i + j));
//             t = GetT_TMesh(mesh, atVectorI(mesh->tgtIndices, 3*i + j));
//             b = GetT_TMesh(mesh, atVectorI(mesh->btgtIndices, 3*i + j));

//             // t = normalize(t - n * dot(n, t));
//             float dotnt = DOT_PRODUCT(n[0], n[1], n[2],
//                                       t[0], t[1], t[2]);
//             t[0] -= n[0] * dotnt;
//             t[1] -= n[1] * dotnt;
//             t[2] -= n[2] * dotnt;
//             VectorNormalize(&t[0], &t[1], &t[2]);
//             //VectorNormalize(&b[0], &b[1], &b[2]);
//         }
//     }
// }

void TriangleMesh::tangentTMesh() {

	bthash_t bthash(*this);
	int triang_n = numTriangles();
	int tgtN = bthash.m_tgtN;
	vector<int>().swap(m_tgtIndices);
	m_tgtIndices.reserve(triang_n * 3);
	vector<int>().swap(m_btgtIndices);
	m_btgtIndices.reserve(triang_n * 3);
	vector<float>(tgtN * 3, 0.0).swap(m_tgtCoords);
	vector<float>(tgtN * 3, 0.0).swap(m_btgtCoords);
	float *P0, *P1, *P2, *Tex0, *Tex1, *Tex2;
	Vector3 T, B, N;
	for(int t = 0; t < triang_n; ++t) {
		// Vertices
		P0 = vCoords(m_vIndices[ t * 3 ]);
		P1 = vCoords(m_vIndices[ t * 3 + 1 ]);
		P2 = vCoords(m_vIndices[ t * 3 + 2 ]);
		// Textures
		Tex0 = texCoords(m_texIndices[ t * 3 ]);
		Tex1 = texCoords(m_texIndices[ t * 3 + 1 ]);
		Tex2 = texCoords(m_texIndices[ t * 3 + 2 ]);
		tangentTriangle(P0, P1, P2,
						Tex0, Tex1, Tex2,
						T, B, N);
		for(int j = 0; j < 3; j++) {
			int idx = bthash.find(m_vIndices[ t * 3 + j],
								  m_nIndices[ t * 3 + j]);
			assert(idx != -1);
			// add tangents
			m_tgtCoords[idx * 3] += T[0];
			m_tgtCoords[idx * 3 + 1] += T[1];
			m_tgtCoords[idx * 3 + 2] += T[2];
			m_tgtIndices.push_back(idx);
			// add bitangents
			m_btgtCoords[idx * 3] += B[0];
			m_btgtCoords[idx * 3 + 1] += B[1];
			m_btgtCoords[idx * 3 + 2] += B[2];
			m_btgtIndices.push_back(idx);
		}
	}
	// normalize T,B
	size_t numVecs = m_tgtCoords.size() / 3;
	for(size_t i = 0; i < numVecs; i++) {
		Vector3 T(&m_tgtCoords[i * 3]);
		T = T.normalize();
		m_tgtCoords[i * 3] = T[0];
		m_tgtCoords[i * 3 + 1] = T[1];
		m_tgtCoords[i * 3 + 2] = T[2];
		Vector3 B(&m_btgtCoords[i * 3]);
		B = B.normalize();
		m_btgtCoords[i * 3] = B[0];
		m_btgtCoords[i * 3 + 1] = B[1];
		m_btgtCoords[i * 3 + 2] = B[2];
	}
	//gram_schmidt();
	/* // Calculate handedness */
	/* // if (dot(cross(n, t), b) < 0.0f) t = -t */
	/* crossVV(&Q1[0], &Q1[1], &Q1[2], */
	/*         n[0], n[1], n[2], */
	/*         t[0], t[1], t[2]); */
	/* if(DOT_PRODUCT(Q1[0], Q1[1], Q1[2], */
	/*                b[0], b[1], b[2]) < 0.0f) { */
	/*     t[0] *= -1.0f; */
	/*     t[1] *= -1.0f; */
	/*     t[2] *= -1.0f; */
	/* } */
	m_vbo_uptodate = 0;
}

void TriangleMesh::setFaceted() {
	size_t numtriangles = numTriangles();
	vector<float>(3 * numtriangles).swap(m_nCoords);
	vector<int>(3 * numtriangles).swap(m_nIndices);
	for (size_t t = 0; t < numtriangles; ++t) {
		int i0 = m_vIndices[3 * t];
		int i1 = m_vIndices[3 * t + 1];
		int i2 = m_vIndices[3 * t + 2];
		// Calculate face normal
		Vector3 N = normalTriangle(Vector3(vCoords(i0)),
									Vector3(vCoords(i1)),
									Vector3(vCoords(i2)));
		m_nCoords[3 * t] = N[0];
		m_nCoords[3 * t + 1] = N[1];
		m_nCoords[3 * t + 2] = N[2];
		m_nIndices[3 * t] = t;
		m_nIndices[3 * t + 1] = t;
		m_nIndices[3 * t + 2] = t;
	}
	m_vbo_uptodate = 0;
}

static void tangentTriangle(const float v0[3], const float v1[3], const float v2[3],
							const float uv0[2], const float uv1[2], const float uv2[2],
							Vector3 & T, Vector3 & B, Vector3 & N) {

	// Q1 = v1 - v0
	Vector3 Q1(v1[0] - v0[0],
			   v1[1] - v0[1],
			   v1[2] - v0[2]);
	// Q2 = v2 - v0
	Vector3 Q2(v2[0] - v0[0],
			   v2[1] - v0[1],
			   v2[2] - v0[2]);

	// (s1,t1) = uv1 - uv0
	float s1  = uv1[0] - uv0[0];
	float t1  = uv1[1] - uv0[1];
	// (s2,t2) = uv2 - uv0
	float s2  = uv2[0] - uv0[0];
	float t2  = uv2[1] - uv0[1];
	float r = 1.0f / (s1 * t2 - s2 * t1);

	// tangent
	T = ((Q1 * t2 - Q2 * t1) * r).normalize();

	// bitangent
	B = ((Q2 * s1 - Q1 * s2) * r).normalize();

	// normal = tangent x bitangent
	N = crossVectors(T, B);
}

// taken from:
// http://www.iquilezles.org/www/articles/normals/normals.htm

void TriangleMesh::setSmooth() {
	int   *T;
	float *P0, *P1, *P2;
	float *NN;

	// Init new normals
	vector<int>(m_vIndices).swap(m_nIndices);
	vector<float>(m_vCoords.size(), 0.0f).swap(m_nCoords);
	// Accumulate face normals to compute vertex normals
	int triang_n = numTriangles();
	for(int t = 0; t < triang_n; ++t) {
		int i0 = m_vIndices[ t * 3];
		int i1 = m_vIndices[ t * 3 + 1];
		int i2 = m_vIndices[ t * 3 + 2];
		P0 = vCoords(i0);
		P1 = vCoords(i1);
		P2 = vCoords(i2);
		Vector3 N(unormalTriangle(P0, P1, P2));
		//T = TriangNidxTMesh(thisMesh, i);
		// i0
		m_nCoords[i0 * 3    ] += N[0];
		m_nCoords[i0 * 3 + 1] += N[1];
		m_nCoords[i0 * 3 + 2] += N[2];
		// i1
		m_nCoords[i1 * 3    ] += N[0];
		m_nCoords[i1 * 3 + 1] += N[1];
		m_nCoords[i1 * 3 + 2] += N[2];
		// i2
		m_nCoords[i2 * 3    ] += N[0];
		m_nCoords[i2 * 3 + 1] += N[1];
		m_nCoords[i2 * 3 + 2] += N[2];
	}
	// Normalize vertex normals
	renormalize();
}

void TriangleMesh::includeBBox(BBox & box) const {
	for (int i = 0, m = m_vCoords.size() / 3; i < m; ++i) {
		Vector3 V(&m_vCoords[ 3*i ]);
		box.add(V);
	}
}

void TriangleMesh::includeBBox(BBox * box) const {
	includeBBox(*box);
}

void TriangleMesh::applyTrfm(const Trfm3D * trfm) {
	int i;
	float *v;
	int points_n, normals_n;
	for (int i = 0, N = numVertices();
		 i < N; ++i) {
		float *P = vCoords(i);
		Vector3 aux = trfm->transformPoint(P);
		P[0] = aux[0];
		P[1] = aux[1];
		P[2] = aux[2];
	}
	for (int i = 0, N = numNormals();
		 i < N; ++i) {
		float *P = nCoords(i);
		Vector3 aux = trfm->transformPoint(P);
		P[0] = aux[0];
		P[1] = aux[1];
		P[2] = aux[2];
	}
	renormalize();
	m_vbo_uptodate = 0;
}

const Material *TriangleMesh::getMaterial(bool front) const {
	if (front) return m_materialFront;
	else return m_materialBack;
}

Material *TriangleMesh::getMaterial(bool front) {
	if (front) return m_materialFront;
	else return m_materialBack;
}

void TriangleMesh::setMaterial(Material *mat, bool front) {
	m_materialFront = mat;
	m_materialBack = mat;
}

void TriangleMesh::print() const {

	printf("Vertex, Normals, ... coordinates\n");

	printf("%zu V: ", numVertices());
	writeV(m_vCoords);
	printf("\n");
	printf("%zu N: ", numNormals());
	writeV(m_nCoords);
	printf("\n");
	printf("%zu Tex: ", numTexCoords());
	writeV(m_texCoords);
	printf("\n");
	printf("%zu Tgt: ", numTangents());
	writeV(m_tgtCoords);
	printf("\n");
	printf("%zu Btgt: ", numBitangents());
	writeV(m_btgtCoords);
	printf("\n");

	printf("Triangles\n");
	printf("(");
	for(size_t t = 0; t < m_vIndices.size() / 3; ++t) {
		for (int s = 0; s < 3; ++s) {
			printf(" %d", m_vIndices[3 * t + s]);
			if (m_nIndices.size()) printf(",%d", m_nIndices[3 * t + s]);
			if (m_texIndices.size()) printf(",%d", m_texIndices[3 * t + s]);
			if (m_tgtIndices.size()) printf(",%d", m_tgtIndices[3 * t + s]);
			if (m_btgtIndices.size()) printf(",%d", m_btgtIndices[3 * t + s]);
		}
	}
	printf(" )\n");
}
