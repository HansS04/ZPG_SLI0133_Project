#version 330 core
out vec4 FragColor;

in vec3 Normal;    
in vec3 FragPos;   
in vec2 TexCoords;

uniform vec3 u_ViewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material u_Material;

uniform bool u_IsUnlit; 

uniform sampler2D u_DiffuseTexture;
uniform bool u_HasDiffuseTexture;

struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform vec3 u_AmbientLight;

const int MAX_DIR_LIGHTS = 2;
uniform DirLight u_DirLights[MAX_DIR_LIGHTS];
uniform int u_DirLightCount;

const int MAX_POINT_LIGHTS = 8;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

const int MAX_SPOT_LIGHTS = 4;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
uniform int u_SpotLightCount;

uniform SpotLight u_Flashlight;
uniform bool u_FlashlightOn;

vec4 CalculateLightBase(vec3 lightColor, vec3 lightDir, vec3 norm, vec3 viewDir, vec3 albedo) {
    float diff_intensity = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(lightColor * diff_intensity * albedo, 1.0);

    vec3 halfDir = normalize(lightDir + viewDir);
    float spec_intensity = pow(max(dot(norm, halfDir), 0.0), u_Material.shininess);
    vec4 specular = vec4(lightColor * spec_intensity * u_Material.specular, 1.0);
    
    return diffuse + specular;
}

vec4 CalculateDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(-light.direction);
    return CalculateLightBase(light.color, lightDir, norm, viewDir, albedo);
}

vec4 CalculatePointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec4 result = CalculateLightBase(light.color, lightDir, norm, viewDir, albedo);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    return result * attenuation;
}

vec4 CalculateSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    if(intensity > 0.0) {
        vec4 result = CalculateLightBase(light.color, lightDir, norm, viewDir, albedo);
        
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        
        return result * attenuation * intensity;
    }
    return vec4(0.0);
}

void main() {
    vec3 albedo = u_Material.diffuse;
    if (u_HasDiffuseTexture) {
        albedo *= texture(u_DiffuseTexture, TexCoords).rgb;
    }

    if (u_IsUnlit) {
        FragColor = vec4(albedo, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);

    vec4 result = vec4(u_AmbientLight * u_Material.ambient * albedo, 1.0);
    
    for (int i = 0; i < u_DirLightCount; i++) {
        result += CalculateDirLight(u_DirLights[i], norm, viewDir, albedo);
    }
    for (int i = 0; i < u_PointLightCount; i++) {
        result += CalculatePointLight(u_PointLights[i], norm, FragPos, viewDir, albedo);
    }
    for (int i = 0; i < u_SpotLightCount; i++) {
        result += CalculateSpotLight(u_SpotLights[i], norm, FragPos, viewDir, albedo);
    }
    if (u_FlashlightOn) {
        result += CalculateSpotLight(u_Flashlight, norm, FragPos, viewDir, albedo);
    }

    FragColor = result;
}