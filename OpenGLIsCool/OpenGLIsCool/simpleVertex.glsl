#version 420

// Vertices coming INTO the vertex shader
in vec4 vPos;
in vec4 vNormal;
in vec4 vCol;
in vec4 vUVx2;

out vec4 fColour;
out vec4 fNormal;
out vec4 fUVx2;
out vec4 fVertWorldPos;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProj;

uniform sampler2D textureHeightMap;
uniform bool bUseHeightMap;
uniform vec2 textureOffsetVS;
uniform mat4 matTextRotation;

void main()
{
    vec3 positionXYZ = vec3(vPos.xyz);

    if (bUseHeightMap)
    {
        vec4 uvActual = matTextRotation * vec4(vUVx2.st, 0.0f, 0.0f);

        vec4 textureHeight = texture(textureHeightMap, uvActual.st);

        float height = (textureHeight.r + textureHeight.g + textureHeight.b) / 3.0f;
        const float SCALE_RATIO = 5.0f;
        positionXYZ.y += (height * SCALE_RATIO);
    }

    mat4 MVP = matProj * matView * matModel;
    gl_Position = MVP * vec4(positionXYZ, 1.0);

    // remove translation and scaling from the normals
    fNormal = inverse(transpose(matModel)) * vec4(vNormal.xyz, 1.0f);

    fColour = vCol;

    fUVx2 = vUVx2;

    fVertWorldPos = matModel * vec4(positionXYZ, 1.0);
}
