//BackGround Grid Shader

#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;
layout(location = 4) out mat4 fragView;
layout(location = 8) out mat4 fragProj;

uniform mat4 u_View;
uniform mat4 u_Proj;


out float near;
out float far;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = view;
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    near = 0.1;
    far = 10.0;
    fragView = inverse(u_View);
    fragProj = u_Proj;
    vec3 p = a_Position;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, u_View, u_Proj).xyz; // unprojecting on the near plane
    farPoint =  UnprojectPoint(p.x, p.y, 1.0, u_View, u_Proj).xyz; // unprojecting on the far plane
    gl_Position = vec4(p.x,p.y,p.z, 1.0); // using directly the clipped coordinates
}



#type fragment
#version 420 core



layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 4) in mat4 fragView;
layout(location = 8) in mat4 fragProj;
layout(location = 0) out vec4 outColor; 

in float near; 
in float far; 
uniform float u_GridSize;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
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
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.x,pos.y+.05,pos.z, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}
void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    vec3 depthPos3D = nearPoint + t * ((farPoint*2.0));

    depthPos3D = vec3(depthPos3D.x,depthPos3D.y,depthPos3D.z);
    gl_FragDepth = computeDepth(depthPos3D);
    

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    outColor = (grid(fragPos3D, .1, false) +grid(fragPos3D, 10, false) + grid(fragPos3D, 1, true)) * float(t > 0); // adding multiple resolution for the grid
    outColor.a *= fading;
}
