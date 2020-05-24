#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform sampler2D texture_sampler;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color     = vec3(0.99f, 0.83f, 0.50f);
uniform float color_alpha = 1.0;
float ambiant;
float diffuse;
uniform float specular = 0.5;
uniform int specular_exponent = 128;

uniform vec3 light;
uniform vec3 sun_color;
uniform float ambiant_sun;
uniform float diffuse_sun;


void main()
{
    ambiant = ambiant_sun;
    diffuse = diffuse_sun;

    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);


    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), specular_exponent);


    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*sun_color;

    FragColor = vec4(c, color_texture.a*fragment.color.a*color_alpha);
}
