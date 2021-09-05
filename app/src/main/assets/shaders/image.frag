#version 300 es

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

uniform sampler2D s_textureY;
uniform sampler2D s_textureU;
uniform sampler2D s_textureV;

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;

void main()
{
    float y=texture(s_textureY,texCoord).r;
    float u=texture(s_textureU,texCoord).r;
    float v=texture(s_textureV,texCoord).r;
    u=u-0.5;
    v=v-0.5;
    float r=y+1.403*v;
    float g=y-0.344*u-0.714*v;
    float b=y+1.770*u;
    FragColor=vec4(r,g,b,1.0);
}