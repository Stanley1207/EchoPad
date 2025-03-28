// EventBus.hpp
#ifndef EventBus_hpp
#define EventBus_hpp

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

struct EventSubscription {
    std::string event_type;
    luabridge::LuaRef component;
    luabridge::LuaRef function;
    
    EventSubscription(const std::string& type, luabridge::LuaRef comp, luabridge::LuaRef func)
        : event_type(type), component(comp), function(func) {}
};

class EventBus {
public:
    
    static void Publish(const std::string& event_type, luabridge::LuaRef event_object);
    
    static void Subscribe(const std::string& event_type, luabridge::LuaRef component, luabridge::LuaRef function);
    
    static void Unsubscribe(const std::string& event_type, luabridge::LuaRef component, luabridge::LuaRef function);
    
    static void ProcessEvents();

private:
    static inline std::vector<EventSubscription> pending_subscriptions;
    static inline std::vector<EventSubscription> pending_unsubscriptions;
    
    static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> subscriptions;
};

#endif /* EventBus_hpp */
