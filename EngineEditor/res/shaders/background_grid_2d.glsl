//BackGround Grid 2D Shader

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
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, u_ViewProjection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, u_ViewProjection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}



#type fragment
#version 330 core
uniform int u_GridSize;
 in vec3 nearPoint; // nearPoint calculated in vertex shader
 in vec3 farPoint; // farPoint calculated in vertex shader

layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xy * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.4, 0.4, 0.4, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.5 * minimumx && fragPos3D.x < 0.5 * minimumx){
        color.x = 0.0;
        color.y = 1.0;
        color.z = 0.0;
        color.a = 1.0;
    }
    // x axis
    if(fragPos3D.y > -0.5 *minimumz && fragPos3D.y < 0.5*minimumz){
        color.x = 1.0;
        color.y = 0.0;
        color.z = 0.0;
        color.a = 1.0;

    }
    return color;
}

void main() {
    float t = -nearPoint.x / (farPoint.z - nearPoint.x);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    outColor = grid(fragPos3D, 1.0);
}