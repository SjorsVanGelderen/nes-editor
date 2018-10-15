#ifndef CHARACTER_PROP_H
#define CHARACTER_PROP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "appstatus.h"
#include "media.h"
#include "palette.h"
#include "iprop.h"
#include "offset.h"

class CharacterProp : public IProp
{
public:
    ~CharacterProp();
    
    AppStatus Setup(GLuint textureId = 0);
    AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);
    bool      Click(glm::vec2 mouse);

private:
    static GLuint programId;
    static GLuint vertexBufferId;
    static GLuint indexBufferId;
    static GLuint pixelsTextureId;
    static GLint  mvpUniformId;
    static GLint  pixelsUniformId;
    static GLint  mouseUniformId;
    
    static std::vector<GLfloat>     vertices;
    static std::vector<GLuint>      indices;
    static std::vector<std::string> filenames;
    static std::vector<GLubyte>     pixels;

    static glm::mat4 model;
};

#endif
