#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>

glm::mat4 ModelMatrix;
glm::mat4 ViewMatrix;
glm::mat4 ProjectMatrix;
glm::mat4 ViewPortMatrix;

const glm::vec3 Ambient = glm::vec3(0.5, 0.5, 0.5);

class Camera;
class Draw;
class Material;

Camera* camera;
Draw* dw;
Material* currentMat;
