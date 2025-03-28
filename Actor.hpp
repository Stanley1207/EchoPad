//
//  Actor.hpp
//  game_engine
//
//  Created by Stanley  on 2/9/25.
//

#ifndef Actor_hpp
#define Actor_hpp

#include "Component.hpp"

class Actor {
public:
    std::string name = "";
    int id;
    
    bool dont_destroy = false;
    
    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components;

    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> to_add_components;
    std::vector<std::string> to_remove_components;
    
    
    void callStart();
    void callUpdate();
    void callLateUpdate();
    
    std::string GetName() const { return name; }
    int GetID() const { return id; }
    luabridge::LuaRef GetComponentByKey(const std::string& key);
    luabridge::LuaRef GetComponent(const std::string& type);
    luabridge::LuaRef GetComponents(const std::string& type);
    
    luabridge::LuaRef AddComponent(std::string type_name);
    void RemoveComponent(luabridge::LuaRef component_ref);
    
    void processAddedComponents();
    void processRemovedComponents();
    
    void injectConvenienceRef(std::shared_ptr<luabridge::LuaRef> component_ref){
        (*component_ref)["actor"] = this;
    }
    
    void onTriggerEnter(Collision collision);
    void onTriggerExit(Collision collision);
    
    void onCollisionEnter(Collision collision);
    void onCollisionExit(Collision collision);
    
    
    Actor(){};
};


class Template{
public:
    static void readTemplates();
    static Actor GetTemplated(std::string templateName);
    
private:
    static inline std::unordered_map<std::string, Actor> loadedTemplates;
    
};


#endif /* Actor_hpp */
