//
//  Engine.hpp
//  game_engine
//
//  Created by Stanley  on 2/19/25.
//

#ifndef Engine_hpp
#define Engine_hpp


#include "Renderer.hpp"
#include "Scene.hpp"



class Engine{
private:
    static rapidjson::Document doc;

    static bool game_running;
        
    static Renderer renderer;
    
    static std::string scene_path;
    
    static std::string scene_name;
        
    static void processInput();
    
    static void checkingRendering();
    
    static void loadInitialSettings();
    
    
public:
    static void initialize();
    
    static void playGame();
    
    static std::string getScenePath(){return scene_path;}
};


#endif /* Engine_hpp */
