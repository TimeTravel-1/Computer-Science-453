// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
//========= include math and vector
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// specify that we want the OpenGL core profile before including GLFW headers
// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

using namespace std;
using std::vector;

// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Add function prototypes.
void createCircle(float radius, vector<float>& vertices,vector<float>& colours);
void createSpiral(int level, vector<float>& vertices,vector<float>& colours);
void createSquare(int level, vector<float>& vertices,vector<float>& colours);
void square(int level, int shade, vector<float>& vertices, vector<float>& colours);
void createTriangle(int level, vector<float>& vertices,vector<float>& colours);
void triangle(int level, int shade, vector<float>& vertices, vector<float>& colours);
void createKoch(int level, vector<float>& vertices,vector<float>& colours);
void koch(int level, int shade, vector<float>& vertices, vector<float>& colours);
void createFern(long level, vector<float>& vertices,vector<float>& colours);
void fern(long level, int shade, vector<float>& vertices, vector<float>& colours);

// condition is used to indicate which scene is currently drawn.
int condition = 4;

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

struct MyShader
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint  vertex;
	GLuint  fragment;
	GLuint  program;

	// initialize shader and program names to zero (OpenGL reserved value)
	MyShader() : vertex(0), fragment(0), program(0)
	{}
};

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
	// load shader source from files
	string vertexSource = LoadSource("vertex.glsl");
	string fragmentSource = LoadSource("fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

MyGeometry geometry;
GLuint drawingMode = GL_TRIANGLES;

// create buffers and fill with geometry data, returning true if successful
//========= Add parameters
bool InitializeGeometry(MyGeometry *geometry, const vector<float>& vertices, const vector<float>& colours)
{
	//========= update element count
	geometry->elementCount = vertices.size() / 2;

	// these vertex attribute indices correspond to those specified for the
	// input variables in the vertex shader
	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	//========= Update buffer data vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	//========= Update budder data colours
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*colours.size(), &colours[0], GL_STATIC_DRAW);

	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	// assocaite the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer
//========= Render with given renderMode
void RenderScene(MyGeometry *geometry, MyShader *shader, GLuint renderMode = GL_TRIANGLES)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(renderMode, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// When "S" is pressed, draw "Squares and Diamonds".
	else if(key == GLFW_KEY_S)
	{
		//Condition indicates which scene is currently drawn (used for drawing iterations).
		condition = 2;
		DestroyGeometry(&geometry);
		vector<float> vertices;
		vector<float> colours;
		createSquare(1, vertices, colours);
		drawingMode = GL_LINES;
		InitializeGeometry(&geometry, vertices, colours);
	}
	//When "P" is pressed, draw "Parametric Spiral".
	else if(key == GLFW_KEY_P)
	{
		condition = 3;
		DestroyGeometry(&geometry);
		vector<float> vertices;
		vector<float> colours;
		createSpiral(1, vertices, colours);
		drawingMode = GL_LINE_STRIP;
		InitializeGeometry(&geometry, vertices, colours);
	}
	//When "T" is pressed, draw "Sierpinski Triangle".
	else if(key == GLFW_KEY_T)
	{
		condition = 4;
		DestroyGeometry(&geometry);
		vector<float> vertices;
		vector<float> colours;
		createTriangle(1, vertices, colours);
		drawingMode = GL_TRIANGLES;
		InitializeGeometry(&geometry, vertices, colours);
		
	}
	else if(key == GLFW_KEY_K)
	{
		condition = 5;
		DestroyGeometry(&geometry);
		vector<float> vertices;
		vector<float> colours;
		createKoch(1, vertices, colours);
		drawingMode = GL_LINES;
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if(key == GLFW_KEY_F)
	{
		condition = 6;
		DestroyGeometry(&geometry);
		vector<float> vertices;
		vector<float> colours;

		createFern(2500, vertices, colours);

		for (int i = 0; i < vertices.size()/2; i=i+2)
		{
			vertices[i] = vertices[i]/2.6558;
			vertices[i+1] = vertices[i+1]/5.0-1.0;
		}

		drawingMode = GL_POINTS;
		InitializeGeometry(&geometry, vertices, colours);
	}
	//When "1" is pressed, draw first iteration of current scene.
	else if (key == GLFW_KEY_1)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(1, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);

		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(1, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
			
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(1, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(1, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(2500, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "2" is pressed, draw second iteration of current scene.
	else if (key == GLFW_KEY_2)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(2,vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(2, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);		
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(2, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(2, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(5000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "3" is pressed, draw third iteration of current scene.
	else if (key == GLFW_KEY_3)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(3, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(3, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(3, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(3, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(10000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "4" is pressed, draw fourth iteration of current scene.
	else if (key == GLFW_KEY_4)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(4, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(4, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(4, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(4, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(20000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "5" is pressed, draw fifth iteration of current scene.
	else if (key == GLFW_KEY_5)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(5, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(5, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(5, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(5, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(30000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "6" is pressed, draw sixth iteration of current scene.
	else if (key == GLFW_KEY_6)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(6, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(6, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(6, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(6, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(40000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "7" is pressed, draw seventh iteration of current scene.
	else if (key == GLFW_KEY_7)
	{
		if (condition == 2)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(7, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(7, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(7, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(7, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(50000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
	//When "8" is pressed, draw eighth iteration of current scene.
	else if (key == GLFW_KEY_8)
	{
		if (condition == 2)
		{	
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSquare(8, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 3)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createSpiral(8, vertices, colours);
			drawingMode = GL_LINE_STRIP;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 4)
		{
			condition = 4;
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createTriangle(8, vertices, colours);
			drawingMode = GL_TRIANGLES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if (condition == 5)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createKoch(8, vertices, colours);
			drawingMode = GL_LINES;
			InitializeGeometry(&geometry, vertices, colours);
		}
		else if(condition == 6)
		{
			DestroyGeometry(&geometry);
			vector<float> vertices;
			vector<float> colours;
			createFern(60000, vertices, colours);

			for (int i = 0; i < vertices.size()/2; i=i+2)
			{
				vertices[i] = vertices[i]/2.6558;
				vertices[i+1] = vertices[i+1]/5.0-1.0;
			}

			drawingMode = GL_POINTS;
			InitializeGeometry(&geometry, vertices, colours);
		}	
	}
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initilize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(512, 512, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	MyShader shader;
	if (!InitializeShaders(&shader)) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// Draw initial triangle (changed to be the base triangle of Sierpinski Triangle).
	vector<float> vertices(6);
	vertices[0] = 0.0;
	vertices[1] = 0.7794228634059947820873508536776425651242623642146712;

	vertices[2] = -.9;
	vertices[3] = -0.7794228634059947820873508536776425651242623642146712;

	vertices[4] = .9;
	vertices[5] = -0.7794228634059947820873508536776425651242623642146712;
	vector<float> colours(9);

	colours[0] = 1;
	colours[1] = 0;
	colours[2] = 0;

	colours[3] = 0;
	colours[4] = 1;
	colours[5] = 0;

	colours[6] = 0;
	colours[7] = 0;
	colours[8] = 1;

	// call function to create and fill buffers with geometry data
	if (!InitializeGeometry(&geometry, vertices, colours))
		cout << "Program failed to intialize geometry!" << endl;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		// call function to draw our scene
		//========= Add drawing mode
		RenderScene(&geometry, &shader, drawingMode);

		// scene is rendered to the back buffer, so swap to front for display
		glfwSwapBuffers(window);

		// sleep until next event before drawing again
		glfwWaitEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	DestroyShaders(&shader);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
	
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------
	cout << endl;
	cout << "Usage: Press \"S\" for Squares and Diamonds, \"P\" for Parametric Spiral, and \"T\" for Sierpinski Triangle. Press the numbers in the range \"1-8\" inclusive for the corresponding number of iterations in each scene." << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Create spiral. level is used to indicate which iteration of spiral is drawn.
void createSpiral(int level, vector<float>& vertices,vector<float>& colours)
{

	//maximum indicates the number of whole revoluations: 2pi, 4pi, 6pi...
	int maximum = level * 2 * 3.14159265358979323846;
	//radius is used as a scaling factor to make spiral end near the edge of the window.
	//0.9 is used instead of 1 since there is to be a small border (for aesthetics).
	float radius = 0.9/(float) maximum;

	//corresponds to maximum expressed in degrees.
	int revolutions = 360 * level;
	
	//Change colours from blue to green as spiral progresses from center to edge.
	float green = 0.f;
	float blue = 1.f;

	for(float i=0; i<=revolutions; ++i)
	{
		//Add corresponding point to vertices.
		float rad = (float)i*3.14159265358979323846f/180.f;
		vertices.push_back(radius * rad * (float) cos(rad));
		vertices.push_back(radius * rad * (float) sin(rad));

		//Add corresponding colours.
		colours.push_back(0);
		colours.push_back(green);
		colours.push_back(blue);

		//Adjust colour gradient smoothly.
		green += 1.0f/(float) revolutions;
		blue -= 1.0f/(float) revolutions;
	}
}

void createKoch(int level, vector<float>& vertices, vector<float>& colours)
{
	//Using a width of 1.8, we note that for an equilateral triangle, this means the bottom side is of length 1.8.
	//We note that the height is therefore sqrt(1.8^2-.9^2). Dividing by 2 gives coordinates for top vertex of triangle.
	//This evaluates to approximately 0.7794228634059947820873508536776425651242623642146712.

	//A triangle is formed with the following vertices.
	vertices.push_back(0.0);
	vertices.push_back(0.7794228634059947820873508536776425651242623642146712);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(-0.9+.2);
	vertices.push_back(-0.7794228634059947820873508536776425651242623642146712+.34641);
	colours.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);

	vertices.push_back(-0.9+.2);
	vertices.push_back(-0.7794228634059947820873508536776425651242623642146712+.34641);
	colours.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);

	vertices.push_back(0.9-.2);
	vertices.push_back(-0.7794228634059947820873508536776425651242623642146712+.34641);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(1.0);

	vertices.push_back(0.9-.2);
	vertices.push_back(-0.7794228634059947820873508536776425651242623642146712+.34641);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(1.0);

	vertices.push_back(0.0);
	vertices.push_back(0.7794228634059947820873508536776425651242623642146712);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	

	//This is used to draw additional triangles inside for each successive iteration.
	if (level > 1)
	{
		koch(level, 1, vertices, colours);
	}
}

//Draws additional triangles.
//shade is used to progrssively alter the shade of the colours between geometry at different nested levels.
void koch(int level, int shade, vector<float>& vertices, vector<float>& colours)
{

	//Used to temporarily store information to replace original data points.
	vector<float> newVertices;
	vector<float> newColours;

	//Loop for each line.
	for (int i = 0; i < vertices.size()/4; i ++)
	{
		float a1 = vertices[4*i];
		float a2 = vertices[4*i + 1];
		float b1 = vertices[4*i + 2];
		float b2 = vertices[4*i + 3];

		float distance = sqrt(pow(b2-a2, 2.0) + pow(b1-a1, 2.0));
		float angle = atan2(b2-a2, b1-a1);

		//Draw the vertices of the three new triangles
		newVertices.push_back(vertices[4*i]);
		newVertices.push_back(vertices[4*i+1]);
		newVertices.push_back((2.0f*a1+b1)/3.0f);
		newVertices.push_back((2.0f*a2+b2)/3.0f);

		newVertices.push_back((2.0f*a1+b1)/3.0f);
		newVertices.push_back((2.0f*a2+b2)/3.0f);
		newVertices.push_back((2.0f*a1+b1)/3.0f + distance/3.0f*cos(angle-3.14159/3.0f));
		newVertices.push_back((2.0f*a2+b2)/3.0f + distance/3.0f*sin(angle-3.14159/3.0f));
		
		newVertices.push_back((2.0f*a1+b1)/3.0f + distance/3.0f*cos(angle-3.14159/3.0f));
		newVertices.push_back((2.0f*a2+b2)/3.0f + distance/3.0f*sin(angle-3.14159/3.0f));
		newVertices.push_back((2.0f*b1+a1)/3.0f);
		newVertices.push_back((2.0f*b2+a2)/3.0f);
		
		newVertices.push_back((2.0f*b1+a1)/3.0f);
		newVertices.push_back((2.0f*b2+a2)/3.0f);
		newVertices.push_back(vertices[4*i+2]);
		newVertices.push_back(vertices[4*i+3]);
		
		
		
		
		

	}

	//Loop for each triangle.
	for (int i = 0; i < colours.size()/6; i ++)
	{
		//Set appropriate colours for each of the three new triangles using shade.
		newColours.push_back(colours[6*i]);
		newColours.push_back(colours[6*i+1]);
		newColours.push_back(colours[6*i+2]);
		newColours.push_back(colours[6*i]*2.0/3.0 + colours[6*i+3]/3.0);
		newColours.push_back(colours[6*i+1]*2.0/3.0 + colours[6*i+4]/3.0);
		newColours.push_back(colours[6*i+2]*2.0/3.0 + colours[6*i+5]/3.0);

		newColours.push_back(colours[6*i]*2.0/3.0 + colours[6*i+3]/3.0);
		newColours.push_back(colours[6*i+1]*2.0/3.0 + colours[6*i+4]/3.0);
		newColours.push_back(colours[6*i+2]*2.0/3.0 + colours[6*i+5]/3.0);
		newColours.push_back(colours[6*i]/2.0 + colours[6*i+3]/2.0);
		newColours.push_back(colours[6*i+1]/2.0 + colours[6*i+4]/2.0);
		newColours.push_back(colours[6*i+2]/2.0 + colours[6*i+5]/2.0);

		newColours.push_back(colours[6*i]/2.0 + colours[6*i+3]/2.0);
		newColours.push_back(colours[6*i+1]/2.0 + colours[6*i+4]/2.0);
		newColours.push_back(colours[6*i+2]/2.0 + colours[6*i+5]/2.0);
		newColours.push_back(colours[6*i]/3.0 + colours[6*i+3]*2.0/3.0);
		newColours.push_back(colours[6*i+1]/3.0 + colours[6*i+4]*2.0/3.0);
		newColours.push_back(colours[6*i+2]/3.0 + colours[6*i+5]*2.0/3.0);

		newColours.push_back(colours[6*i]/3.0 + colours[6*i+3]*2.0/3.0);
		newColours.push_back(colours[6*i+1]/3.0 + colours[6*i+4]*2.0/3.0);
		newColours.push_back(colours[6*i+2]/3.0 + colours[6*i+5]*2.0/3.0);
		newColours.push_back(colours[6*i+3]);
		newColours.push_back(colours[6*i+4]);
		newColours.push_back(colours[6*i+5]);
	}

	//Clear the original set of data.
	vertices.clear();
	colours.clear();

	//Set with new vertex and colour data.
	for (int i = 0; i < newVertices.size(); i++)
	{
		vertices.push_back(newVertices[i]);
	}

	for (int i = 0; i < newColours.size(); i++)
	{
		colours.push_back(newColours[i]);
	}

	//Recursively call as necessary to represent appropriate iteration.
	if (level > 2)
	{
		koch(level - 1, shade + 1, vertices, colours);
	}
}

void createFern(long level, vector<float>& vertices, vector<float>& colours)
{
	vertices.push_back(0.0);
	vertices.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);
	
	srand(time(NULL));

	//This is used to draw additional triangles inside for each successive iteration.
	if (level > 1)
	{
		fern(level, 1, vertices, colours);
	}
}

//Draws additional triangles.
//shade is used to progrssively alter the shade of the colours between geometry at different nested levels.
void fern(long level, int shade, vector<float>& vertices, vector<float>& colours)
{
	int random = rand()%100+1;
	int k = vertices.size();
	
	if (random == 1)
	{
		vertices.push_back(0);
		vertices.push_back((vertices[k-1]*.16));
	}
	else if (random <= 86)
	{
		vertices.push_back((vertices[k-2]*.85+vertices[k-1]*.04));
		vertices.push_back((vertices[k-2]*-.04+vertices[k-1]*.85+1.6));
	}
	else if (random <= 93)
	{
		vertices.push_back((vertices[k-2]*.2+vertices[k-1]*-.26));
		vertices.push_back((vertices[k-2]*.23+vertices[k-1]*.22+1.6));

	}
	else if (random <= 100)
	{
		vertices.push_back((vertices[k-2]*-.15+vertices[k-1]*.28));
		vertices.push_back((vertices[k-2]*.26+vertices[k-1]*.24+.44));
	}

	colours.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);

	//Recursively call as necessary to represent appropriate iteration.
	if (level > 2)
	{
		fern(level - 1, shade + 1, vertices, colours);
	}
}

//Create square.
void createSquare(int level, vector<float>& vertices,vector<float>& colours)
{
	//Each block that follows corresponds to the formation of a line of the square.
	//Once again, a small border is used so lines do not reach edge. Range from -0.9 to 0.9.
	//Initial colour of all sides is black. 
	vertices.push_back(-0.9);
	vertices.push_back(-0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(-0.9);
	vertices.push_back(0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(-0.9);
	vertices.push_back(0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(0.9);
	vertices.push_back(0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(0.9);
	vertices.push_back(0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(0.9);
	vertices.push_back(-0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(0.9);
	vertices.push_back(-0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(-0.9);
	vertices.push_back(-0.9);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	//Each block that follows corresponds to the formation of a line of the diamond.
	//Initial colour is red.
	vertices.push_back(-0.9);
	vertices.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(0.0);
	vertices.push_back(0.9);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(0.0);
	vertices.push_back(0.9);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(0.9);
	vertices.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(0.9);
	vertices.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(0.0);
	vertices.push_back(-0.9);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(0.0);
	vertices.push_back(-0.9);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);
	vertices.push_back(-0.9);
	vertices.push_back(-0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	//This is used to draw additional squares and diamonds inside for each successive iteration.
	if (level > 1)
	{
		square(level, 1, vertices, colours);
	}

}

//Draws additional squares and diamonds.
//shade is used to progrssively alter the shade of the colours between geometry at different nested levels.
void square(int level, int shade, vector<float>& vertices, vector<float>& colours)
{
	//Find the vertex in vertices that indicates start of current innermost square.
	int size = vertices.size();
	int start = size-32;

	//Indicates vertices of square to be drawn.
	float distance = (vertices[start+3] - vertices[start+1])/4;
	
	//Each block below indicates a new line of the square.
	//Shade of colour is lightened as more squares are drawn.
	vertices.push_back(-distance);
	vertices.push_back(-distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	vertices.push_back(-distance);
	vertices.push_back(distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);

	vertices.push_back(-distance);
	vertices.push_back(distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	vertices.push_back(distance);
	vertices.push_back(distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);

	vertices.push_back(distance);
	vertices.push_back(distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	vertices.push_back(distance);
	vertices.push_back(-distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);

	vertices.push_back(distance);
	vertices.push_back(-distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	vertices.push_back(-distance);
	vertices.push_back(-distance);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);
	colours.push_back(0.0 + .12*shade);

	//Each block below indicates a new line of the square.
	//Shade of colour is lightened as more diamonds are drawn.
	vertices.push_back(-distance);
	vertices.push_back(0);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);
	vertices.push_back(0);
	vertices.push_back(distance);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);

	vertices.push_back(0);
	vertices.push_back(distance);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);
	vertices.push_back(distance);
	vertices.push_back(0);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);

	vertices.push_back(distance);
	vertices.push_back(0);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);
	vertices.push_back(0);
	vertices.push_back(-distance);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);

	vertices.push_back(0);
	vertices.push_back(-distance);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);
	vertices.push_back(-distance);
	vertices.push_back(0);
	colours.push_back(1.0 );
	colours.push_back(0.0 + 0.12*shade);
	colours.push_back(0.0 + 0.12*shade);

	//This is used to draw additional triangles inside for each successive iteration.
	if (level > 2)
	{
		//Recursively call the drawing of more squares as necessary
		square(level - 1, shade + 1, vertices, colours);
	}
}

//Create Sierpinski triangle.
void createTriangle(int level, vector<float>& vertices, vector<float>& colours)
{
	//Using a width of 1.8, we note that for an equilateral triangle, this means the bottom side is of length 1.8.
	//We note that the height is therefore sqrt(1.8^2-.9^2). Dividing by 2 gives coordinates for top vertex of triangle.
	//This evaluates to approximately 0.7794228634059947820873508536776425651242623642146712.

	//A triangle is formed with the following vertices.
	vertices.push_back(0.0);
	vertices.push_back(0.7794228634059947820873508536776425651242623642146712);
	colours.push_back(1.0);
	colours.push_back(0.0);
	colours.push_back(0.0);

	vertices.push_back(-0.9);
	vertices.push_back(-0.7794228634059947820873508536776425651242623642146712);
	colours.push_back(0.0);
	colours.push_back(1.0);
	colours.push_back(0.0);

	vertices.push_back(0.9);
	vertices.push_back(-0.7794228634059947820873508536776425651242623642146712);
	colours.push_back(0.0);
	colours.push_back(0.0);
	colours.push_back(1.0);

	//This is used to draw additional triangles inside for each successive iteration.
	if (level > 1)
	{
		triangle(level, 1, vertices, colours);
	}
}

//Draws additional triangles.
//shade is used to progrssively alter the shade of the colours between geometry at different nested levels.
void triangle(int level, int shade, vector<float>& vertices, vector<float>& colours)
{

	//Used to temporarily store information to replace original data points.
	vector<float> newVertices;
	vector<float> newColours;

	//Loop for each triangle.
	for (int i = 0; i < vertices.size()/6; i ++)
	{
		//Find coordinates needed to draw three additional triangles within original.
		float horizontal = (vertices[6*i+2] + vertices[6*i+0])/2;
		float vertical = (vertices[6*i+3] + vertices[6*i+1])/2;
		float horizontal2 = (vertices[6*i+4] + vertices[6*i+0])/2;

		//Draw the vertices of the three new triangles.
		newVertices.push_back(vertices[6*i]);
		newVertices.push_back(vertices[6*i+1]);
		newVertices.push_back(horizontal);
		newVertices.push_back(vertical);
		newVertices.push_back(horizontal2);
		newVertices.push_back(vertical);

		newVertices.push_back(horizontal);
		newVertices.push_back(vertical);
		newVertices.push_back(vertices[6*i+2]);
		newVertices.push_back(vertices[6*i+3]);
		newVertices.push_back(vertices[6*i]);
		newVertices.push_back(vertices[6*i+3]);

		newVertices.push_back(horizontal2);
		newVertices.push_back(vertical);
		newVertices.push_back(vertices[6*i]);
		newVertices.push_back(vertices[6*i+3]);
		newVertices.push_back(vertices[6*i+4]);
		newVertices.push_back(vertices[6*i+3]);
	}

	//Loop for each triangle.
	for (int i = 0; i < colours.size()/9; i ++)
	{
		//Set appropriate colours for each of the three new triangles using shade.
		newColours.push_back(colours[9*i]);
		newColours.push_back(colours[9*i+1]);
		newColours.push_back(colours[9*i+2]);
		newColours.push_back(colours[9*i]);
		newColours.push_back(colours[9*i+1]);
		newColours.push_back(colours[9*i+2]);
		newColours.push_back(colours[9*i]);
		newColours.push_back(colours[9*i+1]);
		newColours.push_back(colours[9*i+2]);

		newColours.push_back(colours[9*i+3]-.04*shade);
		newColours.push_back(colours[9*i+4]-.04*shade);
		newColours.push_back(colours[9*i+5]-.04*shade);
		newColours.push_back(colours[9*i+3]-.04*shade);
		newColours.push_back(colours[9*i+4]-.04*shade);
		newColours.push_back(colours[9*i+5]-.04*shade);
		newColours.push_back(colours[9*i+3]-.04*shade);
		newColours.push_back(colours[9*i+4]-.04*shade);
		newColours.push_back(colours[9*i+5]-.04*shade);

		newColours.push_back(colours[9*i+6]+.04*shade);
		newColours.push_back(colours[9*i+7]+.04*shade);
		newColours.push_back(colours[9*i+8]+.04*shade);
		newColours.push_back(colours[9*i+6]+.04*shade);
		newColours.push_back(colours[9*i+7]+.04*shade);
		newColours.push_back(colours[9*i+8]+.04*shade);
		newColours.push_back(colours[9*i+6]+.04*shade);
		newColours.push_back(colours[9*i+7]+.04*shade);
		newColours.push_back(colours[9*i+8]+.04*shade);

	}

	//Clear the original set of data.
	vertices.clear();
	colours.clear();

	//Set with new vertex and colour data.
	for (int i = 0; i < newVertices.size(); i++)
	{
		vertices.push_back(newVertices[i]);
	}

	for (int i = 0; i < newColours.size(); i++)
	{
		colours.push_back(newColours[i]);
	}

	//Recursively call as necessary to represent appropriate iteration.
	if (level > 2)
	{
		triangle(level - 1, shade + 1, vertices, colours);
	}
}
