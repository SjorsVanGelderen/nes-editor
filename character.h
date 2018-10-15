#ifndef CHARACTER_H
#define CHARACTER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

#include "app.h"
#include "appstatus.h"
#include "media.h"
#include "palette.h"
#include "offset.h"

class Character
{
public:
    ~Character();
    static AppStatus Setup(GLuint textureId);
    static AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);
    static bool      Click(glm::vec2 mouse);
    static glm::vec2 GetPosition();
    static glm::vec2 GetSize();
    static void      Move(glm::vec2 p);
    static float     GetZoom();
    static void      Zoom(GLfloat x);

    static AppStatus            SetCharacter(std::vector<GLubyte> character);
    static std::vector<GLubyte> GetCharacter();
    static std::vector<GLubyte> GetPixels();

private:
    static void CharacterToTexture();
    
    static const glm::vec2 size;
    static const GLfloat   maxZoom;
    
    static glm::vec3 position;
    static float     zoom;
    static glm::mat4 model;
    
    static GLuint programId;
    static GLuint vertexBufferId;
    static GLuint indexBufferId;
    static GLuint paletteTextureId;
    static GLuint characterTextureId;
    static GLint  mvpUniformId;
    static GLint  mouseUniformId;
    static GLint  samplesUniformId;
    static GLint  activeSampleUniformId;
    static GLint  activeColorUniformId;
    static GLint  paletteTextureUniformId;
    static GLint  characterTextureUniformId;
    
    static std::vector<GLfloat>     vertices;
    static std::vector<GLuint>      indices;
    static std::vector<std::string> filenames;
    static std::vector<GLubyte>     character;
    static std::vector<GLubyte>     pixels;
};

#endif
