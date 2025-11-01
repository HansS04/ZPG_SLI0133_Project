#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec4 u_Color;
uniform vec3 u_ViewPos;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight u_PointLight;


void main() {
    vec3 norm = normalize(Normal);
    vec3 lightColor = u_PointLight.color;
    
    vec3 lightDir = u_PointLight.position - FragPos;
    vec3 lightDirNormalized = normalize(lightDir);

    float ambientStrength = 0.1;
    vec4 ambient = vec4(ambientStrength * lightColor * u_Color.rgb, 1.0);

    float diff_intensity = max(dot(norm, lightDirNormalized), 0.0);
    
    vec4 diff = vec4(lightColor * diff_intensity * u_Color.rgb, 1.0); 

    FragColor = ambient + diff;
}