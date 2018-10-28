#ifndef MEDIA_H
#define MEDIA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <map>

#include "appstatus.h"
#include "debug.h"
#include "character.h"

class Character;

class Media
{
public:
    static std::pair<AppStatus, GLuint> LoadShaderProgram(std::vector<std::string> filenames);
    
    // static std::pair<AppStatus, std::vector<GLubyte>> LoadSamples();
    // static std::pair<AppStatus, std::vector<GLubyte>> LoadCharacter();
    
    static AppStatus SaveSamples();
    static AppStatus LoadSamples();
    static AppStatus SaveCharacter();
    static AppStatus LoadCharacter();

private:
    static std::pair<AppStatus, GLuint> LoadShader(std::string filename);

    /* static std::vector<GLuint>           shaders; */
    static std::map<std::string, GLuint> shaderPrograms;
};

#endif
