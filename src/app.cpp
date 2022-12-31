#include "app.h"

#include <glmhelper.h>
#include <iostream>

App::App(RenderFramework defaultFramework) {

  mWindowWidth = INITIAL_WINDOW_WIDTH;
  mWindowHeight = INITIAL_WINDOW_HEIGHT;

  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    throw std::runtime_error("failed to initialise glfw!");

  mRender = new Render(defaultFramework);

  if(mRender->NoApiLoaded()) {
    throw std::runtime_error("failed to load any graphics apis");
  }

  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "App", nullptr, nullptr);
  if (!mWindow)
  {
    glfwTerminate();
    throw std::runtime_error("failed to create glfw window!");
  }

  glfwSetWindowUserPointer(mWindow, this);
  glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
  glfwSetCursorPosCallback(mWindow, mouse_callback);
  glfwSetScrollCallback(mWindow, scroll_callback);
  glfwSetKeyCallback(mWindow, key_callback);
  glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, glfwRawMouseMotionSupported());

  int width = mWindowWidth;
  int height = mWindowHeight;

  mRender->LoadRender(mWindow, glm::vec2(mWindowWidth, mWindowHeight));

  if (FIXED_WINDOW_RATIO)
    glfwSetWindowAspectRatio(mWindow, width, height);

  loadAssets();

  fpcam = Camera::FirstPerson(glm::vec3(3.0f, 0.0f, 2.0f));
  finishedDrawSubmit = true;
}

App::~App() {
  if (submitDraw.joinable())
    submitDraw.join();
  delete mRender;
  glfwTerminate();
}

void App::loadAssets() {
    if(current == Scene::Test1)
	loadTestScene1(assetsLoaded);
    if(current == Scene::Test2)
	loadTestScene2(assetsLoaded);
    mRender->LoadResourcesToGPU();
    mRender->UseLoadedResources();
}

void App::run() {
  while (!glfwWindowShouldClose(mWindow)) {
    update();
    if (mWindowWidth != 0 && mWindowHeight != 0)
      draw();
  }
}

void App::resize(int windowWidth, int windowHeight) {
  if (submitDraw.joinable())
    submitDraw.join();
  this->mWindowWidth = windowWidth;
  this->mWindowHeight = windowHeight;
  if (mRender != nullptr && mWindowWidth != 0 && mWindowHeight != 0)
    mRender->FramebufferResize();
}

void App::update() {
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif
  glfwPollEvents();

  controls();

  if(sceneChangeInProgress && assetsLoaded) {
      assetLoadThread.join();
      if(submitDraw.joinable()) 
	  submitDraw.join();
      std::cout << "loading done\n";
      mRender->LoadResourcesToGPU();
      mRender->UseLoadedResources();
      sceneChangeInProgress = false;
      current = current == Scene::Test1 ? Scene::Test2 : Scene::Test1;
  }

  rotate += timer.FrameElapsed() * 0.001f;
  fpcam.update(input, previousInput, timer);

  postUpdate();
#ifdef TIME_APP_DRAW_UPDATE
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "update: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
#endif
}

void App::controls()
{
  if (input.Keys[GLFW_KEY_F] && !previousInput.Keys[GLFW_KEY_F]) {
    if (glfwGetWindowMonitor(mWindow) == nullptr) {
	    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	    glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mode->width,
                           mode->height, mode->refreshRate);
    } else {
	    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	    glfwSetWindowMonitor(mWindow, NULL, 0, 0, mWindowWidth, mWindowHeight,
                           mode->refreshRate);
    }
  }
  if (input.Keys[GLFW_KEY_ESCAPE] && !previousInput.Keys[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
  }
  const float speed = 0.001f;
  if (input.Keys[GLFW_KEY_INSERT]) {
    lightDir.x += speed * timer.FrameElapsed();
  }
  if (input.Keys[GLFW_KEY_HOME]) {
    lightDir.x -= speed * timer.FrameElapsed();
  }
  if (input.Keys[GLFW_KEY_DELETE]) {
    lightDir.z += speed * timer.FrameElapsed();
  }
  if (input.Keys[GLFW_KEY_END]) {
    lightDir.z -= speed * timer.FrameElapsed();
  }
  if (input.Keys[GLFW_KEY_PAGE_UP]) {
    lightDir.y += speed * timer.FrameElapsed();
  }
  if (input.Keys[GLFW_KEY_PAGE_DOWN]) {
    lightDir.y -= speed * timer.FrameElapsed();
  }

  if (input.Keys[GLFW_KEY_G]) {
      mRender->setTargetResolution(glm::vec2(1000, 100));
  }
  if (input.Keys[GLFW_KEY_H]) {
      mRender->setForceTargetRes(false);
  }
  if (input.Keys[GLFW_KEY_V]) {
      mRender->setVsync(true);
  }
   if (input.Keys[GLFW_KEY_B]) {
       mRender->setVsync(false);
  }
   if(!sceneChangeInProgress) {
       if(input.Keys[GLFW_KEY_1]) {
	   if(current != Scene::Test1) {
	       assetsLoaded = false;
		   assetLoadThread = std::thread(&App::loadTestScene1, this, std::ref(assetsLoaded));
	       sceneChangeInProgress = true;
	   }
       }
       if(input.Keys[GLFW_KEY_2]) {
	   if(current != Scene::Test2) {
	       assetsLoaded = false;
	       assetLoadThread = std::thread(&App::loadTestScene2, this, std::ref(assetsLoaded));
	       sceneChangeInProgress = true;
	   }
       }
   }
   mRender->setLightDirection(lightDir);
}

void App::postUpdate() {
  previousInput = input;
  input.offset = 0;
  timer.Update();
  mRender->set3DViewMatrixAndFov(fpcam.getViewMatrix(), fpcam.getZoom(),
                                 glm::vec4(fpcam.getPos(), 0.0));
}

void App::draw() {
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif

#ifdef MULTI_UPDATE_ON_SLOW_DRAW
  if (!finishedDrawSubmit)
    return;
  finishedDrawSubmit = false;
#endif
  if (submitDraw.joinable())
    submitDraw.join();

  if(current==Scene::Test1)
      drawTestScene1();
  if(current==Scene::Test2)
      drawTestScene2();

  if(mRender->getRenderFramework() == RenderFramework::VULKAN) {
      submitDraw = std::thread(&Render::EndDraw, mRender, std::ref(finishedDrawSubmit));
  } else {
      mRender->EndDraw(finishedDrawSubmit);
      finishedDrawSubmit = true;
  }

#ifdef TIME_APP_DRAW_UPDATE
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "draw: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
#endif
}

glm::vec2 App::correctedPos(glm::vec2 pos)
{
  if (mRender->isTargetResForced())
      return glm::vec2(
	pos.x * (mRender->getTargetResolution().x / (float)mWindowWidth),
	pos.y * (mRender->getTargetResolution().y / (float)mWindowHeight));

  return glm::vec2(pos.x, pos.y);
}

glm::vec2 App::correctedMouse()
{
  return correctedPos(glm::vec2(input.X, input.Y));
}

void App::loadTestScene1(std::atomic<bool> &loaded) {
  testModel1 = mRender->LoadModel("models/testScene.fbx");
  monkeyModel1 = mRender->LoadModel("models/monkey.obj");
  colouredCube1 = mRender->LoadModel("models/ROOM.fbx");
  testTex1 = mRender->LoadTexture("textures/error.png");
  testFont1 = mRender->LoadFont("textures/Roboto-Black.ttf");
  loaded = true;
}

void App::drawTestScene1() {

  mRender->Begin3DDraw();

  auto model = glm::translate(
      glm::scale(
          glm::rotate(glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0, 0, 1)),
                      glm::radians(270.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
          glm::vec3(1.0f)),
      glm::vec3(0, 3, 0));

  mRender->DrawModel(monkeyModel1, model, glm::inverseTranspose(model));

  model = glm::translate(
      glm::scale(
          glm::rotate(glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0, 0, 1)),
                      glm::radians(270.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
          glm::vec3(0.01f)),
      glm::vec3(0, 0, 0));

  mRender->DrawModel(testModel1, model, glm::inverseTranspose(model));

  model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(0.0f, -30.0f, -15.0f)),
                                 glm::radians(270.0f),
                                 glm::vec3(-1.0f, 0.0f, 0.0f)),
                     glm::vec3(4.0f));
  mRender->DrawModel(colouredCube1, model, glm::inverseTranspose(model));

  mRender->Begin2DDraw();


  mRender->DrawString(testFont1, "Scene 1", glm::vec2(10, 100), 40, -1.0f, glm::vec4(1), 0.0f);
  if(sceneChangeInProgress) {
    mRender->DrawString(testFont1, "Loading", glm::vec2(200, 400), 40, -1.0f, glm::vec4(1), 0.0f);
   }

     mRender->DrawQuad(
      testTex1, glmhelper::getModelMatrix(glm::vec4(400, 100, 100, 100), 0, -1),
      glm::vec4(1), glm::vec4(0, 0, 1, 1));

  mRender->DrawQuad(testTex1,
                    glmhelper::getModelMatrix(glm::vec4(0, 0, 400, 400), 0, 0),
                    glm::vec4(1, 0, 1, 0.3), glm::vec4(0, 0, 1, 1));
}

void App::loadTestScene2(std::atomic<bool> &loaded) {
  monkeyModel2 = mRender->LoadModel("models/monkey.obj");
  colouredCube2 = mRender->LoadModel("models/ROOM.fbx");
  testFont2 = mRender->LoadFont("textures/Roboto-Black.ttf");
  loaded = true;
}

void App::drawTestScene2() {
    mRender->Begin3DDraw();
  auto model = glm::translate(
      glm::scale(
          glm::rotate(glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0, 0, 1)),
                      glm::radians(270.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
          glm::vec3(1.0f)),
      glm::vec3(0, 2, 0));

  mRender->DrawModel(monkeyModel2, model, glm::inverseTranspose(model));
  model = glm::translate(
      glm::scale(
          glm::rotate(glm::rotate(glm::mat4(1.0f), rotate * 0.5f, glm::vec3(0, 0, 1)),
                      glm::radians(270.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
          glm::vec3(1.0f)),
      glm::vec3(1, 2, 0));
    mRender->DrawModel(monkeyModel2, model, glm::inverseTranspose(model));
    model = glm::translate(
      glm::scale(
          glm::rotate(glm::rotate(glm::mat4(1.0f), rotate * 2.0f, glm::vec3(0, 0, 1)),
                      glm::radians(270.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
          glm::vec3(1.0f)),
      glm::vec3(2, 2, 0));
    mRender->DrawModel(monkeyModel2, model, glm::inverseTranspose(model));

      model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(0.0f, -30.0f, -15.0f))

                                     ,
                                 glm::radians(270.0f),
                                 glm::vec3(-1.0f, 0.0f, 0.0f)),
                     glm::vec3(1.0f));

      mRender->DrawModel(colouredCube2, model, glm::inverseTranspose(model));
        model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(0.0f, -30.0f, -15.0f))

                                     ,
                                 glm::radians(270.0f),
                                 glm::vec3(-1.0f, 0.0f, 0.0f)),
                     glm::vec3(1.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  mRender->DrawModel(colouredCube2, model, glm::inverseTranspose(model));
        model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(0.0f, -30.0f, -15.0f))

                                     ,
                                 glm::radians(270.0f),
                                 glm::vec3(-1.0f, 0.0f, 0.0f)),
                     glm::vec3(1.0f));
  mRender->DrawModel(colouredCube2, model, glm::inverseTranspose(model));
      

  mRender->Begin2DDraw();

  mRender->DrawString(testFont2, "Scene 2", glm::vec2(10, 100), 40, -0.4f, glm::vec4(1), 0.0f);

  if(sceneChangeInProgress) {
      mRender->DrawString(testFont2, "Loading", glm::vec2(200, 400), 40, -0.4f, glm::vec4(1), 0.0f);
  }    
}



/*
 *       GLFW CALLBACKS
 */

void App::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->resize(width, height);
}

void App::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->input.X = xpos;
  app->input.Y = ypos;
}
void App::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->input.offset = yoffset;
}

void App::key_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mode) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      app->input.Keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      app->input.Keys[key] = false;
    }
  }
}

void App::mouse_button_callback(GLFWwindow *window, int button, int action,
                                int mods) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

  if (button >= 0 && button < 8) {
    if (action == GLFW_PRESS) {
      app->input.Buttons[button] = true;
    } else if (action == GLFW_RELEASE) {
      app->input.Buttons[button] = false;
    }
  }
}

void App::error_callback(int error, const char *description) {
  throw std::runtime_error(description);
}
