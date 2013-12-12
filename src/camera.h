//
// Camera Class from the base code of Project 6 of UPenn CIS-565 Fall 2013
//

#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    Camera(glm::vec3 start_pos, glm::vec3 start_dir, glm::vec3 up) : 
        pos(start_pos.x, start_pos.y), z(start_pos.z), up(up), 
        start_dir(start_dir), start_left(glm::cross(start_dir,up)), rx(0), ry(0) { }
    
    void adjust(float dx, float dy, float dz, float tx, float ty, float tz);
    glm::vec3 get_pos();
    glm::mat4x4 get_view();

    float rx;
    float ry;
    float z;
    glm::vec2 pos;
    glm::vec3 up;
    glm::vec3 start_left;
    glm::vec3 start_dir;
};

#endif