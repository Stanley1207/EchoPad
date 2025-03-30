//
//  Rigidbody.hpp
//  game_engine
//
//  Created by Stanley  on 3/25/25.
//

#ifndef Rigidbody_hpp
#define Rigidbody_hpp

#include <unordered_map>
#include <string>
#include <optional>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <unordered_set>
#include "glm/glm.hpp"
#include "SDL/SDL.h"
#include "Helper.h"
#include "AudioHelper.h"
#include "EngineHelper.hpp"
#include "SDL_ttf/SDL_ttf.h"
#include "SDL_mixer/SDL_mixer.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Input.hpp"
#include "box2d/box2d.h"
#include "EventBus.hpp"
#include "ParticleSystem.hpp"

class Actor;


class HitResult {
public:
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger = false;
};

class Collision {
public:
    Actor* other;
    b2Vec2 point;
    b2Vec2 normal;
    b2Vec2 relative_velocity;
};


class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;
    
    void EndContact(b2Contact* contact) override;
};


// Raycast callback class for single hit detection
class RaycastCallback : public b2RayCastCallback {
public:
    RaycastCallback() : m_fixture(nullptr), m_fraction(1.0f), m_actor(nullptr) {}
    
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
        // Skip phantom fixtures - these are bodies that have both has_collider and has_trigger set to false
        if (fixture->IsSensor() && fixture->GetFilterData().categoryBits == 0x0004) {
            return -1;  // Continue the raycast
        }
        
        // Skip fixtures with no collider and no trigger (phantom fixtures)
        if (fixture->IsSensor() && !fixture->GetFilterData().categoryBits) {
            return -1;  // Continue the raycast
        }
        
        // Skip if the ray starts inside the fixture (fraction is 0)
        if (fraction == 0) {
            return -1;  // Continue the raycast
        }
        
        Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
        if (!actor) {
            return -1;  // Continue the raycast
        }
        
        // Store the hit information
        m_fixture = fixture;
        m_point = point;
        m_normal = normal;
        m_fraction = fraction;
        m_actor = actor;
        m_is_trigger = fixture->IsSensor();
        
        return fraction;  // Clip the ray at this hit point
    }
    
    b2Fixture* m_fixture;
    b2Vec2 m_point;
    b2Vec2 m_normal;
    float m_fraction;
    Actor* m_actor;
    bool m_is_trigger;
};

// Raycast callback for collecting all hits
class RaycastAllCallback : public b2RayCastCallback {
public:
    struct RaycastHit {
        b2Fixture* fixture;
        b2Vec2 point;
        b2Vec2 normal;
        float fraction;
        Actor* actor;
        bool is_trigger;
        
        // For sorting hits by distance
        bool operator<(const RaycastHit& other) const {
            return fraction < other.fraction;
        }
    };
    
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
        // Skip phantom fixtures - bodies with both has_collider and has_trigger set to false
        if (fixture->IsSensor() && fixture->GetFilterData().categoryBits == 0x0004) {
            return -1;  // Continue the raycast
        }
        
        // Skip fixtures with no collider and no trigger (phantom fixtures)
        if (fixture->IsSensor() && !fixture->GetFilterData().categoryBits) {
            return -1;  // Continue the raycast
        }
        
        // Skip if the ray starts inside the fixture (fraction is 0)
        if (fraction == 0) {
            return -1;  // Continue the raycast
        }
        
        Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
        if (!actor) {
            return -1;  // Continue the raycast
        }
        
        RaycastHit hit;
        hit.fixture = fixture;
        hit.point = point;
        hit.normal = normal;
        hit.fraction = fraction;
        hit.actor = actor;
        hit.is_trigger = fixture->IsSensor();
        
        hits.push_back(hit);
        
        return 1.0f;  // Continue the raycast to find all fixtures
    }
    std::vector<RaycastHit> hits;
};


class Rigidbody{
public:
    bool enabled = true;
    std::string key = "???";
    
    Actor* actor = nullptr;
    std::string type = "Rigidbody";
    
    bool onStart_called = false;
    
    float x = 0.0f;
    float y = 0.0f;
    std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0f;
    float density = 1.0f;
    float angular_friction = 0.3f;
    float rotation = 0.0f;
    
    
    //Collider
    bool has_collider = true;
    std::string collider_type = "box";
    float collider_width = 1.0f;
    float collider_height = 1.0f;
    float collider_friction = 0.3f;
    float collider_radius = 0.5f;
    float collider_bounciness = 0.3f;
    
    //Trigger
    bool has_trigger = true;
    std::string trigger_type = "box";
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;
    
    b2Body* body = nullptr;
        
    void OnStart();
    void OnDestroy();
    
    void AddForce(b2Vec2 force);
    void SetVelocity(b2Vec2 velocity);
    void SetPosition(b2Vec2 position);
    void SetRotation(float degree_clockwise);
    void SetGravityScale(float gravity);
    void SetAngularVelocity(float degree_clockwise);
    void SetUpDirection(b2Vec2 dir);
    void SetRightDirection(b2Vec2 dir);
    
    
    b2Vec2 GetPosition();
    float GetRotation();
    b2Vec2 GetVelocity();
    float GetAngularVelocity();
    float GetGravityScale();
    b2Vec2 GetUpDirection();
    b2Vec2 GetRightDirection();

};


class RigidbodyManager {
public:
    static bool has_been_initialized;
    static b2World* physics_world;
    
    
    static luabridge::LuaRef Raycast(const b2Vec2& pos, const b2Vec2& dir, float dist);
    static luabridge::LuaRef RaycastAll(const b2Vec2& pos, const b2Vec2& dir, float dist);

    static void Initialize();
    static void Step();
    
    static void Cleanup();

};

#endif /* Rigidbody_hpp */
