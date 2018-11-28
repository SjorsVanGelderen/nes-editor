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
#include "tool.h"
#include "media.h"
#include "palette.h"
#include "offset.h"
#include "idrawable.h"

struct CharacterDrawable;

class Character
{
public:
    static AppStatus Start(GLuint textureId);
    static AppStatus Stop();
    static AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);

    static bool Click(glm::vec2 mouse);
    static void Move(glm::vec2 p);
    static void Zoom(GLfloat x);

    static float     GetZoom();
    static glm::vec2 GetPosition();
    static glm::vec2 GetSize();

    static std::shared_ptr<IDrawable> GetDrawable();

    static AppStatus SetCharacter(std::vector<GLubyte> character);

    static std::vector<GLubyte> GetCharacter();
    static std::vector<GLubyte> GetPixels();

private:
    static void CharacterToTexture();
    
    static const glm::vec2 size;
    static const GLfloat   maxZoom;
    
    static float zoom;
    static float nametableZoom;
    
    static glm::vec3 position;
    static glm::vec3 nametablePosition;
    static glm::mat4 model;
    
    static GLuint programId;
    static GLuint vertexBufferId;
    static GLuint indexBufferId;
    static GLuint paletteTextureId;
    static GLuint characterTextureId;
    
    static GLint mvpUniformId;
    static GLint mouseUniformId;
    static GLint samplesUniformId;
    static GLint activeSampleUniformId;
    static GLint activeColorUniformId;
    static GLint toolUniformId;
    static GLint plottingUniformId;
    static GLint plotStartUniformId;
    static GLint paletteTextureUniformId;
    static GLint characterTextureUniformId;
    
    static std::vector<GLfloat>     vertices;
    static std::vector<GLuint>      indices;
    static std::vector<std::string> filenames;
    static std::vector<GLubyte>     character;
    static std::vector<GLubyte>     pixels;

    static std::shared_ptr<CharacterDrawable> drawable;
};

struct CharacterDrawable : public IDrawable
{
    CharacterDrawable() : IDrawable() {}

    AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse) override
    {
        return Character::Draw(projection, view, mouse);
    }

    glm::vec2 GetPosition() override
    { 
        return Character::GetPosition();
    }

    glm::vec2 GetSize() override
    {
        return Character::GetSize();
    }

    bool Click(glm::vec2 mouse) override
    {
        return Character::Click(mouse);
    }
};

#endif
