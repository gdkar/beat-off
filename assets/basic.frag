#version 150

in vec2 v_texcoord;

uniform sampler2D tex0;

out vec4 f_out;

void main()
{
    f_out= texture(tex0, v_texcoord);
}

