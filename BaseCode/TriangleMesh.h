#ifndef _TRIANGLE_MESH_INCLUDE
#define _TRIANGLE_MESH_INCLUDE


#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "octree.h"


using namespace std;


// Class TriangleMesh contains the geometry of a mesh built out of triangles.
// Both the vertices and the triangles are stored in vectors.
// TriangleMesh also manages the ids of the copy in the GPU, so as to 
// be able to render it using OpenGL.


class TriangleMesh
{

public:
	TriangleMesh();
	~TriangleMesh();

	void addVertex(const glm::vec3 &position);
	void addTriangle(int v0, int v1, int v2);

	void initVertices(const vector<float> &newVertices);
	void initTriangles(const vector<int> &newTriangles);

	void buildCube();
	
	void computeTriangles(unordered_map<glm::vec3*, int>& vertices_representative, vector<int> &triangle_representatives);
	void generateLODs(ShaderProgram &program, int simplification_mode, const char* filename);
	void generateLOD(ShaderProgram& program, int lod);
	void sendToOpenGL(ShaderProgram &program);
	void sendToOpenGL(ShaderProgram& program, int lod);
	void sendToOpenGL(ShaderProgram& program, vector<glm::vec3*>& representatives, vector<int> triangle_representatives, int lod_number);
	void render() const;
	void render(int lod_number) const;
	void free();
	vector<glm::vec3> vertices;
	vector<int> triangles;

private: 

	GLuint vao_lod[4];
	GLuint vbo_lod[4];
	GLint posLocation, normalLocation;

	int triangle_size[4];

	Octree *tree;
	
};


#endif // _TRIANGLE_MESH_INCLUDE


