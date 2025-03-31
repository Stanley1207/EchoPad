#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <vector>
#include "Component.hpp"
#include "Helper.h"

class Actor;

struct Particle {
    float x = 0.0f;
    float y = 0.0f;
    bool is_active = true;
    float size = 1.0f;
    b2Body* body = nullptr;
    SDL_Color color = {255, 255, 255, 255};
};

class ParticleSystem {
public:
    float x = 0.0f;
    float y = 0.0f;
    bool enabled = true;
    std::string key = "???";
    Actor* actor = nullptr;
    std::string type = "ParticleSystem";
    bool onStart_called = false;
    
    std::string image = "";
    
    float pivot_x = 0.5f;
    float pivot_y = 0.5f;
    
    int sorting_order = 9999;
    float scale = 1.0f;
    
    SDL_Color tint = {255, 255, 255, 255};
    
    float radius = 0.5f;
    float starting_x_pos = 0.0f;
    float starting_y_pos = 0.0f;

    
    float emit_angle_min = 0.0f;
    float emit_angle_max = 360.0f;
    float emit_radius_min = 0.0f;
    float emit_radius_max = 0.5f;
    
    RandomEngine emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
    RandomEngine emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
    
    
    
    void OnStart();
    void OnUpdate();
    void OnDestroy();

private:
    void decideStartPos();
    void createDefaultParticle();
    
};

#endif /* ParticleSystem_hpp */
