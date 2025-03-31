#include "ParticleSystem.hpp"
#include "Actor.hpp"
#include "Renderer.hpp"
#include <cmath>

using namespace std;

void ParticleSystem::OnStart(){
    if (image == "") {
        ImageDB::CreateDefaultParticletextureWithName(key);
        image = key;
    }
    
    local_frame_number = 0;
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
    
    particles.push(newParticle);
}


void ParticleSystem::OnUpdate() {
    if (!enabled) return;
    
    emitParticle();
    
    queue<Particle*> tempQueue;
    while (!particles.empty()) {
        Particle* particle = particles.front();
        particles.pop();
        
        Renderer::DrawEx(
            image,
            particle->x,
            particle->y,
            0, // rotation
            1.0f, // scale_x
            1.0f, // scale_y
            pivot_x,
            pivot_y,
            particle->color.r, particle->color.g, particle->color.b, particle->color.a,
            sorting_order
        );
        
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

