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

class Samples
{
public:
    ~Samples();
    static AppStatus Setup(GLuint textureId);
    static AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);
    static bool      Click(glm::vec2 mouse);
    static glm::vec2 GetPosition();
    static glm::vec2 GetSize();
    static void      SetColor(GLubyte paletteIndex);
    static GLuint    GetActiveSample(); 
    static GLuint    GetActiveColor();
    static void      SetSamples(std::vector<GLuint> newSamples);
    
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
    static GLint  mvpUniformId;
    static GLint  mouseUniformId;
    static GLint  samplesUniformId;
    static GLint  activeSampleUniformId;
    static GLint  activeColorUniformId;
    
    static std::vector<GLfloat>     vertices;
    static std::vector<GLuint>      indices;
    static std::vector<std::string> filenames;
    
    static std::shared_ptr<std::vector<GLuint>> samples;
};

#endif
