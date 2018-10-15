#include "app.h"

const std::string App::CAPTION = "NES editor";

AppMode App::mode = AppMode::CharacterMode;

bool App::dragging = false;
bool App::canSave  = true;
bool App::canLoad  = true;
bool App::newClick = false;

glm::vec2 App::mouse = glm::vec2(0, 0);
glm::vec2 App::click = glm::vec2(0, 0);

GLuint App::vaoId;

glm::vec2 App::size        = glm::uvec2(960, 720);
glm::vec2 App::frustumSize = glm::uvec2(50, 50);
GLfloat   App::aspect      = (float)App::size.y / (float)App::size.x;

glm::mat4 App::projection = glm::ortho(
    -frustumSize.x,
    frustumSize.x,
    -frustumSize.y * aspect,
    frustumSize.y * aspect,
    0.0f,
    100.0f
    );
    
glm::mat4 App::view = glm::lookAt(
    glm::vec3(0, 0, 1),
    glm::vec3(0, 0, 0),
    glm::vec3(0, 1, 0)
    );

std::unique_ptr<Palette>   App::palette;
std::unique_ptr<Samples>   App::samples;
std::unique_ptr<Character> App::character;
std::unique_ptr<Nametable> App::nametable;

std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> App::window =
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>
        ( nullptr
        , glfwDestroyWindow
        );

AppStatus App::Start()
{
    {
        const auto result = StartGLFW();
        if(result != AppStatus::Success) return result;
    }

    {
        const auto result = StartGLEW();
        if(result != AppStatus::Success) return result;
    }

    {
        const auto result = StartGL();
        if(result != AppStatus::Success) return result;
    }

    palette = std::make_unique<Palette>();

    {
        const auto result = palette->Setup();
        if(result != AppStatus::Success) return result;
    }

    samples = std::make_unique<Samples>();

    {
        const auto result = samples->Setup(palette->GetPaletteTextureId());
        if(result != AppStatus::Success) return result;
    }

    character = std::make_unique<Character>();

    {
        const auto result = character->Setup(palette->GetPaletteTextureId());
        if(result != AppStatus::Success) return result;
    }

    nametable = std::make_unique<Nametable>();

    {
        const auto result = nametable->Setup(palette->GetPaletteTextureId());

        if(result != AppStatus::Success) return result;
    }

    auto w = window.get();
    
    while(
        !glfwWindowShouldClose(w)
        && glfwGetKey(window.get(), GLFW_KEY_ESCAPE) != GLFW_PRESS
        )
    {
        
        dragging = glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS;

        if(glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        {
            if(canSave)
            {
                canSave = false;
                Media::SaveCharacter();
            }
        }
        else
        {
            canSave = true;
        }
        
        if(glfwGetKey(w, GLFW_KEY_L) == GLFW_PRESS)
        {
            if(canLoad)
            {
                canLoad = false;
                Media::LoadCharacter();
            }
        }
        else
        {
            canLoad = true;
        }

        if(glfwGetKey(w, GLFW_KEY_1) == GLFW_PRESS)
        {
            mode = AppMode::CharacterMode;
        }
        else if(glfwGetKey(w, GLFW_KEY_2) == GLFW_PRESS)
        {
            mode = AppMode::NametableMode;
        }
        else if(glfwGetKey(w, GLFW_KEY_3) == GLFW_PRESS)
        {
            mode = AppMode::AttributeTableMode;
        }
        
        const auto result = Update();
        if(result != AppStatus::Success) return result;
    }
    
    return Stop();
}

AppStatus App::Update()
{
    if(glfwGetWindowAttrib(window.get(), GLFW_FOCUSED))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool clickConsumed = !newClick;

        switch(mode)
        {
        case AppMode::CharacterMode:
            {
                const auto m = App::ScreenToSurface(mouse, palette->GetPosition(), palette->GetSize());
                
                if(!clickConsumed && m.x != -1)
                {
                    palette->Click(m);
                    clickConsumed = true;
                }
        
                const auto result = palette->Draw(projection, view, m);
                if(result != AppStatus::Success) return result;
            }

            {
                const auto m = App::ScreenToSurface(mouse, samples->GetPosition(), samples->GetSize());
        
                if(!clickConsumed && m.x != -1)
                {
                    samples->Click(m);
                    clickConsumed = true;
                }
        
                const auto result = samples->Draw(projection, view, m);
                if(result != AppStatus::Success) return result;
            }

            {
                const auto m = App::ScreenToSurface(
                    mouse,
                    character->GetPosition() * character->GetZoom(),
                    character->GetSize(), character->GetZoom()
                    );

                if(!clickConsumed && m.x != -1)
                {
                    character->Click(m);
                    clickConsumed = true;
                }

                const auto result = character->Draw(projection, view, m);
                if(result != AppStatus::Success) return result;
            }
            break;

        case AppMode::NametableMode:
            {
                const auto m = App::ScreenToSurface(
                    mouse,
                    nametable->GetPosition() * nametable->GetZoom(),
                    nametable->GetSize(), nametable->GetZoom()
                    );

                if(!clickConsumed && m.x != -1)
                {
                    nametable->Click(m);
                    clickConsumed = true;
                }

                const auto result = nametable->Draw(projection, view, m);
                if(result != AppStatus::Success) return result;
            }
            break;

        case AppMode::AttributeTableMode:
            break;

        default:
            break;
        }
    
        glfwSwapBuffers(window.get());
    }
    
    glfwPollEvents();

    return AppStatus::Success;
}

AppStatus App::Stop()
{
    const GLuint vaoIds[] { vaoId };
    glDeleteVertexArrays(1, vaoIds);

    glfwTerminate();
    
    return AppStatus::Success;
}

AppStatus App::StartGLFW()
{
    if(!glfwInit())
    {
        return AppStatus::FailureGLFWInit;
    }
    
    glfwSetErrorCallback(Debug::GLFWError);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window =
        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>
        ( glfwCreateWindow
          ( size.x
            , size.y
            , CAPTION.c_str()
            , NULL
            , NULL
              )
          , glfwDestroyWindow
            );
    
    if(!window)
    {
        glfwTerminate();
        return AppStatus::FailureWindow;
    }
    
    glfwMakeContextCurrent(window.get());

    glfwSetInputMode(window.get(), GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPosCallback(window.get(), App::GLFWCursorPositionCallback);
    glfwSetMouseButtonCallback(window.get(), App::GLFWMouseButtonCallback);
    glfwSetScrollCallback(window.get(), App::GLFWScrollCallback);
    
    return AppStatus::Success;
}

AppStatus App::StartGLEW()
{
    glewExperimental = true;

    if(glewInit() != GLEW_OK) return AppStatus::FailureGLEWInit;
    
    return AppStatus::Success;
}

AppStatus App::StartGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    return AppStatus::Success;
}

GLfloat App::GetAspect()
{
    return aspect;
}

glm::vec2 App::GetSize()
{
    return size;
}

glm::vec2 App::GetFrustumSize()
{
    return frustumSize;
}

glm::vec2 App::ScreenToSurface(glm::vec2 point, glm::vec2 position, glm::vec2 size, float zoom)
{
    position.y = -position.y;
    
    const auto dist   = glm::vec2(point.x - position.x, point.y - position.y);

    return abs(dist.x) < (size.x / 2) * zoom
        && abs(dist.y) < (size.y / 2) * zoom
         ? glm::vec2(
             (dist.x + (size.x * zoom) / 2) / (size.x * zoom),
             (dist.y + (size.y * zoom) / 2) / (size.y * zoom)
             )
         : glm::vec2(-1, -1);
}

void App::GLFWCursorPositionCallback(GLFWwindow* window, double mX, double mY)
{
    auto mouseX = mX / size.x;
    auto mouseY = mY / size.y;

    mouseX = mouseX < 0.0f ? 0.0f : mouseX > 1.0f ? 1.0f : mouseX;
    mouseY = mouseY < 0.0f ? 0.0f : mouseY > 1.0f ? 1.0f : mouseY;

    const auto frustumSizeY = frustumSize.y * aspect;
    
    const auto newMouse = glm::vec2(
        mouseX * frustumSize.x * 2 - frustumSize.y,
        mouseY * frustumSizeY  * 2 - frustumSizeY
        );

    if(dragging)
    {
        const auto displacement = glm::vec2(
            -(mouse.x - newMouse.x),
            mouse.y - newMouse.y
            );

        switch(mode)
        {
        case AppMode::CharacterMode:
            character->Move(displacement);
            break;

        case AppMode::NametableMode:
            break;

        case AppMode::AttributeTableMode:
            break;

        default:
            break;
        }
    }

    mouse = newMouse;
}

void App::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mode)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            click    = mouse;
            newClick = true;
        }
        else if(action == GLFW_RELEASE)
        {
            newClick = false;
        }
    }
}

void App::GLFWScrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
    switch(mode)
    {
    case AppMode::CharacterMode:
        character->Zoom(offsetY);
        break;

    case AppMode::NametableMode:
        nametable->Zoom(offsetY);
        break;

    case AppMode::AttributeTableMode:
        break;

    default:
        break;
    }
}
