#version 400

in vec2 texPos;
uniform sampler2D Tex;

void main() {
	vec4 fragColor;

	float len = sqrt( (texPos.x - 0.5)*(texPos.x - 0.5) + ( texPos.y - 0.5) * (texPos.y - 0.5) );
	if ( (0.05 < len && len < 0.10) || 
		 (0.15 < len && len < 0.20) || 
		 (0.25 < len && len < 0.30) || 
		 (0.35 < len && len < 0.40) ||
		 (0.45 < len && len < 0.50) ) {
		fragColor = vec4(0.8, 0.0, 0.0, 1.0);
	} else {
		fragColor = texture(Tex, texPos); 
	}
	gl_FragColor = fragColor; 
}
