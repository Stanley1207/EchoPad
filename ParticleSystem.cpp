#include "ParticleSystem.hpp"
#include "Actor.hpp"
#include "Renderer.hpp"
#include <cmath>

using namespace std;

void ParticleSystem::OnStart(){
    
    local_frame_number = 0;
    
    if(frames_between_bursts < 1){
        frames_between_bursts = 1;
    }
    
    if(burst_quantity < 1){
        burst_quantity = 1;
    }
    
    if(duration_frames < 1){
        duration_frames = 1;
    }
    
    emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
    emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
    
    rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
    scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);
    
    speed_distribution = RandomEngine(start_speed_min, start_speed_max, 498);
    rotation_speed_distribution = RandomEngine(rotation_speed_min, rotation_speed_max, 305);
    
    
    if(end_scale != 1.0f){
        has_end_scale = true;
    }
    
    if(end_color_r != 255 || end_color_g != 255 || end_color_b != 255 || end_color_a != 255){
        has_end_color = true;
    }
    
}


void ParticleSystem::decideStartPosVel(){
    float angle_radian = glm::radians(emit_angle_distribution.Sample());
    float radius = emit_radius_distribution.Sample();
    
    float cos_angle = glm::cos(angle_radian);
    float sin_angle = glm::sin(angle_radian);
    
    starting_x_pos = x + cos_angle * radius;
    starting_y_pos = y + sin_angle * radius;
    
    float speed = speed_distribution.Sample();
    starting_x_vel = cos_angle * speed;
    starting_y_vel = sin_angle * speed;
}


void ParticleSystem::emitParticle() {
    decideStartPosVel();
    
    Particle* newParticle = new Particle();
    newParticle->x = starting_x_pos;
    newParticle->y = starting_y_pos;
    
    newParticle->initial_scale = scale_distribution.Sample();
    newParticle->scale = newParticle->initial_scale;
    
    newParticle->rotation = rotation_distribution.Sample();
    
    newParticle->color.r = start_color_r;
    newParticle->color.g = start_color_g;
    newParticle->color.b = start_color_b;
    newParticle->color.a = start_color_a;
    newParticle->initial_color = newParticle->color;

    newParticle->velocity_x = starting_x_vel;
    newParticle->velocity_y = starting_y_vel;
    
    newParticle->rotation_speed = rotation_speed_distribution.Sample();
    newParticle->frame_age = 0;
    
    particles.push(newParticle);
}


void ParticleSystem::OnUpdate() {
    if (!enabled) return;
    
    if (local_frame_number == 0 && image == "") {
        ImageDB::CreateDefaultParticletextureWithName(key);
        image = key;
    }
    
    if(local_frame_number % frames_between_bursts == 0){
        for (int i = 0; i < burst_quantity; i++) {
            emitParticle();
        }
    }
    
    queue<Particle*> tempQueue;
    while (!particles.empty()) {
        Particle* particle = particles.front();
        particles.pop();
        
        if (particle->frame_age >= duration_frames) {
            delete particle;
            continue;
        }
        
        float lifetime_progress = static_cast<float>(particle->frame_age) / static_cast<float>(duration_frames);
        
        particle->velocity_x += gravity_scale_x;
        particle->velocity_y += gravity_scale_y;
        
        particle->velocity_x *= drag_factor;
        particle->velocity_y *= drag_factor;
        
        particle->rotation_speed *= angular_drag_factor;
        
        particle->x += particle->velocity_x;
        particle->y += particle->velocity_y;
        
        particle->rotation += particle->rotation_speed;
        
        if(has_end_scale){
            particle->scale = glm::mix(particle->initial_scale, end_scale, lifetime_progress);
        }
        
        if(has_end_color){
            particle->color.r = glm::mix(float(particle->initial_color.r), float(end_color_r), lifetime_progress);
            particle->color.g = glm::mix(float(particle->initial_color.g), float(end_color_g), lifetime_progress);
            particle->color.b = glm::mix(float(particle->initial_color.b), float(end_color_b), lifetime_progress);
            particle->color.a = glm::mix(float(particle->initial_color.a), float(end_color_a), lifetime_progress);

        }
        
        Renderer::DrawEx(image, particle->x, particle->y, particle->rotation, particle->scale, particle->scale, pivot_x, pivot_y, particle->color.r, particle->color.g, particle->color.b, particle->color.a, sorting_order);
        
        particle->frame_age++;
        tempQueue.push(particle);
    }
    
    particles = tempQueue;
    
    local_frame_number++;
}

void ParticleSystem::OnDestroy(){
    while (!particles.empty()) {
        Particle* particle = particles.front();
        particles.pop();
        delete particle;
    }
}

