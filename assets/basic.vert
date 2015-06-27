#version 150

in vec2 g_position;
in vec2 g_texcoord;
in vec2 a_size;

out vec4 v_position;
out vec2 v_texcoord;

void main()
{
    v_position = vec4(g_position, 0.0, 1.0);
//    cell_ = cell;
//    size_ = size;
}

