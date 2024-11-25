#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include <algorithm>
#include <iostream>

class Camera
{
  private:
    glm::vec3 target;
    glm::vec3 up;
    float distance;
    float pitch; 
    float heading;

    int buttons_mouse;  
    glm::vec2 origin_mouse;

    int screen_width;
    int screen_height;


  public:
    Camera() ;
    Camera(const Camera&) = default;
    Camera(Camera &&) = default;
    glm::mat4 projection() const;
    glm::mat4 view() const;

    void common_reshape(int w, int h);
    void common_mouse (int button, int action, int x, int y);
    void common_motion(int x, int y);

    int height() const;
    int width() const;

    glm::vec3 position() const;
    glm::vec3 lookat() const;

};

#endif
