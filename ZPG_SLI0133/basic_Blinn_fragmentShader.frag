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

const float AMBIENT_STRENGTH = 0.1;
const float SPECULAR_SHININESS = 32.0; 

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 lightDir = normalize(u_PointLight.position - FragPos);
    vec3 lightColor = u_PointLight.color;

    float distance = length(u_PointLight.position - FragPos);
    float attenuation = 1.0 / (u_PointLight.constant + u_PointLight.linear * distance + u_PointLight.quadratic * (distance * distance));

    vec4 ambient = vec4(AMBIENT_STRENGTH * lightColor * u_Color.rgb, 1.0);
    
    float diff_intensity = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(lightColor * diff_intensity * u_Color.rgb, 1.0);
    diffuse *= attenuation;

    
    vec3 halfDir = normalize(lightDir + viewDir);

    float spec_intensity = pow(max(dot(norm, halfDir), 0.0), SPECULAR_SHININESS);

    vec4 specular = vec4(lightColor * spec_intensity * vec3(1.0, 1.0, 1.0), 1.0);
    specular *= attenuation;

    FragColor = ambient + diffuse + specular;
}