#include "Lava.hpp"
#include <random>



using namespace vcl;

std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0, 1.0);
std::uniform_real_distribution<float> distrib2(0.985, 1.0);
std::uniform_real_distribution<float> distrib3(0.5, 1.0);


vec3 lava_model::evaluate_lava(float u, float v)
{
    const float x = 5 * (u - 0.5f);
    const float y = 5 * (v - 0.5f);
    float z = 0.1f;

    return { x,y,z };
}

void lava_model::create_particule()
{
    //create particle object
    float r = 0.5f; // radius of particles
    particle = mesh_primitive_sphere(r);
    particle.uniform.color = { 0.5f, 0.5f, 0.5f };

    //create a fire particle
    
    /*float r2 = 1.0f; // radius of particles
    particle_fire = mesh_primitive_sphere(r);
    particle_fire.uniform.color = { 1.f, 0.38f, 0.27f };
    */

    //init first particle
    vec3 p0 = { 0,0,29.2f };
    float theta = 2 * 3.14f * distrib(generator);
    vec3 v0 = vec3(std::cos(theta), std::sin(theta), 15.0f); //-->direction x et y au hasard et une vitesse verticale fixée
    float scale = 1;
    particles_fire.push_back({ p0,v0,scale });

    /*
    //init first fire particle
    p0 = { 0,0,29.2f };
    theta = 2 * 3.14f * distrib(generator);
    v0 = vec3(std::cos(theta), std::sin(theta), 15.0f); //-->direction x et y au hasard et une vitesse verticale fixée
    scale = 1;
    particles.push_back({ p0,v0,scale });
    */

    // Delay between emission of a new particles
    timer.periodic_event_time_step = 0.01f;
}  

void lava_model::update_particles()
{
    const float dt = timer.update(); // dt: Elapsed time between last frame

    const bool is_new_particle = timer.event;

    if (is_new_particle)
    {
        float randt = distrib(generator);
        particle_structure new_particle;
        const vec3 p0 = { -0.5f+randt,-0.5f+randt,29.2f };

        // Initial speed is random. (x,z) components are uniformly distributed along a circle.
        const float theta = 2 * 3.14f * randt ;
        const vec3 v0 = vec3(1.5*std::cos(theta), 1.5*std::sin(theta),15.f*distrib3(generator)); //-->direction x et y au hasard et une vitesse verticale fixée
        const float scale = 1;
        particles.push_back({ p0,v0,scale});
        /*
        if (distrib(generator) < 0.1) { // 1/10 times we generate a fire ball
            const float theta = 2 * 3.14f * randt;
            const vec3 v0 = vec3(1.5 * std::cos(theta), 1.5 * std::sin(theta), 15.f * distrib3(generator)); //-->direction x et y au hasard et une vitesse verticale fixée
            const float scale = 1;
            particles_fire.push_back({ p0,v0,scale });
        }*/

    }

    // Evolve position of particles
    const vec3 g = { 0.0f,0.0f,-0.5f };
    for (particle_structure& particle : particles)
    {
        const float m = 0.01f; // particle mass
        vec3& p = particle.p;
        vec3& v = particle.v;

        const vec3 F = m * g;

        // Numerical integration
        v = v + dt * F / m;  // F/m = acceleration
        p = p + dt * v;
    }
    /*
    for (particle_structure& particle : particles_fire)
    {
        const float m = 0.01f; // particle mass
        vec3& p = particle.p;
        vec3& v = particle.v;

        const vec3 F = m * g * 5;

        // Numerical integration
        v = v + dt * F / m;  // F/m = acceleration
        p = p + dt * v;
    }*/

    // Update particule size  
    for (auto it = particles.begin(); it != particles.end(); ++it) {
        it->scale *= distrib2(generator);
    }
    
    
    // Remove particles that are too high or too small
    for (auto it = particles.begin(); it != particles.end(); ++it)
        if (it->p.z > 80 || it->scale < 0.0000001)
            it = particles.erase(it);
    /*
    for (auto it = particles_fire.begin(); it != particles_fire.end(); ++it)
        if (it->p.z < 3)
            it = particles.erase(it);
            */
}

void lava_model::draw_particles(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    // Display particles
    for (particle_structure& part : particles)
    {
        particle.uniform.transform.scaling = part.scale;
        particle.uniform.transform.translation = part.p;
        draw(particle, scene.camera, shaders["mesh"]);
        particle.uniform.transform.scaling = 1;

    }
    /*
    // Display particles
    for (particle_structure& part : particles_fire)
    {
        particle.uniform.transform.scaling = part.scale;
        particle.uniform.transform.translation = part.p;
        draw(particle, scene.camera, shaders["mesh"]);
        particle.uniform.transform.scaling = 1;

    }*/
}


mesh lava_model::create_lava()
{
    const size_t N = 20;
    mesh lava; 
    lava.position.resize(N * N);
    lava.texture_uv.resize(N * N);

    // Fill terrain geometry
    for (size_t ku = 0; ku < N; ++ku)
    {
        for (size_t kv = 0; kv < N; ++kv)
        {
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);
            lava.position[kv + N * ku] = evaluate_lava(u, v);
            lava.texture_uv[kv + N * ku] = { 5.0f * u, 5.0f * v };
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

            lava.connectivity.push_back(triangle_1);
            lava.connectivity.push_back(triangle_2);
        }
    }
    return lava;
}


void lava_model::update_lava(mesh_drawable& lava, buffer<vec3>& current_position, buffer<vec3>& current_normals, buffer<uint3> connectivity, float t, float tmax, perlin_noise p)
{
    const size_t N = 20;
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
            float amplitude = 0.07f + 0.1f * exp(-3.0f * norm(r));

            // Compute spatial wave vector
            float k = 1.0f + 13.0f * exp(2.0f * norm(r));

            // Compute wave pulsation
            float min_pulsation = 4.0f * 3.14159f / tmax ;
            float pulsation = 2.0f * min_pulsation;

            // Compute coordinates : progressive wave
            float old_position = current_position[kv + N * ku][2];
            current_position[kv + N * ku][2] = 0.8f + amplitude * cos(k * norm(r) + pulsation * t);

            /* Apply perlin noise */
            float scaling = p.scaling * (1.0f + 0.05f * (1.0f - cos(pulsation * t)));
            const float noise = perlin(scaling * (u - 0.5f), scaling * (v - 0.5f), p.octave, p.persistency);
            current_position[kv + N * ku][2] *= noise;
        }
    }
    vcl::normal(current_position, connectivity, current_normals);
    lava.update_normal(current_normals);
    lava.update_position(current_position);
}



void lava_model::set_lava()
{
    lava_cpu = create_lava();
    lava_positions = lava_cpu.position;
    lava_normals = lava_cpu.normal;
    lava_connectivity = lava_cpu.connectivity;
    lava_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/lava/lava.png"));
    lava = lava_cpu;
    lava.uniform.transform.translation = { 0,0,29.2f };

    // Illumination parameters
    lava.uniform.shading.ambiant = 0.6f;
    lava.uniform.shading.diffuse = 0.4;
    lava.uniform.shading.specular = 0.8f;
    lava.uniform.shading.specular_exponent = 512;


    lava_perlin.height = 0.025f;
    lava_perlin.octave = 8;
    lava_perlin.persistency = 0.25f;
    lava_perlin.scaling = 1.0f;
}

void lava_model::draw_lava(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glUseProgram(shaders["mesh_sun"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lava_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    draw(lava, scene.camera, shaders["mesh_sun"]);

}
