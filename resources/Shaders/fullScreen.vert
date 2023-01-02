#version 460 core

out vec2 vTexCoords;

// Small trick from Randall Rauwendaal
// Drawing a single triangle that covers all the screen without requiring any vertex buffer
void main(void) {
    vTexCoords = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(vTexCoords * 2.0 + -1.0, 0.0, 1.0);
}
