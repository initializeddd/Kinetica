#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 uBaseColor;
uniform float uMetallic;
uniform float uRoughness;
uniform bool uUseVertexColor;

out vec4 FragColor;

void main() {
    vec3 albedo = uUseVertexColor ? Color : uBaseColor;
    float NdotL = max(dot(normalize(Normal), vec3(0,1,0)), 0.2);
    FragColor = vec4(albedo * NdotL, 1.0);
}
