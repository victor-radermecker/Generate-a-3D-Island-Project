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
vec3 color     = vec3(1.0, 1.0, 1.0);
uniform float color_alpha = 1.0;
float ambiant  = 1.0;
uniform float diffuse  = 0.0;
uniform float specular = 0.0;
uniform int specular_exponent = 128;

vec3 light = vec3(camera_position.x, camera_position.y, camera_position.z);
uniform float time;

void main()
{

    float time_normalized = time / 120.0;

    if(time_normalized < 0.25)
    {
        color = vec3(0.9 - 4 * 0.1 * time_normalized, 0.7 + 4 * 0.1 * time_normalized, 0.6 + 4 * 0.2 * time_normalized);
        ambiant = 0.3 + 4 * 0.5 * time_normalized;

    } else if (time_normalized < 0.5)
    {
        color = vec3(0.8 - 4 * 0.1 * (time_normalized-0.25), 0.8 - 4 * 0.2 * (time_normalized-0.25), 0.8 - 4 * 0.2 * (time_normalized-0.25));
        ambiant = 0.8 - 4 * 0.5 * (time_normalized - 0.25);
    } else if(time_normalized < 0.75)
    {
        color = vec3(0.7 - 4 * 0.5 * (time_normalized-0.5), 0.6 - 4 * 0.4 * (time_normalized-0.5), 0.6);
        ambiant = 0.3 - 4 * 0.2 * (time_normalized - 0.5);

    } else
    {
        color = vec3(0.2 + 4 * 0.7 * (time_normalized-0.75), 0.2 + 4 * 0.5 * (time_normalized-0.75), 0.6);
        ambiant = 0.1 + 4 * 0.2 * (time_normalized - 0.75);
    }

    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);


    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), specular_exponent);


    vec3 white = vec3(1.0);
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb;

    FragColor = vec4(c, color_texture.a*fragment.color.a*color_alpha);
}
