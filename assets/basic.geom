#version 150

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 a_position[];
in vec2 a_size[];
in float a_depth[];
in int  m_id[];
out vec4 g_position;
out vec2 g_texcoord;

uniform vec2 atlasSize;

void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-0.5,  0.5, 0.0, 0.0);
    gl_Position *= vec4(0.08, 0.1066666667, 1.0, 1.0);
    g_texcoord = a_position[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.0, 0.0);
    gl_Position *= vec4(0.08, 0.1066666667, 1.0, 1.0);
    g_texcoord = a_position[0]+a_size[0]*vec2(1,0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5,  0.5, 0.0, 0.0);
    gl_Position *= vec4(0.08, 0.1066666667, 1.0, 1.0);
    g_texcoord = a_position[0]+a_size[0]*vec2(0,1);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5, -0.5, 0.0, 0.0);
    gl_Position *= vec4(0.08, 0.1066666667, 1.0, 1.0);
    g_texcoord = a_position[0]+a_size[0]*vec2(1,1);
    EmitVertex();

    EndPrimitive();
}

