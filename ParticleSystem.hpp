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
    float scale = 1.0f;
    int rotation = 0;
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
    
    int frames_between_bursts = 1;
    int burst_quantity = 1;
    
    float start_scale_min = 1.0f;
    float start_scale_max = 1.0f;
    
    float rotation_min = 0.0f;
    float rotation_max = 0.0f;
    
    int start_color_r = 255;
    int start_color_g = 255;
    int start_color_b = 255;
    int start_color_a = 255;
    
    
    RandomEngine emit_angle_distribution;
    RandomEngine emit_radius_distribution;
    
    RandomEngine rotation_distribution;
    RandomEngine scale_distribution;
    
    std::queue<Particle*> particles;
    
    int local_frame_number = 0;

    void OnStart();
    void OnUpdate();
    void OnDestroy();

private:
    void decideStartPos();
    void emitParticle();
    
};

#endif /* ParticleSystem_hpp */
