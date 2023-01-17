#ifndef APP_H
#define APP_H

#include <atomic>
#include <thread>

#include "render.h"

#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <input.h>
#include <timer.h>

#include "camera.h"

//#define TIME_APP_DRAW_UPDATE
//#define MULTI_UPDATE_ON_SLOW_DRAW

class App {
public:
  App(RenderFramework defaultFramework);
  ~App();
  void run();
  void resize(int windowWidth, int windowHeight);

#pragma region GLFW_CALLBACKS
  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow *window, double xoffset,
                              double yoffset);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mode);
  static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                    int mods);
  static void error_callback(int error, const char *description);
#pragma endregion
  Input input;

private:
  void loadAssets();
  void update();
  void controls();
  void postUpdate();
  void draw();

  void loadTestScene1(std::atomic<bool> &loaded);
  void drawTestScene1();
  void loadTestScene2(std::atomic<bool> &loaded);
  void drawTestScene2();

  const bool FIXED_WINDOW_RATIO = false;

  enum class Scene {
    Test1,
    Test2,
  };
  Scene current = Scene::Test1;

  glm::vec2 correctedPos(glm::vec2 pos);
  glm::vec2 correctedMouse();

  const int INITIAL_WINDOW_WIDTH = 1000;
  const int INITIAL_WINDOW_HEIGHT = 500;

  GLFWwindow *mWindow;
  Render *mRender;
  int mWindowWidth, mWindowHeight;
  Input previousInput;
  gamehelper::Timer timer;
  Camera::FirstPerson fpcam;

  glm::vec4 lightDir = glm::vec4(0.0f, -0.5f, -1.0f, 0.0f);
  float rotate = 0.0f;

  std::thread submitDraw;
  std::atomic<bool> finishedDrawSubmit;
  std::thread assetLoadThread;
  std::atomic<bool> assetsLoaded;

  bool sceneChangeInProgress = false;

  Resource::Model testModel1;
  Resource::Model monkeyModel1;
  Resource::Model colouredCube1;
  Resource::Texture testTex1;
  Resource::Font testFont1;

  Resource::Model testModel2;
  Resource::Model monkeyModel2;
  Resource::Model colouredCube2;
  Resource::Texture testTex2;
  Resource::Font testFont2;
};

#endif
