#version 330 core
out vec4 FragColor;

in vec3 Normal;    
in vec3 FragPos;   

uniform vec4 u_Color;
uniform vec3 u_ViewPos;

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

// --- NOVÉ UNIFORMY PRO SVÌTLA ---
uniform vec3 u_AmbientLight;
uniform DirLight u_DirLight;
uniform bool u_DirLightOn;

const int MAX_POINT_LIGHTS = 8;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

uniform SpotLight u_Flashlight;
uniform bool u_FlashlightOn;
// ---

const float SPECULAR_SHININESS = 32.0; 

// --- POMOCNÉ FUNKCE ---

// Spoleèný výpoèet (difuzní + spekulární)
vec4 CalculateLightBase(vec3 lightColor, vec3 lightDir, vec3 norm, vec3 viewDir) {
    // Difuzní
    float diff_intensity = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(lightColor * diff_intensity * u_Color.rgb, 1.0);

    // Spekulární
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec_intensity = pow(max(dot(norm, halfDir), 0.0), SPECULAR_SHININESS);
    vec4 specular = vec4(lightColor * spec_intensity * vec3(1.0, 1.0, 1.0), 1.0);
    
    return diffuse + specular;
}

// Výpoèet pro smìrové svìtlo
vec4 CalculateDirLight(DirLight light, vec3 norm, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    return CalculateLightBase(light.color, lightDir, norm, viewDir);
}

// Výpoèet pro bodové svìtlo
vec4 CalculatePointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec4 result = CalculateLightBase(light.color, lightDir, norm, viewDir);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    return result * attenuation;
}

// Výpoèet pro baterku
vec4 CalculateSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    if(intensity > 0.0) {
        vec4 result = CalculateLightBase(light.color, lightDir, norm, viewDir);
        
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        
        return result * attenuation * intensity;
    }
    return vec4(0.0);
}

// --- HLAVNÍ FUNKCE ---
void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);

    // 1. Ambientní svìtlo
    vec4 result = vec4(u_AmbientLight * u_Color.rgb, 1.0);
    
    // 2. Smìrové svìtlo
    if (u_DirLightOn) {
        result += CalculateDirLight(u_DirLight, norm, viewDir);
    }

    // 3. Bodová svìtla (svìtlušky)
    for (int i = 0; i < u_PointLightCount; i++) {
        result += CalculatePointLight(u_PointLights[i], norm, FragPos, viewDir);
    }

    // 4. Baterka
    if (u_FlashlightOn) {
        result += CalculateSpotLight(u_Flashlight, norm, FragPos, viewDir);
    }

    FragColor = result;
}