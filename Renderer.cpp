//
//  Renderer.cpp
//  game_engine
//
//  Created by Stanley  on 2/19/25.
//

#include "Renderer.hpp"

using namespace std;

string Renderer::game_title = "";

SDL_Window* Renderer::window = nullptr;
SDL_Renderer* Renderer::renderer = nullptr;
glm::vec2 Renderer::window_size = glm::vec2(640, 360);
glm::vec2 Renderer::current_cam_pos = glm::vec2(0.0f, 0.0f);

Uint8 Renderer::clear_color_r = 255;
Uint8 Renderer::clear_color_g = 255;
Uint8 Renderer::clear_color_b = 255;
Uint8 Renderer::clear_color_a = 255;

float Renderer::zoom_factor = 1.0f;
float Renderer::cam_ease_factor = 1.0f;

glm::vec2 Renderer::cam_offset = glm::vec2(0.0f, 0.0f);

int Renderer::request_order = 0;


SDL_Texture* ImageDB::loadImageTexture(SDL_Renderer* renderer, const string& image_name){
    if(imageCache.find(image_name) != imageCache.end()){
        return imageCache[image_name];
    }
    
    string image_path = "resources/images/"+ image_name + ".png";
    
    SDL_Texture* texture = IMG_LoadTexture(renderer, image_path.c_str());
    
    if(!texture){
        cout << "error: missing image " << image_name;
        exit(0);
    }
    
    imageCache[image_name] = texture;
    return texture;
}


void ImageDB::clearCache(){
    for(auto& pair: imageCache){
        SDL_DestroyTexture(pair.second);
    }
    imageCache.clear();
}


void ImageDB::CreateDefaultParticletextureWithName(const std::string &name){
    if(imageCache.find(name) != imageCache.end()){return;}
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);
    
    Uint32 whit_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    SDL_FillRect(surface, NULL, whit_color);
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::getRenderer(), surface);
    
    SDL_FreeSurface(surface);
    imageCache[name] = texture;
}



TTF_Font* FontDB::loadFontTexture(SDL_Renderer *renderer, const string &font_name, int font_size) {
    if (fontCache.find(font_name) != fontCache.end()) {
        if (fontCache[font_name].find(font_size) != fontCache[font_name].end()) {
            return fontCache[font_name][font_size];
        }
    }
    
    std::string font_path = "resources/fonts/" + font_name + ".ttf";
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), font_size);
    
    if (!font) {
        cout << "error: font " << font_name << " missing";
        exit(0);
    }
    
    if (fontCache.find(font_name) == fontCache.end()) {
        fontCache[font_name] = std::unordered_map<int, TTF_Font*>();
    }
    
    fontCache[font_name][font_size] = font;
    
    return font;
}


void FontDB::clearCache() {
    for (auto& [fontName, sizeMap] : fontCache) {
        for (auto& [fontSize, fontPtr] : sizeMap) {
            if (fontPtr != nullptr) {
                TTF_CloseFont(fontPtr);
            }
        }
    }
    fontCache.clear();
}

Mix_Chunk* AudioDB::loadAudio(const string &audio_name){
    if (audioCache.find(audio_name) != audioCache.end()){
        return audioCache[audio_name];
    }
    
    string audio_path = "resources/audio/" + audio_name + ".wav";
    Mix_Chunk* audio = AudioHelper::Mix_LoadWAV(audio_path.c_str());
    
    
    if (!audio){
        string audio_path = "resources/audio/" + audio_name + ".ogg";
        audio = AudioHelper::Mix_LoadWAV(audio_path.c_str());
    }
    

    if (!audio){
        cout << "error: failed to play audio clip " << audio_name;
        exit(0);
    }
    
    audioCache[audio_name] = audio;
    return audio;
}


void AudioDB::clearCache(){
    audioCache.clear();
}


Renderer::Renderer() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    if(AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        exit(0);
    }
    AudioHelper::Mix_AllocateChannels(50);

    window = nullptr;
    renderer = nullptr;
}


Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    ImageDB::clearCache();
    FontDB::clearCache();
    AudioDB::clearCache();
}



void Renderer::DrawText(const std::string &text_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a){
    
    TextRendereRequest request;
    
    request.text = text_content;
    request.x = x;
    request.y = y;
    request.font = font_name;
    SDL_Color text_color = {Uint8(r),Uint8(g),Uint8(b),Uint8(a)};
    request.color = text_color;
    request.size = font_size;
    
    textToDraw.push(request);
}


void Renderer::render(){
    SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);

    while (!sceneToDraw.empty()) {
        ImageRenderRequest request = sceneToDraw.top();
        sceneToDraw.pop();
        renderScene(request);
    }
    
    
    SDL_RenderSetScale(renderer, 1.0f, 1.0f);
    
    while (!UIToDraw.empty()) {
        ImageRenderRequest request = UIToDraw.top();
        UIToDraw.pop();
        renderUI(request);
    }
    

    while(!textToDraw.empty()){
        TextRendereRequest request = textToDraw.front();
        textToDraw.pop();
        
        renderText(request);
    }

    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    while(!pixelToDraw.empty()){
        PixelRenderRequest request =  pixelToDraw.front();
        pixelToDraw.pop();
        renderPixel(request);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}


void Renderer::renderText(TextRendereRequest request){
    TTF_Font* font_texture = FontDB::loadFontTexture(renderer, request.font, request.size);
    
    SDL_Surface* surface = TTF_RenderText_Solid(font_texture, request.text.c_str(), request.color);
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    SDL_FRect rect = {request.x, request.y, static_cast<float>(surface->w), static_cast<float>(surface->h)};
    
    Helper::SDL_RenderCopy(renderer, texture, nullptr, &rect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void Renderer::PlayAudio(int channel, std::string clip_name, bool does_loop){
    
    Mix_Chunk* bgm = AudioDB::loadAudio(clip_name);
    if (bgm) {
        int loop;
        if(does_loop){
            loop = -1;
        }else{
            loop = 0;
        }
        AudioHelper::Mix_PlayChannel(channel, bgm, loop);
    }
}


void Renderer::HaltAudio(int channel){
    AudioHelper::Mix_HaltChannel(channel);
}


void Renderer::SetAudioVolume(int channel, float volume){
    AudioHelper::Mix_Volume(channel, volume);
}


void Renderer::DrawUI(std::string image_name, float x, float y){
    ImageRenderRequest request;
    request.image = image_name;
    request.x = x;
    request.y = y;
    
    UIToDraw.push(request);
}


void Renderer::DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order){
    ImageRenderRequest request;
    request.image = image_name;
    request.x = x;
    request.y = y;
    SDL_Color color = {Uint8(r),Uint8(g),Uint8(b),Uint8(a)};
    request.color = color;
    request.sorting_order = int(sorting_order);
    
    UIToDraw.push(request);
}


void Renderer::Draw(std::string image_name, float x, float y){
    ImageRenderRequest request;
    request.image = image_name;
    request.x = x;
    request.y = y;
    
    sceneToDraw.push(request);
}


void Renderer::DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order){
    ImageRenderRequest request;
    request.image = image_name;
    request.x = x;
    request.y = y;
    request.rotation = static_cast<int>(rotation_degrees);
    request.scale_x = scale_x;
    request.scale_y = scale_y;
    request.pivot_x = pivot_x;
    request.pivot_y = pivot_y;
    SDL_Color color = {Uint8(r),Uint8(g),Uint8(b),Uint8(a)};
    request.color = color;
    request.sorting_order = int(sorting_order);
    
    request.request_order = request_order;
    request_order++;
    
    sceneToDraw.push(request);
}


void Renderer::DrawPixel(float x, float y, float r, float g, float b, float a){
    PixelRenderRequest request;
    SDL_Color color = {Uint8(r),Uint8(g),Uint8(b),Uint8(a)};
    request.x = static_cast<int>(x);
    request.y = static_cast<int>(y);
    request.color = color;
    
    pixelToDraw.push(request);
}


void Renderer::renderScene(ImageRenderRequest request){
    SDL_Texture* texture = ImageDB::loadImageTexture(renderer, request.image);
    
    SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
    SDL_SetTextureAlphaMod(texture, request.color.a);
    
    
    float image_width = 0.0f, image_height = 0.0f;
    Helper::SDL_QueryTexture(texture, &image_width, &image_height);
    
    float screenX = (window_size.x / (2.0f * zoom_factor));
    float screenY = (window_size.y / (2.0f * zoom_factor));

    
//    int pivot_x = static_cast<int>(request.pivot_x * image_width * request.scale_x);
//    
//    int pivot_y = static_cast<int>(request.pivot_y * image_height * request.scale_y);
    
    float pivot_x = request.pivot_x * image_width * request.scale_x;
    
    float pivot_y = request.pivot_y * image_height * request.scale_y;
    
    
    SDL_FRect dst_rect;
    dst_rect.w = image_width * glm::abs(request.scale_x);
    dst_rect.h = image_height * glm::abs(request.scale_y);
    
    dst_rect.x = screenX + (request.x - current_cam_pos.x) * 100.0f - pivot_x;
    dst_rect.y = screenY + (request.y - current_cam_pos.y) * 100.0f - pivot_y;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (request.scale_x < 0) {
        flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
    }
    if (request.scale_y< 0) {
        flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
    }
    
    SDL_FPoint rotation_center = {static_cast<float>(pivot_x), static_cast<float>(pivot_y)};
    
    Helper::SDL_RenderCopyEx(0, "", renderer, texture, nullptr, &dst_rect, request.rotation, &rotation_center, flip);
    
    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, 255);
}


void Renderer::renderUI(ImageRenderRequest request){
    SDL_Texture* texture = ImageDB::loadImageTexture(renderer, request.image);
    
    SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
    SDL_SetTextureAlphaMod(texture, request.color.a);
    
    
    float image_width = 0.0f, image_height = 0.0f;
    Helper::SDL_QueryTexture(texture, &image_width, &image_height);
    
//    float screenX = (window_size.x / (2.0f));
//    float screenY = (window_size.y / (2.0f));
//    
//    float pivot_x = request.pivot_x * image_width * request.scale_x;
//    
//    float pivot_y = request.pivot_y * image_height * request.scale_y;
//    
    
    SDL_FRect dst_rect;
    dst_rect.w = image_width;
    dst_rect.h = image_height;
    
    dst_rect.x = request.x;
    dst_rect.y = request.y;
    
    SDL_FPoint rotation_center = {
        static_cast<float>(request.pivot_x * dst_rect.w),
        static_cast<float>(request.pivot_y * dst_rect.h)
    };

    
//    SDL_FPoint rotation_center = {static_cast<float>(pivot_x), static_cast<float>(pivot_y)};
    
    Helper::SDL_RenderCopyEx(
        0,
        "",
        renderer,
        texture,
        nullptr,
        &dst_rect,
        request.rotation,
        &rotation_center,
        SDL_FLIP_NONE
    );
    
    
    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, 255);
}


void Renderer::renderPixel(PixelRenderRequest request){
    Uint8 oldR, oldG, oldB, oldA;
    SDL_GetRenderDrawColor(renderer, &oldR, &oldG, &oldB, &oldA);
        
    SDL_SetRenderDrawColor(renderer, request.color.r, request.color.g, request.color.b, request.color.a);
    
    SDL_RenderDrawPoint(renderer, request.x, request.y);
    
    SDL_SetRenderDrawColor(renderer, oldR, oldG, oldB, oldA);
    
}


void Renderer::SetPosition(float x, float y){
    current_cam_pos.x = x;
    current_cam_pos.y = y;
}

float Renderer::GetPositionX(){
    return current_cam_pos.x;
}

float Renderer::GetPositionY(){
    return current_cam_pos.y;
}

void Renderer::SetZoom(float zoomFactor){
    zoom_factor = zoomFactor;
}

float Renderer::GetZoom(){
    return zoom_factor;
}
