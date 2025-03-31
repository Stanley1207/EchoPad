#include "ParticleSystem.hpp"
#include "Actor.hpp"
#include "Renderer.hpp"
#include <cmath>


void ParticleSystem::OnStart(){
    decideStartPos();
    
    if(image == ""){
        ImageDB::CreateDefaultParticletextureWithName(key);
        image = key;
    }
    
}


void ParticleSystem::decideStartPos(){
    float angle_radian = glm::radians(emit_angle_distribution.Sample());
    float radius = emit_radius_distribution.Sample();
    
    float cos_angle = glm::cos(angle_radian);
    float sin_angle = glm::sin(angle_radian);
    
    starting_x_pos = x + cos_angle * radius;
    starting_y_pos = y + sin_angle * radius;
}


void ParticleSystem::createDefaultParticle(){
//    Particle* newParticle = new Particle();
    
}

void ParticleSystem::OnUpdate(){
    
    
}

void ParticleSystem::OnDestroy(){
    
    
    
}

