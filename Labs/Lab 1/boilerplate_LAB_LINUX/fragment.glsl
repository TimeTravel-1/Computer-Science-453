// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
//mouse position
uniform vec2 mousePos;


void main(void)
{
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	// ==========================================================================
	vec2 fragPos = gl_FragCoord.xy/512.0;
	vec2 vToMouse = fragPos - (mousePos/512.0);
	float distToMouse = 1-length(vToMouse);







    // write colour output without modification
    FragmentColour = vec4(Colour, 0);
}
