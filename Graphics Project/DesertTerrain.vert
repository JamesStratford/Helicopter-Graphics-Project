#version 130

varying vec3 N;
varying vec3 v;  

out vec3 Normal;
out vec3 FragPos;


void main()
{
    // apply the model view matrix
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
	// normalize the normals      
    N = normalize(gl_NormalMatrix * gl_Normal);

    FragPos = vec3(gl_ModelViewProjectionMatrix * vec4(v, 1.0));
    Normal = N; 
    gl_Position = vec4(FragPos, 1.0);
}
