#ifndef NAMETABLE_H
#define NAMETABLE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

#include "appstatus.h"
#include "media.h"
#include "offset.h"
#include "app.h"

class App;

class Nametable
{
public:
    ~Nametable();
    static AppStatus Setup(GLuint textureId);
    static void      Zoom(float x);
    static AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);
    static bool Click(glm::vec2 mouse);
    static glm::vec2 GetSize();
    static glm::vec2 GetPosition();
    static GLfloat   GetZoom();

private:
    static const glm::vec2 size;
    static const GLfloat   maxZoom;
    
    static glm::vec3 position;
    static GLfloat   zoom;

    static GLuint programId;
    static GLuint vertexBufferId;
    static GLuint indexBufferId;
    static GLuint paletteTextureId;
    static GLint  mvpUniformId;
    static GLint  mouseUniformId;

    static std::vector<GLfloat>     vertices;
    static std::vector<GLuint>      indices;
    static std::vector<std::string> filenames;
    static std::vector<GLfloat>     pixels;

    static glm::mat4 model;
};

#endif
