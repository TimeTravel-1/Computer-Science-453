// ==========================================================================
// Image File Reading Support Functions
//  - requires Sean Barrett's image loader: https://github.com/nothings/stb
//  - or the Magick++ development libraries: http://www.imagemagick.org
//  - or the FreeImage library: http://freeimage.sourceforge.net
//
// You may use this code (or not) however you see fit for your work.
//
// Set the #defines below to choose the image library you have installed on
// your system, that you will be using for your assignment. Then compile and
// link this source file with your project.
//
// Authors: Sonny Chan, Alex Brown, Lee Ringham
//          University of Calgary
// Date:    January-November 2016
// ==========================================================================

#include <string>
#include <iostream>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include "ImageReader.h"

using namespace std;

// --------------------------------------------------------------------------
// Set these defines to choose which image library to use for loading image
// files from disk. Obviously, you should only set one!

#define USE_STB_IMAGE
// #define USE_IMAGEMAGICK
// #define USE_FREEIMAGE

// --------------------------------------------------------------------------
// stb_image implementation of the InitializeTexture() function
#ifdef USE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool InitializeTexture(MyTexture *texture, const string &imageFileName)
{
    // ask stb_image to organize the data buffer in OpenGL axes convention
    stbi_set_flip_vertically_on_load(true);

    // use stb_image to read the file
    int width, height, numComponents;
    unsigned char *data = stbi_load(imageFileName.c_str(), &width, &height,
                                    &numComponents, 0);

    // return false on image load error
    if (data == 0) return false;

    texture->width = width;
    texture->height = height;

    // create a texture name to associate our image data with
    if (!texture->textureName)
        glGenTextures(1, &texture->textureName);
    glBindTexture(texture->textureType, texture->textureName);

    // allocate the GPU texture memory for an image of the given size and format
    GLuint format = numComponents == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(texture->textureType, 0, format, texture->width, texture->height,
                 0, format, GL_UNSIGNED_BYTE, data);

    // set the texture minification and magnification filters
    glTexParameteri(texture->textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // clean up
    glBindTexture(texture->textureType, 0);
    stbi_image_free(data);

    return true;
}

#endif
// --------------------------------------------------------------------------
// ImageMagick implementation of the InitializeTexture() function
#ifdef USE_IMAGEMAGICK

#include <Magick++.h>

bool InitializeTexture(MyTexture *texture, const string &imageFileName)
{
    Magick::Image myImage;
    
    // try to read the provided image file
    try {
        myImage.read(imageFileName);
    }
    catch (Magick::Error &error) {
        cout << "Magick++ failed to read image " << imageFileName << endl;
        cout << "ERROR: " << error.what() << endl;
        return false;
    }
    
    // store the image width and height into the texture structure
    texture->width = myImage.columns();
    texture->height = myImage.rows();
    
    // create a Magick++ pixel cache from the image for direct access to data
    Magick::Pixels pixelCache(myImage);
    Magick::PixelPacket *pixels;
    pixels = pixelCache.get(0, 0, texture->width, texture->height);
    
    // determine the number of stored bytes per pixel channel in the cache
    GLenum channelDataType;
    switch (sizeof(Magick::Quantum)) {
        case 4:     channelDataType = GL_UNSIGNED_INT;      break;
        case 2:     channelDataType = GL_UNSIGNED_SHORT;    break;
        default:    channelDataType = GL_UNSIGNED_BYTE;
    }
    
    // create a texture name to associate our image data with
    if (!texture->textureName)
        glGenTextures(1, &texture->textureName);
    glBindTexture(texture->textureType, texture->textureName);

    // allocate the GPU texture memory for an image of the given size and format
    glTexImage2D(texture->textureType, 0, GL_RGBA, texture->width, texture->height,
                 0, GL_BGRA, channelDataType, 0);

    // send image pixel data to OpenGL texture memory, reversing the vertical axis
    for (int row = 0; row < texture->height; ++row)
    {
        int offset = (texture->height - row - 1) * texture->width;
        glTexSubImage2D(texture->textureType, 0, 0, row, texture->width, 1,
                        GL_BGRA, channelDataType, pixels + offset);
    }

    // set the texture minification and magnification filters
    glTexParameteri(texture->textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // unbind this texture
    glBindTexture(texture->textureType, 0);
    
    return true;
}

#endif
// --------------------------------------------------------------------------
// FreeImage implementation of the InitializeTexture() function
#ifdef USE_FREEIMAGE

#include <FreeImage.h>

bool InitializeTexture(MyTexture *texture, const string &imageFileName)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP* dib = 0;
    BYTE* bits = 0;

	fif = FreeImage_GetFileType(imageFileName.c_str(), 0);
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(imageFileName.c_str());
	if (fif == FIF_UNKNOWN)
		return false;

	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, imageFileName.c_str());
	if (!dib)
		return false;

	bits = FreeImage_GetBits(dib);
    texture->width = FreeImage_GetWidth(dib);
    texture->height = FreeImage_GetHeight(dib);
    if ((bits == 0) || (texture->width == 0) || (texture->height == 0))
		return false;

    glGenTextures(1, &texture->textureName);
    glBindTexture(texture->textureType, texture->textureName);
    glTexImage2D(texture->textureType, 0, GL_RGB, texture->width, texture->height,
		0, GL_BGR, GL_UNSIGNED_BYTE, bits);

    glTexParameteri(texture->textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(texture->textureType, 0);

	FreeImage_Unload(dib);

	return true;
}

#endif
// --------------------------------------------------------------------------
