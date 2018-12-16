#ifndef PALETTE_H
#define PALETTE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

#include "app.h"
#include "appstatus.h"
#include "media.h"
#include "samples.h"
#include "idrawable.h"

class Samples;
struct PaletteDrawable;

class Palette
{
public:
  static AppStatus Start();
  static AppStatus Stop();
  static AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);
  
  static bool Click(glm::vec2 mouse);
  static bool Release(glm::vec2 mouse);
  
  static glm::vec2 GetPosition();
  static glm::vec2 GetSize();
  static GLuint    GetPaletteTextureId();

  static std::shared_ptr<IDrawable> GetDrawable();
  
  static const std::vector<GLubyte> paletteRGB;

private:
  static const glm::vec2 size;
  
  static glm::mat4 model;
  static glm::vec3 position;
  
  static GLuint programId;
  static GLuint vertexBufferId;
  static GLuint indexBufferId;
  static GLuint paletteTextureId;
  
  static GLint mvpUniformId;
  static GLint mouseUniformId;
  
  static std::vector<GLfloat>     vertices;
  static std::vector<GLuint>      indices;
  static std::vector<std::string> filenames;
  static std::vector<GLfloat>     pixels;

  static std::shared_ptr<PaletteDrawable> drawable;
};

struct PaletteDrawable : public IDrawable
{
  PaletteDrawable() : IDrawable() {}

  AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse) override
  {
    return Palette::Draw(projection, view, mouse);
  }

  glm::vec2 GetPosition() override
  { 
    return Palette::GetPosition();
  }

  glm::vec2 GetSize() override
  {
    return Palette::GetSize();
  }

  bool Click(glm::vec2 mouse) override
  {
    return Palette::Click(mouse);
  }

  bool Release(glm::vec2 mouse) override
  {
    return Palette::Release(mouse);
  }
};

#endif
