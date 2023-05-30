#version 130  

// time variable for the sin wave
uniform float time;

// outgoing normal (N) and vertex (v) for fragment shader
varying vec3 N;
varying vec3 v;    

void main(void)  
{     
    // apply the model view matrix
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
	// normalize the normals      
    N = normalize(gl_NormalMatrix * gl_Normal);


    // create a sin wave
   	float wave;

	// copy the current vertex location to a temporary vector
	vec4 newPosition = gl_Vertex;

	// compute a sine wave as a function of time
   	wave = 1 + 0.1  *sin(time * 0.01);

	// modulate the position of the vertex based on the wave 
    // smaller in x when bigger in y
    newPosition.x =  gl_Vertex.x * 1/wave;
	newPosition.y =  gl_Vertex.y * wave;
	
	// apply the model view and projection matrices to the wavy position
	// and pass it on in the pipeline
   	gl_Position = gl_ModelViewProjectionMatrix * newPosition;
	
}
