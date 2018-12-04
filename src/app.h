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
#include "interactionmode.h"
#include "tool.h"
#include "media.h"
#include "debug.h"
#include "palette.h"
#include "samples.h"
#include "character.h"
#include "nametable.h"
#include "button.h"
#include "idrawable.h"

class Palette;
class Samples;
class Character;
class Nametable;
class Button;

class App
{
  public:
    static AppStatus Start();
    
    static AppMode   GetMode();    
    static Tool      GetTool();
    static bool      GetPlotting();
    static glm::vec2 GetPlotStart();
    static GLfloat   GetAspect();
    static glm::vec2 GetSize();
    static glm::vec2 GetFrustumSize();

    static void SetTool(Tool t);

    static glm::vec2 ScreenToSurface(
        glm::vec2 point,
        glm::vec2 position,
        glm::vec2 size,
        GLfloat zoom = 1.0f
        );

  private:
    static AppStatus StartGLFW();
    static AppStatus StartGLEW();
    static AppStatus StartGL();
    static AppStatus Stop();
    
    static AppStatus Update();
    
    static AppStatus UpdateDrawable
        ( bool* clickConsumed
        , bool* releaseConsumed
        , std::shared_ptr<IDrawable> drawable
        , bool zoomable = false
        );
    
    static void GLFWCursorPositionCallback(
        GLFWwindow* window,
        double mouseX,
        double mouseY
        );
    
    static void GLFWMouseButtonCallback(
        GLFWwindow* window,
        int button,
        int action,
        int mode
        );
    
    static void GLFWScrollCallback(
        GLFWwindow* window,
        double offsetX,
        double offsetY
        );
    
    static const std::string CAPTION;

    static AppMode         mode;
    static InteractionMode interactionMode;
    
    static Tool tool;
    static bool dragging;
    static bool plotting;
    static bool canSave;
    static bool canLoad;
    static bool newClick;
    static bool newRelease;
    static bool canZoom;
    
    static glm::vec2 mouse;
    static glm::vec2 click;
    static glm::vec2 plotStart;

    static glm::vec2 size;
    static glm::vec2 frustumSize;
    static glm::mat4 projection;
    static glm::mat4 view;
    static GLfloat   aspect;
    
    static GLuint vaoId;

    static std::unique_ptr<Palette>   palette;
    static std::unique_ptr<Samples>   samples;
    static std::unique_ptr<Character> character;
    static std::unique_ptr<Nametable> nametable;

    static std::shared_ptr<Button> buttonPencil;
    static std::shared_ptr<Button> buttonLine;
    static std::shared_ptr<Button> buttonRectangle;
    static std::shared_ptr<Button> buttonEllipse;
    static std::shared_ptr<Button> buttonAbout;
    static std::shared_ptr<Button> buttonSave;
    static std::shared_ptr<Button> buttonLoad;
    
    static bool dirty;
    static GLFWwindow* window;
};

#endif
