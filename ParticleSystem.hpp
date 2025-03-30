#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <vector>
#include "Component.hpp"
#include "Helper.h"

// Forward declarations
class Actor;

// Structure to represent a single particle
struct Particle {
    float x;
    float y;
    bool is_active;
    // More properties will be added in later test suites
};

class ParticleSystem {
public:
    bool enabled = true;
    std::string key = "???";
    
    Actor* actor = nullptr;
    std::string type = "ParticleSystem";
    
    bool onStart_called = false;
    
    // Default property values as specified in Test Suite #0
    float x = 0.0f;
    float y = 0.0f;
    
    // Particle generation properties
    int frames_between_bursts = 1;
    int burst_quantity = 1;
    int local_frame_number = 0;
    
    // Emission shape properties
    float emit_radius_min = 0.0f;
    float emit_radius_max = 0.5f;
    float emit_angle_min = 0.0f;
    float emit_angle_max = 360.0f;
    
    // Appearance properties
    std::string image = "";
    int sorting_order = 9999;
    
    // Internal state
    SDL_Texture* default_texture = nullptr;
    std::vector<Particle> particles;
    std::queue<int> free_particle_indices;
    
    // Random engines for particle generation
    RandomEngine angle_random;
    RandomEngine radius_random;
    
    // Methods
    void OnStart();
    void OnUpdate();
    void OnDestroy();
    
    // Create a default particle texture
    void CreateDefaultTexture();
    
    // Emit a new particle
    void EmitParticle();
    
    // Helper methods for particle management
    int AllocateParticle();
    void RenderParticles();
};

#endif /* ParticleSystem_hpp */
