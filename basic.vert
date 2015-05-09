layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;  
out vec3 ourColor;

void main()
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(position, 1.0);
    ourColor = color;
}