#version 130

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(0.9, 0.85, 0.7);

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.9, 0.85, 0.7);

    vec3 result = (ambient + diffuse);
    gl_FragColor = vec4(result, 1.0);
}
