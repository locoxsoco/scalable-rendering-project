#include <iostream>
#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Scene.h"
#include "PLYReader.h"


Scene::Scene()
{
	cube = NULL;
	armadillo = NULL;
	bunny = NULL;
	dragon = NULL;
	frog = NULL;
	happy = NULL;
	horse = NULL;
	lucy = NULL;
	maxplanck = NULL;
	moai = NULL;
	sphere = NULL;
	tetrahedron = NULL;
	torus = NULL;
	new_mesh = NULL;
}

Scene::~Scene()
{
	if(cube != NULL)
		delete cube;
	if(armadillo != NULL)
		delete armadillo;
	if (bunny != NULL)
		delete bunny;
	if (dragon != NULL)
		delete dragon;
	if (frog != NULL)
		delete frog;
	if (happy != NULL)
		delete happy;
	if (horse != NULL)
		delete horse;
	if (lucy != NULL)
		delete lucy;
	if (maxplanck != NULL)
		delete maxplanck;
	if (moai != NULL)
		delete moai;
	if (sphere != NULL)
		delete sphere;
	if (tetrahedron != NULL)
		delete tetrahedron;
	if (torus != NULL)
		delete torus;
	if (new_mesh != NULL)
		delete new_mesh;

}


// Initialize the scene. This includes the cube we will use to render
// the floor and walls, as well as the camera.

void Scene::init()
{
	// Simplification modes REPRESENTATIVE_MEAN, REPRESENTATIVE_QEM
	simplification_mode = REPRESENTATIVE_QEM;
	initShaders();
	cube = new TriangleMesh();
	cube->buildCube();
	cube->sendToOpenGL(basicProgram);
	////Scene::loadMesh("../../../../models/Armadillo.ply", armadillo); //heavy
	loadMesh("../../../../models/bunny.ply", bunny);
	////Scene::loadMesh("../../../../models/dragon.ply", dragon); //heavy
	loadMesh("../../../../models/frog.ply", frog);
	//loadMesh("../../../../models/happy.ply", happy); //heavy
	//loadMesh("../../../../models/horse.ply", horse);
	////Scene::loadMesh("../../../../models/lucy.ply", lucy); //super heavy!
	//Scene::loadMesh("../../../../models/maxplanck.ply", maxplanck);
	////Scene::loadMesh("../../../../models/moai.ply", moai); weird load
	////Scene::loadMesh("../../../../models/sphere.ply", sphere; weird load
	//loadMesh("../../../../models/tetrahedron.ply", tetrahedron);
	////Scene::loadMesh("../../../../models/torus.ply", torus); weird load
	currentTime = 0.0f;
	
	camera.init(glm::vec3(0.f, 0.5f, 2.f));
}

// Loads the mesh into CPU memory and sends it to GPU memory (using GL)
bool Scene::loadMesh(const char* filename)
{
	return loadMesh(filename, new_mesh);
}

bool Scene::loadMesh(const char *filename, TriangleMesh* &mesh)
{
#pragma warning( push )
#pragma warning( disable : 4101)
	PLYReader reader;
#pragma warning( pop ) 

	if(mesh != NULL)
	{
		mesh->free();
		delete mesh;
	}
	mesh = new TriangleMesh();
	bool bSuccess = reader.readMesh(filename, *mesh);
	if (bSuccess) {
		mesh->generateLODs(basicProgram,simplification_mode);
		//mesh->sendToOpenGL(basicProgram);
	}
	
	return bSuccess;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
}

// Render the scene. First the room, then the mesh it there is one loaded.
void Scene::transformModel(glm::vec3 traslation, glm::vec3 scale) {
	glm::mat3 normalMatrix;
	glm::mat4 modelview;

	modelview = camera.getModelViewMatrix();
	modelview = glm::translate(modelview, traslation);
	modelview = glm::scale(modelview, scale);
	basicProgram.setUniformMatrix4f("modelview", modelview);
	normalMatrix = glm::inverseTranspose(modelview);
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);

}

void Scene::transformModel(glm::vec3 traslation) {
	glm::mat3 normalMatrix;
	glm::mat4 modelview;

	modelview = camera.getModelViewMatrix();
	modelview = glm::translate(modelview, traslation);
	basicProgram.setUniformMatrix4f("modelview", modelview);
	normalMatrix = glm::inverseTranspose(modelview);
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);

}

void Scene::render()
{
	glm::mat3 normalMatrix;
	glm::mat4 modelview;
	glm::vec3 traslate;

	basicProgram.use();
	basicProgram.setUniformMatrix4f("projection", camera.getProjectionMatrix());
 	
 	renderRoom();
	renderModels();
 	
	if(new_mesh != NULL)
	{
 		basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);

		basicProgram.setUniformMatrix4f("modelview", camera.getModelViewMatrix());
		normalMatrix = glm::inverseTranspose(camera.getModelViewMatrix());
		basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
		new_mesh->render();

		traslate = glm::vec3(1.f, 0.f, 0.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(2.f, 0.f, 0.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(3.f, 0.f, 0.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(4.f, 0.f, 0.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(1.f, 0.f, 1.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(2.f, 0.f, 1.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(3.f, 0.f, 1.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(4.f, 0.f, 1.f);
		transformModel(traslate);
		new_mesh->render();

		traslate = glm::vec3(0.f, 0.f, 1.f);
		transformModel(traslate);
		new_mesh->render();
	}
}

VectorCamera &Scene::getCamera()
{
  return camera;
}

// Load, compile, and link the vertex and fragment shader

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/basic.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/basic.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

// Render the room. Both the floor and the walls are instances of the
// same initial cube scaled and translated to build the room.
void Scene::renderRoom()
{	
	glm::vec3 traslate;
	glm::vec3 scale;

 	basicProgram.setUniform4f("color", 0.5f, 0.5f, 0.55f, 1.0f);

	// Stomach Room

	traslate = glm::vec3(0.f, -0.5f, 0.f);
	scale = glm::vec3(13.f, 1.f, 13.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(8.f, -0.5f, -5.f);
	scale = glm::vec3(3.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(0.f, -0.5f, 10.f);
	scale = glm::vec3(3.f, 1.f, 7.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-5.f, -0.5f, -8.5f);
	scale = glm::vec3(3.f, 1.f, 4.f);
	transformModel(traslate, scale);
	cube->render();
	
	traslate = glm::vec3(1.f, 1.f, 10.f);
	scale = glm::vec3(1.f, 2.f, 9.f);
	transformModel(traslate, scale);
	cube->render();
	
	traslate = glm::vec3(3.5f, 1.f, 6.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();
	
	traslate = glm::vec3(6.f, 1.f, 1.f);
	scale = glm::vec3(1.f, 2.f, 11.f);
	transformModel(traslate, scale);
	cube->render();
	
	traslate = glm::vec3(8.f, 1.f, -4.f);
	scale = glm::vec3(5.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(3.f, 1.f, -6.f);
	scale = glm::vec3(15.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-4.f, 1.f, -8.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-6.f, 1.f, -5.f);
	scale = glm::vec3(1.f, 2.f, 13.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-6.f, 1.f, 4.5f);
	scale = glm::vec3(1.f, 2.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-3.5f, 1.f, 6.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-1.f, 1.f, 10.f);
	scale = glm::vec3(1.f, 2.f, 9.f);
	transformModel(traslate, scale);
	cube->render();

	// Right Leg Room

	traslate = glm::vec3(-5.f, -0.5f, -13.5f);
	scale = glm::vec3(7.f, 1.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-3.f, 1.f, -11.f);
	scale = glm::vec3(3.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-2.f, 1.f, -13.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-3.f, 1.f, -16.f);
	scale = glm::vec3(3.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-7.f, 1.f, -16.f);
	scale = glm::vec3(3.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-8.f, 1.f, -13.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-7.f, 1.f, -11.f);
	scale = glm::vec3(3.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Right Foot Room

	traslate = glm::vec3(-5.f, -0.5f, -18.f);
	scale = glm::vec3(3.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-7.f, -0.5f, -22.5f);
	scale = glm::vec3(7.f, 1.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-4.f, 1.f, -20.5f);
	scale = glm::vec3(1.f, 2.f, 10.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-7.f, 1.f, -25.f);
	scale = glm::vec3(7.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-10.f, 1.f, -22.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-8.f, 1.f, -20.f);
	scale = glm::vec3(5.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-6.f, 1.f, -18.f);
	scale = glm::vec3(1.f, 2.f, 5.f);
	transformModel(traslate, scale);
	cube->render();

	// Left Leg Room

	traslate = glm::vec3(12.5f, -0.5f, -5.f);
	scale = glm::vec3(6.f, 1.f, 7.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(15.f, 1.f, -3.f);
	scale = glm::vec3(1.f, 2.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(15.f, 1.f, -7.f);
	scale = glm::vec3(1.f, 2.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(12.5f, 1.f, -8.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(10.f, 1.f, -7.f);
	scale = glm::vec3(1.f, 2.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(10.f, 1.f, -3.f);
	scale = glm::vec3(1.f, 2.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(12.5f, 1.f, -2.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Left Foot Room

	traslate = glm::vec3(17.5f, -0.5f, -5.f);
	scale = glm::vec3(4.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(23.f, -0.5f, -1.5f);
	scale = glm::vec3(7.f, 1.f, 10.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(26.f, 1.f, -1.5f);
	scale = glm::vec3(1.f, 2.f, 10.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(20.5f, 1.f, -6.f);
	scale = glm::vec3(12.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(17.5f, 1.f, -4.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(20.f, 1.f, -0.5f);
	scale = glm::vec3(1.f, 2.f, 8.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(23.f, 1.f, 3.f);
	scale = glm::vec3(7.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Right Arm Room

	traslate = glm::vec3(0.f, -0.5f, 15.f);
	scale = glm::vec3(15.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-10.5f, -0.5f, 15.f);
	scale = glm::vec3(6.f, 1.f, 9.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-8.f, 1.f, 17.5f);
	scale = glm::vec3(1.f, 2.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-4.5f, 1.f, 16.f);
	scale = glm::vec3(8.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-4.5f, 1.f, 14.f);
	scale = glm::vec3(8.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-8.f, 1.f, 12.5f);
	scale = glm::vec3(1.f, 2.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-10.5f, 1.f, 11.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-13.f, 1.f, 12.5f);
	scale = glm::vec3(1.f, 2.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-13.f, 1.f, 17.5f);
	scale = glm::vec3(1.f, 2.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-10.5f, 1.f, 19.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Right Hand Room

	traslate = glm::vec3(-10.f, -0.5f, 2.f);
	scale = glm::vec3(7.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-18.f, -0.5f, 4.f);
	scale = glm::vec3(9.f, 1.f, 7.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-18.f, -0.5f, 10.5f);
	scale = glm::vec3(3.f, 1.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-16.5f, -0.5f, 15.f);
	scale = glm::vec3(6.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-15.f, 1.f, 14.f);
	scale = glm::vec3(5.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-17.f, 1.f, 10.5f);
	scale = glm::vec3(1.f, 2.f, 8.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-15.5f, 1.f, 7.f);
	scale = glm::vec3(4.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-14.f, 1.f, 5.f);
	scale = glm::vec3(1.f, 2.f, 5.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-10.f, 1.f, 3.f);
	scale = glm::vec3(9.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-14.f, 1.f, 1.f);
	scale = glm::vec3(17.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-22.f, 1.f, 4.f);
	scale = glm::vec3(1.f, 2.f, 7.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-20.5f, 1.f, 7.f);
	scale = glm::vec3(4.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-19.f, 1.f, 11.5f);
	scale = glm::vec3(1.f, 2.f, 10.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-16.f, 1.f, 16.f);
	scale = glm::vec3(7.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Left Arm Room

	traslate = glm::vec3(10.5f, -0.5f, 15.f);
	scale = glm::vec3(6.f, 1.f, 5.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(8.f, 1.f, 16.5f);
	scale = glm::vec3(1.f, 2.f, 2.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(10.5f, 1.f, 17.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(13.f, 1.f, 16.5f);
	scale = glm::vec3(1.f, 2.f, 2.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(13.f, 1.f, 13.5f);
	scale = glm::vec3(1.f, 2.f, 2.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(10.5f, 1.f, 13.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(8.f, 1.f, 13.5f);
	scale = glm::vec3(1.f, 2.f, 2.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(4.5f, 1.f, 14.f);
	scale = glm::vec3(8.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(4.5f, 1.f, 16.f);
	scale = glm::vec3(8.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Left Hand Room

	traslate = glm::vec3(17.f, -0.5f, 15.f);
	scale = glm::vec3(7.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(19.f, -0.5f, 18.5f);
	scale = glm::vec3(3.f, 1.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(19.f, -0.5f, 24.f);
	scale = glm::vec3(9.f, 1.f, 7.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(9.5f, -0.5f, 26.f);
	scale = glm::vec3(10.f, 1.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(23.f, 1.f, 24.f);
	scale = glm::vec3(1.f, 2.f, 7.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(21.5f, 1.f, 21.f);
	scale = glm::vec3(4.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(20.f, 1.f, 17.5f);
	scale = glm::vec3(1.f, 2.f, 8.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(16.5f, 1.f, 14.f);
	scale = glm::vec3(8.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(15.5f, 1.f, 16.f);
	scale = glm::vec3(6.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(18.f, 1.f, 18.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(16.5f, 1.f, 21.f);
	scale = glm::vec3(4.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(15.f, 1.f, 23.f);
	scale = glm::vec3(1.f, 2.f, 5.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(9.5f, 1.f, 25.f);
	scale = glm::vec3(12.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(13.5f, 1.f, 27.f);
	scale = glm::vec3(20.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	// Head Room

	traslate = glm::vec3(0.f, -0.5f, 18.5f);
	scale = glm::vec3(3.f, 1.f, 4.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(0.f, -0.5f, 25.f);
	scale = glm::vec3(9.f, 1.f, 9.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(4.f, 1.f, 28.f);
	scale = glm::vec3(1.f, 2.f, 3.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(4.f, 1.f, 23.f);
	scale = glm::vec3(1.f, 2.f, 5.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(2.5f, 1.f, 21.f);
	scale = glm::vec3(4.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(1.f, 1.f, 18.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-1.f, 1.f, 18.5f);
	scale = glm::vec3(1.f, 2.f, 6.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-2.5f, 1.f, 21.f);
	scale = glm::vec3(4.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(-4.f, 1.f, 25.f);
	scale = glm::vec3(1.f, 2.f, 9.f);
	transformModel(traslate, scale);
	cube->render();

	traslate = glm::vec3(0.f, 1.f, 29.f);
	scale = glm::vec3(9.f, 2.f, 1.f);
	transformModel(traslate, scale);
	cube->render();
}

void Scene::renderModels()
{
	glm::vec3 traslate;

	basicProgram.setUniform4f("color", 0.5f, 0.5f, 0.55f, 1.0f);

	// Stomach Room

	// Right Leg Room

	traslate = glm::vec3(-3.5f, 0.f, -12.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-3.5f, 0.f, -13.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-3.5f, 0.f, -14.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-3.5f, 0.f, -15.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-6.5f, 0.f, -12.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-6.5f, 0.f, -13.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-6.5f, 0.f, -14.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-6.5f, 0.f, -15.f);
	transformModel(traslate);
	bunny->render();

	// Right Foot Room

	traslate = glm::vec3(-8.f, 0.f, -21.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-8.f, 0.f, -24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, -21.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, -22.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, -23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, -24.f);
	transformModel(traslate);
	bunny->render();

	// Left Leg Room

	traslate = glm::vec3(12.f, 0.f, -4.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(13.f, 0.f, -4.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(12.f, 0.f, -6.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(13.f, 0.f, -6.f);
	transformModel(traslate);
	bunny->render();

	// Left Foot Room

	traslate = glm::vec3(21.f, 0.f, -1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(21.f, 0.f, 0.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(21.f, 0.f, 1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, -1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 0.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(24.f, 0.f, -1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(24.f, 0.f, 0.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(24.f, 0.f, 1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(25.f, 0.f, -1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(25.f, 0.f, 0.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(25.f, 0.f, 1.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(23.f, 0.f, 2.f);
	transformModel(traslate);
	bunny->render();

	// Right Arm Room

	traslate = glm::vec3(-9.f, 0.f, 12.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-10.f, 0.f, 12.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-11.f, 0.f, 12.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-12.f, 0.f, 12.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, 13.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-10.f, 0.f, 13.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-11.f, 0.f, 13.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-12.f, 0.f, 13.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, 17.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-10.f, 0.f, 17.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-11.f, 0.f, 17.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-12.f, 0.f, 17.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-9.f, 0.f, 18.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-10.f, 0.f, 18.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-11.f, 0.f, 18.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-12.f, 0.f, 18.f);
	transformModel(traslate);
	bunny->render();

	// Right Hand Room

	traslate = glm::vec3(-17.f, 0.f, 2.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-18.f, 0.f, 2.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-19.f, 0.f, 2.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-20.f, 0.f, 2.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-21.f, 0.f, 2.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-18.f, 0.f, 3.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-19.f, 0.f, 3.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-20.f, 0.f, 3.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-21.f, 0.f, 3.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-19.f, 0.f, 4.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-20.f, 0.f, 4.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-21.f, 0.f, 4.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-20.f, 0.f, 5.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-21.f, 0.f, 5.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-21.f, 0.f, 6.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-15.f, 0.f, 5.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-15.f, 0.f, 6.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-16.f, 0.f, 6.f);
	transformModel(traslate);
	bunny->render();

	// Left Arm Room

	traslate = glm::vec3(9.f, 0.f, 14.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(10.f, 0.f, 14.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(11.f, 0.f, 14.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(12.f, 0.f, 14.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(9.f, 0.f, 16.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(10.f, 0.f, 16.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(11.f, 0.f, 16.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(12.f, 0.f, 16.f);
	transformModel(traslate);
	bunny->render();
	
	// Left Hand Room

	traslate = glm::vec3(16.f, 0.f, 22.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(17.f, 0.f, 22.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(18.f, 0.f, 22.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(16.f, 0.f, 23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(17.f, 0.f, 23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(16.f, 0.f, 24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 22.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(21.f, 0.f, 23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(21.f, 0.f, 24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(20.f, 0.f, 24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 25.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(21.f, 0.f, 25.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(20.f, 0.f, 25.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(19.f, 0.f, 25.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(22.f, 0.f, 26.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(21.f, 0.f, 26.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(20.f, 0.f, 26.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(19.f, 0.f, 26.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(18.f, 0.f, 26.f);
	transformModel(traslate);
	bunny->render();

	// Head Room

	traslate = glm::vec3(2.f, 0.f, 23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(2.f, 0.f, 24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(2.f, 0.f, 25.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-2.f, 0.f, 23.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-2.f, 0.f, 24.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(-2.f, 0.f, 25.f);
	transformModel(traslate);
	bunny->render();

	traslate = glm::vec3(0.f, 0.f, 28.f);
	transformModel(traslate);
	bunny->render();
}