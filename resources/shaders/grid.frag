#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float opacity;
    float zoom;
    vec2 offset;
    vec2 size;
};

void main() {
    vec2 coord = (qt_TexCoord0 * size - offset) / zoom;
    
    // Anti-aliased grid
    vec2 grid = abs(fract(coord / 20.0 - 0.5) - 0.5) / fwidth(coord / 20.0);
    float line = min(grid.x, grid.y);
    float alpha = 1.0 - min(line, 1.0);
    
    // Major grid lines
    vec2 grid2 = abs(fract(coord / 100.0 - 0.5) - 0.5) / fwidth(coord / 100.0);
    float line2 = min(grid2.x, grid2.y);
    float alpha2 = 1.0 - min(line2, 1.0);
    
    fragColor = vec4(0.6, 0.6, 0.6, max(alpha * 0.1, alpha2 * 0.3) * opacity);
}
