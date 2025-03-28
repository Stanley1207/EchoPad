//
//  Component.cpp
//  game_engine
//
//  Created by Stanley  on 3/20/25.
//

#include "Component.hpp"
#include "Actor.hpp"
#include "Scene.hpp"
#include "Engine.hpp"
#include "Rigidbody.hpp"

using namespace std;

unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> Component::component_tables;
lua_State* Component::lua_state = nullptr;

string Component::componentPath = "resources/component_types";

void Component::initialize(){
    
    if(!filesystem::exists(componentPath)){
        initializeState();
        initializeFunctions();
        return;
    }
    
    initializeState();
    initializeFunctions();
    initializeComponents();
}


void Component::initializeState(){
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
}


void Component::initializeFunctions(){
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Debug")
        .addFunction("Log", &Component::print)
        .addFunction("LogError", &Component::printError)
        .endNamespace();
    
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponent)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Actor")
        .addFunction("Find", &SceneDB::Find)
        .addFunction("FindAll", &SceneDB::FindAll)
        .addFunction("Instantiate", &SceneDB::Instantiate)
        .addFunction("Destroy", &SceneDB::Destroy)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Application")
        .addFunction("Quit", &Component::Quit)
        .addFunction("GetFrame", &Component::GetFrame)
        .addFunction("Sleep", &Component::Sleep)
        .addFunction("OpenURL", &Component::OpenURL)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Input")
        .addFunction("GetKey", &Input::GetKey)
        .addFunction("GetKeyDown", &Input::GetKeyDown)
        .addFunction("GetKeyUp", &Input::GetKeyUp)
        .addFunction("GetMousePosition", &Component::InputGetMousePosition)
        .addFunction("GetMouseButton", &Input::GetMouseButton)
        .addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
        .addFunction("HideCursor", &Input::HideCursor)
        .addFunction("ShowCursor", &Input::ShowCursor)
        .endNamespace();
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Text")
        .addFunction("Draw", &Renderer::DrawText)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Audio")
        .addFunction("Play", &Renderer::PlayAudio)
        .addFunction("Halt", &Renderer::HaltAudio)
        .addFunction("SetVolume",&Renderer::SetAudioVolume)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Image")
        .addFunction("DrawUI", &Renderer::DrawUI)
        .addFunction("DrawUIEx", &Renderer::DrawUIEx)
        .addFunction("Draw", &Renderer::Draw)
        .addFunction("DrawEx", &Renderer::DrawEx)
        .addFunction("DrawPixel", &Renderer::DrawPixel)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Camera")
        .addFunction("SetPosition", &Renderer::SetPosition)
        .addFunction("GetPositionX", &Renderer::GetPositionX)
        .addFunction("GetPositionY", &Renderer::GetPositionY)
        .addFunction("SetZoom", &Renderer::SetZoom)
        .addFunction("GetZoom", &Renderer::GetZoom)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Scene")
        .addFunction("Load", &SceneDB::Load)
        .addFunction("GetCurrent", &SceneDB::GetCurrent)
        .addFunction("DontDestroy", &SceneDB::DontDestroy)
        .endNamespace();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void(*)(float, float)>()
        .addProperty("x", &b2Vec2::x)
        .addProperty("y", &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
        .addFunction("__sub", &b2Vec2::operator_sub)
        .addFunction("__mul", &b2Vec2::operator_mul)
        .addStaticFunction("Distance", static_cast<float(*)(const b2Vec2&, const b2Vec2&)>(&b2Distance))
        .addStaticFunction("Dot", static_cast<float(*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endClass();

    
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Collision>("Collision")
        .addData("other", &Collision::other)
        .addData("point", &Collision::point)
        .addData("relative_velocity", &Collision::relative_velocity)
        .addData("normal", &Collision::normal)
        .endClass();
    
    
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<HitResult>("HitResult")
        .addData("actor", &HitResult::actor)
        .addData("point", &HitResult::point)
        .addData("normal", &HitResult::normal)
        .addData("is_trigger", &HitResult::is_trigger)
        .endClass();

    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Physics")
        .addFunction("Raycast", &RigidbodyManager::Raycast)
        .addFunction("RaycastAll", &RigidbodyManager::RaycastAll)
        .endNamespace();
    
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Event")
        .addFunction("Publish", &EventBus::Publish)
        .addFunction("Subscribe", &EventBus::Subscribe)
        .addFunction("Unsubscribe", &EventBus::Unsubscribe)
        .endNamespace();
    
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Rigidbody>("Rigidbody")
        .addConstructor<void(*) (void)>()
        .addData("x", &Rigidbody::x)
        .addData("y", &Rigidbody::y)
        .addData("enabled", &Rigidbody::enabled)
        .addData("key", &Rigidbody::key)
        .addData("body_type", &Rigidbody::body_type)
        .addData("precise", &Rigidbody::precise)
        .addData("gravity_scale", &Rigidbody::gravity_scale)
        .addData("density", &Rigidbody::density)
        .addData("angular_friction", &Rigidbody::angular_friction)
        .addData("rotation", &Rigidbody::rotation)
        .addData("has_collider", &Rigidbody::has_collider)
        .addData("has_trigger", &Rigidbody::has_trigger)
        .addData("width", &Rigidbody::collider_width)
        .addData("height", &Rigidbody::collider_height)
        .addData("radius", &Rigidbody::collider_radius)
        .addData("friction", &Rigidbody::collider_friction)
        .addData("bounciness", &Rigidbody::collider_bounciness)
        .addData("collider_type", &Rigidbody::collider_type)
        .addData("actor", &Rigidbody::actor)
        .addData("onStart_called", &Rigidbody::onStart_called)
        .addData("type", &Rigidbody::type)
        .addData("trigger_type", &Rigidbody::trigger_type)
        .addData("trigger_width", &Rigidbody::trigger_width)
        .addData("trigger_height", &Rigidbody::trigger_height)
        .addData("trigger_radius", &Rigidbody::trigger_radius)
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("GetVelocity", &Rigidbody::GetVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .addFunction("AddForce", &Rigidbody::AddForce)
        .addFunction("SetVelocity", &Rigidbody::SetVelocity)
        .addFunction("SetPosition", &Rigidbody::SetPosition)
        .addFunction("SetRotation", &Rigidbody::SetRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
        .endClass();
    
    
    
}


void Component::initializeComponents(){
    
    for(const auto& entry : filesystem::directory_iterator(componentPath)){
        int status = luaL_dofile(lua_state, entry.path().string().c_str());
        if (status != LUA_OK){
            cout << "problem with lua file " << entry.path().stem().string();
            exit(0);
        }
        
        string componentName = entry.path().stem().string();
        component_tables.insert({componentName, make_shared<luabridge::LuaRef>(luabridge::getGlobal(lua_state, componentName.c_str()))
        });
    }
}


void Component::print(const string &message){
    cout << message << endl;
}


void Component::printError(const std::string &message){
    cout << "Error: " << message << endl;
    exit(0);
}


void Component::establishInheritance(luabridge::LuaRef& instance, luabridge::LuaRef& parent) {
    luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
    new_metatable["__index"] = parent;
    
    instance.push(lua_state);
    new_metatable.push(lua_state);
    lua_setmetatable(lua_state, -2);
    lua_pop(lua_state, 1);
}


std::shared_ptr<luabridge::LuaRef> Component::applyComponent(const std::string& type, const std::string& key){
    if (type == "Rigidbody"){
        Rigidbody* rigidbody = new Rigidbody();
        luabridge::LuaRef componentRef(lua_state, rigidbody);

        componentRef["key"] = key;
        componentRef["type"] = type;
        componentRef["enabled"] = true;
        componentRef["onStart_called"] = false;
        return make_shared<luabridge::LuaRef>(componentRef);
    }
    
    if(component_tables.find(type) == component_tables.end()){
        cout << "error: failed to locate component " << type;
        exit(0);
    }
    
    luabridge::LuaRef instance = luabridge::newTable(lua_state);
    establishInheritance(instance, *component_tables[type]);
    
    instance["key"] = key;
    instance["type"] = type;
    instance["enabled"] = true;
    instance["onStart_called"] = false;
    
    
    return make_shared<luabridge::LuaRef>(instance);
}


void Component::applyOverrides(const std::shared_ptr<luabridge::LuaRef>& component,
                                      const rapidjson::Value& properties) {
    for (auto it = properties.MemberBegin(); it != properties.MemberEnd(); ++it) {
        std::string propName = it->name.GetString();
        
        if (propName == "type") continue;
        
        if (it->value.IsString()) {
            (*component)[propName] = it->value.GetString();
        }
        else if (it->value.IsInt()) {
            (*component)[propName] = it->value.GetInt();
        }
        else if (it->value.IsDouble()) {
            (*component)[propName] = it->value.GetDouble();
        }
        else if (it->value.IsBool()) {
            (*component)[propName] = it->value.GetBool();
        }
    }
}


void Component::callOnStart(const std::shared_ptr<luabridge::LuaRef>& component, const string name) {
    
    luabridge::LuaRef enabled = (*component)["enabled"];
    if(!enabled.isBool() || !enabled.cast<bool>()){
        return;
    }
    
    luabridge::LuaRef startCalled = (*component)["onStart_called"];
    if (startCalled.isBool() && startCalled.cast<bool>()) {
        return;
    }
    (*component)["onStart_called"] = true;

    if ((*component).isInstance<Rigidbody>()) {
        Rigidbody* rb = (*component).cast<Rigidbody*>();
        rb->OnStart();
        return;
    }
    
    luabridge::LuaRef onStart = (*component)["OnStart"];
    if(onStart.isFunction()){
        try{
            onStart(*component);
        }
        catch (luabridge::LuaException e){
            cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << endl;
        }
    }
}


void Component::callOnUpdate(const std::shared_ptr<luabridge::LuaRef>& component, const string name) {
    luabridge::LuaRef enabled = (*component)["enabled"];
    if(!enabled.isBool() || !enabled.cast<bool>()){
        return;
    }
    
    luabridge::LuaRef onUpdate = (*component)["OnUpdate"];
    if(onUpdate.isFunction()){
        try{
            onUpdate(*component);
        }
        catch (luabridge::LuaException e){
            cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << endl;
        }
    }
}


void Component::callOnLateUpdate(const std::shared_ptr<luabridge::LuaRef>& component, const string name) {
    luabridge::LuaRef enabled = (*component)["enabled"];
    if(!enabled.isBool() || !enabled.cast<bool>()){
        return;
    }
    luabridge::LuaRef onLateUpdate = (*component)["OnLateUpdate"];
    if(onLateUpdate.isFunction()){
        try{
            onLateUpdate(*component);
        }
        catch (luabridge::LuaException e){
            cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << endl;
        }
    }
}

void Component::callOnDestroy(const std::shared_ptr<luabridge::LuaRef>& component, const string name) {
    if ((*component).isInstance<Rigidbody>()) {
        Rigidbody* rb = (*component).cast<Rigidbody*>();
        rb->OnDestroy();
        return;
    }
    
    // For Lua components
    luabridge::LuaRef onDestroy = (*component)["OnDestroy"];
    if (onDestroy.isFunction()) {
        try {
            onDestroy(*component);
        } catch (luabridge::LuaException e) {
            cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << endl;
        }
    }
}


std::shared_ptr<luabridge::LuaRef> Component::cloneComponent(const std::shared_ptr<luabridge::LuaRef>& original, const std::string& key) {
    // First check if it's a C++ component (like Rigidbody)
    if ((*original).isInstance<Rigidbody>()) {
        // Handle Rigidbody directly
        auto rb = (*original).cast<Rigidbody*>();
        auto newComponent = applyComponent("Rigidbody", key);
        
        // Copy properties manually
        Rigidbody* newRb = (*newComponent).cast<Rigidbody*>();
        newRb->x = rb->x;
        newRb->y = rb->y;
        newRb->body_type = rb->body_type;
        newRb->precise = rb->precise;
        newRb->gravity_scale = rb->gravity_scale;
        newRb->density = rb->density;
        newRb->angular_friction = rb->angular_friction;
        newRb->rotation = rb->rotation;
        newRb->has_collider = rb->has_collider;
        newRb->has_trigger = rb->has_trigger;
        newRb->collider_type = rb->collider_type;
        newRb->collider_width = rb->collider_width;
        newRb->collider_height = rb->collider_height;
        newRb->collider_friction = rb->collider_friction;
        newRb->collider_radius = rb->collider_radius;
        newRb->collider_bounciness = rb->collider_bounciness;
        newRb->trigger_type = rb->trigger_type;
        newRb->trigger_width = rb->trigger_width;
        newRb->trigger_height = rb->trigger_height;
        newRb->trigger_radius = rb->trigger_radius;
        
        return newComponent;
    }
    
    // For Lua components, use the existing metatable approach
    string type = "OutputMessage";
    
    luaL_dostring(lua_state,
                 "function getComponentType(comp) "
                 "  local mt = getmetatable(comp) "
                 "  if mt and mt.__index then "
                 "    for k, v in pairs(_G) do "
                 "      if type(v) == 'table' and v == mt.__index then "
                 "        return k "
                 "      end "
                 "    end "
                 "  end "
                 "  return 'OutputMessage' "
                 "end");
    
    luabridge::LuaRef getTypeFunc = luabridge::getGlobal(lua_state, "getComponentType");
    type = getTypeFunc(*original).cast<string>();
    
    auto newComponent = applyComponent(type, key);
    
    luaL_dostring(lua_state,
                 "function copyProperties(src, dst) "
                 "  for k, v in pairs(src) do "
                 "    if k ~= '__index' and type(v) ~= 'function' and k ~= 'key' then "
                 "      dst[k] = v "
                 "    end "
                 "  end "
                 "end");
    
    luabridge::LuaRef copyFunc = luabridge::getGlobal(lua_state, "copyProperties");
    copyFunc(*original, *newComponent);
    
    return newComponent;
}

void Component::Quit(){
    exit(0);
}


void Component::Sleep(int milliseconds){
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}


int Component::GetFrame(){
    return Helper::GetFrameNumber();
}


void Component::OpenURL(const std::string &url){
#ifdef _WIN32
    std::string command = "start " + url;
    std::system(command.c_str());
#elif defined(__APPLE__)
    std::string command = "open " + url;
    std::system(command.c_str());
#else
    std::string command = "xdg-open " + url;
    std::system(command.c_str());
#endif
}


luabridge::LuaRef Component::InputGetMousePosition() {
    glm::vec2 pos = Input::GetMousePosition();
    luabridge::LuaRef result = luabridge::newTable(lua_state);
    result["x"] = pos.x;
    result["y"] = pos.y;
    return result;
}
