//BackGround Grid Shader

#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
out vec3 nearPoint;
out vec3 farPoint;

uniform mat4 u_ViewProjection;



vec3 UnprojectPoint(float x, float y, float z, mat4 viewProj) {
    mat4 aviewProj = inverse(viewProj);
    vec4 unprojectedPoint =  aviewProj * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

// normal vertice projection
void main() {
    vec3 p = a_Position;
    nearPoint = UnprojectPoint(p.x, p.y, -1.0, u_ViewProjection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, u_ViewProjection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}



#type fragment
#version 330 core
uniform float u_GridSize;
 in vec3 nearPoint; // nearPoint calculated in vertex shader
 in vec3 farPoint; // farPoint calculated in vertex shader

layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord)*1.3;
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}
void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    outColor = grid(fragPos3D, 10.0) * float(t > 0);
}