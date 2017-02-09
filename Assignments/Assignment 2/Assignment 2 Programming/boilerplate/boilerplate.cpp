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
#include <vector>
#include <math.h>

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace std;
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
// Global variables to store information
int condition = 1;
int scale = 0;
int rotation = 0;
float magnify = 1;
float yVal;
float xVal;
float xv = -1;
float yv = -1;
bool lbutton_down = false;
vector<float> temp(12);
vector<float> vertices(12);
vector<float> textureCoords(12);
float imgWidth = 512;
float imgHeight = 512;

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
// Functions to set up OpenGL buffers for storing textures

struct MyTexture
{
	GLuint textureID;
	GLuint target;
	int width;
	int height;

	// initialize object names to zero (OpenGL reserved value)
	MyTexture() : textureID(0), target(0), width(0), height(0)
	{}
};

bool InitializeTexture(MyTexture* texture, const char* filename, GLuint target = GL_TEXTURE_2D)
{
	int numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(filename, &texture->width, &texture->height, &numComponents, 0);
	if (data != nullptr)
	{
		texture->target = target;
		glGenTextures(1, &texture->textureID);
		glBindTexture(texture->target, texture->textureID);
		GLuint format = numComponents == 3 ? GL_RGB : GL_RGBA;
		glTexImage2D(texture->target, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);

		// Note: Only wrapping modes supported for GL_TEXTURE_RECTANGLE when defining
		// GL_TEXTURE_WRAP are GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clean up
		glBindTexture(texture->target, 0);
		stbi_image_free(data);
		return !CheckGLErrors();
	}
	return true; //error
}

MyTexture texture;
MyShader shader;
MyShader shader2;

// deallocate texture-related objects
void DestroyTexture(MyTexture *texture)
{
	glBindTexture(texture->target, 0);
	glDeleteTextures(1, &texture->textureID);
}

void SaveImage(const char* filename, int width, int height, unsigned char *data, int numComponents = 3, int stride = 0)
{
	if (!stbi_write_png(filename, width, height, numComponents, data, stride))
		cout << "Unable to save image: " << filename << endl;
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry, int choice)
{
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
	// Sets the vertices to appropriate aspect ratio to display each image
	if (choice != 5)
	{
		imgWidth = 512;
		imgHeight = 512;
		// Default for mandrill image
		vertices[0] = -1.f;
		vertices[1] = -1.f;
		vertices[2] = -1.f;
		vertices[3] = 1.f;
		vertices[4] = 1.f;
		vertices[5] = 1.f;
		vertices[6] = -1.f;
		vertices[7] = -1.f;
		vertices[8] = 1.f;
		vertices[9] = 1.f;
		vertices[10] = 1.f;
		vertices[11] = -1.f;
		
		textureCoords[0] = 0.f;
		textureCoords[1] = 0.f;
		textureCoords[2] = 0.f;
		textureCoords[3] = 512.f;
		textureCoords[4] = 512.f;
		textureCoords[5] = 512.f;
		textureCoords[6] = 0.f;
		textureCoords[7] = 0.f;
		textureCoords[8] = 512.f;
		textureCoords[9] = 512.f;
		textureCoords[10] = 512.f;
		textureCoords[11] = 0.f;

		// Set vertices and texture coordinates for mandrill or test image
		if (choice == 0)
		{
			imgWidth = 512;
		imgHeight = 512;
			vertices[0] = -1.f;
			vertices[1] = -1.f;
			vertices[2] = -1.f;
			vertices[3] = 1.f;
			vertices[4] = 1.f;
			vertices[5] = 1.f;
			vertices[6] = -1.f;
			vertices[7] = -1.f;
			vertices[8] = 1.f;
			vertices[9] = 1.f;
			vertices[10] = 1.f;
			vertices[11] = -1.f;
			
			textureCoords[0] = 0.f;
			textureCoords[1] = 0.f;
			textureCoords[2] = 0.f;
			textureCoords[3] = 512.f;
			textureCoords[4] = 512.f;
			textureCoords[5] = 512.f;
			textureCoords[6] = 0.f;
			textureCoords[7] = 0.f;
			textureCoords[8] = 512.f;
			textureCoords[9] = 512.f;
			textureCoords[10] = 512.f;
			textureCoords[11] = 0.f;
		}
		// Set vertices and texture coordinates for aerial image
		else if (choice == 1)
		{
			imgWidth = 2000;
		imgHeight = 931;
			vertices[1] = -.4655f;
			vertices[3] = .4655f;
			vertices[5] = .4655f;
			vertices[7] = -.4655f;
			vertices[9] = .4655f;
			vertices[11] = -.4655f;
			
			textureCoords[3] = 931.f;
			textureCoords[4] = 2000.f;
			textureCoords[5] = 931.f;
			textureCoords[8] = 2000.f;
			textureCoords[9] = 931.f;
			textureCoords[10] =2000.f;
		}
		// Set vertices and texture coordinates for uclogo image
		else if (choice == 2)
		{
			imgWidth = 692;
		imgHeight = 516;
			vertices[1] = -0.7456647399f;
			vertices[3] = 0.7456647399f;
			vertices[5] = 0.7456647399f;
			vertices[7] = -0.7456647399f;
			vertices[9] = 0.7456647399f;
			vertices[11] = -0.7456647399f;
			
			textureCoords[3] = 516.f;
			textureCoords[4] = 692.f;
			textureCoords[5] = 516.f;
			textureCoords[8] = 692.f;
			textureCoords[9] = 516.f;
			textureCoords[10] =692.f;
		}
		// Set vertices and texture coordinates for thirsk image
		else if (choice == 3)
		{
			imgWidth = 400;
		imgHeight = 591;
			vertices[0] = -0.6768189509;
			vertices[2] = -0.6768189509;
			vertices[4] = 0.6768189509;
			vertices[6] = -0.6768189509;
			vertices[8] = 0.6768189509;
			vertices[10] = 0.6768189509;
			
			textureCoords[3] = 591.f;
			textureCoords[4] = 400.f;
			textureCoords[5] = 591.f;
			textureCoords[8] = 400.f;
			textureCoords[9] = 591.f;
			textureCoords[10] =400.f;
		}
		// Set vertices and texture coordinates for pattern image
		else if (choice == 4)
		{
			imgWidth = 2048;
		imgHeight = 1536;
			vertices[1] = -0.75;
			vertices[3] = 0.75;
			vertices[5] = 0.75;
			vertices[7] = -0.75;
			vertices[9] = 0.75;
			vertices[11] = -0.75;
			
			textureCoords[3] = 1536.f;
			textureCoords[4] = 2048.f;
			textureCoords[5] = 1536.f;
			textureCoords[8] = 2048.f;
			textureCoords[9] = 1536.f;
			textureCoords[10] =2048.f;
		}
	}

	const GLfloat colours[][3] = {
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	geometry->elementCount = 6;

	// these vertex attribute indices correspond to those specified for the
	// input variables in the vertex shader
	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;
	const GLuint TEXTURE_INDEX = 2;

	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	//
	glGenBuffers(1, &geometry->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*textureCoords.size(), &textureCoords[0], GL_STATIC_DRAW);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	// Tell openGL how the data is formatted
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glVertexAttribPointer(TEXTURE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(TEXTURE_INDEX);

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

MyGeometry geometry;
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

void RenderScene(MyGeometry *geometry, MyTexture* texture, MyShader *shader)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	glBindTexture(texture->target, texture->textureID);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindTexture(texture->target, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

void RenderScene2(MyGeometry *geometry, MyTexture* texture, MyShader *shader)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	glBindTexture(texture->target, texture->textureID);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindTexture(texture->target, 0);
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
// cursorPosCallback method
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Sample the x and y coordinates
	xVal = xpos;
	yVal = ypos;

	// While the button is pressed, translate image
	if (lbutton_down)
	{
		// Change vertices according to where mouse has moved while clicked
		for(int i = 0; i<12; i= i+2)
		{
			vertices[i] = temp[i] + (xpos-xv)/256;
			vertices[i+1] = temp[i+1] - (ypos-yv)/256;
		}

		// Apply the changes
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 5);
	}
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
// mouse_callback method
static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	lbutton_down = false;

	// If button is pressed, perform the following actions
    if (button == GLFW_MOUSE_BUTTON_LEFT) 
    {
    	// When button is pressed, sample the x and y coordinates, then store the current location of vertices to compare against
        if(GLFW_PRESS == action)
        {
        	xv = xVal;
    		yv = yVal;
    		for(int i = 0; i < 12; i++)
    		{
    			temp[i] = vertices[i];
   	 		}
        	lbutton_down = true;
        }
        else if(GLFW_RELEASE == action)
            lbutton_down = false;
    }

}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{	
		glfwSetWindowShouldClose(window, GL_TRUE);
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
	else if (key == GLFW_KEY_Q)
	{
		// Q key sets to mandrill image
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 0);
		DestroyTexture(&texture);
		InitializeTexture(&texture, "image1-mandrill.png", GL_TEXTURE_RECTANGLE);
	}
	else if (key == GLFW_KEY_W)
	{
		// W key sets to aerial image
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 1);
		DestroyTexture(&texture);
		InitializeTexture(&texture, "image3-aerial.jpg", GL_TEXTURE_RECTANGLE);
	}
	else if (key == GLFW_KEY_E)
	{
		// E key sets to uclogo image
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 2);
		DestroyTexture(&texture);
		InitializeTexture(&texture, "image2-uclogo.png", GL_TEXTURE_RECTANGLE);
	}
	else if (key == GLFW_KEY_R)
	{
		// R key sets to thirsk image
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 3);
		DestroyTexture(&texture);
		InitializeTexture(&texture, "image4-thirsk.jpg", GL_TEXTURE_RECTANGLE);
	}
	else if (key == GLFW_KEY_T)
	{
		// T key sets to pattern image
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 4);
		DestroyTexture(&texture);
		InitializeTexture(&texture, "image5-pattern.png", GL_TEXTURE_RECTANGLE);
	}
	else if (key == GLFW_KEY_Y)
	{
		// Y key sets to test image
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 0);
		DestroyTexture(&texture);
		InitializeTexture(&texture, "test.png", GL_TEXTURE_RECTANGLE);
	}
	else if (key == GLFW_KEY_SPACE)
	{
		// Remove image effects and set to original image
		scale = 0;
	}
	else if (key == GLFW_KEY_1)
	{
		// Applies L = 0.333 R + 0.333 G + 0.333 B Color Effect
		scale= 1;
	}
	else if (key == GLFW_KEY_2)
	{
		// Applies L = 0.299 R + 0.587 G + 0.114 B Color Effect
		scale = 2;
	}
	else if (key == GLFW_KEY_3)
	{
		// Applies L = 0.213 R + 0.715 G + 0.072 B Color Effect
		scale = 3;
	}
	else if (key == GLFW_KEY_4)
	{	
		// Applies Sepia tone Color Effect
		scale = 4;
	}
	else if (key == GLFW_KEY_5)
	{
		// Applies Horizontal Sobel Edge Effect
		scale = 5;
	}
	else if (key == GLFW_KEY_6)
	{
		// Applies Vertical Sobel Edge Effect
		scale = 6;
	}
	else if (key == GLFW_KEY_7)
	{
		// Applies Unsharp Mask Edge Effect
		scale = 7;
	}
	else if (key == GLFW_KEY_8)
	{
		// Applies 3x3 Gaussian Blur
		scale = 8;
	}
	else if (key == GLFW_KEY_9)
	{
		// Applies 5x5 Gaussian Blur
		scale = 9;
	}
	else if (key == GLFW_KEY_0)
	{
		// Applies 7x7 Gaussian Blur
		scale = 10;
	}
	else if (key == GLFW_KEY_Z)
	{
		// Applies 7x7 Gaussian Blur
		scale = 11;
	}
	else if (key == GLFW_KEY_X)
	{
		// Applies 7x7 Gaussian Blur
		scale = 12;
	}
	else if (key == GLFW_KEY_C)
	{
		// Applies 7x7 Gaussian Blur
		scale = 13;
	}
	else if (key == GLFW_KEY_V)
	{
		scale = 14;
	}
	else if (key == GLFW_KEY_B)
	{
		scale = 15;
	}
	else if (key == GLFW_KEY_LEFT)
	{
		// Rotates the image counterclockwise
		for (int i = 0; i < 12; i= i+2)
		{
			// Determine the x and y of vertices
			float opp = vertices[i+1];
			float adj = vertices[i];

			// Increase angle by 0.5 degree
			float angle = atan2(opp, adj) *180/ 3.14159265 + .5;
			float distance = sqrt(pow(opp, 2.0) + pow(adj, 2.0));

			// Set new location of vertices
			vertices[i] = distance*cos(angle*3.14159265/180);
			vertices[i+1] = distance*sin(angle*3.14159265/180);
		}

		//Apply
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 5);
	}
	else if (key == GLFW_KEY_RIGHT)
	{
		// Rotates the image clockwise
		for (int i = 0; i < 12; i= i+2)
		{
			float opp = vertices[i+1];
			float adj = vertices[i];

			// Decrease by 0.5 degree
			float angle = atan2(opp, adj) *180/ 3.14159265 - .5;
			float distance = sqrt(pow(opp, 2.0) + pow(adj, 2.0));

			vertices[i] = distance*cos(angle*3.14159265/180);
			vertices[i+1] = distance*sin(angle*3.14159265/180);
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 5);
	}
	else if  (key == GLFW_KEY_UP)
	{
		// Magnification factor
		magnify = 1.005;
		for (int i = 0; i < 12; i= i+2)
		{
			// Obtain location of vertices
			float opp = vertices[i+1];
			float adj = vertices[i];

			float angle = atan2(opp, adj) *180/ 3.14159265;
			// Magnify image by factor
			float distance = magnify*sqrt(pow(opp, 2.0) + pow(adj, 2.0));
			
			// Update vertices
			vertices[i] = distance*cos(angle*3.14159265/180);
			vertices[i+1] = distance*sin(angle*3.14159265/180);
		}

		// Apply
		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 5);
		condition = 1;
	}
	else if  (key == GLFW_KEY_DOWN)
	{
		// 
		magnify = 1/1.005;
		for (int i = 0; i < 12; i= i+2)
		{
			float opp = vertices[i+1];
			float adj = vertices[i];

			float angle = atan2(opp, adj) *180/ 3.14159265;
			// Minimize image by factor
			float distance = magnify*sqrt(pow(opp, 2.0) + pow(adj, 2.0));
			
			vertices[i] = distance*cos(angle*3.14159265/180);
			vertices[i+1] = distance*sin(angle*3.14159265/180);
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, 5);
		condition = 0;
	}
	
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
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
	// Set CursorPosCallback and MouseButtonCallback
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwMakeContextCurrent(window);

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	if (!InitializeShaders(&shader)) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// call function to create and fill buffers with geometry data
	if (!InitializeGeometry(&geometry, 0))
		cout << "Program failed to intialize geometry!" << endl;

	if(!InitializeTexture(&texture, "image1-mandrill.png", GL_TEXTURE_RECTANGLE))
		cout << "Program failed to intialize texture!" << endl;

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
	// Use uniform to send information to fragment.glsl
	glUseProgram(shader.program);
	GLint scaleUniform = glGetUniformLocation(shader.program, "selection");
	glUniform1i(scaleUniform, scale);

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		


	

		// Use uniform to send information to fragment.glsl
		glUseProgram(shader.program);
		glUniform1i(scaleUniform, scale);

		if (scale >= 11 && scale <= 15)
		{
			MyTexture frameTexture;
	
			int numComponents=3;
			frameTexture.target = GL_TEXTURE_RECTANGLE;
			glGenTextures(1, &frameTexture.textureID);
			glBindTexture(frameTexture.target, frameTexture.textureID);
			GLuint format = numComponents == 3 ? GL_RGB : GL_RGBA;
			glTexImage2D(frameTexture.target, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, NULL);

			// Note: Only wrapping modes supported for GL_TEXTURE_RECTANGLE when defining
			// GL_TEXTURE_WRAP are GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER
			glTexParameteri(frameTexture.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(frameTexture.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(frameTexture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(frameTexture.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Clean up
			glBindTexture(frameTexture.target, 0);
			
	
			GLuint frameBuffer;
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture(
		    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameTexture.textureID, 0
			);

			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

			glViewport(0, 0, imgWidth, imgHeight);
			RenderScene(&geometry, &texture, &shader); //render scene with texture

			if (scale == 11)
			{
				scale =20;
			}
			else if (scale == 12)
			{
				scale = 21;
			}
			else if (scale == 13)
			{
				scale = 22;
			}
			else if (scale == 14)
			{
				scale = 23;
			}
			else if (scale == 15)
			{
				scale = 24;
			}
			
			glUseProgram(shader.program);
			glUniform1i(scaleUniform, scale);


			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, 512, 512);
			RenderScene(&geometry, &frameTexture, &shader); //render scene with texture
			if (scale == 20)
			{
				scale=11;
			}
			else if (scale == 21)
			{
				scale = 12;
			}
			else if (scale == 22)
			{
				scale = 13;
			}
			else if (scale == 23)
			{
				scale = 14;
			}
			else if (scale == 24)
			{
				scale = 15;
			}
		}

		if (scale != 11 && scale != 12 && scale!= 13 && scale != 14 && scale != 15)
		{
			RenderScene(&geometry, &texture, &shader); //render scene with texture
		}

		glfwSwapBuffers(window);

		glfwPollEvents();
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
	cout << "Usage: \t Press the keys Q, W, E, R, T, and Y to select the different images. " << endl;
	cout << "\t Press 1, 2, or 3 for the different greyscale conversions, and 4 for the sepia color effect." << endl;
	cout << "\t Press 5 for horizontal Sobel filter, 6 for vertical Sobel filter, and 7 for the unsharp mark operator." << endl;
	cout << "\t Press 8 for the 3x3 Gaussian blur, 9 for the 5x5 Gaussian blur, and 0 for the 7x7 Gaussian blur." << endl;
	cout << "\t Press the space bar to remove the effects and obtain original image." << endl;
	cout << "\t Press the up arrow key to increase magnification, and the down arrow key to decrease magnification." << endl;
	cout << "\t Press the left arrow key to rotate counterclockwise, and the right arrow key to rotate clockwise." << endl;
	cout << "\t Drag with the mouse to translate the image." << endl;
	cout << "\t Press the keys Z, X, C, V, and B to select the different 1D Gaussian kernels. " << endl;


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
