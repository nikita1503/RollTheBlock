#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include <thread>
#include <ao/ao.h>
#include <mpg123.h>
#include <time.h>


#include <list>

void update_level();
void rotate();
void* play_audio(string audioFile);
