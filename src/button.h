#ifndef BUTTON_H
#define BUTTON_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory>

#include "appstatus.h"
#include "offset.h"
#include "media.h"
#include "idrawable.h"
#include "palette.h"

class Palette;
struct ButtonDrawable;

class Button : std::enable_shared_from_this<Button>
{
  public:
    Button(std::string which, GLuint index);
    ~Button();

    AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse);

    bool Click(glm::vec2 mouse);

    glm::vec2 GetSize();
    glm::vec2 GetPosition();

    std::shared_ptr<IDrawable> GetDrawable();
    
    void SetPosition(glm::vec2 newPosition);

  private:
    glm::vec2 size;
    glm::vec3 position;
    glm::mat4 model;

    GLuint programId;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    GLuint textureId;
    
    GLint mvpUniformId;
    GLint mouseUniformId;

    std::vector<GLfloat> vertices;
    std::vector<GLuint>  indices;

    std::shared_ptr<ButtonDrawable> drawable;
};

struct ButtonDrawable : public IDrawable
{
    ButtonDrawable(Button* b) : IDrawable()
    {
        button = b;
    }

    AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse) override
    {
        return button->Draw(projection, view, mouse);
    }

    glm::vec2 GetPosition() override
    { 
        return button->GetPosition();
    }

    glm::vec2 GetSize() override
    {
        return button->GetSize();
    }

    bool Click(glm::vec2 mouse) override
    {
        return button->Click(mouse);
    }

    ~ButtonDrawable()
    {
      button = nullptr;
    }

    Button* button;
};

#endif
