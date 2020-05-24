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
float ambiant  = 0.8;
float diffuse  = 0.8;
uniform float specular = 0.5;
uniform int specular_exponent = 128;

uniform float time;

void main()
{
    float time_normalized = time / 120.0;
    vec3 light = vec3(- 800.0f * cos(3.14f / 60.0f * time), 300.0f, 200.0f * cos(3.14f / 60.0f * time));

    vec3 sun_color;
    if(time_normalized < 0.25)
    {
        sun_color = vec3(0.9 - 4 * 0.1 * time_normalized, 0.7, 0.5 + 4 * 0.1 * time_normalized);
        ambiant = 0.6 + 4 * 0.3 * time_normalized;
        diffuse = 0.6 + 4 * 0.2 * time_normalized;

    } else if (time_normalized < 0.5)
    {
        sun_color = vec3(0.8 - 4 * 0.1 * (time_normalized-0.25), 0.7 - 4 * 0.2 * (time_normalized-0.25), 0.6 - 4 * 0.1 * (time_normalized-0.25));
        ambiant = 0.9 - 4 * 0.3 * (time_normalized - 0.25);
        diffuse = 0.8 - 4 * 0.2 * (time_normalized - 0.25);
    } else if(time_normalized < 0.75)
    {
        sun_color = vec3(0.7 - 4 * 0.5 * (time_normalized-0.5), 0.5 - 4 * 0.3 * (time_normalized-0.5), 0.5);
        ambiant = 0.6 - 4 * 0.2 * (time_normalized - 0.5);
        diffuse = 0.6 - 4 * 0.4 * (time_normalized - 0.5);

    } else
    {
        sun_color = vec3(0.2 + 4 * 0.7 * (time_normalized-0.75), 0.2 + 4 * 0.5 * (time_normalized-0.75), 0.5);
        ambiant = 0.4 + 4 * 0.2 * (time_normalized - 0.75);
        diffuse = 0.2 + 4 * 0.4 * (time_normalized - 0.75);
    }

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
