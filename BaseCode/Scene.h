#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "VectorCamera.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	bool loadMesh(const char *filename);
	bool loadMesh(const char* filename, TriangleMesh* &mesh);
	void update(int deltaTime);
	void render();

  VectorCamera &getCamera();

private:
	void initShaders();
	void computeModelViewMatrix();
	
	void transformModel(glm::vec3 traslation);
	void transformModel(glm::vec3 traslation, glm::vec3 scale);
	void renderRoom();
	void renderModels();

private:
  VectorCamera camera;
	TriangleMesh *cube,
		*armadillo,
		*bunny,
		*dragon,
		*frog,
		*happy,
		*horse,
		*lucy,
		*maxplanck,
		*moai,
		*sphere,
		*tetrahedron,
		*torus,
		*new_mesh;
	ShaderProgram basicProgram;
	float currentTime;
	int simplification_mode;

};


#endif // _SCENE_INCLUDE

