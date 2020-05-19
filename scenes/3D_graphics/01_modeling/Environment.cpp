#include "Environment.hpp"
#include <algorithm>
#include <iostream> 
#include <stdlib.h>    
#include <ctime>

using namespace vcl;


// --------------------------------------------------- //
//                  Terrain Generation                 //
// --------------------------------------------------- //


// Generate terrain mesh
mesh terrain_model::create_terrain(std::string type = "None", vec3 p0 = { 0,0,0 })
{
    // Number of samples of the terrain is N x N
    const size_t N = 300;
    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.texture_uv.resize(N * N); //Initalize buffer for textures
    for (size_t ku = 0; ku < N; ++ku)
    {
        for (size_t kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f); // ex. u=0.5 quand on est à la fin du terrain donc u = 99/99 = 1
            const float v = kv / (N - 1.0f);
            vec3 position = evaluate_terrain(u, v, type, p0);
            // Compute coordinates
            terrain.position[kv + N * ku] = position;
            if(type == "volcano")
                terrain.texture_uv[kv + N * ku] = { 15.0f * u , 15.0f * v  };
            else
                terrain.texture_uv[kv + N * ku] = { 30.0f * u, 30.0f * v };

            //terrain.color[kv + N * ku] = { c,c,c,1.0f };
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

    terrain.push_back(create_terrain("volcano", { 0,0,0 }));
    terrain.push_back(create_terrain("sand", { 0,0,0 }));
    terrain[0].uniform.color = { 1.0f, 1.0f, 1.0f };

    terrain[0].uniform.shading.ambiant = 0.6;
    terrain[0].uniform.shading.diffuse = 0.7;
    terrain[0].uniform.shading.specular = 0.1;
    terrain[0].uniform.shading.specular_exponent = 64;

    terrain[1].uniform.shading.ambiant = 0.6;
    terrain[1].uniform.shading.diffuse = 0.8;
    terrain[1].uniform.shading.specular = 0.1;
    terrain[1].uniform.shading.specular_exponent = 64;


    texture_ids.sand_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/sand.png"));
    texture_ids.grass_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/grass-rock.png"));
    texture_ids.rock_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/volcano-rock.png"));

}


// --------------------------------------------------- //
//                  Ocean Generation                   //
// --------------------------------------------------- //



vec3 terrain_model::evaluate_ocean(float u, float v)
{
    const float x = 200 * (u - 0.5f);
    const float y = 200 * (v - 0.5f);
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


void terrain_model::update_ocean(mesh_drawable& ocean, buffer<vec3>& current_position, buffer<vec3>& current_normals, buffer<uint3> connectivity, float t, float tmax, perlin_noise p)
{
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
            float pulsation = 2.0f * min_pulsation;


            // Compute coordinates : progressive wave
            float old_position = current_position[kv + N * ku][2];
            current_position[kv + N * ku][2] = 0.8f + amplitude * cos(k * norm(r) + pulsation * t);

            /* Apply perlin noise */
            float scaling = p.scaling * (1.0f + 0.05f * (1.0f - cos(pulsation * t)));
            const float noise = perlin(scaling * (u - 0.5f), scaling * (v - 0.5f), p.octave, p.persistency);
            current_position[kv + N * ku][2] *= noise;

            /*
            // If the wave "hit" the ground
            // To modify later : ut and vt are the generalised coordinates in the referential of the terrain
            float ut = (u - 0.25f) * 2.0f;
            float vt = (v - 0.25f) * 2.0f;

            if (ut <= 1 && ut >= 0 && vt <= 1 && vt >= 0)
            {
                float terrain_height = evaluate_terrain_z(ut, vt);
                if (old_position > terrain_height && current_position[kv + N * ku][2] <= terrain_height)
                {
                    current_position[kv + N * ku][2] = terrain_height - 0.005f;
                }
            }
            */
        }
    }
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
    ocean.uniform.color = { 0.6f,1.0f,1.0f };
    ocean.uniform.transform.translation = { 0,0,1.0f };

    // Illumination parameters
    ocean.uniform.shading.ambiant = 0.6f;
    ocean.uniform.shading.diffuse = 0.4;
    ocean.uniform.shading.specular = 0.8f;
    ocean.uniform.shading.specular_exponent = 512;


    ocean_perlin.height = 0.05f;
    ocean_perlin.octave = 10;
    ocean_perlin.persistency = 0.55f;
    ocean_perlin.scaling = 2.0f;
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
    
    //normal 2 (cratère)
    const float d2 = norm(vec2(u, v) - u0) / (sigma0 / 2);
    z -= (h0 / 2) * std::exp(-d2 * d2);

    //normal 3 (cratère) --> élargir la base du cone
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
    float x = 200 * (u - 0.5f);
    float y = 200 * (v - 0.5f);
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
    //Cratère pour ne pas dépasser sur le volcan
    const float d = norm(vec2(u, v) - vec2(0.5,0.5)) / (0.15);
    z -= 10.0f * std::exp(-d * d);

    return z;

}


void island_param::set_island_parameters(std::vector<vec2>& u0, std::vector<float>& h, std::vector<float>& sigma, int& N)
{
    N = 8;
    srand((unsigned)time(0)); // use current time as seed for random generator
    
    for (int i = 0;i < N;i++) {
        float r2 = ((double)rand() / (RAND_MAX));
        float r1 = ((double)rand() / (RAND_MAX));
        //u0.push_back({ r1*0.8f,r2*0.8f  });
        h.push_back(r1 * 15 + r2 * 15);
        //sigma.push_back(r1 / 10.0f);
    }
    int n = 3.5;
    u0.push_back(vec2(0.2, 0.6));
    u0.push_back(vec2(0.12, 0.15));
    u0.push_back(vec2(0.2, 0.82));
    u0.push_back(vec2(0.3, 0.14));
    u0.push_back(vec2(0.1, 0.2));
    u0.push_back(vec2(0.2, 0.35));
    u0.push_back(vec2(0.9, 0.85));
    u0.push_back(vec2(0.78, 0.32));
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




