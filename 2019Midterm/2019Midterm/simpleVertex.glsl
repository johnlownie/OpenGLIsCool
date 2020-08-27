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

void main()
{
    vec3 positionXYZ = vec3(vPos.xyz);
    mat4 MVP = matProj * matView * matModel;
    gl_Position = MVP * vec4(positionXYZ, 1.0);

    // remove translation and scaling from the normals
    fNormal = inverse(transpose(matModel)) * vec4(vNormal.xyz, 1.0f);

    fColour = vCol;
//    fColour.rgb *= 0.001f;
//    fColour.rgb += objectColour.rgb;

    fUVx2 = vUVx2;

    fVertWorldPos = matModel * vec4(positionXYZ, 1.0);
}
