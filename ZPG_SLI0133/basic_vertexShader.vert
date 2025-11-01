#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main() {
    FragPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));
    
    Normal = mat3(transpose(inverse(u_ModelMatrix))) * aNormal;  
    
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(aPos, 1.0);
}