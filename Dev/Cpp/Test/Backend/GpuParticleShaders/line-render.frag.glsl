#version 330

precision highp float;
precision highp int;

in vec4 v_Color;
out vec4 o_FragColor;

void main() {
	o_FragColor = v_Color;
}
