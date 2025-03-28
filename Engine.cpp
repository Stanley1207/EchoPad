//
//  Engine.cpp
//  game_engine
//
//  Created by Stanley  on 2/19/25.
//

#include "Engine.hpp"

using namespace std;

bool Engine::game_running = true;
//bool Engine::proceed_to_next_scene = false;

rapidjson::Document Engine::doc;
Renderer Engine::renderer;

string Engine::scene_name = "";
string Engine::scene_path = "";


void Engine::initialize(){
    Input::Init();
    
    loadInitialSettings();

    SceneDB::loadScene();
}


void Engine::loadInitialSettings(){
    std::string gamePath = "resources/game.config";
    std::string renderPath = "resources/rendering.config";
    
    if (!filesystem::exists("resources")) {
        cout << "error: resources/ missing";
        exit(0);
    }
    
    if(!filesystem::exists(gamePath)){
        cout << "error: resources/game.config missing";
        exit(0);
    }else{
        EngineHelper::ReadJsonFile(gamePath, doc);
        
        if(doc.HasMember("game_title") && doc["game_title"].IsString()){
            Renderer::game_title = doc["game_title"].GetString();
        }
        
        if(doc.HasMember("initial_scene") && doc["initial_scene"].IsString()){
            scene_name = doc["initial_scene"].GetString();
            scene_path = "resources/scenes/" + scene_name + ".scene";
            if(!filesystem::exists(scene_path)){
                cout << "error: scene " + scene_name + " is missing";
                exit(0);
            }
            SceneDB::scene_path = scene_path;
            SceneDB::next_scene = scene_name;
        }else{
            cout << "error: initial_scene unspecified";
            exit(0);
        }
    }
    
    if(filesystem::exists(renderPath)){
        checkingRendering();
    }
    
    Component::initialize();
    
    RigidbodyManager::Initialize();
    
    Template::readTemplates();
    
}


void Engine::checkingRendering(){
    std::string renderPath = "resources/rendering.config";
    
    EngineHelper::ReadJsonFile(renderPath, doc);
    
    if(doc.HasMember("x_resolution") && doc["x_resolution"].IsInt()){
        renderer.window_size.x = doc["x_resolution"].GetInt();
    }
    
    if(doc.HasMember("y_resolution") && doc["y_resolution"].IsInt()){
        renderer.window_size.y = doc["y_resolution"].GetInt();
    }
    
    if(doc.HasMember("clear_color_r") && doc["clear_color_r"].IsInt()){
        renderer.clear_color_r= doc["clear_color_r"].GetInt();
    }
    
    if(doc.HasMember("clear_color_g") && doc["clear_color_g"].IsInt()){
        renderer.clear_color_g = doc["clear_color_g"].GetInt();
    }
    
    if(doc.HasMember("clear_color_b") && doc["clear_color_b"].IsInt()){
        renderer.clear_color_b = doc["clear_color_b"].GetInt();
    }
    
    if(doc.HasMember("cam_offset_x") && doc["cam_offset_x"].IsNumber()){
        renderer.cam_offset.x = doc["cam_offset_x"].GetFloat();
    }
    
    if(doc.HasMember("cam_offset_y") && doc["cam_offset_y"].IsNumber()){
        renderer.cam_offset.y = doc["cam_offset_y"].GetFloat();
    }
    
    if(doc.HasMember("zoom_factor") && doc["zoom_factor"].IsNumber()){
        renderer.zoom_factor = doc["zoom_factor"].GetFloat();
    }
    
    if(doc.HasMember("cam_ease_factor") && doc["cam_ease_factor"].IsNumber()){
        renderer.cam_ease_factor = doc["cam_ease_factor"].GetFloat();
}}


void Engine::playGame(){
    renderer.window = Helper::SDL_CreateWindow(renderer.game_title.c_str(), 100, 100, renderer.window_size.x, renderer.window_size.y, SDL_WINDOW_SHOWN);

    renderer.renderer = Helper::SDL_CreateRenderer(renderer.window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetRenderDrawColor(renderer.renderer, renderer.clear_color_r, renderer.clear_color_g, renderer.clear_color_b, renderer.clear_color_a);
        
    while(game_running){
        processInput();
        SDL_RenderClear(renderer.renderer);
        
        SceneDB::currentScene.updateActors();
        
        EventBus::ProcessEvents();
        
        RigidbodyManager::Step();

        Renderer::render();
        
        Helper::SDL_RenderPresent(renderer.renderer);
        Input::LateUpdate();
        
        if(SceneDB::proceed_to_next_scene){
            SceneDB::loadScene();
        }
    }
}


void Engine::processInput(){
    SDL_Event event;
    while (Helper::SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game_running = false;
            return;
        }
        
        Input::ProcessEvent(event);
    }
}
