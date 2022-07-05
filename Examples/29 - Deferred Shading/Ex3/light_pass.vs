#version 330

layout(location = 0) in vec3 Position;

// The Model, View and Projection matrices = WVP
// World = Trans, Rot, Scale
// View = LookAt
// Projection = perspective/ortho
// uniform mat4 gWVP;

uniform mat4 model;
uniform mat4 camMatrix;

void main()
{
    gl_Position = camMatrix * model * vec4(Position, 1.0);
}
