#version 430

layout (location = 0) in vec3 aPos;

out vec4 frag_Color;

uniform float offsetX;
uniform float offsetY;

uniform vec4 v_Color;

void main(void){
	gl_Position = vec4(aPos.x + offsetX, aPos.y + offsetY, aPos.z, 1.0);
	frag_Color = v_Color;
}