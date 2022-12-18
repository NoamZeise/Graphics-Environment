#ifndef GRAPHICS_ENV_RENDERER
#define GRAPHICS_ENV_RENDERER

#include "vulkan-render/render.h"

//#include "opengl-render/render.h"

//temp until og one working
namespace glenv {
    class Render {
        public:
        Render(GLFWwindow *window) {}
        Render(GLFWwindow *window, glm::vec2 target) {}
        ~Render() {}
        static bool LoadOpenGL() { return false; }

        Resource::Texture LoadTexture(std::string filepath) { return Resource::Texture(); }
        Resource::Font LoadFont(std::string filepath) {return Resource::Font(); }
        Resource::Model LoadModel(std::string filepath) { return Resource::Model(); }
        Resource::Model LoadAnimatedModel(std::string filepath,
                                                      std::vector<Resource::ModelAnimation> *pGetAnimations) { return Resource::Model(); }

        void LoadResourcesToGPU() {}
        void UseLoadedResources() {}

        void Begin3DDraw() {}
        void BeginAnim3DDraw() {}
        void Begin2DDraw() {}
        void DrawModel(Resource::Model model, glm::mat4 modelMatrix,
                       glm::mat4 normalMatrix) {}
        void DrawAnimModel(Resource::Model model, glm::mat4 modelMatrix,
                           glm::mat4 normalMatrix,
                           Resource::ModelAnimation *animation) {}
        void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix,
                      glm::vec4 colour, glm::vec4 texOffset){}
        void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix,
                      glm::vec4 colour) {}
        void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix) {}
        void DrawString(Resource::Font font, std::string text,
                        glm::vec2 position, float size, float depth,
                        glm::vec4 colour, float rotate){}
        void DrawString(Resource::Font font, std::string text,
                        glm::vec2 position, float size, float depth,
                        glm::vec4 colour) {}
        float MeasureString(Resource::Font font, std::string text, float size){ return 0.0f; }
        void EndDraw(std::atomic<bool> &submit){}

        void FramebufferResize(){}

        void set3DViewMatrixAndFov(glm::mat4 view, float fov, glm::vec4 camPos){}
        void set2DViewMatrixAndScale(glm::mat4 view, float scale){}
        void setLightDirection(glm::vec4 lightDir){}
        void setForceTargetRes(bool force) {}
        bool isTargetResForced() { return false; }
        void setTargetResolution(glm::vec2 resolution) {}
        glm::vec2 getTargetResolution() { return glm::vec2(1, 1); }
        void setVsync(bool vsync) {}
    };
}

#include <string>
#include <vector>

enum class RenderFramework {
   VULKAN,
   OPENGL,
};

#define _RENDER_NO_FN(vk, gl) switch(renderer) { \
        case RenderFramework::VULKAN: vk; break;    \
        case RenderFramework::OPENGL: gl; break;    \
    }

#define _RENDER_FN(...) switch(renderer) { \
        case RenderFramework::VULKAN: return vkRender->__VA_ARGS__; break;    \
        case RenderFramework::OPENGL: return glRender->__VA_ARGS__; break;    \
    }


class Render {
    public:
        Render() { Render(VULKAN); }
        Render(RenderFramework preferredRenderer);
        ~Render() {  _RENDER_NO_FN(delete vkRender, delete glRender); }
        bool NoApiLoaded() { return noApiLoaded; }
        void LoadRender(GLFWwindow *window) { _RENDER_NO_FN(
                vkRender = new vkenv::Render(window),
                glRender = new glenv::Render(window)) }

        void LoadRender(GLFWwindow *window, glm::vec2 target) { _RENDER_NO_FN(
                vkRender = new vkenv::Render(window, target),
                glRender = new glenv::Render(window, target)) }

        Resource::Texture LoadTexture(std::string filepath) { _RENDER_FN(LoadTexture(filepath)) }
        Resource::Font LoadFont(std::string filepath) { _RENDER_FN(LoadFont(filepath)) }
        Resource::Model LoadModel(std::string filepath) { _RENDER_FN(LoadModel(filepath))}
        Resource::Model LoadAnimatedModel(std::string filepath,
                                          std::vector<Resource::ModelAnimation> *pGetAnimations)
        {_RENDER_FN(LoadAnimatedModel(filepath, pGetAnimations))}

        void LoadResourcesToGPU() {_RENDER_FN(LoadResourcesToGPU())}
        void UseLoadedResources() {_RENDER_FN(UseLoadedResources())}

        void Begin3DDraw() { _RENDER_FN(Begin3DDraw())}
        void BeginAnim3DDraw() {_RENDER_FN(BeginAnim3DDraw())}
        void Begin2DDraw() {_RENDER_FN(Begin2DDraw())}
        void DrawModel(Resource::Model model, glm::mat4 modelMatrix,
                       glm::mat4 normalMatrix) {_RENDER_FN(DrawModel(model, modelMatrix, normalMatrix))}
        void DrawAnimModel(Resource::Model model, glm::mat4 modelMatrix,
                           glm::mat4 normalMatrix,
                           Resource::ModelAnimation *animation) {_RENDER_FN(DrawAnimModel(model, modelMatrix, normalMatrix, animation))}
        void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix,
                      glm::vec4 colour, glm::vec4 texOffset) {_RENDER_FN(DrawQuad(texture, modelMatrix, colour, texOffset))}
        void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix,
                      glm::vec4 colour){_RENDER_FN(DrawQuad(texture, modelMatrix, colour))}
        void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix) {_RENDER_FN(DrawQuad(texture, modelMatrix))}
        void DrawString(Resource::Font font, std::string text,
                        glm::vec2 position, float size, float depth,
                        glm::vec4 colour, float rotate) {_RENDER_FN(DrawString(font, text, position, size, depth, colour, rotate))}
        void DrawString(Resource::Font font, std::string text,
                        glm::vec2 position, float size, float depth,
                        glm::vec4 colour) {_RENDER_FN(DrawString(font, text, position, size, depth, colour))}
        float MeasureString(Resource::Font font, std::string text, float size) {_RENDER_FN(MeasureString(font, text, size))}
        void EndDraw(std::atomic<bool> &submit) {_RENDER_FN(EndDraw(submit))}

        void FramebufferResize() {_RENDER_FN(FramebufferResize())}

        void set3DViewMatrixAndFov(glm::mat4 view, float fov, glm::vec4 camPos){_RENDER_FN(set3DViewMatrixAndFov(view, fov, camPos))}
        void set2DViewMatrixAndScale(glm::mat4 view, float scale) {_RENDER_FN(set2DViewMatrixAndScale(view, scale))}
        void setLightDirection(glm::vec4 lightDir) { _RENDER_FN(setLightDirection(lightDir))}
        void setForceTargetRes(bool force) { _RENDER_FN(setForceTargetRes(force))}
        bool isTargetResForced() { _RENDER_FN(isTargetResForced())}
        void setTargetResolution(glm::vec2 resolution) { _RENDER_FN(setTargetResolution(resolution))}
        glm::vec2 getTargetResolution() {_RENDER_FN(getTargetResolution())}
        void setVsync(bool vsync) { _RENDER_FN(setVsync(vsync))}

      private:

        RenderFramework renderer;
        bool noApiLoaded = false;
        vkenv::Render* vkRender;
        glenv::Render* glRender;
};

#endif
