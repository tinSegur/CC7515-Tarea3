#version 330
layout (GL_TRIANGLES) in;
layout (GL_LINE_STRIP) out;

in vec3 Normal[3];
in vec3 fragPos[3];
in vec3 worldPos[3];


uniform float min_h;
uniform float max_h;



void main() {
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
    int max_i = 0;

    for (int j = 0; j<2; j++){
        if (worldPos[j].y > max_vh){
            max_vh = worldPos[j].y;
            max_i = j;
        }
    }

    for (int i = 0; i<7; i++){
        if (max_vh < h) break;
        if (min_vh > h) continue;

        float interp_fac = (h - min_vh)/(worldPos[min_i+1%3].y - min_vh);
        vec3 interp1 = mix(worldPos[min_i], worldPos[(min_i+1)%3], interp_fac);
        









        h += (max_h - min_h)/7;


    }

}