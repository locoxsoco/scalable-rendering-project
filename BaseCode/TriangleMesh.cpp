#include <iostream>
#include <vector>
#include <unordered_map>
#include "TriangleMesh.h"

#define GLM_ENABLE_EXPERIMENTAL //hashable glm vec
#include <glm/gtx/hash.hpp>
#include <Eigen/Dense>

using namespace std;


TriangleMesh::TriangleMesh()
{
	vao_lod0 = -1;
	vbo_lod0 = -1;
}

TriangleMesh::~TriangleMesh()
{
	free();
}


void TriangleMesh::addVertex(const glm::vec3 &position)
{
  vertices.push_back(position);
}

void TriangleMesh::addTriangle(int v0, int v1, int v2)
{
  triangles.push_back(v0);
  triangles.push_back(v1);
  triangles.push_back(v2);
}

void TriangleMesh::initVertices(const vector<float> &newVertices)
{
	vertices.resize(newVertices.size() / 3);
	for(unsigned int i=0; i<vertices.size(); i++)
		vertices[i] = glm::vec3(newVertices[3*i], newVertices[3*i+1], newVertices[3*i+2]);
}

void TriangleMesh::initTriangles(const vector<int> &newTriangles)
{
	triangles = newTriangles;
}

void TriangleMesh::buildCube()
{
	float vertices[] = {-1, -1, -1,
                      1, -1, -1,
                      1,  1, -1,
                      -1,  1, -1,
                      -1, -1,  1,
                      1, -1,  1,
                      1,  1,  1,
                      -1,  1,  1
								};

	int faces[] = {3, 1, 0, 3, 2, 1,
                 5, 6, 7, 4, 5, 7,
                 7, 3, 0, 0, 4, 7,
                 1, 2, 6, 6, 5, 1,
                 0, 1, 4, 5, 4, 1,
                 2, 3, 7, 7, 6, 2
						  };

	int i;

	for(i=0; i<8; i+=1)
		addVertex(0.5f * glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	for(i=0; i<12; i++)
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
	triangle_size = triangles.size();
}

void TriangleMesh::sendToOpenGL(ShaderProgram &program)
{
	vector<float> data;
	
	for(unsigned int tri=0; tri<triangles.size(); tri+=3)
	{
		glm::vec3 normal;

		normal = glm::cross(vertices[triangles[tri+1]] - vertices[triangles[tri]], 
							vertices[triangles[tri+2]] - vertices[triangles[tri]]);
		normal = glm::normalize(normal);
		for(unsigned int vrtx=0; vrtx<3; vrtx++)
		{
			data.push_back(vertices[triangles[tri + vrtx]].x);
			data.push_back(vertices[triangles[tri + vrtx]].y);
			data.push_back(vertices[triangles[tri + vrtx]].z);

			data.push_back(normal.x);
			data.push_back(normal.y);
			data.push_back(normal.z);
		}
	}

  // Send data to OpenGL
	glGenVertexArrays(1, &vao_lod0);
	glBindVertexArray(vao_lod0);
	glGenBuffers(1, &vbo_lod0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lod0);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 3, 6 * sizeof(float), 0);
	normalLocation = program.bindVertexAttribute("normal", 3, 6*sizeof(float), (void *)(3*sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void TriangleMesh::sendToOpenGL(ShaderProgram& program, vector<glm::vec3*>& representatives, vector<int> triangle_representatives)
{
	vector<float> data;

	for (unsigned int tri = 0; tri < triangle_representatives.size(); tri += 3)
	{
		glm::vec3 normal;

		normal = glm::cross(*representatives[triangle_representatives[tri + 1]] - *representatives[triangle_representatives[tri]],
			*representatives[triangle_representatives[tri + 2]] - *representatives[triangle_representatives[tri]]);
		normal = glm::normalize(normal);
		for (unsigned int vrtx = 0; vrtx < 3; vrtx++)
		{
			data.push_back((*representatives[triangle_representatives[tri + vrtx]]).x);
			data.push_back((*representatives[triangle_representatives[tri + vrtx]]).y);
			data.push_back((*representatives[triangle_representatives[tri + vrtx]]).z);

			data.push_back(normal.x);
			data.push_back(normal.y);
			data.push_back(normal.z);
		}
	}

	// Send data to OpenGL
	glGenVertexArrays(1, &vao_lod0);
	glBindVertexArray(vao_lod0);
	glGenBuffers(1, &vbo_lod0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lod0);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 3, 6 * sizeof(float), 0);
	normalLocation = program.bindVertexAttribute("normal", 3, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void TriangleMesh::render() const
{
	glBindVertexArray(vao_lod0);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(normalLocation);
	glDrawArrays(GL_TRIANGLES, 0, triangle_size);
	glBindVertexArray(0);
}

void TriangleMesh::computeTriangles(unordered_map<glm::vec3*, int>& vertices_representative, vector<int> &triangle_representatives) {
	for (unsigned int tri = 0; tri < triangles.size(); tri+=3) {
		int v0_representative_location = vertices_representative.at(&vertices[triangles[tri]]);
		int v1_representative_location = vertices_representative.at(&vertices[triangles[tri+1]]);
		int v2_representative_location = vertices_representative.at(&vertices[triangles[tri+2]]);
		if (v0_representative_location != v1_representative_location &&
			v0_representative_location != v2_representative_location &&
			v1_representative_location != v2_representative_location) {
			triangle_representatives.push_back(v0_representative_location);
			triangle_representatives.push_back(v1_representative_location);
			triangle_representatives.push_back(v2_representative_location);
		}
	}
	return;
}

void TriangleMesh::generateLODs(ShaderProgram& program, int simplification_mode)
{
	// Build Octree
	int depth_level = 10;
	tree =  new Octree(glm::vec3(-0.5, 0.f, -0.5), glm::vec3(0.5, 1.f, 0.5), depth_level);
	for (int i = 0; i < vertices.size(); i++) {
		tree->insert(vertices[i]);
	}
	// Compute representative vertex per cluster
	int lod_number = 5;
	
	vector<glm::vec3*> representatives;
	unordered_map<glm::vec3*, int> vertices_representative;

	vector<glm::vec3*> quadrics;
	unordered_map<glm::vec3*, Eigen::Matrix4f*> vertices_quadric;

	if (simplification_mode == REPRESENTATIVE_MEAN) {
		tree->computeRepresentatives(representatives, vertices_representative, lod_number);
		vector<int> triangle_representatives;
		computeTriangles(vertices_representative, triangle_representatives);
		triangle_size = triangle_representatives.size();
		sendToOpenGL(program, representatives, triangle_representatives);
	}
	else if (simplification_mode == REPRESENTATIVE_QEM) {
		for (unsigned int tri = 0; tri < triangles.size(); tri+=3) {
			glm::vec3 normal;
			normal = glm::cross(vertices[triangles[tri + 1]] - vertices[triangles[tri]],
				vertices[triangles[tri + 2]] - vertices[triangles[tri]]);
			normal = glm::normalize(normal);

			Eigen::Matrix4f quadric;
			float d = -normal.x * vertices[triangles[tri]].x - normal.y * vertices[triangles[tri]].y - normal.z * vertices[triangles[tri]].z;
			float a2 = normal.x * normal.x;
			float ab = normal.x * normal.y;
			float ac = normal.x * normal.z;
			float ad = normal.x * d;
			float b2 = normal.y * normal.y;
			float bc = normal.y * normal.z;
			float bd = normal.y * d;
			float c2 = normal.z * normal.z;
			float cd = normal.z * d;
			float d2 = d * d;
			quadric <<
				a2, ab, ac, ad,
				ab, b2, bc, bd,
				ac, bc, c2, cd,
				ad, bd, cd, d2;

			for (unsigned int vrtx = 0; vrtx < 3; vrtx++)
			{
				if (vertices_quadric.find(&vertices[triangles[tri + vrtx]]) != vertices_quadric.end()) {
					*vertices_quadric[&vertices[triangles[tri + vrtx]]] = *vertices_quadric[&vertices[triangles[tri + vrtx]]] + quadric;
				}
				else {
					Eigen::Matrix4f* new_quadric = new Eigen::Matrix4f();
					*new_quadric = quadric;
					vertices_quadric[&vertices[triangles[tri + vrtx]]] = new_quadric;
				}
			}			
		}

	}
	return;
	
}

void TriangleMesh::free()
{
	if (vbo_lod0 != -1) {
		glDeleteBuffers(1, &vbo_lod0);
		glDeleteBuffers(1, &vbo_lod1);
		glDeleteBuffers(1, &vbo_lod2);
		glDeleteBuffers(1, &vbo_lod3);
	}
	if (vao_lod0 != -1) {
		glDeleteVertexArrays(1, &vao_lod0);
		glDeleteVertexArrays(1, &vao_lod1);
		glDeleteVertexArrays(1, &vao_lod2);
		glDeleteVertexArrays(1, &vao_lod3);
	}
	
	vertices.clear();
	triangles.clear();
}



