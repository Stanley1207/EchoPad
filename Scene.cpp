// Scene.cpp
#include "Scene.hpp"
#include "Engine.hpp"

using namespace std;

Scene SceneDB::currentScene;
bool SceneDB::proceed_to_next_scene = false;
string SceneDB::next_scene = "";
string SceneDB::scene_path;


Scene::Scene(){
    name = "";
    actors.clear();
}


Scene::~Scene(){
    name = "";
    actors.clear();
}


void Scene::updateActors(){
    processActorCreation();
    
    for (auto& [id, actor] : actors) {
        actor->processAddedComponents();
    }

    for(auto& [id,  actor] : actors){
        actor->callUpdate();
    }
    
    for(auto& [id, actor] : actors){
        actor->callLateUpdate();
    }
    
    for (auto& [id, actor] : actors) {
        actor->processRemovedComponents();
    }
    
    processActorDestruction();
}

void SceneDB::loadScene(){
    if(proceed_to_next_scene && next_scene != ""){
        scene_path = "resources/scenes/" + next_scene + ".scene";
        if(!filesystem::exists(scene_path)){
            cout << "error: scene " << next_scene << " is missing";
            exit(0);
        }
    }
    
    // Collect actors to preserve
    vector<std::shared_ptr<Actor>> preserved_actors;
    for (auto& [id, actor] : currentScene.actors) {
        if (actor->dont_destroy) {
            preserved_actors.push_back(actor);
        }
    }
    
    currentScene = Scene();
    currentScene.name = next_scene;
    
    proceed_to_next_scene = false;
    rapidjson::Document doc;
    
    EngineHelper::ReadJsonFile(scene_path, doc);
    int idcount = 1;
    
    // First add actors from the scene file
    if(doc.HasMember("actors") && doc["actors"].IsArray()){
        for(auto& actor : doc["actors"].GetArray()){
            Actor act;
            
            if(actor.HasMember("template") && actor["template"].IsString()){
                string templateName = actor["template"].GetString();
                Actor templateActor = Template::GetTemplated(templateName);
                
                if (act.name.empty()) {
                    act.name = templateActor.name;
                }
                
                for (const auto& [key, templateComp] : templateActor.components) {
                    if (act.components.find(key) == act.components.end()) {
                        act.components[key] = Component::cloneComponent(templateComp, key);
                    }
                }
            }
            
            if(actor.HasMember("name") && actor["name"].IsString()){
                act.name = actor["name"].GetString();
            }
            
            if(actor.HasMember("components") && actor["components"].IsObject()){
                for (auto& comp : actor["components"].GetObject()) {
                    string key = comp.name.GetString();
                    auto& value = comp.value;
                    
                    if (act.components.find(key) != act.components.end()) {
                        Component::applyOverrides(act.components[key], value);
                    }
                    
                    if (value.HasMember("type") && value["type"].IsString()) {
                        std::string type = value["type"].GetString();
                        act.components[key] = Component::applyComponent(type, key);
                        Component::applyOverrides(act.components[key], value);
                    }
                }
            }
            
            act.id = idcount;
            currentScene.actors[act.id] = std::make_shared<Actor>(std::move(act));
            idcount++;
        }
    }
        
    int preservedId = 10000;
    for (auto& actor : preserved_actors) {
        actor->id = preservedId++;
        currentScene.actors[actor->id] = actor;
    }
    
    for (auto& [id, actor] : currentScene.actors) {
        for (auto& [key, component_ref] : actor->components) {
            actor->injectConvenienceRef(component_ref);
        }
    }
    
    for (auto& [id, actor] : currentScene.actors) {
        bool was_preserved = false;
        if (actor->dont_destroy && id >= 10000) {
            was_preserved = true;
        }
        
        if (!was_preserved) {
            actor->callStart();
        }
    }
}

Actor* SceneDB::Find(const std::string& name) {
    for (auto& [id, actor] : currentScene.actors) {
        if (actor->dont_destroy && actor->name == name) {
            return actor.get();
        }
    }
    
    // If no preserved actor found, check all other actors
    for (auto& [id, actor] : currentScene.actors) {
        // Skip actors that are preserved (already checked) or marked for destruction
        if (actor->dont_destroy ||
            std::find(currentScene.actors_to_destroy.begin(),
                      currentScene.actors_to_destroy.end(),
                      id) != currentScene.actors_to_destroy.end()) {
            continue;
        }
        
        if (actor->name == name) {
            return actor.get();
        }
    }
    
    // Check pending actors
    for (auto& actor : currentScene.actors_to_add) {
        if (actor->name == name) {
            return actor.get();
        }
    }
    
    return nullptr;
}


luabridge::LuaRef SceneDB::FindAll(const std::string& name) {
    luabridge::LuaRef result = luabridge::newTable(Component::lua_state);
    int index = 1;
    
    // Create a set of IDs marked for destruction for faster lookup
    std::unordered_set<int> destroy_ids;
    for (int id : currentScene.actors_to_destroy) {
        destroy_ids.insert(id);
    }
    
    // Add matching actors that aren't marked for destruction
    for (auto& [id, actor] : currentScene.actors) {
        if (actor->name == name && destroy_ids.find(id) == destroy_ids.end()) {
            result[index++] = actor.get();
        }
    }
    
    // Also check pending actors
    for (auto& actor : currentScene.actors_to_add) {
        if (actor->name == name) {
            result[index++] = actor.get();
        }
    }
    
    return result;
}


Actor* SceneDB::Instantiate(const std::string &template_name){
    Actor templateActor = Template::GetTemplated(template_name);
    
    // Create a new actor
    std::shared_ptr<Actor> newActor = std::make_shared<Actor>();
    newActor->name = templateActor.name;
    
    // Clone components from template
    for (const auto& [key, comp] : templateActor.components) {
        newActor->components[key] = Component::cloneComponent(comp, key);
    }
    
    // Inject actor references
    for (auto& [key, component_ref] : newActor->components) {
        newActor->injectConvenienceRef(component_ref);
    }
    
    static int nextId = 10000;
    newActor->id = nextId++;
        
    currentScene.actors_to_add.push_back(newActor);
    
    return newActor.get();
}




void Scene::processActorCreation(){
    for(auto& actor : actors_to_add){
        actors[actor->id] = actor;
        actor->callStart();
    }
    actors_to_add.clear();
}

void SceneDB::Destroy(Actor* actor) {
    for (auto& [key, comp] : actor->components) {
        Component::callOnDestroy(comp, actor->name);
        (*comp)["enabled"] = false;
    }
    
    currentScene.actors_to_destroy.push_back(actor->id);
}

//void Scene::processActorDestruction() {
//    for (int id : actors_to_destroy) {
//        actors.erase(id);
//    }
//    actors_to_destroy.clear();
//}

void Scene::processActorDestruction() {
    for (int id : actors_to_destroy) {
        auto it = actors.find(id);
        if (it != actors.end() && !it->second->dont_destroy) {
            actors.erase(id);
        }
    }
    actors_to_destroy.clear();
}


void SceneDB::Load(const std::string& scene_name){
    next_scene = scene_name;
    proceed_to_next_scene = true;
}


string SceneDB::GetCurrent(){
    return currentScene.name;
}


void SceneDB::DontDestroy(Actor *actor){
    actor->dont_destroy = true;
}

