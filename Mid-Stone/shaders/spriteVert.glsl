#version 450
#extension GL_ARB_separate_shader_objects: enable


layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 uvCoords;



layout (location = 0) uniform mat4 projectionMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 modelMatrix;
out vec2 TexCoord;

uniform vec2 uvScale;  // (cellW, cellH)
uniform vec2 uvOffset; // (u0, v0)

void main() {

    TexCoord = uvCoords * uvScale + uvOffset;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0);
}