#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include "Application.h"


//Remove console (only works in Visual Studio)
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

// Our state
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#define TIME_PER_FRAME 1000.f / 60.f // Approx. 60 fps


static int prevTime;
static Application app; // This object represents our whole app


void my_display_code()
{
	ImGui::Begin("ImGui menu");                          // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	glm::vec3 cameraPos = Application::instance().getCameraPosition();
	ImGui::Text("Position: %.3f %.3f %.3f", cameraPos[0], cameraPos[1], cameraPos[2]);
	ImGui::End();
}

void glut_display_func()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	my_display_code();

	// Rendering
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	Application::instance().render();
	
	glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glutSwapBuffers();
	glutPostRedisplay();
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.



// If a key is pressed this callback is called

static void keyboardDownCallback(unsigned char key, int x, int y)
{
	Application::instance().keyPressed(key);
}

// If a key is released this callback is called

static void keyboardUpCallback(unsigned char key, int x, int y)
{
	Application::instance().keyReleased(key);
}

// If a special key is pressed this callback is called

static void specialDownCallback(int key, int x, int y)
{
	Application::instance().specialKeyPressed(key);
}

// If a special key is released this callback is called

static void specialUpCallback(int key, int x, int y)
{
	Application::instance().specialKeyReleased(key);
}

// Same for changes in mouse cursor position

static void motionCallback(int x, int y)
{
	Application::instance().mouseMove(x, y);
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)x, (float)y);
}

// Same for mouse button presses or releases

static void mouseCallback(int button, int state, int x, int y)
{
  int buttonId;
  
  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    buttonId = 0;
    break;
  case GLUT_RIGHT_BUTTON:
    buttonId = 1;
    break;
  case GLUT_MIDDLE_BUTTON:
    buttonId = 2;
    break;
  }

  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent((float)x, (float)y);

  if (state == GLUT_DOWN)
  {
	  Application::instance().mousePress(buttonId);
	  io.AddMouseButtonEvent(button, state == GLUT_DOWN);
  }
	else if(state == GLUT_UP)
		Application::instance().mouseRelease(buttonId);
		io.AddMouseButtonEvent(button, state == GLUT_DOWN);
}

// Resizing the window calls this function

static void resizeCallback(int width, int height)
{
  Application::instance().resize(width, height);
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)width, (float)height);
}

// Called whenever the windows needs to be redrawn
// This includes trying to render the scene at 60 FPS

static void drawCallback()
{
	Application::instance().render();
	glutSwapBuffers();
}

// When there is no other event to be processed GLUT calls this function
// Here we update the scene and order GLUT to redraw the window every 
// 60th of a second, so that we get 60 FPS

static void idleCallback()
{
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = currentTime - prevTime;
	
	if(deltaTime > TIME_PER_FRAME)
	{
		// Every time we enter here is equivalent to a game loop execution
		if(!Application::instance().update(deltaTime))
			exit(0);
		prevTime = currentTime;
		glutPostRedisplay();
	}
}

void motionCallback2(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)x, (float)y);
}


int main(int argc, char **argv)
{
	// GLUT initialization
	glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow(argv[0]);

	// Register all callbacks. GLUT will call the functions whenever 
	// the corresponding event is triggered
	glutReshapeFunc(resizeCallback);
	glutDisplayFunc(glut_display_func);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	// FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
	ImGui_ImplGLUT_Init();
	//ImGui_ImplGLUT_InstallFuncs();

	glutIdleFunc(idleCallback);
	glutKeyboardFunc(keyboardDownCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialDownCallback);
	glutSpecialUpFunc(specialUpCallback);
	glutMouseFunc(mouseCallback);
#ifdef __FREEGLUT_EXT_H__
	glutMouseWheelFunc(ImGui_ImplGLUT_MouseWheelFunc);
#endif
	glutMotionFunc(motionCallback2);
	glutPassiveMotionFunc(motionCallback);

	ImGui_ImplOpenGL2_Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// GLEW will take care of OpenGL extension functions
	glewExperimental = GL_TRUE;
	glewInit();

	// Application instance initialization
	Application::instance().init();
	if (argc > 1)
		Application::instance().loadMesh(argv[1]);
	prevTime = glutGet(GLUT_ELAPSED_TIME);

	// GLUT gains control of the application
	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	return 0;
}