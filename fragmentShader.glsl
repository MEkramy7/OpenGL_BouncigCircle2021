#version 430

out vec4 gl_FragColor;
in vec4 frag_Color;

void main(void)
{
	gl_FragColor = frag_Color;
}