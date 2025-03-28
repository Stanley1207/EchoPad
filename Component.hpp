//
//  Component.hpp
//  game_engine
//
//  Created by Stanley  on 3/20/25.
//

#ifndef Component_hpp
#define Component_hpp

#include "Rigidbody.hpp"
#include "EventBus.hpp"


class Component{
private:
    static std::string componentPath;

    static void initializeState();
    static void initializeFunctions();
    static void initializeComponents();
    
    static void print(const std::string& message);
    static void printError(const std::string& message);
    
    static void Quit();
    static void Sleep(int milliseconds);
    static int GetFrame();
    static void OpenURL(const std::string& url);
    
    static luabridge::LuaRef InputGetMousePosition();


public:

    static lua_State* lua_state;
    
    static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> component_tables;
    
    static void initialize();
    
    static void establishInheritance(luabridge::LuaRef& instance, luabridge::LuaRef& parent);
    
    static std::shared_ptr<luabridge::LuaRef> applyComponent(const std::string& type, const std::string& key);
    
    static void applyOverrides(const std::shared_ptr<luabridge::LuaRef>& component, const rapidjson::Value& properties);
    
    static std::shared_ptr<luabridge::LuaRef> cloneComponent(const std::shared_ptr<luabridge::LuaRef>& original, const std::string& key);

    static void callOnStart(const std::shared_ptr<luabridge::LuaRef>& component, const std::string name);
    
    static void callOnUpdate(const std::shared_ptr<luabridge::LuaRef>& component, const std::string name);
    
    static void callOnLateUpdate(const std::shared_ptr<luabridge::LuaRef>& component, const std::string name);
    
    static void callOnDestroy(const std::shared_ptr<luabridge::LuaRef>& component, const std::string name);
};


#endif /* Component_hpp */
