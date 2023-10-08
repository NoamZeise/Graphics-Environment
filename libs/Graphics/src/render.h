#ifndef GRAPHICS_ENV_RENDERER
#define GRAPHICS_ENV_RENDERER

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <atomic>

#include <graphics/resources.h>
#include <graphics/render_config.h>
#include <graphics/shader_structs.h>

#include <glm/glm.hpp>

namespace vkenv{
    class Render;
}

namespace glenv {
    class GLRender;
}

/*
  A wrapper around OpenGL-Env and Vulkan-Env Render.
  These two share the same public interface, so this class
  will call whichever is selected as the RenderFramework
 */

enum class RenderFramework {
   Vulkan,
   OpenGL,
};


#define pFrameworkSwitch(render, vk, gl) switch(render->getRenderFramework()) { \
   case RenderFramework::Vulkan: vk; break;    \
        case RenderFramework::OpenGL: gl; break;    \
}

class Render {
public:
    Render(RenderFramework preferredRenderer);
    ~Render();
    bool NoApiLoaded() { return noApiLoaded; }
    void LoadRender(GLFWwindow *window);
    void LoadRender(GLFWwindow *window, RenderConfig renderConf);

    Resource::ResourcePool CreateResourcePool();
    void DestroyResourcePool(Resource::ResourcePool pool);
    void setResourcePoolInUse(Resource::ResourcePool pool, bool usePool);

    Resource::Texture LoadTexture(std::string filepath);
    Resource::Texture LoadTexture(Resource::ResourcePool pool, std::string filepath);
    // Load 2D image data, takes ownership of data, 4 channels
    Resource::Texture LoadTexture(unsigned char* data, int width, int height);
    Resource::Texture LoadTexture(Resource::ResourcePool pool, unsigned char* data,
				  int width, int height);
    Resource::Font LoadFont(std::string filepath);
    Resource::Model LoadModel(Resource::ModelType type, std::string filepath,
			      std::vector<Resource::ModelAnimation> *pAnimations);
    Resource::Model LoadModel(Resource::ResourcePool pool, Resource::ModelType type,
			      std::string filepath,
			      std::vector<Resource::ModelAnimation> *pAnimations);
    Resource::Model LoadModel(Resource::ModelType type, ModelInfo::Model& model,
			      std::vector<Resource::ModelAnimation> *pAnimations);
    Resource::Model LoadModel(Resource::ResourcePool pool, Resource::ModelType type,
			      ModelInfo::Model& model,
			      std::vector<Resource::ModelAnimation> *pAnimations);
    Resource::Model Load3DModel(std::string filepath);
    Resource::Model Load3DModel(ModelInfo::Model& model);
    Resource::Model LoadAnimatedModel(std::string filepath, std::vector<Resource::ModelAnimation> *pGetAnimations);
	
    void LoadResourcesToGPU();
    void LoadResourcesToGPU(Resource::ResourcePool pool);
    void UseLoadedResources();

    void Begin3DDraw();
    void BeginAnim3DDraw() ;
    void Begin2DDraw();
    
    void DrawModel(Resource::Model model, glm::mat4 modelMatrix,
		   glm::mat4 normalMatrix);
    void DrawModel(Resource::Model model, glm::mat4 modelMatrix,
		   glm::mat4 normalMatrix, glm::vec4 colour);
    void DrawAnimModel(Resource::Model model, glm::mat4 modelMatrix,
		       glm::mat4 normalMatrix,
		       Resource::ModelAnimation *animation);
    void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix,
		  glm::vec4 colour, glm::vec4 texOffset);
    void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix,
		  glm::vec4 colour);
    void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix);
    void DrawString(Resource::Font font, std::string text, glm::vec2 position,
		    float size, float depth, glm::vec4 colour, float rotate);
    void DrawString(Resource::Font font, std::string text, glm::vec2 position,
		    float size, float depth, glm::vec4 colour);
    float MeasureString(Resource::Font font, std::string text, float size);
    void EndDraw(std::atomic<bool> &submit);
    
    void FramebufferResize();
    
    void set3DViewMatrixAndFov(glm::mat4 view, float fov, glm::vec4 camPos);
    void set2DViewMatrixAndScale(glm::mat4 view, float scale);
    void setLightingProps(BPLighting lighting);

    void setRenderConf(RenderConfig renderConf);
    RenderConfig getRenderConf();
    void setTargetResolution(glm::vec2 resolution);
    glm::vec2 getTargetResolution();

  RenderFramework getRenderFramework() {
    return renderer;
  }

private:
  RenderFramework renderer;
  bool noApiLoaded = false;
  vkenv::Render *vkRender;
  glenv::GLRender *glRender;
};

#endif
