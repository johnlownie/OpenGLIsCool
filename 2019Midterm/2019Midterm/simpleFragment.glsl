#version 420

in vec4 fColour;
in vec4 fNormal;
in vec4 fUVx2;
in vec4 fVertWorldPos;

out vec4 outputColour;

struct sLight
{
    vec4 position;
    vec4 diffuse;
    vec4 specular;
    vec4 atten;
    vec4 direction;
    vec4 param1;
    vec4 param2;
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBER_OF_LIGHTS = 1;
uniform sLight theLights[NUMBER_OF_LIGHTS];

uniform vec3 diffuseColour;
uniform vec4 specularColour;

uniform vec4 eyeLocation;

uniform bool hasNoLighting;

vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular);

void main()
{
    if (hasNoLighting)
    {
        outputColour.rgb = diffuseColour.rgb;
        outputColour.a = 1.0f;
    }
    else
    {
        outputColour = calculateLightContrib(diffuseColour, vec3(fNormal.xyz), vec3(fVertWorldPos), vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular)
{
    vec3 norm = normalize(vertexNormal);
    vec4 finalObjectColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    {
        // is the light off?
        if (theLights[i].param2.x == 0.0f)
        {
            continue;
        }
    
        int iLightType = int(theLights[i].param1.x);
        if (iLightType == DIRECTIONAL_LIGHT_TYPE)
        {
            vec3 lightContrib = theLights[i].diffuse.rgb;

            float dotProduct = dot(-theLights[i].direction.xyz, normalize(norm.xyz));
            dotProduct = max(0.0f, dotProduct);
            lightContrib *= dotProduct;

            finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[i].diffuse.rgb * lightContrib);

            return finalObjectColour;
        }

        // Contribution for this light
        vec3 vLightToVertex = theLights[i].position.xyz - vertexWorldPos.xyz;
        float distanceToLight = length(vLightToVertex);
        vec3 lightVector = normalize(vLightToVertex);
        
        float dotProduct = dot(lightVector, vertexNormal.xyz);
        dotProduct = max(0.0f, dotProduct);

        vec3 lightDiffuseContrib = dotProduct * theLights[i].diffuse.rgb;

        // Specular
        vec3 lightSpecularContrib = vec3(0.0f);
        vec3 reflectVector = reflect(-lightVector, normalize(norm.xyz));

        vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

        // To simplify, we are not using the light specular value, just the object's
        float objectSpecularPower = vertexSpecular.w;
        lightSpecularContrib = pow(max(0.0f, dot(eyeVector, reflectVector)), objectSpecularPower) * vertexSpecular.rgb;

        // Attenuation
        float attenuation = 1.0f / (theLights[i].atten.x + theLights[i].atten.y * distanceToLight + theLights[i].atten.z * distanceToLight * distanceToLight);

        lightDiffuseContrib *= attenuation;
        lightSpecularContrib *= attenuation;

        if (iLightType == SPOT_LIGHT_TYPE)
        {
            vec3 vertexToLight = vertexWorldPos.xyz - theLights[i].position.xyz;
            vertexToLight = normalize(vertexToLight);

            float currentLightRayAngle = dot(vertexToLight.xyz, theLights[i].direction.xyz);
            currentLightRayAngle = max(0.0f, currentLightRayAngle);

            float outerConeAngleCos = cos(radians(theLights[i].param1.z));
            float innerConeAngleCos = cos(radians(theLights[i].param1.y));

            if (currentLightRayAngle < outerConeAngleCos)
            {
                lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
                lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
            }
            else if (currentLightRayAngle < innerConeAngleCos)
            {
                float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
                lightDiffuseContrib *= penumbraRatio;
                lightSpecularContrib *= penumbraRatio;
            }
        }

        finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb);
    }

    finalObjectColour.a = 0.0f;

    return finalObjectColour;
}