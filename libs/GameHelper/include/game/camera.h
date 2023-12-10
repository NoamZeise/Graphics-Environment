#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include "input.h"
#include "timer.h"

namespace camera {
  
  class FirstPerson {
  public:
      FirstPerson() { _position = glm::vec3(0.0f, 0.0f, 0.0f); };
      FirstPerson(glm::vec3 position);
      glm::mat4 getViewMatrix();
      float getZoom();
      glm::vec3 getPos() { return _position; }
      void setPos(glm::vec3 pos);
      virtual void update(Input &input, Timer &timer);
  protected:
      glm::vec3 _position;
      glm::vec3 _front;
      glm::vec3 _up;
      glm::vec3 _right;
      glm::vec3 _worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
      glm::mat4 view = glm::mat4(1.0f);
      bool viewUpdated = true;

      float _yaw = 200.0f;
      float _pitch = -20.0f;

      float _speed = 0.01f;
      float _sensitivity = 0.05f;
      float _zoom = 45.0f;

      void calculateVectors();
  };


} // namesapce end

#endif
