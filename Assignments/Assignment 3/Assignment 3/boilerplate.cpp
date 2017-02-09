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
#include <vector>
#include <iterator>
#include "GlyphExtractor.h"

// Specify that we want the OpenGL core profile before including GLFW headers
#ifndef LAB_LINUX
	#include <glad/glad.h>
#else
	#define GLFW_INCLUDE_GLCOREARB
	#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

//STB
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
GLuint LinkProgram(GLuint vertexShader, GLuint TCSshader, GLuint TESshader, GLuint fragmentShader); 
GLuint LinkProgram2(GLuint vertexShader, GLuint fragmentShader);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Variables that are used in program
int choose = 1;
int condition = 0;
int speed = 0;
float moveRate = 0.0;
int fontc = 0;
// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

struct MyShader
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint  vertex;
	GLuint  TCS; 
	GLuint  TES; 
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
	string TCSSource = LoadSource("tessControl.glsl");
	string TESSource = LoadSource("tessEval.glsl"); 
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource); 
	shader->TCS = CompileShader(GL_TESS_CONTROL_SHADER, TCSSource);
	shader->TES = CompileShader(GL_TESS_EVALUATION_SHADER, TESSource);

	// link shad11er program
	shader->program = LinkProgram(shader->vertex, shader->TCS, shader->TES, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Shader for drawing lines and points
struct MyShader2
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint  vertex;
	GLuint  fragment;
	GLuint  program;

	// initialize shader and program names to zero (OpenGL reserved value)
	MyShader2() : vertex(0), fragment(0), program(0)
	{}
};

bool InitializeShaders2(MyShader2 *shader)
{
	// load shader source from files
	string vertexSource = LoadSource("vertex2.glsl");
	string fragmentSource = LoadSource("fragment2.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram2(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

void DestroyShaders2(MyShader2 *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
	glDeleteShader(shader->TCS); 
	glDeleteShader(shader->TES); 
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

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Geometries for drawing patches, lines and points
MyGeometry geometry;
MyGeometry pointGeo;
MyGeometry lineGeo;

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry, const vector<float>& vertices, const vector<float>& colours)
{
	geometry->elementCount = vertices.size()/2; 

	// these vertex attribute indices correspond to those specified for the
	// input variables in the vertex shader
	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
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

void RenderScene(MyGeometry *geometry, MyShader *shader)
{
	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);

	glDrawArrays(GL_PATCHES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// RenderScene for drawing either lines or points
void RenderScene(MyGeometry *geometry, MyShader2 *shader, int i)
{

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	if (i == 1)
	{
		// Draw lines
		glDrawArrays(GL_LINES, 0, geometry->elementCount);
	}
	else if (i == 2)
	{
		// Draw points
		glDrawArrays(GL_POINTS, 0, geometry->elementCount);
	}
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
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// Keyboard input used to control program
	else if (key == GLFW_KEY_1)
	{
		// Set variables
		choose = 0;
		condition = 1;
		speed = 0;
		fontc = 0;

		// Fill vector with vertices scaled to fit window
		vector<float> vertices(24);

		vertices[0] = (1.f/2.5f);
		vertices[1] = (1.f/2.5f);
		vertices[2] = (2.f/2.5f);
		vertices[3] = (-1.f/2.5f);
		vertices[4] = (0.f/2.5f);
		vertices[5] = (-1.f/2.5f);
		
		vertices[6] = (0.f/2.5f);
		vertices[7] = (-1.f/2.5f);
		vertices[8] = (-2.f/2.5f);
		vertices[9] = (-1.f/2.5f);
		vertices[10] = (-1.f/2.5f);
		vertices[11] = (1.f/2.5f);
		
		vertices[12] = (-1.f/2.5f);
		vertices[13] = (1.f/2.5f);
		vertices[14] = (0.f/2.5f);
		vertices[15] = (1.f/2.5f);
		vertices[16] = (1.f/2.5f);
		vertices[17] = (1.f/2.5f);
		
		vertices[18] = (1.2f/2.5f);
		vertices[19] = (0.5f/2.5f);
		vertices[20] = (2.5f/2.5f);
		vertices[21] = (1.f/2.5f);
		vertices[22] = (1.3f/2.5f);
		vertices[23] = (-0.4f/2.5f);

		// Fill vector with purple colour (teapot)
		vector<float> colours;

		for (int p = 0; p < 12; p++)
		{
			colours.push_back(1.0);
			colours.push_back(0.0);
			colours.push_back(1.0);
		}

		// Translate vertices to center of window
		for (int z = 0; z < vertices.size(); z+=2)
		{
			vertices[z] -= 0.1;
		}

		InitializeGeometry(&geometry, vertices, colours);

		// Fill vector with certices for control polygon
		vector<float> newVertex;
		
		newVertex.push_back(1.0);
		newVertex.push_back(1.0);
		newVertex.push_back(2.0);
		newVertex.push_back(-1.0);
		newVertex.push_back(2.0);
		newVertex.push_back(-1.0);
		newVertex.push_back(0.0);
		newVertex.push_back(-1.0);

		newVertex.push_back(0.0);
		newVertex.push_back(-1.0);
		newVertex.push_back(-2.0);
		newVertex.push_back(-1.0);
		newVertex.push_back(-2.0);
		newVertex.push_back(-1.0);
		newVertex.push_back(-1.0);
		newVertex.push_back(1.0);

		newVertex.push_back(-1.0);
		newVertex.push_back(1.0);
		newVertex.push_back(0.0);
		newVertex.push_back(1.0);
		newVertex.push_back(0.0);
		newVertex.push_back(1.0);
		newVertex.push_back(1.0);
		newVertex.push_back(1.0);

		newVertex.push_back(1.2);
		newVertex.push_back(0.5);
		newVertex.push_back(2.5);
		newVertex.push_back(1.0);
		newVertex.push_back(2.5);
		newVertex.push_back(1.0);
		newVertex.push_back(1.3);
		newVertex.push_back(-0.4);

		// Scale and shift vertices to appropriate position
		for (int y = 0; y < newVertex.size(); y+=2)
		{
			newVertex[y] = newVertex[y]/2.5;
			newVertex[y+1] = newVertex[y+1]/2.5;
			newVertex[y] -= 0.1;
		}

		// Colour control polygon grey
		vector<float> colours2;
		for (int p = 0; p < 16; p++)
		{
			colours2.push_back(0.5);
			colours2.push_back(0.5);
			colours2.push_back(0.5);
		}

		// Colour blue curve endpoints and red off curve control points
		vector<float> colours3;
		for (int p = 0; p < 12; p++)
		{
			if ((p%3 == 0) || (p%3 == 2))
			{
				colours3.push_back(0.0);
				colours3.push_back(0.0);
				colours3.push_back(1.0);
			}
			else
			{
				colours3.push_back(1.0);
				colours3.push_back(0.0);
				colours3.push_back(0.0);
			}
		}

		InitializeGeometry(&lineGeo, newVertex, colours2);
		InitializeGeometry(&pointGeo, vertices, colours3);
	}
	else if (key == GLFW_KEY_2)
	{
		// Set variables
		choose = 1;
		condition = 1;
		speed = 0;
		fontc = 0;

		// Set vertices of fish
		vector<float> vertices1(40);
		
		vertices1[0] = (1.f/5.5f);
		vertices1[1] = (1.f/5.5f);
		vertices1[2] = (4.f/5.5f);
		vertices1[3] = (0.f/5.5f);
		vertices1[4] = (6.f/5.5f);
		vertices1[5] = (2.f/5.5f);
		vertices1[6] = (9.f/5.5f);
		vertices1[7] = (1.f/5.5f);

		vertices1[8] = (8.f/5.5f);
		vertices1[9] = (2.f/5.5f);
		vertices1[10] = (0.f/5.5f);
		vertices1[11] = (8.f/5.5f);
		vertices1[12] = (0.f/5.5f);
		vertices1[13] = (-2.f/5.5f);
		vertices1[14] = (8.f/5.5f);
		vertices1[15] = (4.f/5.5f);

		vertices1[16] = (5.f/5.5f);
		vertices1[17] = (3.f/5.5f);
		vertices1[18] = (3.f/5.5f);
		vertices1[19] = (2.f/5.5f);
		vertices1[20] = (3.f/5.5f);
		vertices1[21] = (3.f/5.5f);
		vertices1[22] = (5.f/5.5f);
		vertices1[23] = (2.f/5.5f);

		vertices1[24] = (3.f/5.5f);
		vertices1[25] = (2.2f/5.5f);
		vertices1[26] = (3.5f/5.5f);
		vertices1[27] = (2.7f/5.5f);
		vertices1[28] = (3.5f/5.5f);
		vertices1[29] = (3.3f/5.5f);
		vertices1[30] = (3.f/5.5f);
		vertices1[31] = (3.8f/5.5f);

		vertices1[32] = (2.8f/5.5f);
		vertices1[33] = (3.5f/5.5f);
		vertices1[34] = (2.4f/5.5f);
		vertices1[35] = (3.8f/5.5f);
		vertices1[36] = (2.4f/5.5f);
		vertices1[37] = (3.2f/5.5f);
		vertices1[38] = (2.8f/5.5f);
		vertices1[39] = (3.5f/5.5f);

		// Set orange colour for fish
		vector<float> colours1;
		
		for (int j = 0; j< 20; j++)
		{
			colours1.push_back(1.0);
			colours1.push_back(0.5);
			colours1.push_back(0.0);
		}
		
		// Translate image to center of window
		for (int i = 0; i < vertices1.size(); i+=2)
		{
			vertices1[i] -= 9.0/11.0;
			vertices1[i+1] -= 6.0/11.0;
		}

		InitializeGeometry(&geometry, vertices1, colours1);
		
		// Colour control polygon
		vector<float> colours4;
		for (int p = 0; p < 30; p++)
		{
			colours4.push_back(0.5);
			colours4.push_back(0.5);
			colours4.push_back(0.5);
		}

		// Colour blue curve endpoints and red off curve control points
		vector<float> colours5;
		for (int p = 0; p < 20; p++)
		{
			if ((p%4 == 0) || (p%4 == 3))
			{
				colours5.push_back(0.0);
				colours5.push_back(0.0);
				colours5.push_back(1.0);
			}
			else
			{
				colours5.push_back(1.0);
				colours5.push_back(0.0);
				colours5.push_back(0.0);
			}
		}

		// Fill vector with control polygon vertices
		vector<float> newVertex;

		newVertex.push_back(1.0f);
		newVertex.push_back(1.0f);
		newVertex.push_back(4.0f);
		newVertex.push_back(0.0f);
		newVertex.push_back(4.0f);
		newVertex.push_back(0.0f);
		newVertex.push_back(6.0f);
		newVertex.push_back(2.0f);
		newVertex.push_back(6.0f);
		newVertex.push_back(2.0f);
		newVertex.push_back(9.0f);
		newVertex.push_back(1.0f);

		newVertex.push_back(8.0f);
		newVertex.push_back(2.0f);
		newVertex.push_back(0.0f);
		newVertex.push_back(8.0f);
		newVertex.push_back(0.0f);
		newVertex.push_back(8.0f);
		newVertex.push_back(0.0f);
		newVertex.push_back(-2.0f);
		newVertex.push_back(0.0f);
		newVertex.push_back(-2.0f);
		newVertex.push_back(8.0f);
		newVertex.push_back(4.0f);

		newVertex.push_back(5.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(2.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(2.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(3.0f);
		newVertex.push_back(5.0f);
		newVertex.push_back(2.0f);

		newVertex.push_back(3.0f);
		newVertex.push_back(2.2f);
		newVertex.push_back(3.5f);
		newVertex.push_back(2.7f);
		newVertex.push_back(3.5f);
		newVertex.push_back(2.7f);
		newVertex.push_back(3.5f);
		newVertex.push_back(3.3f);
		newVertex.push_back(3.5f);
		newVertex.push_back(3.3f);
		newVertex.push_back(3.0f);
		newVertex.push_back(3.8f);

		newVertex.push_back(2.8f);
		newVertex.push_back(3.5f);
		newVertex.push_back(2.4f);
		newVertex.push_back(3.8f);
		newVertex.push_back(2.4f);
		newVertex.push_back(3.8f);
		newVertex.push_back(2.4f);
		newVertex.push_back(3.2f);
		newVertex.push_back(2.4f);
		newVertex.push_back(3.2f);
		newVertex.push_back(2.8f);
		newVertex.push_back(3.5f);
		
		// Scale control polygon
		for (int y = 0; y < newVertex.size(); y+=2)
		{
			newVertex[y] = newVertex[y]/5.5;
			newVertex[y+1] = newVertex[y+1]/5.5;
		}

		// Translate control polygon
		for (int y = 0; y < newVertex.size(); y+=2)
		{
			newVertex[y] -= 9.0/11.0;
			newVertex[y+1] -= 6.0/11.0;
		}

		InitializeGeometry(&pointGeo, vertices1, colours5);
		InitializeGeometry(&lineGeo, newVertex, colours4);
	}
	else if (key == GLFW_KEY_5)
	{
		// Set variables
		choose = 1;
		condition = 0;
		speed = 0;
		fontc=0;

		vector<float> vertices;
		vector<float> colours;

		float blue = 0.0;
		float green = 1.0;

		// Iterate over characters in string
		string name = "David";
		string::iterator sit;

		// Extract glyph from font file
		GlyphExtractor extractor;
		extractor.LoadFontFile("CPSC453-A3-Fonts/Inconsolata.otf");

		for (sit = name.begin(); sit < name.end(); sit++)
		{
			MyGlyph glyph = extractor.ExtractGlyph(*sit);
			for (int i = 0; i < glyph.contours.size(); i++)
			{
				MyContour contour = glyph.contours[i];
				for (int j = 0; j < contour.size(); j++)
				{
					MySegment segment = contour[j];
					for (int k = 0; k < segment.degree + 1; k++)
					{
						if (segment.degree == 1)
						{
							// Push back vertices twice for straight segments
							for (int s = 0; s < 2; s++)
							{
								vertices.push_back(segment.x[k]/1.5);
								vertices.push_back(segment.y[k]/1.5);

								colours.push_back(0.0);
								colours.push_back(blue);
								colours.push_back(green);
							}
						}
						else
						{
							// Push back vertices
							vertices.push_back(segment.x[k]/1.5);
							vertices.push_back(segment.y[k]/1.5);

							colours.push_back(0.0);
							colours.push_back(blue);
							colours.push_back(green);
						}
					}
				}
			}

			// Alter colour of each character
			blue += 0.2;
			green -= 0.2;

			// Account for spacing between glyphs
			for (int l = 0; l < vertices.size(); l +=2)
			{
				vertices[l] -= glyph.advance/1.5;
			}
		}

		// Position vertices to fit nicely in window
		for (int m = 0; m < vertices.size(); m +=2)
		{
			vertices[m] += .85;
			vertices[m+1] -= 0.1;
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if (key == GLFW_KEY_3)
	{
		// Set variables
		choose = 0;
		condition = 0;
		speed = 0;
		fontc=0;

		vector<float> vertices;
		vector<float> colours;

		float green = 0.5;
		float blue = 0;

		// Set string to iterate over
		string name = "David";
		string::iterator sit;

		// Extract glyph from font file
		GlyphExtractor extractor;
		extractor.LoadFontFile("CPSC453-A3-Fonts/Lora-Regular.ttf");

		for (sit = name.begin(); sit < name.end(); sit++)
		{
			MyGlyph glyph = extractor.ExtractGlyph(*sit);
			for (int i = 0; i < glyph.contours.size(); i++)
			{
				MyContour contour = glyph.contours[i];
				for (int j = 0; j < contour.size(); j++)
				{
					MySegment segment = contour[j];
					int count = 0;

					for (int k = 0; k < segment.degree + 1; k++)
					{
						if (segment.degree == 1)
						{
							if (count == 0)
							{
								for (int s = 0; s < 2; s++)
								{
									// Push back vertices twice (for straight segments)
									vertices.push_back(segment.x[k]/1.5);
									vertices.push_back(segment.y[k]/1.5);
										
									colours.push_back(1.0);
									colours.push_back(green);
									colours.push_back(blue);
								}
							}
							else
							{
								// Push back vertices (for straight segments)
								vertices.push_back(segment.x[k]/1.5);
								vertices.push_back(segment.y[k]/1.5);

								colours.push_back(1.0);
								colours.push_back(green);
								colours.push_back(blue);
							}
							count += 1;
						}
						else
						{
							// Push back vertices
							vertices.push_back(segment.x[k]/1.5);
							vertices.push_back(segment.y[k]/1.5);

							colours.push_back(1.0);
							colours.push_back(green);
							colours.push_back(blue);
						}
					}
				}
			}

			// Change colour of each character
			green -= 0.1;
			blue += 0.2;

			// Account for spacing
			for (int l = 0; l < vertices.size(); l +=2)
			{
				vertices[l] -= glyph.advance/1.5;
			}
		}

		// Position vertices to fit in window
		for (int m = 0; m < vertices.size(); m +=2)
		{
			vertices[m] += .85;
			vertices[m+1] -= 0.1;
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if (key == GLFW_KEY_4)
	{
		// Set variables
		choose = 1;
		condition = 0;
		speed = 0;
		fontc=0;

		vector<float> vertices;
		vector<float> colours;
		float green = 0.0;
		
		// Set name to iterate over characters
		string name = "David";
		string::iterator sit;

		// Set font
		GlyphExtractor extractor;
		extractor.LoadFontFile("CPSC453-A3-Fonts/SourceSansPro-Semibold.otf");

		for (sit = name.begin(); sit < name.end(); sit++)
		{
			MyGlyph glyph = extractor.ExtractGlyph(*sit);
			for (int i = 0; i < glyph.contours.size(); i++)
			{
				MyContour contour = glyph.contours[i];
				for (int j = 0; j < contour.size(); j++)
				{
					MySegment segment = contour[j];
					for (int k = 0; k < segment.degree + 1; k++)
					{
						if (segment.degree == 1)
						{
							for (int s = 0; s < 2; s++)
							{
								// Push back font twice (straight segments)
								vertices.push_back(segment.x[k]/1.5);
								vertices.push_back(segment.y[k]/1.5);
								
								colours.push_back(1.0);
								colours.push_back(green);
								colours.push_back(0.0);
							}
						}
						else
						{
							// Push back vertices
							vertices.push_back(segment.x[k]/1.5);
							vertices.push_back(segment.y[k]/1.5);

							colours.push_back(1.0);
							colours.push_back(green);
							colours.push_back(0.0);
						}
					}
				}
			}

			// Change colour of character
			green += 0.2;

			// Account for spacing of characters
			for (int l = 0; l < vertices.size(); l +=2)
			{
				vertices[l] -= glyph.advance/1.5;
			}
		}

		// Move vertices to fit within window
		for (int m = 0; m < vertices.size(); m +=2)
		{
			vertices[m] += .85;
			vertices[m+1] -= 0.1;
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if (key == GLFW_KEY_6)
	{
		// Set variables
		choose = 0;
		condition = 0;
		speed = 1;
		moveRate = 0.1;
		float advanced = 0.0;
		fontc = 1;

		vector<float> vertices;
		vector<float> colours;

		float green = 0.5;
		float blue = 0;

		// String of text to be animated
		string name = "The quick brown fox jumps over the lazy dog.";
		string::iterator sit;

		// Extract font
		GlyphExtractor extractor;
		extractor.LoadFontFile("CPSC453-A3-Fonts/AlexBrush-Regular.ttf");

		// Similar to above loops used to extract characters
		for (sit = name.begin(); sit < name.end(); sit++)
		{
			MyGlyph glyph = extractor.ExtractGlyph(*sit);
			for (int i = 0; i < glyph.contours.size(); i++)
			{
				MyContour contour = glyph.contours[i];
				for (int j = 0; j < contour.size(); j++)
				{
					MySegment segment = contour[j];
					int count = 0;

					for (int k = 0; k < segment.degree + 1; k++)
					{
						if (segment.degree == 1)
						{
							if (count == 0)
							{
								for (int s = 0; s < 2; s++)
								{
									vertices.push_back(segment.x[k]/1.5 + advanced);
									vertices.push_back(segment.y[k]/1.5);
										
									colours.push_back(1.0);
									colours.push_back(0.0);
									colours.push_back(0.0);
								}
							}
							else
							{
								vertices.push_back(segment.x[k]/1.5 + advanced);
								vertices.push_back(segment.y[k]/1.5);

								colours.push_back(1.0);
								colours.push_back(0.0);
								colours.push_back(0.0);
							}
							count += 1;
						}
						else
						{
							vertices.push_back(segment.x[k]/1.5 + advanced);
							vertices.push_back(segment.y[k]/1.5);

							colours.push_back(1.0);
							colours.push_back(0.0);
							colours.push_back(0.0);
						}
					}
				}
			}

			// Increase spacing after each character
			advanced += glyph.advance/1.5;
			
		}

		// Shift to set position to start animation
		for (int m = 0; m < vertices.size(); m +=2)
		{
			vertices[m] += 1.0;
			vertices[m+1] -= 0.1;
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if (key == GLFW_KEY_7)
	{
		// Set variables
		choose = 1;
		condition = 0;
		speed = 1;
		moveRate= 0.1;
		float advanced = 0.0;
		fontc=2;

		vector<float> vertices;
		vector<float> colours;

		float green = 0.5;
		float blue = 0;

		// Set string to animate over
		string name = "The quick brown fox jumps over the lazy dog.";
		string::iterator sit;

		// Extract glyph
		GlyphExtractor extractor;
		extractor.LoadFontFile("CPSC453-A3-Fonts/Inconsolata.otf");

		for (sit = name.begin(); sit < name.end(); sit++)
		{
			MyGlyph glyph = extractor.ExtractGlyph(*sit);
			for (int i = 0; i < glyph.contours.size(); i++)
			{
				MyContour contour = glyph.contours[i];
				for (int j = 0; j < contour.size(); j++)
				{
					MySegment segment = contour[j];
					for (int k = 0; k < segment.degree + 1; k++)
					{
						if (segment.degree == 1)
						{
							for (int s = 0; s < 2; s++)
							{
								vertices.push_back(segment.x[k]/1.5 + advanced);
								vertices.push_back(segment.y[k]/1.5);
								
								colours.push_back(0.0);
								colours.push_back(1.0);
								colours.push_back(0.0);
							}
						}
						else
						{
							vertices.push_back(segment.x[k]/1.5 + advanced);
							vertices.push_back(segment.y[k]/1.5);

							colours.push_back(0.0);
							colours.push_back(1.0);
							colours.push_back(0.0);
						}
					}
				}
			}
			// Set spacing between characters
			advanced += glyph.advance/1.5;
			
		}

		// Set vertices to starting position to animate
		for (int m = 0; m < vertices.size(); m +=2)
		{
			vertices[m] += 1.0;
			vertices[m+1] -= 0.1;
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if (key == GLFW_KEY_8)
	{
		// Set variables
		choose = 1;
		condition = 0;
		speed = 1;
		moveRate= 0.1;
		float advanced = 0.0;
		fontc=3;

		vector<float> vertices;
		vector<float> colours;

		float green = 0.5;
		float blue = 0;

		// Set string to animate
		string name = "The quick brown fox jumps over the lazy dog.";
		string::iterator sit;

		// Extract glyph
		GlyphExtractor extractor;
		extractor.LoadFontFile("CPSC453-A3-Fonts/SourceSansPro-Semibold.otf");

		for (sit = name.begin(); sit < name.end(); sit++)
		{
			MyGlyph glyph = extractor.ExtractGlyph(*sit);
			for (int i = 0; i < glyph.contours.size(); i++)
			{
				MyContour contour = glyph.contours[i];
				for (int j = 0; j < contour.size(); j++)
				{
					MySegment segment = contour[j];
					for (int k = 0; k < segment.degree + 1; k++)
					{
						if (segment.degree == 1)
						{
							for (int s = 0; s < 2; s++)
							{
								vertices.push_back(segment.x[k]/1.5 + advanced);
								vertices.push_back(segment.y[k]/1.5);
								
								colours.push_back(0.0);
								colours.push_back(0.0);
								colours.push_back(1.0);
							}
						}
						else
						{
							vertices.push_back(segment.x[k]/1.5 + advanced);
							vertices.push_back(segment.y[k]/1.5);

							colours.push_back(0.0);
							colours.push_back(0.0);
							colours.push_back(1.0);
						}
					}
				}
			}

			// Set spacing between characters
			advanced += glyph.advance/1.5;
			
		}

		// Set vertices to starting position to animate
		for (int m = 0; m < vertices.size(); m +=2)
		{
			vertices[m] += 1.0;
			vertices[m+1] -= 0.1;
		}

		DestroyGeometry(&geometry);
		InitializeGeometry(&geometry, vertices, colours);
	}
	else if (key == GLFW_KEY_A)
	{
		// Change the speed to slowest speed
		if (fontc != 0)
		{
			speed = 1;
		}
	}
	else if (key == GLFW_KEY_S)
	{
		// Change the speed to medium speed
		if (fontc != 0)
		{
			speed = 2;
		}
	}
	else if (key == GLFW_KEY_D)
	{
		// Change the speed to fastest speed
		if (fontc != 0)
		{
			speed = 3;
		}
	}
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// Set variables
	choose = 0;
	condition = 1;
	speed = 0;
	fontc = 0;

	// Set vertices scaled to fit window for teapot
	vector<float> vertices(24);

	vertices[0] = (1.f/2.5f);
	vertices[1] = (1.f/2.5f);
	vertices[2] = (2.f/2.5f);
	vertices[3] = (-1.f/2.5f);
	vertices[4] = (0.f/2.5f);
	vertices[5] = (-1.f/2.5f);
		
	vertices[6] = (0.f/2.5f);
	vertices[7] = (-1.f/2.5f);
	vertices[8] = (-2.f/2.5f);
	vertices[9] = (-1.f/2.5f);
	vertices[10] = (-1.f/2.5f);
	vertices[11] = (1.f/2.5f);
		
	vertices[12] = (-1.f/2.5f);
	vertices[13] = (1.f/2.5f);
	vertices[14] = (0.f/2.5f);
	vertices[15] = (1.f/2.5f);
	vertices[16] = (1.f/2.5f);
	vertices[17] = (1.f/2.5f);
		
	vertices[18] = (1.2f/2.5f);
	vertices[19] = (0.5f/2.5f);
	vertices[20] = (2.5f/2.5f);
	vertices[21] = (1.f/2.5f);
	vertices[22] = (1.3f/2.5f);
	vertices[23] = (-0.4f/2.5f);

	// Set purple colour for teapot
	vector<float> colours;

	for (int p = 0; p < 12; p++)
	{
		colours.push_back(1.0);
		colours.push_back(0.0);
		colours.push_back(1.0);
	}

	// Shift vertices to fit window
	for (int z = 0; z < vertices.size(); z+=2)
	{
		vertices[z] -= 0.1;
	}

	// Set control polygon
	vector<float> newVertex;
		
	newVertex.push_back(1.0);
	newVertex.push_back(1.0);
	newVertex.push_back(2.0);
	newVertex.push_back(-1.0);
	newVertex.push_back(2.0);
	newVertex.push_back(-1.0);
	newVertex.push_back(0.0);
	newVertex.push_back(-1.0);

	newVertex.push_back(0.0);
	newVertex.push_back(-1.0);
	newVertex.push_back(-2.0);
	newVertex.push_back(-1.0);
	newVertex.push_back(-2.0);
	newVertex.push_back(-1.0);
	newVertex.push_back(-1.0);
	newVertex.push_back(1.0);

	newVertex.push_back(-1.0);
	newVertex.push_back(1.0);
	newVertex.push_back(0.0);
	newVertex.push_back(1.0);
	newVertex.push_back(0.0);
	newVertex.push_back(1.0);
	newVertex.push_back(1.0);
	newVertex.push_back(1.0);

	newVertex.push_back(1.2);
	newVertex.push_back(0.5);
	newVertex.push_back(2.5);
	newVertex.push_back(1.0);
	newVertex.push_back(2.5);
	newVertex.push_back(1.0);
	newVertex.push_back(1.3);
	newVertex.push_back(-0.4);

	// Scale and shift control polygon
	for (int y = 0; y < newVertex.size(); y+=2)
	{
		newVertex[y] = newVertex[y]/2.5;
		newVertex[y+1] = newVertex[y+1]/2.5;
		newVertex[y] -= 0.1;
	}

	// Set colour of control polygon
	vector<float> colours2;
	for (int p = 0; p < 16; p++)
	{
		colours2.push_back(0.5);
		colours2.push_back(0.5);
		colours2.push_back(0.5);
	}

	// Set colour of blue curve endpoints and red off curve control points
	vector<float> colours3;
	for (int p = 0; p < 12; p++)
	{
		if ((p%3 == 0) || (p%3 == 2))
		{
			colours3.push_back(0.0);
			colours3.push_back(0.0);
			colours3.push_back(1.0);
		}
		else
		{
			colours3.push_back(1.0);
			colours3.push_back(0.0);
			colours3.push_back(0.0);
		}
	}


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
	glfwMakeContextCurrent(window);
	
	//Intialize GLAD if not lab linux
	#ifndef LAB_LINUX
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}
	#endif
	
	// query and print out information about our OpenGL environment
	QueryGLVersion();
	glPointSize(5.f);
	// call function to load and compile shader programs
	MyShader shader;
	if (!InitializeShaders(&shader)) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// Initialize shader for points and lines
	MyShader2 shader2;
	if (!InitializeShaders2(&shader2))
	{
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// call function to create and fill buffers with geometry data
	if (!InitializeGeometry(&geometry, vertices, colours))
		cout << "Program failed to intialize geometry!" << endl;

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// Initialize geometry for lines and points
	InitializeGeometry(&lineGeo, newVertex, colours2);
	InitializeGeometry(&pointGeo, vertices, colours3);

	// Change between quadratic and cubic bezier
	if (choose == 0)
	{
		glPatchParameteri(GL_PATCH_VERTICES, 3);
	}
	else if (choose == 1)
	{
		glPatchParameteri(GL_PATCH_VERTICES, 4);
	}

	// Send uniform information
	glUseProgram(shader.program);
	GLint vertexUniform = glGetUniformLocation(shader.program, "selection");
	glUniform1i(vertexUniform, choose);

	glUseProgram(shader.program);
	GLint speedUniform = glGetUniformLocation(shader.program, "scrollrate");
	glUniform1i(speedUniform, speed);

	glUseProgram(shader.program);
	GLint moveUniform = glGetUniformLocation(shader.program, "movement");
	glUniform1f(moveUniform, moveRate);

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		// Change between quadratic and cubic bezier
		if (choose == 0)
		{
			glPatchParameteri(GL_PATCH_VERTICES, 3);
		}
		else if (choose == 1)
		{
			glPatchParameteri(GL_PATCH_VERTICES, 4);
		}
		glUseProgram(shader.program);
		glUniform1i(vertexUniform, choose);

		glUseProgram(shader.program);
		glUniform1i(speedUniform, speed);

		glUseProgram(shader.program);
		glUniform1f(moveUniform, moveRate);

		// RenderScene for curves or lines and points
		if (condition== 0)
		{
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			RenderScene(&geometry, &shader);
		}
		if (condition == 1)
		{
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			RenderScene(&geometry, &shader);
			RenderScene(&lineGeo, &shader2, 1);
			RenderScene(&pointGeo, &shader2, 2);
		}
			
		// Change the speed depending on which font is currently used
		if (fontc == 1)
		{
			// Move characters at specified speed
			if (speed == 1 && moveRate < 12)
			{
				moveRate += 0.01;	
			}
			else if (speed == 2 && moveRate < 12)
			{
				moveRate += 0.02;	
			}
			else if (speed == 3 && moveRate < 12)
			{
				moveRate += 0.03;	
			}
			// Reset to start animation over
			else
			{
				moveRate = 0.0;
			}
		}	
		if (fontc == 2)
		{
			// Move characters at specified speed
			if (speed == 1 && moveRate < 17)
			{
				moveRate += 0.01;	
			}
			else if (speed == 2 && moveRate < 17)
			{
				moveRate += 0.02;	
			}
			else if (speed == 3 && moveRate < 17)
			{
				moveRate += 0.03;	
			}
			// Reset to start animation over
			else
			{
				moveRate = 0.0;
			}
		}	
		if (fontc == 3)
		{
			// Move characters at specified speed
			if (speed == 1 && moveRate < 16)
			{
				moveRate += 0.01;	
			}
			else if (speed == 2 && moveRate < 16)
			{
				moveRate += 0.02;	
			}
			else if (speed == 3 && moveRate < 16)
			{
				moveRate += 0.03;	
			}
			// Reset to start animation over
			else
			{
				moveRate = 0.0;
			}
		}				
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	DestroyGeometry(&lineGeo);
	DestroyGeometry(&pointGeo);
	DestroyShaders(&shader);
	DestroyShaders2(&shader2);
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
GLuint LinkProgram(GLuint vertexShader, GLuint TCSshader, GLuint TESshader, GLuint fragmentShader) 
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (TCSshader) glAttachShader(programObject, TCSshader); 
	if (TESshader) glAttachShader(programObject, TESshader); 
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

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// LinkProgram for lines and points
GLuint LinkProgram2(GLuint vertexShader, GLuint fragmentShader)
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