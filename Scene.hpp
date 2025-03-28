//
//  Scene.hpp
//  game_engine
//
//  Created by Stanley  on 2/9/25.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <iostream>
#include <vector>
#include "Actor.hpp"


class Scene{
public:
    std::string name;
    
    std::map<int, std::shared_ptr<Actor>> actors;
    
    static inline std::vector<std::shared_ptr<Actor>> actors_to_add;
    
    static inline std::vector<int> actors_to_destroy;
        
    void updateActors();
        
    void processActorCreation();
    
    void processActorDestruction();


    Scene();
    
    ~Scene();

};

class SceneDB{
public:
    static Scene currentScene;
    
    static bool proceed_to_next_scene;
    
    static std::string next_scene;
    
    static std::string scene_path;
    
    static void loadScene();
    
    static Actor* Find(const std::string& name);
    
    static luabridge::LuaRef FindAll(const std::string& name);
    
    static Actor* Instantiate(const std::string& template_name);
    
    static void Load(const std::string& scene_name);
    
    static std::string GetCurrent();
    
    static void DontDestroy(Actor* actor);
    
    static void Destroy(Actor* actor);
    
    
private:


    
};

#endif /* Scene_hpp */
