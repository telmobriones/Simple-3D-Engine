#include "mg.h"
#include <cmath>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include "scenes.h"
#include "skybox.h"

// global variables
static float step = 0.5;										 // advance/retreat step
const static float angle_step = 1.0f * Constants::degree_to_rad; // angular step (rotations, etc)
static Node *displayNode;										 // the selected node
static int check_cull = 0;
float offset = 0;
// Animation settings
// The time in milliseconds between timer ticks
static int MG_TIMERMSECS = 33;
static bool runAnimation = false;
// Global variables for measuring time (in milli-seconds)
static int startTime;
static int prevTime;
// Global variables mouse
static int mouse_pre_x = -1;
static int mouse_pre_y = -1;

static void switchAllLights(bool onOff)
{
	for (auto it = LightManager::instance()->begin(), end = LightManager::instance()->end();
		 it != end; ++it)
		it->switchLight(onOff);
}

// Toggle switch of light number 'ln'
static void toggleLightSwitch(int ln)
{
	assert(ln > 0);
	for (auto it = LightManager::instance()->begin(), end = LightManager::instance()->end();
		 ln && it != end; ++it)
	{
		--ln;
		if (!ln)
		{
			printf("[I] switch light \'%s\' %s\n", it->getName(), !it->isOn() ? "ON" : "OFF");
			it->switchLight(!it->isOn());
		}
	}
}

// Init OpenGL rendering context (including rendering window)

static void InitRenderContext(int argc, char **argv,
							  int width, int height,
							  int wposx, int wposy)
{

	GLenum glew_err;

	// Init openGL and create a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(wposx, wposy);
	// create one window for OpenGL graphics
	glutCreateWindow("browser");

	// Uncomment following line if you have problems initiating GLEW
	//
	// glewExperimental = GL_TRUE;

	glew_err = glewInit();

	if (glew_err != GLEW_OK)
	{
		fprintf(stderr, "Error when calling glewInit: %s\n", glewGetString(glew_err));
		exit(1);
	}

	//////////////////////////
	// Set OpenGL state values

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Clear color
	glViewport(0, 0, width, height);	  // Viewport Transformation

	// Enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Cull bakface triangles
	glFrontFace(GL_CCW); // use counter-clockwise to decide whether triangle is backface

	// Turn Depth Testing On
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);	  // Use z-buffer for writing
	glDepthFunc(GL_LEQUAL);	  // Write if depth is less or equal value stored in z-buffer
	glDepthRange(0.0f, 1.0f); // Sets range for depth (fragment shader gl_DepthRange variable)

	// Uncomment for automatic gamma correction (but beware for textures!)
	// glEnable(GL_FRAMEBUFFER_SRGB);

	// Aliasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	// Wireframe mode by default (use key 's' to change)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

static void InitShaders()
{
	ShaderManager *mgr = ShaderManager::instance();
	mgr->create("dummy", "Shaders/dummy.vert", "Shaders/dummy.frag");
	mgr->create("pervertex", "Shaders/pervertex.vert", "Shaders/pervertex.frag");
}

static void check_cull_camera()
{
	CameraManager *mgr = CameraManager::instance();
	Camera *theCamera = mgr->find("mainCamera");
	if (!theCamera)
		return; // no main camera
	Camera *mapC = mgr->find("mapC");
	if (!mapC)
	{
		OrthographicCamera *newcam = mgr->createOrthographic("mapC");
		float a = 96.0;
		newcam->init(-a, a, -a, a, 0.1, 1500.0);
		mapC = newcam;
	}
	/* if (!mapC) { */
	/*	PerspectiveCamera *newcam = mgr->createPerspective("mapC"); */
	/*     float a = 128.0; */
	/*	newcam->init(30.0f * Constants::degree_to_rad, 900.0 / 700.0, 0.1f, 1500.0f); */
	/*	//InitConicCamera(mapC, fovy * DEGREE_TO_RAD, 900.0 / 700.0, 0.1f, 1500.0f); */
	/*	mapC = newcam; */
	/* } */

	Vector3 E = theCamera->getPosition();
	Vector3 D = theCamera->getDirection();
	mapC->lookAt(Vector3(E[0], 100.0f, E[2] - 45.0f),
				 Vector3(E[0], 0.0f, E[2] - 45.0f),
				 Vector3(D[0], 0.0f, D[2]));
	RenderState *rs = RenderState::instance();
	rs->loadTrfm(RenderState::projection, mapC->projectionTrfm());
	rs->loadIdentity(RenderState::modelview);
	rs->addTrfm(RenderState::modelview, mapC->viewTrfm());
}

static void Resize(int Width, int Height)
{
	Camera *theCamera = CameraManager::instance()->find("mainCamera");
	if (!theCamera)
		return;		 // no main camera
	if (Height == 0) // Prevent A Divide By Zero If The Window Is Too Small
		Height = 1;
	theCamera->onResize(Width, Height);
	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height); // TODO should go to context
}

static void Render(Camera *theCamera)
{

	RenderState *rs = RenderState::instance();
	LightManager *lmgr = LightManager::instance();

	rs->setCamera(theCamera);
	// draw the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rs->loadTrfm(RenderState::projection, theCamera->projectionTrfm());
	rs->loadTrfm(RenderState::modelview, theCamera->viewTrfm());
	if (check_cull)
		check_cull_camera();
	DisplaySky(theCamera);

	// place lights into scene
	for (auto it = lmgr->begin(), end = lmgr->end();
		 it != end; ++it)
	{
		if (!it->isOn())
			continue;
		it->placeScene();
	}

	Scene::instance()->draw();
}

static void Display()
{

	Camera *theCamera;
	theCamera = CameraManager::instance()->find("mainCamera");
	if (!theCamera)
		return; // no main camera

	Scene::instance()->rootNode()->frustumCull(theCamera); // Frustum Culling

	Render(theCamera);
	glutSwapBuffers();
}

void snapshot()
{
	Texture *snapRT = TextureManager::instance()->find("MG_SNAP");
	if (!snapRT)
	{
		snapRT = TextureManager::instance()->createRTColor("MG_SNAP",
														   900, 700);
	}
	Camera *theCamera = CameraManager::instance()->find("mainCamera");
	if (!theCamera)
		return;											   // no main camera
	Scene::instance()->rootNode()->frustumCull(theCamera); // Frustum Culling
	snapRT->bindGL();
	Render(theCamera);
	int res = snapRT->saveJPG("snapshot.jpg");
	snapRT->unbindGL();
	if (res != 1)
	{
		fprintf(stderr, "Error saving snapshot: %d\nExiting...\n", res);
		exit(-1);
	}
	fprintf(stderr, "[I] Snapshot writen in snapshot.jpg\n");
}

// Keyboard dispatcher when ALT key is pressed
static void Keyboard_alt(unsigned char key)
{

	static bool line_aliasing = true;
	static bool drawBB = false;
	static bool aux;
	RenderState *rs;
	Texture *tex;
	switch (key)
	{
	case 'a':
		// TODO: render context
		line_aliasing = !line_aliasing;
		if (line_aliasing)
		{
			glEnable(GL_LINE_SMOOTH);
		}
		else
		{
			glDisable(GL_LINE_SMOOTH);
		}
		break;
	case 'm':
		MaterialManager::instance()->print();
		break;
	case 't':
		TextureManager::instance()->print();
		break;
	case 'c':
		CameraManager::instance()->print();
		break;
	case 'C':
		aux = !Scene::instance()->rootNode()->getCheckCollision();
		Scene::instance()->rootNode()->setCheckCollision(aux);
		fprintf(stderr, "[I] Check collision %s\n", aux ? "ON" : "OFF");
		break;
	case 'l':
		LightManager::instance()->print();
		break;
	case 'i':
		ImageManager::instance()->print();
		break;
	case 'v':
		RenderState::instance()->top(RenderState::modelview).print();
		break;
	case 'p':
		RenderState::instance()->top(RenderState::projection).print();
		break;
	case 's':
		RenderState::instance()->print();
		Scene::instance()->print_scene_trfm();
		break;
	case 'S':
		ShaderManager::instance()->print();
		break;
	case 'z':
		glEnable(GL_DEPTH_TEST);
		break;
	case 'Z':
		glDisable(GL_DEPTH_TEST);
		break;
	case 'b':
		drawBB = !drawBB;
		RenderState::instance()->drawBBoxes(drawBB);
		break;
	case 'f':
		check_cull = 1 - check_cull;
		break;
	case '0':
		snapshot();
		break;
	case '1':
		displayNode = displayNode->parent();
		break;
	case '2':
		displayNode = displayNode->firstChild();
		break;
	case '3':
		displayNode = displayNode->nextSibling();
		break;
	case '4':
		tex = TextureManager::instance()->find("./obj/cubes/brick.jpg");
		if (tex)
		{
			tex->cycleMagFilter();
		}
		break;
	case '5':
		tex = TextureManager::instance()->find("./obj/cubes/brick.jpg");
		if (tex)
		{
			tex->cycleMinFilter();
		}
		break;
	}
	glutPostRedisplay();
}

// General keyboard dispatcher
static void Keyboard(unsigned char key, int x, int y)
{

	static size_t i = 0;
	int key_mod;
	Avatar *theAvatar;
	Camera *theCamera;
	float fovy;

	key_mod = glutGetModifiers();
	if (key_mod & GLUT_ACTIVE_ALT)
	{
		// If ALT key pressed, call Keyboard_alt and exit
		Keyboard_alt(key);
		return;
	}

	switch (key)
	{
	case '0':
		runAnimation = !runAnimation;
		fprintf(stderr, "[I] Run animation %s\n", runAnimation ? "true" : "false");
		break;
	case '1':
		toggleLightSwitch(1);
		break;
	case '2':
		toggleLightSwitch(2);
		break;
	case '3':
		toggleLightSwitch(3);
		break;
	case '4':
		toggleLightSwitch(4);
		break;
	case '5':
		toggleLightSwitch(5);
		break;
	case '6':
		toggleLightSwitch(6);
		break;
	case '7':
		toggleLightSwitch(7);
		break;
	case 's':
		// Enable Shading
		// TODO: use context
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'S':
		// Disable Shading
		// TODO: use context
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'l':
		// Turn on all lights
		switchAllLights(true);
		break;
	case 'L':
		// Disable Lighting
		switchAllLights(false);
		break;
	case 'z':
		// TODO: context
		glEnable(GL_CULL_FACE);
		break;
	case 'Z':
		// TODO: context
		glDisable(GL_CULL_FACE);
		break;

	case '.':
		theAvatar = AvatarManager::instance()->find("avatar");
		if (theAvatar)
		{
			theAvatar->walkOrFly(!theAvatar->getWalkorFly());
			fprintf(stderr, "[I] Avatar in %s mode.\n",
					theAvatar->getWalkorFly() ? "walk" : "fly");
		}
		break;
	case 'a':
		displayNode->rotateY(-angle_step);
		break;
	case 'd':
		displayNode->rotateY(angle_step);
		break;
	case 'w':
		displayNode->rotateX(-angle_step);
		break;
	case 'q':
		displayNode->rotateZ(-angle_step);
		break;
	case 'Q':
		displayNode->rotateZ(angle_step);
		break;
	case 'x':
		displayNode->rotateX(angle_step);
		break;
	case 'i':
		displayNode->translate(Vector3(2.0 * step, 0.0, 0.0));
		break;
	case 'I':
		displayNode->translate(Vector3(2.0 * -step, 0.0, 0.0));
		break;
	case 'p':
		displayNode->translate(Vector3(0.0, 0.0, 2.0 * step));
		break;
	case 'P':
		displayNode->translate(Vector3(0.0, 0.0, 2.0 * -step));
		break;
	case 'o':
		displayNode->translate(Vector3(0.0, 2.0 * step, 0.0));
		break;
	case 'O':
		displayNode->translate(Vector3(0.0, 2.0 * -step, 0.0));
		break;
	case 'f':
		theCamera = CameraManager::instance()->find("mainCamera");
		if (theCamera)
		{
			theCamera->zoom(std::max(0.01f, theCamera->getZoom() - 0.01f));
		}
		break;
	case 'F':
		theCamera = CameraManager::instance()->find("mainCamera");
		if (theCamera)
		{
			theCamera->zoom(std::min(2.0f * Constants::pi, theCamera->getZoom() + 0.01f));
		}
		break;
	case 27: // ESC
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

// Special keyboard dispatcher (arrow keys, etc).
static void SpecialKey(int key, int x, int y)
{

	Camera *theCamera;

	Avatar *theAvatar = AvatarManager::instance()->find("avatar");
	if (!theAvatar)
		return;

	// Se if shift key is pressed
	bool shift_key = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

	switch (key)
	{
	case GLUT_KEY_RIGHT:
		if (shift_key)
			theAvatar->panX(1);
		else
			theAvatar->leftRight(-0.05f);
		break;
	case GLUT_KEY_LEFT:
		if (shift_key)
			theAvatar->panX(-1);
		else
			theAvatar->leftRight(0.05f);
		break;
	case GLUT_KEY_UP:
		if (shift_key)
			theAvatar->panY(1);
		else
			theAvatar->upDown(0.05f);
		break;
	case GLUT_KEY_DOWN:
		if (shift_key)
			theAvatar->panY(-1);
		else
			theAvatar->upDown(-0.05f);
		break;
	case GLUT_KEY_PAGE_UP:
		theAvatar->advance(1);
		break;
	case GLUT_KEY_PAGE_DOWN:
		theAvatar->advance(-1);
		break;
	case GLUT_KEY_HOME:
		theCamera = CameraManager::instance()->find("mainCamera");
		if (theCamera)
			theCamera->lookAt(Vector3(0.0f, 0.0f, 0.0f),	  // position
							  Vector3(0.0f, -10.0f, -100.0f), // look-at
							  Vector3(0.0f, 1.0f, 0.0f));	  // up vector
		break;
	}
	glutPostRedisplay();
}

void mouseClick(int button, int state,
				int x, int y)
{
	if (button != GLUT_LEFT_BUTTON)
		return;
	if (state == GLUT_UP)
	{
		mouse_pre_x = -1;
		mouse_pre_y = -1;
	}
}

void mouse(int x, int y)
{
	// lower left if (0,0)
	Camera *theCamera = CameraManager::instance()->find("mainCamera");
	float s;
	if (mouse_pre_x >= 0 && (x != mouse_pre_x))
	{
		s = (x - mouse_pre_x) > 0.0 ? 1.0 : -1.0;
		theCamera->arcLeftRight(s * 0.03f);
		glutPostRedisplay();
	}
	if (mouse_pre_y >= 0 && (y != mouse_pre_y))
	{
		s = (y - mouse_pre_y) > 0.0 ? 1.0 : -1.0;
		theCamera->arcOverUnder(s * 0.02f);
		glutPostRedisplay();
	}
	mouse_pre_x = x;
	mouse_pre_y = y;
}

void idle(void)
{
}

void animate(int value)
{
	// Set up the next timer tick (do this first)
	glutTimerFunc(MG_TIMERMSECS, animate, 0);

	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;
	
	
	// ##### REPLACE WITH YOUR OWN GAME/APP MAIN CODE HERE #####
	if (runAnimation)
	{
		// Force a redisplay to render the new image
		if (offset >= 1-0.001)
		{
			offset = 0;
		}
		offset += 0.001;
		RenderState::instance()->setCloudOffset(offset);

		glutPostRedisplay();
	}
	// ##### END OF GAME/APP MAIN CODE #####
	prevTime = currTime;
}

Node *load_scene(int argc, char **argv)
{
	Node *rootNode;
	std::string parameter = argc == 2 ? argv[1] : "Json/scene.json";
	if (parameter == "scene" || parameter == "city")
	{
		rootNode = create_procedural_scene(parameter, 900, 700);
	}
	else
	{
		// load scene from JSON file
		rootNode = parse_scene(parameter);
	}
	return rootNode;
}

int main(int argc, char **argv)
{

	srand(time(0));
	InitRenderContext(argc, argv, 900, 700, 100, 0);

	// set GLUT callback functions
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
	glutReshapeFunc(Resize);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouse);
	// glutIdleFunc( idle );

	displayNode = load_scene(argc, argv);
	Scene::instance()->attach(displayNode);

	// Start the timer (for animations)
	glutTimerFunc(MG_TIMERMSECS, animate, 0);
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

	// Clear screen and begin loop
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glutMainLoop();

	return 0;
}
