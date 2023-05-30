#version 130
 
// incoming varying 
varying vec3 N;
varying vec3 v;    

void main (void)  
{  

   // vector to the light source
   vec3 L = normalize(gl_LightSource[0].position.xyz - v); 
   
   // normalized v vector  
   vec3 E = normalize(-v);  
   
   // reflection vector
   vec3 R = normalize(-reflect(L,N));  
 
   //calculate Ambient incidence Term  
   vec4 Iambient = gl_FrontLightProduct[0].ambient;    

   // calculate Specular Term
   vec4 Ispecular = gl_FrontLightProduct[0].specular * pow(max( dot(R,E), 0.0), gl_FrontMaterial.shininess);
   // keep it between 0 and 1
   Ispecular = clamp(Ispecular, 0.0, 1.0); 

  
   // some toon colors
   const vec4 orange = vec4(0.7, 0.7, 0.7, 1.0);
   const vec4 black  = vec4(0.3, 0.3, 0.3, 1.0);

   // storage for the base toon color
   vec4 toonColor; 

   // compute the amount of diffuse reflection
   vec3  eyeNormalVec = normalize(gl_NormalMatrix * N);
   vec3  eyeLightVec  = normalize(gl_LightSource[0].position.xyz - v.xyz);
   float diffuseAmount = max(dot(eyeLightVec, eyeNormalVec), 0.0);
	 
   // if the diffuse brightness is above a low level, then it is orange
   if (diffuseAmount > 0.1) 
		toonColor = orange;
   else // otherwise it is red
		toonColor = black;

   // write Total Color to the fragment color based on the base toon color
   //  but also with some amount of specular and ambient 
   gl_FragColor = Iambient + toonColor + 0.6f * Ispecular; 
}
 