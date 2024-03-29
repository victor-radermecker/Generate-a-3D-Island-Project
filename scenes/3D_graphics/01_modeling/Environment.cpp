#define _USE_MATH_DEFINES

#include "Environment.hpp"
#include <stdlib.h>    
#include <ctime>
#include <cmath>
#include <random>

using namespace vcl;

//random generator
std::default_random_engine generator_mountains;
std::uniform_real_distribution<float> distrib_mountains(0.2, 2.3);
std::uniform_real_distribution<float> distrib_angle(0.9, 1.15);


// --------------------------------------------------- //
//                  Terrain Generation                 //
// --------------------------------------------------- //


// Generate terrain mesh
mesh terrain_model::create_terrain(std::string type = "None", vec3 p0 = { 0,0,0 })
{
    // Number of samples of the terrain is N x N
    const size_t N = 600;
    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.texture_uv.resize(N * N); //Initalize buffer for textures
    for (size_t ku = 0; ku < N; ++ku)
    {
        for (size_t kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f); 
            const float v = kv / (N - 1.0f);
            vec3 position = evaluate_terrain(u, v, type, p0);
            // Compute coordinates
            terrain.position[kv + N * ku] = position;
            if(type == "volcano")
                terrain.texture_uv[kv + N * ku] = { 15.0f * u , 15.0f * v  };
            else
                terrain.texture_uv[kv + N * ku] = { 30.0f * u, 30.0f * v };
        }
    }
    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for (unsigned int ku = 0; ku < Ns - 1; ++ku)
    {
        for (unsigned int kv = 0; kv < Ns - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku; // current vertex offset

            const uint3 triangle_1 = { idx, idx + 1 + Ns, idx + 1 };
            const uint3 triangle_2 = { idx, idx + Ns, idx + 1 + Ns };

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }
    return terrain;
}


vec3 terrain_model::evaluate_terrain(float u, float v, std::string type = "None", vec3 p0 = { 0,0,0 })
{
    vec3 p = { 0,0,0 };
    
    if (type == "volcano") {
        p = evaluate_terrain_volcano(u, v);
    }
    else if (type == "sand") {
        p = evaluate_terrain_sand(u, v);
    }
    else if (type == "mountain") {
        p = evaluate_terrain_mountain(u, v);
    }

    else {
        float x = 0;
        float y = 0;
        float z = evaluate_terrain_z(u, v);   
        p = { x,y,z };
    }
    return p + p0;
}

// Evaluate height of the terrain for any (u,v) \in [0,1]
float terrain_model::evaluate_terrain_z(float u, float v)
{
    float z = 0;
    // Normal gaussian
    const vec2 u0 = { 0.5f, 0.5f };
    const float h0 = 15.0f;
    const float sigma0 = 0.18f;
    const float d = norm(vec2(u, v) - u0) / sigma0;
    z = h0 * std::exp(-d * d);

    return z;
}

void terrain_model::set_terrain()
{
    islands.set_island_parameters(islands.u0, islands.h, islands.sigma, islands.N);
    islands.set_mountains_parameters(islands.u0_mountains, islands.h_mountains, islands.sigma_mountains, islands.N_mountains);


    terrain.push_back(create_terrain("volcano", { 0,0,0 }));
    terrain.push_back(create_terrain("sand", { 0,0,0 }));
    terrain.push_back(create_terrain("mountain", { 0,0,-0.5f }));

    terrain[0].uniform.shading.specular = 0.1;
    terrain[0].uniform.shading.specular_exponent = 64;

    terrain[1].uniform.shading.specular = 0.1;
    terrain[1].uniform.shading.specular_exponent = 64;

    terrain[2].uniform.shading.specular = 0.1;
    terrain[2].uniform.shading.specular_exponent = 64;

    texture_ids.sand_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/sand.png"));
    texture_ids.grass_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/grass-rock.png"));
    texture_ids.rock_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/volcano-rock.png"));

}


// --------------------------------------------------- //
//                  Ocean Generation                   //
// --------------------------------------------------- //



vec3 terrain_model::evaluate_ocean(float u, float v)
{
    const float x = 300 * (u - 0.5f);
    const float y = 300 * (v - 0.5f);
    float z = 0.3f;

    return { x,y,z };
}

mesh terrain_model::create_ocean()
{
    // Number of samples of the ocean is N x N
    const size_t N = 50;

    mesh ocean; // temporary terrain storage (CPU only)
    ocean.position.resize(N * N);
    ocean.texture_uv.resize(N * N);

    // Fill terrain geometry
    for (size_t ku = 0; ku < N; ++ku)
    {
        for (size_t kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute coordinates
            ocean.position[kv + N * ku] = evaluate_ocean(u, v);
            ocean.texture_uv[kv + N * ku] = { 5.0f * u, 5.0f * v };
        }
    }
    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for (unsigned int ku = 0; ku < Ns - 1; ++ku)
    {
        for (unsigned int kv = 0; kv < Ns - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku; // current vertex offset

            const uint3 triangle_1 = { idx, idx + 1 + Ns, idx + 1 };
            const uint3 triangle_2 = { idx, idx + Ns, idx + 1 + Ns };

            ocean.connectivity.push_back(triangle_1);
            ocean.connectivity.push_back(triangle_2);
        }
    }
    return ocean;
}


void terrain_model::update_ocean(mesh_drawable& ocean, buffer<vec3>& current_position, buffer<vec3>& current_normals, buffer<uint3> connectivity, perlin_noise p)
{
    ocean_timer.update();
    float t = ocean_timer.t;
    float tmax = ocean_timer.t_max;

    const size_t N = 50;
    for (size_t ku = 0; ku < N; ++ku)
    {
        for (size_t kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Vector from center of island to current point
            vec2 r = { 0.5f - u, 0.5f - v };

            // Compute wave amplitude
            // The closer from ilsand, the higher the amplitude
            float amplitude = 0.1f + 0.4f * exp(-3.0f * norm(r));


            // Compute spatial wave vector
            // The closer from the island, the lower the wave langth
            float k = 1.0f + 13.0f * exp(2.0f * norm(r));

            // Compute wave pulsation
            // We assume it is constant
            // It has to be a multiple of 2*pi/tmax
            float min_pulsation = 2.0f * 3.14159f / tmax;
            float pulsation = 24.0f * min_pulsation;


            // Compute coordinates : progressive wave
            float old_position = current_position[kv + N * ku][2];
            current_position[kv + N * ku][2] = 0.8f + amplitude * cos(k * norm(r) + pulsation * t);

            /* Apply perlin noise */
            float scaling = p.scaling * (1.0f + 0.05f * (1.0f - cos(pulsation * t)));
            const float noise = perlin(scaling * (u - 0.5f), scaling * (v - 0.5f), p.octave, p.persistency);
            current_position[kv + N * ku][2] *= noise;

        }
    }
    // Update the normals of the triangles, as well as their positions
    vcl::normal(current_position, connectivity, current_normals);
    ocean.update_normal(current_normals);
    ocean.update_position(current_position);
}


void terrain_model::set_ocean()
{
    ocean_cpu = create_ocean();
    ocean_positions = ocean_cpu.position;
    ocean_normals = ocean_cpu.normal;
    ocean_connectivity = ocean_cpu.connectivity;
    ocean_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/ocean_texture.png"));
    ocean = ocean_cpu;
    ocean.uniform.color = { 0.6f,0.7f,0.9f };
    ocean.uniform.transform.translation = { 0,0,1.0f };

    // Illumination parameters
    ocean.uniform.shading.specular = 0.8f;
    ocean.uniform.shading.specular_exponent = 512;


    ocean_perlin.height = 0.1f;
    ocean_perlin.octave = 10;
    ocean_perlin.persistency = 0.55f;
    ocean_perlin.scaling = 3.0f;

    ocean_timer.t_max = 120.0f;
    ocean_timer.scale = 1.0f;
}





// --------------------------------------------------- //
//                  Volcano Generation                 //
// --------------------------------------------------- //


vec3 terrain_model::evaluate_terrain_volcano(float u, float v)
{
    //set parameters for Perlin Noise
    const float scaling = 2.5;
    const int octave = 8;
    const float persistency = 0.4;
    const float height = 0.5;

    // Evaluate Perlin noise
    const float noise = perlin(scaling * u, scaling * v, octave, persistency);

    float c = 0.3f + 0.7f * noise;
    float x = 60 * (u - 0.5f);
    float y = 60 * (v - 0.5f);
    float z = evaluate_terrain_z_volcano(u, v);
    z *= 1 + c * std::exp(-z / 10);
    return  1.5f * vec3( x,y,z ) - vec3(0,0,2.0f);
}

float terrain_model::evaluate_terrain_z_volcano(float u, float v)
{
    float z = 0;
    const vec2 u0 = { 0.5f, 0.5f };
    const float h0 = 25.0f;
    const float sigma0 = 0.065f;

    //normal 1
    const float d = norm(vec2(u, v) - u0) / sigma0;
    z = h0 * std::exp(-d * d);
    
    //normal 2 (crat�re)
    const float d2 = norm(vec2(u, v) - u0) / (sigma0 / 2);
    z -= (h0 / 2) * std::exp(-d2 * d2);

    //normal 3 (crat�re) --> �largir la base du cone
    const float d3 = norm(vec2(u, v) - u0) / (sigma0 *5);
    z += (h0 / 5) * std::exp(-d3 * d3);

    return z;
}



// --------------------------------------------------- //
//                  Sand Generation                 //
// --------------------------------------------------- //



vec3 terrain_model::evaluate_terrain_sand(float u, float v)
{
    //set parameters for Perlin Noise
    const float scaling = 2.5;
    const int octave = 8;
    const float persistency = 0.6;
    const float height = 0.5;
    const float noise = perlin(scaling * u, scaling * v, octave, persistency);

    float c = 0.3f + 0.7f * noise;
    float x = 240 * (u - 0.5f);
    float y = 240 * (v - 0.5f);
    float z = evaluate_terrain_z_sand(u, v);
    z *= 1 + c * std::exp(- abs(z) /15);

    return { x,y,z };
}


float terrain_model::evaluate_terrain_z_sand(float u, float v)
{
    float z = 0;
    for (int i = 0;i < islands.N;i++) {
        float d = norm(vec2(u, v) - islands.u0[i]) / islands.sigma[i];
        z += islands.h[i] * std::exp(-d * d);
    }
    //Crat�re pour ne pas d�passer sur le volcan
    const float d = norm(vec2(u, v) - vec2(0.5,0.5)) / (0.15);
    z -= 10.0f * std::exp(-d * d);

    return z;

}


void island_param::set_island_parameters(std::vector<vec2>& u0, std::vector<float>& h, std::vector<float>& sigma, int& N)
{
    N = 8;
    
    for (int i = 0;i < N;i++) {
        h.push_back(10);
        h.push_back(5);
        h.push_back(25);
        h.push_back(17);
        h.push_back(23);
        h.push_back(7);
        h.push_back(20);
        h.push_back(3);
    }
    int n = 3.5;
    u0.push_back(vec2(0.25, 0.58));
    u0.push_back(vec2(0.18, 0.21));
    u0.push_back(vec2(0.25, 0.77));
    u0.push_back(vec2(0.33, 0.2));
    u0.push_back(vec2(0.17, 0.25));
    u0.push_back(vec2(0.25, 0.375));
    u0.push_back(vec2(0.83, 0.79));
    u0.push_back(vec2(0.73, 0.35));
    sigma.push_back(0.2f/n);
    sigma.push_back(0.3f/n);
    sigma.push_back(0.25f/n);
    sigma.push_back(0.32f/n);
    sigma.push_back(0.26f/n);
    sigma.push_back(0.14f/n);
    sigma.push_back(0.18f/n);
    sigma.push_back(0.22f/n);
    sigma.push_back(0.47f/n);
}


void terrain_model::draw_terrain(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    // Draw volcano
    glUseProgram(shaders["terrain"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_ids.sand_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uniform(shaders["terrain"], "sand_sampler", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_ids.grass_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uniform(shaders["terrain"], "grass_sampler", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_ids.rock_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uniform(shaders["terrain"], "rock_sampler", 2);

    draw(terrain[0], scene.camera, shaders["terrain"]);


    // Draw mountains
    glUseProgram(shaders["terrain1"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_ids.sand_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uniform(shaders["terrain1"], "sand_sampler", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_ids.grass_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uniform(shaders["terrain1"], "grass_sampler", 1);

    draw(terrain[1], scene.camera, shaders["terrain1"]);
    draw(terrain[2], scene.camera, shaders["terrain1"]);

}


void terrain_model::draw_ocean(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glUseProgram(shaders["mesh_sun"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ocean_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    draw(ocean, scene.camera, shaders["mesh_sun"]);

}




// --------------------------------------------------- //
//                  Mountain chains                    //
// --------------------------------------------------- //


void island_param::set_mountains_parameters(std::vector<vec2>& u0_mountains, std::vector<float>& h_moutains, std::vector<float>& sigma_mountains, int& N_mountains)
{
    N_mountains = 12;
    float radius = 0.45; //(used to set the center of each gaussian);
    float alpha = 1.75 * M_PI / N_mountains;

    std::vector<vec2> u0;
    for (int i = 0;i < N_mountains;i++) {
        float randt = distrib_angle(generator_mountains);
        float x = 0.5 + randt * radius * std::cos(i * alpha);
        float y = 0.5 + randt * radius * std::sin(i * alpha);
        u0_mountains.push_back({ x,y });
        h_mountains.push_back(25.f * distrib_mountains(generator_mountains));
        sigma_mountains.push_back(0.1f * distrib_angle(generator_mountains));
    }
}




vec3 terrain_model::evaluate_terrain_mountain(float u, float v)
{
    //set parameters for Perlin Noise
    const float scaling = 1.5;
    const int octave = 8;
    const float persistency = 0.85;
    const float height = 0.85;
    const float noise = perlin(scaling * u, scaling * v, octave, persistency);

    float c = 0.3f + 0.7f * noise;
    float x = 475 * (u - 0.5f);
    float y = 475 * (v - 0.5f);
    float z = evaluate_terrain_z_mountain(u, v);
    z *= 1 + c * std::exp(-z/20);


    return { x,y,z };
}




float terrain_model::evaluate_terrain_z_mountain(float u, float v)
{
    float z = 0;
    for (int i = 0;i < islands.N_mountains;i++) {
        float d = norm(vec2(u, v) - islands.u0_mountains[i]) / islands.sigma_mountains[i];
        z += islands.h_mountains[i] * std::exp(-d * d);
    }

    return z;

}
