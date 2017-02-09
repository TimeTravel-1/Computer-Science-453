// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 textureCoords;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2DRect tex;
uniform int selection;

void main(void)
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
 	// Set up variables
 	vec2 newCoords;
 	float luminance;

 	// Set colour
 	vec4 colour = texture(tex, textureCoords);
	FragmentColour = colour;

 	if (selection == 1)
 	{
 		// Apply first greyscale effect
 		luminance = colour.r*0.333 + colour.g*0.333 + colour.b*0.333;
 		FragmentColour = vec4(luminance, luminance, luminance, 1.0);
 	}
 	else if (selection == 2)
 	{
 		// Apply second greyscale effect
 		luminance = colour.r*0.299 + colour.g*0.587 + colour.b*0.114;
 		FragmentColour = vec4(luminance, luminance, luminance, 1.0);
 	}
 	else if (selection == 3)
 	{
 		// Apply third greyscale effect
 		// This one looks the most "correct" to me. I think different formulas exist because there may be different applications of the greyscale effects
 		// for different purposes, and different choices of pure red, green, or blue that are chosen that correspond to the different conversions. 
 		luminance = colour.r*0.213 + colour.g*0.715 + colour.b*0.072;
 		FragmentColour = vec4(luminance, luminance, luminance, 1.0);
 	}
 	else if (selection == 4)
 	{
 		// Apply sepia effect
 		float redC = colour.r*0.393 + colour.g*0.769 + colour.b*0.189;
 		float greenC = colour.r*0.349 + colour.g*0.686 + colour.b*0.168;
 		float blueC = colour.r*0.272 + colour.g*0.534 + colour.b*0.131;
 		FragmentColour = vec4(redC, greenC, blueC, 1.0);
 	}
 	else if (selection == 5)
 	{
 		// Convert image to greyscale using third conversion method, then apply filter to luminance
 		// Apply horizontal sobel effect
 		newCoords.x = textureCoords.x - 1;
 		newCoords.y = textureCoords.y - 1;

 		luminance = (texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.x += 1;
 		luminance += 2*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.x += 1;
 		luminance += (texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.y += 2;
 		luminance += -1*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.x -= 1;
 		luminance += -2*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.x -= 1;
 		luminance += -1*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		FragmentColour = vec4(luminance, luminance, luminance, 1.0);
	}
	else if (selection == 6)
 	{
 		// Apply vertical sobel effect
 		newCoords.x = textureCoords.x -1;
 		newCoords.y = textureCoords.y-1;

 		luminance = -1*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.y += 1;
 		luminance += -2*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.y += 1;
 		luminance += -1*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.x += 2;
 		luminance += (texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.y -= 1;
 		luminance += 2*(texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		newCoords.y -= 1;
 		luminance += (texture(tex, newCoords).r*.213 + texture(tex, newCoords).g*.715 + texture(tex, newCoords).b*.072);
 		FragmentColour = vec4(luminance, luminance, luminance, 1.0);
	}
	else if (selection == 7)
	{
		// Apply unsharp mask effect
 		newCoords.x = textureCoords.x;
 		newCoords.y = textureCoords.y;

 		colour = 5*texture(tex, newCoords);
 		newCoords.y += 1;
 		colour += -1*texture(tex, newCoords);
 		newCoords.y -= 2;
 		colour += -1*texture(tex, newCoords);
 		newCoords.x += 1;
 		newCoords.y += 1;
 		colour += -1*texture(tex, newCoords);
 		newCoords.x -= 2;
 		colour += -1*texture(tex, newCoords);
 		FragmentColour = colour;
	}
	else if (selection == 8)
	{
		// Apply 3x3 Gaussian blur
		newCoords.x = textureCoords.x-1;
 		newCoords.y = textureCoords.y-1;

 		colour = .04*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .12*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .04*texture(tex, newCoords);
 		newCoords.y += 1;
 		colour += .12*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .36*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .12*texture(tex, newCoords);
 		newCoords.y += 1;
 		colour += .04*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .12*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .04*texture(tex, newCoords);

 		FragmentColour = colour;
	}
	else if (selection == 9)
	{
		// Apply 5x5 Gaussian blur
		newCoords.x = textureCoords.x-2;
 		newCoords.y = textureCoords.y-2;

 		colour = .0036*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0144*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .024*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0144*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0036*texture(tex, newCoords);
 		newCoords.y += 1;
 		
 		colour += .0144*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0576*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .096*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0576*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0144*texture(tex, newCoords);
 		newCoords.y += 1;
 		
 		colour += .024*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .096*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .16*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .096*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .024*texture(tex, newCoords);
 		newCoords.y += 1;

 		colour += .0144*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0576*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .096*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0576*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0144*texture(tex, newCoords);
 		newCoords.y += 1;
 		
 		colour += .0036*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0144*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .024*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0144*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0036*texture(tex, newCoords);
 	
 		FragmentColour = colour;
	}
	else if (selection == 10)
	{
		// Apply 7x7 Gaussian blur
		newCoords.x = textureCoords.x-3;
 		newCoords.y = textureCoords.y-3;

 		colour = .0009*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0033*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0066*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0084*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0066*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0033*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0009*texture(tex, newCoords);
 		newCoords.y += 1;
 		
 		colour += .0033*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0121*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0308*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0121*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0033*texture(tex, newCoords);
 		newCoords.y += 1;

 		colour += .0066*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0484*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0616*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0484*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0066*texture(tex, newCoords);
 		newCoords.y += 1;

 		colour += .0084*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0308*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0616*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0784*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0616*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0308*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0084*texture(tex, newCoords);
 		newCoords.y += 1;

 		colour += .0066*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0484*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0616*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0484*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0066*texture(tex, newCoords);
 		newCoords.y += 1;

 		colour += .0033*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0121*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0308*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0242*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0121*texture(tex, newCoords);
 		newCoords.x -= 1;
 		colour += .0033*texture(tex, newCoords);
 		newCoords.y += 1;

 		colour += .0009*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0033*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0066*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0084*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0066*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0033*texture(tex, newCoords);
 		newCoords.x += 1;
 		colour += .0009*texture(tex, newCoords);
 		newCoords.y += 1;
 	
 		FragmentColour = colour;
	}
	else if (selection == 11)
	{
		// Apply 51x51 Gaussian blur
		newCoords.x = textureCoords.x - 25;
 		newCoords.y = textureCoords.y;

 		colour = vec4(0.0,0.0,0.0,0.0);
 		
	 		colour += 0.001969*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.002491*texture(tex, newCoords);
	 		newCoords.x += 1;	
	 		colour +=0.003121	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.003874	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.004763	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.005799	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.006994	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.008353	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.009882	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.011579	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.013438	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.015446	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.017584	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.019827	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.022142	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.024491	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.026831	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.029113	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.031288	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.033303	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.03511	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.03666	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.037914	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.038835	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.039398	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.039588	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.039398	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.038835	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.037914	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.03666	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.03511	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.033303	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.031288	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.029113	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.026831	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.024491	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.022142	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.019827	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.017584	*texture(tex, newCoords);
	 		newCoords.x += 1;
	 		colour +=0.015446	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.013438	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.011579	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.009882	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.008353	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.006994	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.005799	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.004763	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.003874	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.003121	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.002491	*texture(tex, newCoords);
	 		newCoords.x += 1;
			colour +=0.001969*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 20)
	{
		// Apply 51x51 Gaussian blur
		newCoords.x = textureCoords.x;
 		newCoords.y = textureCoords.y-25;
 		colour = vec4(0.0,0.0,0.0,0.0);
 		
	 		colour += 0.001969*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.002491*texture(tex, newCoords);
	 		newCoords.y += 1;	
	 		colour +=0.003121	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.003874	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.004763	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.005799	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.006994	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.008353	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.009882	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.011579	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.013438	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.015446	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.017584	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.019827	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.022142	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.024491	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.026831	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.029113	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.031288	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.033303	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.03511	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.03666	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.037914	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.038835	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.039398	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.039588	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.039398	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.038835	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.037914	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.03666	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.03511	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.033303	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.031288	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.029113	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.026831	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.024491	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.022142	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.019827	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.017584	*texture(tex, newCoords);
	 		newCoords.y += 1;
	 		colour +=0.015446	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.013438	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.011579	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.009882	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.008353	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.006994	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.005799	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.004763	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.003874	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.003121	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.002491	*texture(tex, newCoords);
	 		newCoords.y += 1;
			colour +=0.001969*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 12){
		// Apply 101x101 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x - 50;
 		newCoords.y = textureCoords.y;
 		newCoords2.x = textureCoords.x +50;
 		newCoords2.y = textureCoords.y;

 		float sigma = 1.0/5.0*101.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 50; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.x += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.x -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 21){
		// Apply 101x101 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x;
 		newCoords.y = textureCoords.y-50;
 		newCoords2.x = textureCoords.x;
 		newCoords2.y = textureCoords.y+50;

 		float sigma = 1.0/5.0*101.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 50; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.y += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.y -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 13){
		// Apply 201x201 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x - 100;
 		newCoords.y = textureCoords.y;
 		newCoords2.x = textureCoords.x +100;
 		newCoords2.y = textureCoords.y;

 		float sigma = 1.0/5.0*201.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 100; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.x += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.x -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 22){
		// Apply 201x201 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x ;
 		newCoords.y = textureCoords.y- 100;
 		newCoords2.x = textureCoords.x ;
 		newCoords2.y = textureCoords.y+100;

 		float sigma = 1.0/5.0*201.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 100; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.y += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.y -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 14){
		// Apply 301x301 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x - 150;
 		newCoords.y = textureCoords.y;
 		newCoords2.x = textureCoords.x +150;
 		newCoords2.y = textureCoords.y;

 		float sigma = 1.0/5.0*301.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 150; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.x += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.x -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 23){
		// Apply 301x301 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x ;
 		newCoords.y = textureCoords.y- 150;
 		newCoords2.x = textureCoords.x ;
 		newCoords2.y = textureCoords.y+150;

 		float sigma = 1.0/5.0*301.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 150; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.y += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.y -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 15){
		// Apply 401x401 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x - 200;
 		newCoords.y = textureCoords.y;
 		newCoords2.x = textureCoords.x +200;
 		newCoords2.y = textureCoords.y;

 		float sigma = 1.0/5.0*401.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 200; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.x += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.x -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	else if (selection == 24){
		// Apply 401x401 Gaussian blur
		vec2 newCoords2;
		newCoords.x = textureCoords.x ;
 		newCoords.y = textureCoords.y- 200;
 		newCoords2.x = textureCoords.x ;
 		newCoords2.y = textureCoords.y+200;

 		float sigma = 1.0/5.0*401.0;
 		float g = 1.0/sqrt(2*3.14159265359*pow(sigma, 2.0));
 		colour = vec4(0.0,0.0,0.0,0.0);
		
		float counter = 0.0;

		for (int i = 200; i>0; i--)
	 	{
	 			float power = -1.0*pow(i, 2.0)/(2*pow(sigma, 2.0));
	 			float normalVal = g*pow(2.7182818284590452353602874713526,power);

	 			colour += normalVal*texture(tex, newCoords);
	 			newCoords.y += 1;

	 			colour += normalVal*texture(tex, newCoords2);
	 			newCoords2.y -= 1;

	 			counter += 2*normalVal;
	 	}

	 	//For 0
	 	float normalZero = 1.0-counter;
	 	colour += normalZero*texture(tex, newCoords);

	 	FragmentColour = colour;
	}
	
}
