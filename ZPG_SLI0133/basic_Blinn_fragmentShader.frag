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

const int MAX_ADDITIONAL_LIGHTS = 8;
uniform PointLight u_AdditionalLights[MAX_ADDITIONAL_LIGHTS];
uniform int u_AdditionalLightCount;

const float AMBIENT_STRENGTH = 0.1;
const float SPECULAR_SHININESS = 32.0; 

vec4 CalculatePointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff_intensity = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.color * diff_intensity * u_Color.rgb, 1.0);

    vec3 halfDir = normalize(lightDir + viewDir);
    float spec_intensity = pow(max(dot(norm, halfDir), 0.0), SPECULAR_SHININESS);
    vec4 specular = vec4(light.color * spec_intensity * vec3(1.0, 1.0, 1.0), 1.0);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);

    vec4 ambient = vec4(AMBIENT_STRENGTH * u_PointLight.color * u_Color.rgb, 1.0);
    
    vec4 mainLightResult = CalculatePointLight(u_PointLight, norm, FragPos, viewDir);

    vec4 additionalLightsResult = vec4(0.0);
    for (int i = 0; i < u_AdditionalLightCount; i++) {
        additionalLightsResult += CalculatePointLight(u_AdditionalLights[i], norm, FragPos, viewDir);
    }

    FragColor = ambient + mainLightResult + additionalLightsResult;
}