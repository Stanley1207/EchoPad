#include "ParticleSystem.hpp"
#include "Actor.hpp"
#include "Renderer.hpp"
#include <cmath>

void ParticleSystem::OnStart() {
    // Initialize random engines with specified seeds from the homework
    angle_random.Configure(emit_angle_min, emit_angle_max, 2001); // Use seed 2001 for angles
    radius_random.Configure(emit_radius_min, emit_radius_max, 2002); // Use seed 2002 for radius
    
    // Create a default particle texture if needed
    if (default_texture == nullptr) {
        CreateDefaultTexture();
    }
    
    // Reset local frame number
    local_frame_number = 0;
}

void ParticleSystem::OnUpdate() {
    if (!enabled) {
        return;
    }
    
    // Emit particles if it's time for a burst
    if (local_frame_number % frames_between_bursts == 0) {
        for (int i = 0; i < burst_quantity; i++) {
            EmitParticle();
        }
    }
    
    // Render all active particles
    RenderParticles();
    
    // Increment the local frame number
    local_frame_number++;
}

void ParticleSystem::OnDestroy() {
    // Clean up textures if needed
    if (default_texture != nullptr) {
        SDL_DestroyTexture(default_texture);
        default_texture = nullptr;
    }
}

void ParticleSystem::CreateDefaultTexture() {
    // Create a default 8x8 white texture
    SDL_Renderer* renderer = Renderer::renderer;
    
    // Create a temporary surface
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 8, 8, 32, 0, 0, 0, 0);
    if (!surface) {
        std::cout << "Failed to create surface for default particle texture" << std::endl;
        return;
    }
    
    // Fill the surface with white color (RGBA: 255, 255, 255, 255)
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 255, 255, 255, 255));
    
    // Create texture from the surface
    default_texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // Free the temporary surface
    SDL_FreeSurface(surface);
    
    if (!default_texture) {
        std::cout << "Failed to create default particle texture" << std::endl;
    }
}

void ParticleSystem::EmitParticle() {
    // Get next available particle index
    int index = AllocateParticle();
    
    // Calculate particle position using polar coordinates
    float angle = angle_random.Sample() * (M_PI / 180.0f); // Convert degrees to radians
    float radius = radius_random.Sample();
    
    // Convert polar to cartesian coordinates
    float particle_x = x + radius * cos(angle);
    float particle_y = y + radius * sin(angle);
    
    // Initialize the particle
    particles[index].x = particle_x;
    particles[index].y = particle_y;
    particles[index].is_active = true;
}

int ParticleSystem::AllocateParticle() {
    // Check if there are free slots available
    if (!free_particle_indices.empty()) {
        int index = free_particle_indices.front();
        free_particle_indices.pop();
        return index;
    }
    
    // Otherwise, create a new particle
    particles.push_back(Particle());
    return particles.size() - 1;
}

void ParticleSystem::RenderParticles() {
    // Get the texture to use
    SDL_Texture* texture = default_texture;
    if (!image.empty()) {
        texture = ImageDB::loadImageTexture(Renderer::renderer, image);
    }
    
    // Render each active particle
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i].is_active) {
            // Default rendering parameters
            Renderer::DrawEx(
                image.empty() ? "default_particle" : image,
                particles[i].x,
                particles[i].y,
                0.0f,  // rotation
                1.0f,  // scale_x
                1.0f,  // scale_y
                0.5f,  // pivot_x
                0.5f,  // pivot_y
                255,   // r
                255,   // g
                255,   // b
                255,   // a
                sorting_order
            );
        }
    }
}
