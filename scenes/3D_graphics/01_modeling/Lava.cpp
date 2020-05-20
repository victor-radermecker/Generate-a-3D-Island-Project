#include "Lava.hpp"
#include <random>



using namespace vcl;

std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.001, 1.0);
std::uniform_real_distribution<float> distrib2(0.98, 1.0);
std::uniform_real_distribution<float> distrib3(0.5, 1.0);
std::uniform_real_distribution<float> distrib4(0.85, 1.0);
std::uniform_real_distribution<float> distrib5(0.80, 1.0);


vec3 lava_model::evaluate_lava(float u, float v)
{
    const float x = 5 * (u - 0.5f);
    const float y = 5 * (v - 0.5f);
    float z = 0.1f;

    return { x,y,z };
}

void lava_model::create_particule(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    init_objects();

    //init first particle
    vec3 p0 = { 0,0,29.2f };
    float theta = 2 * 3.14f * distrib(generator);
    vec3 v0 = vec3(std::cos(theta), std::sin(theta), 15.0f); //-->direction x et y au hasard et une vitesse verticale fixée
    float scale = 1;
    particles.push_back({ p0,v0,scale });

    //init first cloud fire particle (trainée de feu)
    p0 = { 0,0,0 };
    theta = 2 * 3.14f * distrib(generator);
    v0 = vec3(std::cos(theta), std::sin(theta), 15.0f); //-->direction x et y au hasard et une vitesse verticale fixée
    scale = 1;
    fire_particles.push_back({ p0,v0,scale });

    //init first fire particle at the origin
    p0 = { 0,0,0 };
    v0 = vec3(0, 0, 0); //-->direction x et y au hasard et une vitesse verticale fixée
    scale = 1; 
    //particles.push_back({ p0,v0,scale });
    fire_particle.p = p0; 
    fire_particle.v = v0;
    fire_particle.scale = scale;
    // Delay between emission of a new particles
    timer.periodic_event_time_step = 0.001f;



    init_smoke(shaders, scene);
    init_fire(shaders, scene);
    set_rock();
}  

void lava_model::update_particles()
{
    const float dt = timer.update(); // dt: Elapsed time between last frame
    const bool is_new_particle = timer.event;

    // Smoke
    update_smoke(is_new_particle, dt);

    // Fire particle
    update_fire_particle(dt);

    //Fire cloud
    update_fire_cloud(is_new_particle, dt);
    
            
}

void lava_model::draw_particles(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    // Display particles
    
    for (particle_structure& part : particles)
    {
        //particle.uniform.transform.scaling = part.scale;
        //particle.uniform.transform.translation = part.p;
        //draw(particle, scene.camera, shaders["mesh"]);
        //particle.uniform.transform.scaling = 1;


    }

    if (fire_particle.activated) {
        //fire_sphere.uniform.transform.scaling = fire_particle.scale;
        //fire_sphere.uniform.transform.translation = fire_particle.p;
        //draw(fire_sphere, scene.camera, shaders["mesh"]);
        draw_rock(shaders, scene);
    }
    
    for (particle_structure& part : fire_particles)
    {
        //particle.uniform.transform.scaling = part.scale;
        //particle.uniform.transform.translation = part.p;
        //draw(particle, scene.camera, shaders["mesh"]);
        //particle.uniform.transform.scaling = 1;

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(shaders["mesh_sun"]);
        glEnable(GL_BLEND);
        glDepthMask(false); // deactivate zbuffer writing
        glActiveTexture(GL_TEXTURE0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, sprite_fire.texture_id);

        sprite_fire.uniform.transform.scaling = 2.f * part.scale;
        sprite_fire.uniform.transform.rotation = scene.camera.orientation;
        sprite_fire.uniform.transform.translation = part.p;

        draw(sprite_fire, scene.camera, shaders["mesh"]);
        glDepthMask(true); // Important: reactivate writing in ZBuffer for future drawing
    }

    for (particle_structure& part : particles)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(shaders["mesh_sun"]);
        glEnable(GL_BLEND);
        glDepthMask(false); // deactivate zbuffer writing
        glActiveTexture(GL_TEXTURE0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, sprite_smoke.texture_id);

        sprite_smoke.uniform.transform.scaling = 5.0f * part.scale;
        sprite_smoke.uniform.transform.rotation = scene.camera.orientation;
        sprite_smoke.uniform.transform.translation = part.p;

        draw(sprite_smoke, scene.camera, shaders["mesh"]);
        glDepthMask(true); // Important: reactivate writing in ZBuffer for future drawing
    }
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


void lava_model::init_objects() {
    //create particle object
    float r = 0.5f; // radius of particles
    particle = mesh_primitive_sphere(r);
    particle.uniform.color = { 0.5f, 0.5f, 0.5f };

    //create a fire particle
    float r2 = 1.0f; // radius of particles
    fire_sphere = mesh_primitive_sphere(r2);
    fire_sphere.uniform.color = { 1.f, 0.38f, 0.27f };
}


void lava_model::update_smoke(bool is_new_particle, float dt)
{

    //     __SMOKE__

    if (is_new_particle)
    {
        float randt = distrib(generator);
        particle_structure new_particle;
        const vec3 p0 = { -0.5f + randt,-0.5f + randt,29.2f };

        // Initial speed is random. (x,z) components are uniformly distributed along a circle.
        const float theta = 2 * 3.14f * randt;
        const vec3 v0 = vec3(1.5 * std::cos(theta), 1.5 * std::sin(theta), 15.f * distrib3(generator)); //-->direction x et y au hasard et une vitesse verticale fixée
        const float scale = 1;
        particles.push_back({ p0,v0,scale });
    }

    // Update position
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

    // Update particule size  
    for (auto it = particles.begin(); it != particles.end(); ++it) {
        it->scale *= distrib2(generator);
    }

    // Remove particles that are too high or too small
    for (auto it = particles.begin(); it != particles.end(); ++it)
        if (it->p.z > 80 || it->scale < 0.00001)
            it = particles.erase(it);

}


void lava_model::update_fire_particle(float dt)
{
    //create a new fireball

    const bool new_fire_particle = distrib(generator) > 0.95;
    if (new_fire_particle && fire_particle.activated == false) {
        set_rock_rotation();
        fire_particle.theta = 2 * 3.14f * distrib(generator);
        fire_particle.p = { 0,0,29.2f };
        vec3 v = vec3(6 * std::cos(fire_particle.theta), 6 * std::sin(fire_particle.theta), 22.0f); //-->direction x et y au hasard et une vitesse verticale fixée
        fire_particle.v = { v };
        fire_particle.activated = true;
    }

    // update position fire ball

    if (fire_particle.activated) {
        vec3 g2 = { 0, 0, -9.81 };
        const float m = 0.01f; // particle mass
        vec3& p = fire_particle.p;
        vec3& v = fire_particle.v;

        const vec3 F = m * g2;

        // Numerical integration
        v = v + dt * F / m;  // F/m = acceleration
        p = p + dt * v;
    }

    // Desactivate fire ball 

    if (fire_particle.p.z < -15) {
        fire_particle.activated = false;
        fire_particle.p = { 0,0,0 };
        fire_particle.v = { 0,0,0 };
    }
}

void lava_model::update_fire_cloud(bool is_new_particle, float dt)
{
    if (fire_particle.activated && is_new_particle) {
        float randt = distrib2(generator);
        particle_structure new_particle;
        const vec3 p0 = fire_particle.p;
        const float scale = 1;
        // Initial speed is the opposite of the random particle speed
        fire_particles.push_back({ p0, fire_particle.v ,scale });
    }

    // Update particule size and remove them if they are too small  
    for (auto it = fire_particles.begin(); it != fire_particles.end();) {
        it->scale *= distrib5(generator);
        if (fire_particles.size() > 1) {
            if (it->scale < 0.000001) {
                it = fire_particles.erase(it);
            }
        }
        if (it != fire_particles.end()) {
            it++;
        }
        
    }
}

void lava_model::init_smoke(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    // Smoke sprites

    sprite_smoke = mesh_primitive_quad();
    sprite_smoke.shader = shaders["mesh"];
    sprite_smoke.uniform.color = { 1,1,1 };
    sprite_smoke.uniform.shading = { 1,0,0 };
    float randt = distrib3(generator);
    sprite_smoke.uniform.transform.scaling = randt * 5.f;
    sprite_smoke.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/smoke/smoke.png"));

}

void lava_model::init_fire(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    // Smoke sprites

    sprite_fire = mesh_primitive_quad();
    sprite_fire.shader = shaders["mesh"];
    sprite_fire.uniform.color = { 1,1,1 };
    sprite_fire.uniform.shading = { 1,0,0 };
    float randt = distrib3(generator);
    sprite_fire.uniform.transform.scaling = randt * 2.f;
    sprite_fire.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/smoke/fire.png"));

}

mesh lava_model::create_rock()
{
    mesh rock_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/rocks/Stone_01.OBJ");
    return rock_cpu;
}

void lava_model::set_rock()
{
    rock = create_rock();
    rock.uniform.transform.scaling = 0.05f;
    rock.uniform.transform.rotation = random_rotation;
    rock_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/rocks/rock2.png"));
}


void lava_model::draw_rock(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glUseProgram(shaders["mesh_sun"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rock_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //rock.uniform.transform.rotation = fire_particle.rotation;
    rock.uniform.transform.translation = fire_particle.p;
    draw(rock, scene.camera, shaders["mesh_sun"]);
}

void lava_model::set_rock_rotation()
{

    float randt = distrib(generator);
    random_rotation = rotation_from_axis_angle_mat3(vec3(0, 0, 1), 3.14 * randt);
}