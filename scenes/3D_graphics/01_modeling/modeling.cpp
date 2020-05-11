#include <cmath>
#include <iostream>

#include "modeling.hpp"


#ifdef SCENE_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();

mesh create_ocean();
void update_ocean(mesh_drawable& ocean, buffer<vec3>& ocean_positions, buffer<vec3>& current_normals, buffer<uint3> connectivity, float t, float tmax, perlin_noise p);



/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    terrain = create_terrain();
    terrain.uniform.color = { 0.2f, 0.8f, 0.5f };

    // Create ocean
    ocean_cpu = create_ocean();
    ocean_positions = ocean_cpu.position;
    ocean_normals = ocean_cpu.normal;
    ocean_connectivity = ocean_cpu.connectivity;
    ocean = ocean_cpu;
    ocean.uniform.color = {0.2f,0.5f,0.9f};

    ocean_perlin.height = 0.01f;
    ocean_perlin.octave = 10;
    ocean_perlin.persistency = 0.55f;
    ocean_perlin.scaling = 2.0f;

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    // Timer parameters
    timer.t_max = 10.0f;
    timer.scale = 1.0f;
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();

    set_gui();

    const float t = timer.t;


    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    draw(terrain, scene.camera, shaders["mesh"]);

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        draw(terrain, scene.camera, shaders["wireframe"]);
    }
    
    // Display ocean
    update_ocean(ocean, ocean_positions, ocean_normals, ocean_connectivity, t, timer.t_max, ocean_perlin);
    draw(ocean, scene.camera, shaders["mesh"]);

}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    const vec2 u0 = {0.5f, 0.5f};
    const float h0 = 6.0f;
    const float sigma0 = 0.30f;

    const float d = norm(vec2(u,v)-u0)/sigma0;

    const float z = h0*std::exp(-d*d);

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);

    return {x,y,z};
}


// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluate_terrain(u,v);
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}

vec3 evaluate_ocean(float u, float v)
{
    const float x = 40 * (u - 0.5f);
    const float y = 40 * (v - 0.5f);
    float z = 0.3f;

    return { x,y,z };
}

mesh create_ocean()
{
    // Number of samples of the ocean is N x N
    const size_t N = 75;

    mesh ocean; // temporary terrain storage (CPU only)
    ocean.position.resize(N * N);

    // Fill terrain geometry
    for (size_t ku = 0; ku < N; ++ku)
    {
        for (size_t kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute coordinates
            ocean.position[kv + N * ku] = evaluate_ocean(u,v);
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

void update_ocean(mesh_drawable& ocean, buffer<vec3>& current_position, buffer<vec3>& current_normals, buffer<uint3> connectivity, float t, float tmax, perlin_noise p)
{
    const size_t N = 75;
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
            float amplitude = 0.05f +  1.5f * exp( -6.5f * norm( r));


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
            current_position[kv + N * ku][2] = 0.8f + amplitude * cos(k * norm(r) + pulsation * t );

            /* Apply perlin noise */
            float scaling = p.scaling * (1.0f + 0.05f * (1.0f - cos(pulsation * t)));
            const float noise = perlin(scaling * (u-0.5f), scaling * (v-0.5f), p.octave, p.persistency);
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


void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

}



#endif

