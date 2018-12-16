#ifndef SAMPLES_H
#define SAMPLES_H

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
#include "idrawable.h"

struct SamplesDrawable;

class Samples
{
public:
  static AppStatus Start(GLuint textureId);
  static AppStatus Stop();
  static AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);
  
  static bool Click(glm::vec2 mouse);
  static bool Release(glm::vec2 mouse);

  static void SetColor(GLubyte paletteIndex);
  static void SetSamples(std::vector<GLuint> newSamples);

  static GLuint    GetActiveSample(); 
  static GLuint    GetActiveColor();
  static glm::vec2 GetPosition();
  static glm::vec2 GetSize();

  static std::shared_ptr<IDrawable> GetDrawable();
  
  static std::shared_ptr<std::vector<GLuint>> GetSamples();

private:
  static const glm::vec2 size;
  
  static glm::vec3 position;
  static glm::mat4 model;
  
  static GLuint activeSample;
  static GLuint activeColor;
  
  static GLuint programId;
  static GLuint vertexBufferId;
  static GLuint indexBufferId;
  static GLuint paletteTextureId;
  
  static GLint mvpUniformId;
  static GLint mouseUniformId;
  static GLint samplesUniformId;
  static GLint activeSampleUniformId;
  static GLint activeColorUniformId;
  
  static std::vector<GLfloat>     vertices;
  static std::vector<GLuint>      indices;
  static std::vector<std::string> filenames;
  
  static std::shared_ptr<std::vector<GLuint>> samples;

  static std::shared_ptr<SamplesDrawable> drawable;
};

struct SamplesDrawable : public IDrawable
{
  SamplesDrawable() : IDrawable() {}

  AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse) override
  {
    return Samples::Draw(projection, view, mouse);
  }

  glm::vec2 GetPosition() override
  { 
    return Samples::GetPosition();
  }

  glm::vec2 GetSize() override
  {
    return Samples::GetSize();
  }

  bool Click(glm::vec2 mouse) override
  {
    return Samples::Click(mouse);
  }

  bool Release(glm::vec2 mouse) override
  {
    return Samples::Release(mouse);
  }
};

#endif
