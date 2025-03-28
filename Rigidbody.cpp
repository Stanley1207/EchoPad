//
//  Rigidbody.cpp
//  game_engine
//
//  Created by Stanley  on 3/25/25.
//

#include "Rigidbody.hpp"
#include "Actor.hpp"
#include "Scene.hpp"
#include "Engine.hpp"
#include "Component.hpp"
#include "box2d/box2d.h"

using namespace std;


bool RigidbodyManager::has_been_initialized = false;
b2World* RigidbodyManager::physics_world = nullptr;


b2Vec2 Rigidbody::GetPosition(){
    if (body){
        return body->GetPosition();
    }
    return b2Vec2(x, y);
}


float Rigidbody::GetRotation(){
    if (body){
        return body->GetAngle() * (180.0f / b2_pi);
    }
    return rotation;
}


void RigidbodyManager::Initialize(){
    if(has_been_initialized){
        return;
    }
    
    b2Vec2 gravity = b2Vec2(0.0f, 9.8f);
    
    physics_world = new b2World(gravity);
    
    physics_world->SetContactListener(new ContactListener());
    
    has_been_initialized = true;
}


void RigidbodyManager::Step(){
    if(!has_been_initialized){
        return;
    }
    
    physics_world->Step(1.0f / 60.0f, 8, 3);
}


void Rigidbody::OnStart() {
        
    if (!RigidbodyManager::has_been_initialized) {
        RigidbodyManager::Initialize();
    }
    
    b2BodyDef bodyDef;
    bodyDef.position.Set(x, y);
    bodyDef.angle = rotation * b2_pi / 180.0f;
    
    if (body_type == "dynamic") {
        bodyDef.type = b2_dynamicBody;
    } else if (body_type == "static") {
        bodyDef.type = b2_staticBody;
    } else if (body_type == "kinematic") {
        bodyDef.type = b2_kinematicBody;
    }
    
    bodyDef.bullet = precise;
    bodyDef.gravityScale = gravity_scale;
    bodyDef.angularDamping = angular_friction;
    
    body = RigidbodyManager::physics_world->CreateBody(&bodyDef);

    
    if(!has_collider && !has_trigger){
        b2PolygonShape phantom_shape;
        phantom_shape.SetAsBox(collider_width * 0.5f, collider_height * 0.5f);
        b2FixtureDef phantom_fix_def;
        phantom_fix_def.density = density;
        phantom_fix_def.shape = &phantom_shape;
        
        phantom_fix_def.isSensor = true;
        
        phantom_fix_def.filter.categoryBits = 0x0004;
        phantom_fix_def.filter.maskBits = 0;
        
        phantom_fix_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);
        
        body->CreateFixture(&phantom_fix_def);
        return;
    }
    
    if (has_collider) {
        b2FixtureDef collider_fixture_def;
        collider_fixture_def.isSensor = false;
        collider_fixture_def.friction = collider_friction;
        collider_fixture_def.restitution = collider_bounciness;
        collider_fixture_def.density = density;
        
        collider_fixture_def.filter.categoryBits = 0x0001;
        collider_fixture_def.filter.maskBits = 0x0001;

        collider_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);
        
        if (collider_type == "box") {
            b2PolygonShape boxShape;
            boxShape.SetAsBox(collider_width * 0.5f, collider_height * 0.5f);
            collider_fixture_def.shape = &boxShape;
            body->CreateFixture(&collider_fixture_def);
        }
        else if (collider_type == "circle") {
            b2CircleShape circleShape;
            circleShape.m_radius = collider_radius;
            collider_fixture_def.shape = &circleShape;
            body->CreateFixture(&collider_fixture_def);
        }
    }
    
    if (has_trigger){
        b2FixtureDef trigger_fixture_def;
        trigger_fixture_def.isSensor = true;
        trigger_fixture_def.density = density;
        
        trigger_fixture_def.filter.categoryBits = 0x0002;
        trigger_fixture_def.filter.maskBits = 0x0002;
        
        trigger_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);
        
        if (trigger_type == "box") {
            b2PolygonShape boxShape;
            boxShape.SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
            trigger_fixture_def.shape = &boxShape;
            body->CreateFixture(&trigger_fixture_def);
        }
        else if (trigger_type == "circle") {
            b2CircleShape circleShape;
            circleShape.m_radius = trigger_radius;
            trigger_fixture_def.shape = &circleShape;
            body->CreateFixture(&trigger_fixture_def);
        }
    }
}


void Rigidbody::OnDestroy() {
    if (body && RigidbodyManager::physics_world) {
        RigidbodyManager::physics_world->DestroyBody(body);
        body = nullptr;
    }
}


void RigidbodyManager::Cleanup() {
    if (physics_world) {
        delete physics_world;
        physics_world = nullptr;
    }
    has_been_initialized = false;
}


void Rigidbody::AddForce(b2Vec2 force){
    body->ApplyForceToCenter(force, true);
}


void Rigidbody::SetVelocity(b2Vec2 velocity){
    body->SetLinearVelocity(velocity);
}

void Rigidbody::SetRotation(float degree_clockwise){
    if (body){
        float radian = degree_clockwise * (b2_pi / 180.0f);
        body->SetTransform(body->GetPosition(), radian);
    }else{
        rotation = degree_clockwise;
    }
}

void Rigidbody::SetPosition(b2Vec2 position){
    if(body){
        body->SetTransform(position, body->GetAngle());
    }else{
        x = position.x;
        y = position.y;
    }
    
}


void Rigidbody::SetGravityScale(float gravity){
    body->SetGravityScale(gravity);
}


void Rigidbody::SetAngularVelocity(float degree_clockwise){
    float radian = degree_clockwise * (b2_pi / 180.0f);
    body->SetAngularVelocity(radian);
}


void Rigidbody::SetUpDirection(b2Vec2 dir){
    dir.Normalize();
    float angle = glm::atan(dir.x, -dir.y);

    body->SetTransform(body->GetPosition(), angle);

}

void Rigidbody::SetRightDirection(b2Vec2 dir){
    dir.Normalize();
    float angle = glm::atan(dir.x, -dir.y) - b2_pi / 2.0f;
    body->SetTransform(body->GetPosition(), angle);

}


b2Vec2 Rigidbody::GetVelocity(){
    return body->GetLinearVelocity();
}

float Rigidbody::GetAngularVelocity(){
    float angularV = body->GetAngularVelocity() * (180.0f / b2_pi);
    
    return angularV;
}

float Rigidbody::GetGravityScale(){
    return body->GetGravityScale();
}

b2Vec2 Rigidbody::GetUpDirection(){
    float angle = body->GetAngle();
    b2Vec2 result = b2Vec2(glm::sin(angle), -glm::cos(angle));
    result.Normalize();
    return result;

}

b2Vec2 Rigidbody::GetRightDirection() {
    float angle = body->GetAngle();
    b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
    
    result.Normalize();
    return result;
}


void ContactListener::BeginContact(b2Contact *contact){
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
    Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
    
    if (actorA == nullptr && actorB == nullptr){return;}
    
    Collision collision;
    
    
    collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();

    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    
    if(!fixtureA->IsSensor() && !fixtureB->IsSensor()){
        collision.other = actorB;
        collision.point = world_manifold.points[0];
        collision.normal = world_manifold.normal;
        actorA->onCollisionEnter(collision);
                
        collision.other = actorA;
        actorB->onCollisionEnter(collision);
    }else if(fixtureA->IsSensor() && fixtureB->IsSensor()){
        collision.point = b2Vec2(-999.0f, -999.0f);
        collision.normal = b2Vec2(-999.0f, -999.0f);
        
        collision.other = actorB;
        actorA->onTriggerEnter(collision);
        
        collision.other = actorA;
        actorB->onTriggerEnter(collision);
    }
}



void ContactListener::EndContact(b2Contact *contact){
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
    Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
    
    if (actorA == nullptr && actorB == nullptr){return;}
    
    Collision collision;
    
    
    collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();

    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    
    collision.point = b2Vec2(-999.0f, -999.0f);
    collision.normal = b2Vec2(-999.0f, -999.0f);
    
    if(!fixtureA->IsSensor() && !fixtureB->IsSensor()){
        collision.other = actorB;
        actorA->onCollisionExit(collision);
        
        collision.other = actorA;
        actorB->onCollisionExit(collision);
    }else if(fixtureA->IsSensor() && fixtureB->IsSensor()){
        collision.other = actorB;
        actorA->onTriggerExit(collision);
        
        collision.other = actorA;
        actorB->onTriggerExit(collision);
    }
}


// Add to the existing file

luabridge::LuaRef RigidbodyManager::Raycast(const b2Vec2& pos, const b2Vec2& dir, float dist) {
    // Return nil if conditions aren't met
    if (!has_been_initialized || dist <= 0.0f) {
        return luabridge::LuaRef(Component::lua_state);
    }
    
    // Normalize the direction vector
    b2Vec2 normalized_dir = dir;
    normalized_dir.Normalize();
    
    // Calculate the end point of the ray
    b2Vec2 end = pos + dist * normalized_dir;
    
    // Create the raycast callback
    RaycastCallback callback;
    
    // Perform the raycast
    physics_world->RayCast(&callback, pos, end);
    
    // Check if we hit anything
    if (callback.m_fixture == nullptr) {
        return luabridge::LuaRef(Component::lua_state); // Return nil
    }
    
    // Create and populate a HitResult object
    luabridge::LuaRef hitResult = luabridge::newTable(Component::lua_state);
    hitResult["actor"] = callback.m_actor;
    hitResult["point"] = callback.m_point;
    hitResult["normal"] = callback.m_normal;
    hitResult["is_trigger"] = callback.m_is_trigger;
    
    return hitResult;
}

luabridge::LuaRef RigidbodyManager::RaycastAll(const b2Vec2& pos, const b2Vec2& dir, float dist) {
    // Create a table to hold the results (1-indexed)
    luabridge::LuaRef resultTable = luabridge::newTable(Component::lua_state);
    
    // Return empty table if conditions aren't met
    if (!has_been_initialized || dist <= 0.0f) {
        return resultTable;
    }
    
    // Normalize the direction vector
    b2Vec2 normalized_dir = dir;
    normalized_dir.Normalize();
    
    // Calculate the end point of the ray
    b2Vec2 end = pos + dist * normalized_dir;
    
    // Create the raycast callback
    RaycastAllCallback callback;
    
    // Perform the raycast
    physics_world->RayCast(&callback, pos, end);
    
    // Sort the hits by distance
    std::sort(callback.hits.begin(), callback.hits.end());
    
    // Add all hits to the result table (as 1-indexed Lua table)
    for (size_t i = 0; i < callback.hits.size(); i++) {
        const auto& hit = callback.hits[i];
        
        luabridge::LuaRef hitResult = luabridge::newTable(Component::lua_state);
        hitResult["actor"] = hit.actor;
        hitResult["point"] = hit.point;
        hitResult["normal"] = hit.normal;
        hitResult["is_trigger"] = hit.is_trigger;
        
        resultTable[i + 1] = hitResult; // Lua tables are 1-indexed
    }
    
    return resultTable;
}
