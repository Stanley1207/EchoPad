//
//  Actor.cpp
//  game_engine
//
//  Created by Stanley  on 3/10/25.
//

#include "Actor.hpp"
#include "Engine.hpp"

using namespace std;

void Actor::callStart(){
//    if (start_called) return;
    
    vector<string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        Component::callOnStart(components[key], name);
    }
    
//    start_called = true;
}

void Actor::callUpdate() {
    // Sort components by key for deterministic order
    vector<string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        Component::callOnUpdate(components[key], name);
    }
}

void Actor::callLateUpdate() {
    // Sort components by key for deterministic order
    vector<string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        Component::callOnLateUpdate(components[key], name);
    }
}


luabridge::LuaRef Actor::GetComponentByKey(const std::string& key) {
    auto it = components.find(key);
    if (it != components.end()) {
        return *(it->second);
    }
    return luabridge::LuaRef(components.begin()->second->state());
}


luabridge::LuaRef Actor::GetComponent(const std::string &type_name) {
    std::vector<std::string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        // Check if this component is scheduled for removal
        if (std::find(to_remove_components.begin(), to_remove_components.end(), key) != to_remove_components.end()) {
            continue; // Skip components that are scheduled for removal
        }
        
        luabridge::LuaRef comp_type = (*components[key])["__type"];
        if (comp_type.isString() && comp_type.cast<std::string>() == type_name) {
            return *components[key];
        }
        
        // If not found, try type (for C++ components)
        comp_type = (*components[key])["type"];
        if (comp_type.isString() && comp_type.cast<std::string>() == type_name) {
            return *components[key];
        }
    }
    
    // Return nil if no component is found
    return luabridge::LuaRef(components.begin()->second->state());
}


luabridge::LuaRef Actor::GetComponents(const std::string &type) {
    // Create a new table to hold the components
    luabridge::LuaRef result = luabridge::newTable(components.begin()->second->state());
    int index = 1; // Lua tables are 1-indexed
    
    // Sort keys for deterministic order
    std::vector<std::string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    // Find all components of the given type
    for (const auto& key : keys) {
        // Try __type field first (for Lua components)
        luabridge::LuaRef comp_type = (*components[key])["__type"];
        if (comp_type.isString() && comp_type.cast<std::string>() == type) {
            result[index++] = *components[key]; // Add component to the result table
            continue;
        }
        
        // If not found, try type field (for C++ components)
        comp_type = (*components[key])["type"];
        if (comp_type.isString() && comp_type.cast<std::string>() == type) {
            result[index++] = *components[key]; // Add component to the result table
        }
    }
    
    // Return the table (empty or filled)
    return result;
}



void Template::readTemplates(){
    loadedTemplates.clear();
    
    string templatePath = "resources/actor_templates";
    
    if(!filesystem::exists(templatePath)){
        return;
    }
    
    for(const auto& entry : filesystem::directory_iterator(templatePath)){
        string filePath = entry.path().string();
        string fileName = entry.path().stem().string();
        
        rapidjson::Document doc;
        EngineHelper::ReadJsonFile(filePath, doc);
        
        Actor templateActor;
        
        if(doc.HasMember("name") && doc["name"].IsString()){
            templateActor.name = doc["name"].GetString();
        }
        
        if(doc.HasMember("components") && doc["components"].IsObject()){
            for (auto& comp : doc["components"].GetObject()) {
                string key = comp.name.GetString();
                auto& value = comp.value;
                
                if (value.HasMember("type") && value["type"].IsString()) {
                    std::string type = value["type"].GetString();
                    templateActor.components[key] = Component::applyComponent(type, key);
                    Component::applyOverrides(templateActor.components[key], value);
                }
            }
        }
        
        loadedTemplates[fileName] = templateActor;

    }
}


Actor Template::GetTemplated(std::string templateName) {
    if (loadedTemplates.find(templateName) == loadedTemplates.end()) {
        std::cout << "error: template " << templateName << " is missing";
        exit(0);
    }
    
    Actor actor;
    
    actor.name = loadedTemplates[templateName].name;
    
    for (const auto& [key, templateComp] : loadedTemplates[templateName].components) {
        actor.components[key] = Component::cloneComponent(templateComp, key);
    }
    
    return actor;
}


luabridge::LuaRef Actor::AddComponent(std::string type_name) {
    static int runtime_component_counter = 0;
    
    std::string key = "r" + std::to_string(runtime_component_counter++);
    std::shared_ptr<luabridge::LuaRef> component = Component::applyComponent(type_name, key);
    
    injectConvenienceRef(component);
    
    to_add_components[key] = component;
    
    return *component;
}


void Actor::RemoveComponent(luabridge::LuaRef component_ref) {
    component_ref["enabled"] = false;
    std::string key = component_ref["key"].cast<std::string>();
    
    (*components[key])["enabled"] = false;
    to_remove_components.push_back(key);
}


void Actor::processRemovedComponents(){
    for(const auto& key : to_remove_components){
        Component::callOnDestroy(components[key], name);
        components.erase(key);
    }
    to_remove_components.clear();
}


void Actor::processAddedComponents() {
    for (const auto& [key, component] : to_add_components) {
        components[key] = component;
        Component::callOnStart(component, name);
    }
    
    to_add_components.clear();
}


void Actor::onTriggerEnter(Collision collision){
    vector<string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    for(const auto& key : keys){
        luabridge::LuaRef component = *(components[key]);
        luabridge::LuaRef enabled = component["enabled"];
        
        if(!enabled){
            continue;
        }
                
        luabridge::LuaRef onTriggerEnter = component["OnTriggerEnter"];
        if(onTriggerEnter.isFunction()){
            try{
                onTriggerEnter(component, collision);
            }catch (const luabridge::LuaException& e) {
                std::cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << std::endl;
            }
        }
    }

}

void Actor::onTriggerExit(Collision collision){
    vector<std::string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        luabridge::LuaRef component = *(components[key]);
        luabridge::LuaRef enabled = component["enabled"];
        
        if(!enabled){
            continue;
        }
        
        luabridge::LuaRef onTriggerExit = component["OnTriggerExit"];
        
        if (onTriggerExit.isFunction()) {
            try {
                onTriggerExit(component, collision);
            } catch (const luabridge::LuaException& e) {
                std::cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << std::endl;
            }
        }
    }
}


void Actor::onCollisionEnter(Collision collision){
    vector<string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    for(const auto& key : keys){
        luabridge::LuaRef component = *(components[key]);
        luabridge::LuaRef enabled = component["enabled"];
        
        if(!enabled){
            continue;
        }
                
        luabridge::LuaRef onCollisionEnter = component["OnCollisionEnter"];
        if(onCollisionEnter.isFunction()){
            try{
                onCollisionEnter(component, collision);
            }catch (const luabridge::LuaException& e) {
                std::cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << std::endl;
            }
        }
    }
}

void Actor::onCollisionExit(Collision collision){
    vector<std::string> keys;
    for (const auto& [key, _] : components) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        luabridge::LuaRef component = *(components[key]);
        luabridge::LuaRef enabled = component["enabled"];
        
        if(!enabled){
            continue;
        }
        
        luabridge::LuaRef onCollisionExit = component["OnCollisionExit"];
        
        if (onCollisionExit.isFunction()) {
            try {
                onCollisionExit(component, collision);
            } catch (const luabridge::LuaException& e) {
                std::cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << std::endl;
            }
        }
    }
}
