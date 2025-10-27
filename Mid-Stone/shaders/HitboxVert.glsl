#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertex;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 2) uniform mat4 modelMatrix;



void main() {
    gl_Position = projectionMatrix * modelMatrix * vec4(vertex, 1.0);
}
