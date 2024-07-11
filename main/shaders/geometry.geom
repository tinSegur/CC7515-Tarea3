#version 330
layout (triangles) in;
layout (line_strip, max_vertices=14) out;

in vec3 Normal[3];
in vec3 fragPos[3];
in vec3 worldPos[3];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float min_h = 0;
float max_h = 30;

float get_interp_f(float h, float min_h, float max_h) {
    return (h-min_h)/(max_h - min_h);
}

void main() {
    mat4 mvp = projection*view*model;
    float h = min_h;
    float min_vh = worldPos[0].y;
    int min_i = 0;

    for (int j = 0; j<2; j++){
        if (worldPos[j].y < min_vh){
            min_vh = worldPos[j].y;
            min_i = j;
        }
    }

    float max_vh = worldPos[0].y;

    for (int j = 0; j<2; j++){
        if (worldPos[j].y > max_vh){
            max_vh = worldPos[j].y;
        }
    }

    for (int i = 0; i<7; i++){
        if (max_vh < h) break;
        if (min_vh > h) continue;

        float interp_fac1 = get_interp_f(h, min_h, gl_in[(min_i + 1)%3].gl_Position.y);
        gl_Position = mvp*mix(gl_in[(min_i + 1)%3].gl_Position, gl_in[min_i].gl_Position, interp_fac1);
        EmitVertex();

        float interp_fac2 = get_interp_f(h, min_h, gl_in[(min_i + 2)%3].gl_Position.y);
        gl_Position = mvp*mix(gl_in[(min_i + 2)%3].gl_Position, gl_in[min_i].gl_Position, interp_fac2);
        EmitVertex();

        EndPrimitive();

        h += (max_h - min_h)/7;
    }

}