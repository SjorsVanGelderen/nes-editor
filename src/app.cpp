#include "app.h"

/*
    Interaction
*/
AppMode App::mode = AppMode::CharacterMode;
Tool    App::tool = Tool::Pixel;

bool App::dragging = false;
bool App::plotting = false;
bool App::canSave  = true;
bool App::canLoad  = true;
bool App::newClick = false;

glm::vec2 App::mouse     = glm::vec2(0, 0);
glm::vec2 App::click     = glm::vec2(0, 0);
glm::vec2 App::plotStart = glm::vec2(-1, -1);

/*
    Visuals
*/
const std::string App::CAPTION = "NES editor";

glm::vec2 App::size        = glm::uvec2(960, 720);
glm::vec2 App::frustumSize = glm::uvec2(50, 50);
GLfloat   App::aspect      = (float)App::size.y / (float)App::size.x;

glm::mat4 App::projection = glm::ortho
    ( -frustumSize.x
    , frustumSize.x
    , -frustumSize.y * aspect
    , frustumSize.y * aspect
    , 0.0f
    , 100.0f
    );
    
glm::mat4 App::view = glm::lookAt
    ( glm::vec3(0, 0, 1)
    , glm::vec3(0, 0, 0)
    , glm::vec3(0, 1, 0)
    );

GLuint App::vaoId;

GLFWwindow* App::window;

/*
    Entities
*/
std::unique_ptr<Palette>   App::palette;
std::unique_ptr<Samples>   App::samples;
std::unique_ptr<Character> App::character;
std::unique_ptr<Nametable> App::nametable;

std::shared_ptr<Button> App::buttonPencil;
std::shared_ptr<Button> App::buttonLine;
std::shared_ptr<Button> App::buttonRectangle;
std::shared_ptr<Button> App::buttonEllipse;
std::shared_ptr<Button> App::buttonAbout;
std::shared_ptr<Button> App::buttonSave;
std::shared_ptr<Button> App::buttonLoad;

AppStatus App::Start()
{
    const std::vector<const std::function<AppStatus()>> libraryStarters =
        { StartGLFW
        , StartGLEW
        , StartGL
        , Media::Start
        };

    for(const auto x : libraryStarters)
    {
        const auto result = x();
        if(result != AppStatus::Success) return result;
    }

    palette   = std::make_unique<Palette>();
    samples   = std::make_unique<Samples>();
    character = std::make_unique<Character>();
    nametable = std::make_unique<Nametable>();

    buttonPencil    = std::make_shared<Button>("tool_pencil", 0);
    buttonLine      = std::make_shared<Button>("tool_line", 1);
    buttonRectangle = std::make_shared<Button>("tool_rectangle", 2);
    buttonEllipse   = std::make_shared<Button>("tool_ellipse", 3);
    buttonAbout     = std::make_shared<Button>("about", 4);
    buttonSave      = std::make_shared<Button>("save", 5);
    buttonLoad      = std::make_shared<Button>("load", 6);

    const std::vector<const std::function<AppStatus()>> entityStarters =
        { palette->Start
        , ([]() -> AppStatus { return Samples::Start(palette->GetPaletteTextureId()); })
        , ([]() -> AppStatus { return Character::Start(palette->GetPaletteTextureId()); })
        , ([]() -> AppStatus { return Nametable::Start(palette->GetPaletteTextureId()); })
        };

    for(const auto x : entityStarters)
    {
        const auto result = x();
        if(result != AppStatus::Success) return result;
    }

    // Run the main loop and return the result
    return Update();
}

AppStatus App::Update()
{
    bool clickConsumed = false;

    const std::vector<const std::function<AppStatus(bool clickConsumed)>> characterModeUpdaters
        { [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, Palette::GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, Character::GetDrawable(), true); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, Samples::GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonPencil->GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonLine->GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonRectangle->GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonEllipse->GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonAbout->GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonSave->GetDrawable()); }
        , [&](bool clickConsumed) -> AppStatus { return UpdateDrawable(&clickConsumed, buttonLoad->GetDrawable()); }
        };

    const std::vector<const std::function<AppStatus(bool clickConsumed)>> nametableModeUpdaters
        { // [&](bool clickConsumed) -> AppStatus { return UpdateNametable(&clickConsumed); }
        // , [&](bool clickConsumed) -> AppStatus { return UpdateCharacter(&clickConsumed); }
        };

    const std::vector<const std::function<AppStatus(bool clickConsumed)>> attributeTableModeUpdaters;
    
    while ( !glfwWindowShouldClose(window)
         && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
          )
    {
        // Keyboard input
        {
            // Process dragging input
            dragging = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

            // Process save commands
            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                if(canSave)
                {
                    canSave = false;
                    Media::SaveCharacter();
                }
            }
            else if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            {
                if(canSave)
                {
                    canSave = false;
                    Media::SaveSamples();
                }
            }
            else
            {
                canSave = true;
            }

            // Process load commands
            if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            {
                if(canLoad)
                {
                    canLoad = false;
                    Media::LoadCharacter();
                }
            }
            else if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            {                
                if(canLoad)
                {
                    canLoad = false;
                    Media::LoadSamples();
                }
            }
            else
            {
                canLoad = true;
            }

            // Process mode switch command
            if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            {
                mode = AppMode::CharacterMode;
            }
            else if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            {
                mode = AppMode::NametableMode;
            }
            else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            {
                mode = AppMode::AttributeTableMode;
            }
        }

        if(glfwGetWindowAttrib(window, GLFW_FOCUSED))
        {
            clickConsumed = !newClick;
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            std::vector<const std::function<AppStatus(bool clickConsumed)>> updaters =
                  mode == AppMode::CharacterMode ? characterModeUpdaters
                : mode == AppMode::NametableMode ? nametableModeUpdaters 
                : attributeTableModeUpdaters;

            for(const auto x : updaters)
            {
                const auto result = x(clickConsumed);
                if(result != AppStatus::Success) return result;
            }
            
            glfwSwapBuffers(window);

#ifdef __APPLE__
            // Swap buffers and move the window slightly once
            // This fixes the Mojave startup rendering bug

            static bool macMoved = false;
            int x;
            int y;

            if(!macMoved)
            {
                glfwSwapBuffers(window);
                glfwGetWindowPos(window, &x, &y);
                glfwSetWindowPos(window, x + 1, y);
                glfwSetWindowPos(window, x, y);
                
                macMoved = true;
            }
#endif
        }

        glfwPollEvents();
    }

    // Terminate the application and return the result
    return Stop();
}

AppStatus App::UpdateDrawable
    ( bool* clickConsumed
    , std::shared_ptr<IDrawable> drawable
    , bool zoomable
    )
{
    const auto m = App::ScreenToSurface
        ( mouse
        , drawable->GetPosition() * (zoomable ? Character::GetZoom() : 1.0f)
        , drawable->GetSize()
        , zoomable ? Character::GetZoom() : 1.0f
        );
    
    if(!*clickConsumed && m.x != -1)
    {
        drawable->Click(m); // TODO: Check if the return value is necessary
        *clickConsumed = true;
    }
    
    return drawable->Draw(projection, view, m);
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

    window = glfwCreateWindow
        ( size.x
        , size.y
        , CAPTION.c_str()
        , nullptr
        , nullptr
        );
    
    if(!window)
    {
        glfwTerminate();
        return AppStatus::FailureWindow;
    }
    
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPosCallback(window, App::GLFWCursorPositionCallback);
    glfwSetMouseButtonCallback(window, App::GLFWMouseButtonCallback);
    glfwSetScrollCallback(window, App::GLFWScrollCallback);
    
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

AppStatus App::Stop()
{
    const std::vector<const std::function<AppStatus()>> stoppers
        { Media::Stop
        , Palette::Stop
        , Samples::Stop
        , Character::Stop
        , Nametable::Stop
        };

    for(const auto x : stoppers)
    {
        const auto result = x();
        if(result != AppStatus::Success) return result;
    }
    
    // TODO: Check if libraries like DevIL need to be destroyed as well

    const GLuint vaoIds[] { vaoId };

    glDeleteVertexArrays(1, vaoIds);
    glfwTerminate();
    
    return AppStatus::Success;
}

glm::vec2 App::ScreenToSurface(glm::vec2 point, glm::vec2 position, glm::vec2 size, float zoom)
{
    position.y = -position.y;
    
    const auto dist = glm::vec2(point.x - position.x, point.y - position.y);

    return abs(dist.x) < (size.x / 2) * zoom
        && abs(dist.y) < (size.y / 2) * zoom
         ? glm::vec2
            ( (dist.x + (size.x * zoom) / 2) / (size.x * zoom)
            , (dist.y + (size.y * zoom) / 2) / (size.y * zoom)
            )
         : glm::vec2(-1, -1);
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

AppMode App::GetMode()
{
    return mode;
}

Tool App::GetTool()
{
    return tool;
}

glm::vec2 App::GetPlotStart()
{
    return plotStart;
}

bool App::GetPlotting()
{
    return plotting;
}

void App::GLFWCursorPositionCallback(GLFWwindow* window, double mX, double mY)
{
    auto mouseX = mX / size.x;
    auto mouseY = mY / size.y;

    mouseX = mouseX < 0.0f ? 0.0f 
           : mouseX > 1.0f ? 1.0f 
           : mouseX;

    mouseY = mouseY < 0.0f ? 0.0f 
           : mouseY > 1.0f ? 1.0f 
           : mouseY;

    const auto frustumSizeY = frustumSize.y * aspect;
    
    const auto newMouse = glm::vec2
        ( mouseX * frustumSize.x * 2 - frustumSize.y
        , mouseY * frustumSizeY  * 2 - frustumSizeY
        );

    if(dragging)
    {
        const auto displacement = glm::vec2
            ( -(mouse.x - newMouse.x)
            , mouse.y - newMouse.y
            );

        if(mode == AppMode::CharacterMode)
        {
            character->Move(displacement);
        }
        else if(mode == AppMode::NametableMode)
        {

        }
        else if(mode == AppMode::AttributeTableMode)
        {

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
            newClick = true;
            click    = mouse;

            switch(tool)
            {
            case Tool::RectangleFrame:
            case Tool::RectangleFill:
            case Tool::CircleFrame:
            case Tool::CircleFill:
                plotStart = click;
                plotting = true;
                break;

            default:
                break;
            }
        }
        else if(action == GLFW_RELEASE)
        {
            newClick = false;
            plotting = false;
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
