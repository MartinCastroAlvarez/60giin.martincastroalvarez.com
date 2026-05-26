#version 410 core

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPos;
in vec4 fragPosLightSpace;
in vec3 normal;
in vec2 uv;
in vec3 gouraudColor;

out vec4 FragColor;

uniform Light lightPhongBlinn;
uniform Light lightFaceted;
uniform vec3 objectColor;

uniform sampler2D shadowMapPhongBlinn;
uniform float shadowIntensityPhongBlinn;

float ShadowCalculation(vec4 fragPosLS, vec3 lightDir, vec3 normalVec)
{
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMapPhongBlinn, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normalVec, lightDir)), 0.001);
    
    float shadowVal = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapPhongBlinn, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapPhongBlinn, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadowVal += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadowVal /= 9.0;
    
    if(projCoords.z > 1.0)
        shadowVal = 0.0;
        
    return shadowVal;
}

void main() {
    // 1. PhongBlinn
    vec3 normalVector = normalize(normal);
    vec3 lightDirection = normalize(lightPhongBlinn.position - fragPos);
    float distance = length(lightPhongBlinn.position - fragPos);
    float attenuation = 1.0 / (lightPhongBlinn.constant + lightPhongBlinn.linear * distance + lightPhongBlinn.quadratic * (distance * distance));

    vec3 viewDirection = normalize(-fragPos);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    
    vec3 ambient = lightPhongBlinn.ambient * objectColor;
    float diff = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuse = lightPhongBlinn.diffuse * diff * objectColor;
    float spec = pow(max(dot(normalVector, halfwayDirection), 0.0), 32);
    vec3 specular = lightPhongBlinn.specular * spec;

    float shadowVal = ShadowCalculation(fragPosLightSpace, lightDirection, normalVector);
    vec3 phongBlinnColor = (ambient + (1.0 - shadowVal * shadowIntensityPhongBlinn) * (diffuse + specular)) * attenuation;

    // 2. Faceted
    vec3 xTangent = dFdx(fragPos);
    vec3 yTangent = dFdy(fragPos);
    vec3 normalFaceted = normalize(cross(xTangent, yTangent));
    
    vec3 lightDirFaceted = normalize(lightFaceted.position - fragPos);
    float distanceFaceted = length(lightFaceted.position - fragPos);
    float attenuationFaceted = 1.0 / (lightFaceted.constant + lightFaceted.linear * distanceFaceted + lightFaceted.quadratic * (distanceFaceted * distanceFaceted));

    vec3 halfwayDirFaceted = normalize(lightDirFaceted + viewDirection);
    
    vec3 ambientFaceted = lightFaceted.ambient * objectColor;
    float diffFaceted = max(dot(normalFaceted, lightDirFaceted), 0.0);
    vec3 diffuseFaceted = lightFaceted.diffuse * diffFaceted * objectColor;
    float specFaceted = pow(max(dot(normalFaceted, halfwayDirFaceted), 0.0), 32);
    vec3 specularFaceted = lightFaceted.specular * specFaceted;

    vec3 facetedColor = (ambientFaceted + diffuseFaceted + specularFaceted) * attenuationFaceted;

    // 3. Combine
    vec3 finalColor = phongBlinnColor + gouraudColor + facetedColor;
    FragColor = vec4(finalColor, 1.0);
}
