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
    
    emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
    emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
    
    rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
    scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);
    
}


void ParticleSystem::decideStartPos(){
    float angle_radian = glm::radians(emit_angle_distribution.Sample());
    float radius = emit_radius_distribution.Sample();
    
    float cos_angle = glm::cos(angle_radian);
    float sin_angle = glm::sin(angle_radian);
    
    starting_x_pos = x + cos_angle * radius;
    starting_y_pos = y + sin_angle * radius;
}


void ParticleSystem::emitParticle() {
    decideStartPos();
    
    Particle* newParticle = new Particle();
    newParticle->x = starting_x_pos;
    newParticle->y = starting_y_pos;
    
    newParticle->scale = scale_distribution.Sample();
    newParticle->rotation = rotation_distribution.Sample();
    
    newParticle->color.r = start_color_r;
    newParticle->color.g = start_color_g;
    newParticle->color.b = start_color_b;
    newParticle->color.a = start_color_a;
    
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
        
        Renderer::DrawEx(image, particle->x, particle->y, particle->rotation, particle->scale, particle->scale, pivot_x, pivot_y, particle->color.r, particle->color.g, particle->color.b, particle->color.a, sorting_order);
        
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

