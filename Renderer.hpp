//
//  Renderer.hpp
//  game_engine
//
//  Created by Stanley  on 2/19/25.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <iostream>
#include <vector>
#include <queue>
#include "Actor.hpp"
 

class ImageDB{
public:
    static SDL_Texture* loadImageTexture(SDL_Renderer* renderer, const std::string& image_name);
    static void clearCache();
    
    static void CreateDefaultParticletextureWithName(const std::string& name);
    
private:
    static inline std::unordered_map<std::string, SDL_Texture*> imageCache;
};


class FontDB{
public:
    static TTF_Font* loadFontTexture(SDL_Renderer* renderer, const std::string& font_name, int font_size);
    static void clearCache();
    
private:
    static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fontCache;
};


class AudioDB{
public:
    static Mix_Chunk* loadAudio(const std::string& audio_name);
    static void clearCache();
    
private:
    static inline std::unordered_map<std::string, Mix_Chunk*> audioCache;
};


class TextRendereRequest{
public:
    std::string text;
    std::string font;
    SDL_Color color;
    float size;
    float x;
    float y;
};


class ImageRenderRequest{
public:
    std::string image;
    float x;
    float y;
    SDL_Color color = {255,255,255,255};
    int sorting_order = 0;
    int rotation = 0;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float pivot_x = 0.5f;
    float pivot_y = 0.5f;
    
    int request_order = 0;
};

class PixelRenderRequest{
public:
    int x, y;
    SDL_Color color;
};


struct orderComparator {
    bool operator()(const ImageRenderRequest& a, const ImageRenderRequest& b) const {
        
        if(a.sorting_order == b.sorting_order){
            return a.request_order > b.request_order;
        }

        return a.sorting_order > b.sorting_order;
    }
};


class Renderer{
public:
    Renderer();
    ~Renderer();
    
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    
    static std::string game_title;
    
    static glm::vec2 window_size;
    static glm::vec2 current_cam_pos;
    
    static Uint8 clear_color_r;
    static Uint8 clear_color_g;
    static Uint8 clear_color_b;
    static Uint8 clear_color_a;
    
    static float zoom_factor;
    static float cam_ease_factor;
    
    static glm::vec2 cam_offset;
    
    static void render();
    
    static int request_order;
    
    static void DrawText(const std::string& text_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a);
    
    static void PlayAudio(int channel, std::string clip_name, bool does_loop);
    
    static void HaltAudio(int channel);
    
    static void SetAudioVolume(int channel, float volume);
    
    static void DrawUI(std::string image_name, float x, float y);
    
    static void DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order);
    
    static void Draw(std::string image_name, float x, float y);
    
    static void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);
    
    
    static void DrawPixel(float x, float y, float r, float g, float b, float a);
    
    static void SetPosition(float x, float y);
    
    static float GetPositionX();
    
    static float GetPositionY();
    
    static void SetZoom(float zoomFactor);
    
    static float GetZoom();
    
    static SDL_Renderer* getRenderer(){return renderer;}
        
private:
    static inline std::queue<TextRendereRequest> textToDraw;
    
    static inline std::priority_queue<ImageRenderRequest, std::vector<ImageRenderRequest>, orderComparator> sceneToDraw;
    
    static inline std::priority_queue<ImageRenderRequest, std::vector<ImageRenderRequest>, orderComparator> UIToDraw;
    
    static inline std::queue<PixelRenderRequest> pixelToDraw;
    
    static void renderText(TextRendereRequest request);
    
    static void renderScene(ImageRenderRequest request);
    
    static void renderUI(ImageRenderRequest request);
    
    static void renderPixel(PixelRenderRequest request);
    
    
};

#endif /* Renderer_hpp */
