#ifndef APP_H
#define APP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <math.h>

#include "appstatus.h"
#include "appmode.h"
#include "media.h"
#include "debug.h"
#include "palette.h"
#include "samples.h"
#include "character.h"
#include "nametable.h"

class Palette;
class Samples;
class Character;
class Nametable;

class App
{
public:
    static AppStatus Start();
    static GLfloat   GetAspect();
    static glm::vec2 GetSize();
    static glm::vec2 GetFrustumSize();
    static glm::vec2 ScreenToSurface(glm::vec2 point, glm::vec2 position, glm::vec2 size, float zoom = 1.0f);
    static AppMode   GetMode();

private:
    static AppStatus Update();
    static AppStatus DrawPalette(bool* clickConsumed);
    static AppStatus DrawSamples(bool* clickConsumed);
    static AppStatus DrawCharacter(bool* clickConsumed);
    static AppStatus DrawNametable(bool* clickConsumed);
    static AppStatus Stop();
    static AppStatus StartGLFW();
    static AppStatus StartGLEW();
    static AppStatus StartGL();
    static void      GLFWCursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY);
    static void      GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
    static void      GLFWScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
    
    static const std::string CAPTION;

    static AppMode mode;

    static bool dragging;
    static bool canSave;
    static bool canLoad;
    static bool newClick;
    
    static std::unique_ptr<Palette>   palette;
    static std::unique_ptr<Samples>   samples;
    static std::unique_ptr<Character> character;
    static std::unique_ptr<Nametable> nametable;

    static glm::vec2 size;
    static glm::vec2 frustumSize;
    static GLfloat   aspect;
    static glm::mat4 projection;
    static glm::mat4 view;
    
    static glm::vec2 mouse;
    static glm::vec2 click;
    
    static GLuint vaoId;
    
    static std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> window;
};

#endif
