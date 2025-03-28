// EventBus.cpp
#include "EventBus.hpp"
#include "Component.hpp"


void EventBus::Publish(const std::string& event_type, luabridge::LuaRef event_object) {
    if (subscriptions.find(event_type) == subscriptions.end()) {
        return; // No subscribers for this event type
    }
    
    // Call all subscriber functions for this event type
    for (const auto& [component, function] : subscriptions[event_type]) {
        try {
            function(component, event_object);
        } catch (const luabridge::LuaException& e) {
            std::cout << "\033[31mEvent handling error: " << e.what() << "\033[0m" << std::endl;
        }
    }
}

void EventBus::Subscribe(const std::string& event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
    // Queue subscription to be processed at end of frame
    pending_subscriptions.push_back(EventSubscription(event_type, component, function));
}

void EventBus::Unsubscribe(const std::string& event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
    // Queue unsubscription to be processed at end of frame
    pending_unsubscriptions.push_back(EventSubscription(event_type, component, function));
}

void EventBus::ProcessEvents() {
    // Process subscriptions
    for (const auto& sub : pending_subscriptions) {
        if (subscriptions.find(sub.event_type) == subscriptions.end()) {
            subscriptions[sub.event_type] = std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>();
        }
        
        // Add this subscription
        subscriptions[sub.event_type].push_back(std::make_pair(sub.component, sub.function));
    }
    pending_subscriptions.clear();
    
    // Process unsubscriptions
    for (const auto& unsub : pending_unsubscriptions) {
        if (subscriptions.find(unsub.event_type) == subscriptions.end()) {
            continue; // No subscribers for this event type
        }
        
        auto& subs = subscriptions[unsub.event_type];
        
        // Find and remove the subscription
        for (auto it = subs.begin(); it != subs.end(); /* no increment */) {
            const auto& [sub_component, sub_function] = *it;
            
            // Compare component and function references
            if (unsub.component == sub_component && unsub.function == sub_function) {
                it = subs.erase(it);
            } else {
                ++it;
            }
        }
        
        // Remove empty event type entries
        if (subs.empty()) {
            subscriptions.erase(unsub.event_type);
        }
    }
    pending_unsubscriptions.clear();
}
