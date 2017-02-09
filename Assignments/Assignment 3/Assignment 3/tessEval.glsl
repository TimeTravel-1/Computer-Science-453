//TES transforms new vertices. Runs once for all output vertices

#version 410
layout(isolines) in; // Controls how tesselator creates new geometry

in vec3 teColour[]; // input colours

out vec3 Colour; // colours to fragment shader
uniform int selection;
uniform int scrollrate;
uniform float movement;

void main()
{
    // gl_TessCoord variable lets us know where we are within the patch.
    // In this case, the primitive mode is isolines, so gl_TessCoord.x is distance along curve.
    // if the primitive mode were triangles, gl_TessCoord would be a barycentric coordinate.
    float u = gl_TessCoord.x;
    vec4 test = vec4(movement, 0, 0, 0);

    // Evaluate quadratic bezier
    if (selection == 0)
    {
        // Coefficients of quadratic bezier
        float b0 = (1.0-u)*(1.0-u);
        float b1 = 2*u*(1.0-u);
        float b2 = u * u;

        // Sum for position
        gl_Position = b0 * gl_in[0].gl_Position +
		  b1 * gl_in[1].gl_Position +
          b2 * gl_in[2].gl_Position;

        // Determine colours for new points
        Colour 	= b0 * teColour[0] + 
	     	  b1 * teColour[1] + 
              b2 * teColour[2];
    }
    // Evaluate cubic bezier
    else if (selection == 1)
    {
        //Coefficients of cubic bezier
        float b0 = (1.0-u)*(1.0-u)*(1.0-u);
        float b1 = 3*u*(1.0-u)*(1.0-u);
        float b2 = 3*u*u*(1.0-u);
        float b3 = u*u*u;

        gl_Position = b0 * gl_in[0].gl_Position +
          b1 * gl_in[1].gl_Position +
          b2 * gl_in[2].gl_Position + 
          b3 * gl_in[3].gl_Position;

        Colour  = b0 * teColour[0] + 
            b1 * teColour[1] + 
            b2 * teColour[2] + 
            b3 * teColour[3];
    }

    // Used for scrolling animation
    if (scrollrate != 0)
    {
        // Evaluate for cubic bezier
        if (selection == 1)
        {
            // Coefficients for cubic bezier
            float b0 = (1.0-u)*(1.0-u)*(1.0-u);
            float b1 = 3*u*(1.0-u)*(1.0-u);
            float b2 = 3*u*u*(1.0-u);
            float b3 = u*u*u;

            // Adjust position with offset required for animation
            gl_Position = (b0 * gl_in[0].gl_Position +
              b1 * gl_in[1].gl_Position +
              b2 * gl_in[2].gl_Position + 
              b3 * gl_in[3].gl_Position) - test ;

            Colour  = b0 * teColour[0] + 
                b1 * teColour[1] + 
                b2 * teColour[2] + 
                b3 * teColour[3];
        }
        else if (selection == 0)
        {
            // Evaluate for quadratic bezier
            float b0 = (1.0-u)*(1.0-u);
            float b1 = 2*u*(1.0-u);
            float b2 = u * u;

            // Adjust position with offset required for animation
            gl_Position = (b0 * gl_in[0].gl_Position +
              b1 * gl_in[1].gl_Position +
              b2 * gl_in[2].gl_Position) - test;

            // Determine colours for new points
            Colour  = b0 * teColour[0] + 
                  b1 * teColour[1] + 
                  b2 * teColour[2];
        }
    }
}



