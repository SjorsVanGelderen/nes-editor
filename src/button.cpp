#include "button.h"

const glm::vec2 frustumSize = App::GetFrustumSize();
const float     aspect      = App::GetAspect();

Button::Button(std::string which, GLuint index)
{
    size = glm::vec2(frustumSize.x / 16, frustumSize.x / 16);

    position = glm::vec3(
        (-frustumSize.x) + size.x / 2 + size.x * index + Palette::GetSize().x,
        (-frustumSize.y * aspect) + size.y / 2,
        -0.5f
        );

    model = glm::translate(glm::mat4(1.0f), position);

    vertices = {
        -size.x / 2, size.y / 2, 1.0f,
        0.0f, 0.0f,
        
        size.x / 2, size.y / 2, 1.0f,
        1.0, 0.0f,
        
        size.x / 2, -size.y / 2, 1.0f,
        1.0f, 1.0f,
        
        -size.x / 2, -size.y / 2, 1.0f,
        0.0f, 1.0f
    };

    indices = { 0, 1, 2, 2, 3, 0 };

    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    std::vector<std::string> filenames = { "button.vert", "button.frag" };
    
    const auto programResult = Media::LoadShaderProgram(filenames);
    if(programResult.first != AppStatus::Success)
    {
        // return programResult.first;
        std::cout << "Couldn't load program for button" << std::endl;
    }
    
    programId = programResult.second;
    
    mvpUniformId   = glGetUniformLocation(programId, "mvp");
    mouseUniformId = glGetUniformLocation(programId, "mouse");

    {
        const auto result = Media::LoadTexture("../assets/" + which + ".png");
        if(result.first != AppStatus::Success)
        {
            std::cout << "Failed to load texture" << std::endl;
            std::cout << result.first << std::endl;
        }
        else
        {
            textureId = result.second;
        }
    }

    drawable = std::make_shared<ButtonDrawable>(this);
}

Button::~Button()
{
    GLuint textureIds[] = { textureId };
    GLuint bufferIds[] = { vertexBufferId, indexBufferId };
    
    glDeleteBuffers(2, bufferIds);
    glDeleteProgram(programId);
    glDeleteTextures(1, textureIds);
}

AppStatus Button::Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
{
    const auto mvp = projection * view * model;

    mouse.y = 1.0f - mouse.y;
    
    glUseProgram(programId);

    glUniformMatrix4fv(mvpUniformId, 1, GL_FALSE, &mvp[0][0]);
    glUniform2fv(mouseUniformId, 1, &mouse[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)vertexPositionOffset);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)vertexUvOffset);
    
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return AppStatus::Success;
}

bool Button::Click(glm::vec2 mouse)
{
    return false;
}

glm::vec2 Button::GetSize()
{
    return size;
}

glm::vec2 Button::GetPosition()
{
    return position;
}

std::shared_ptr<IDrawable> Button::GetDrawable()
{
    return drawable;
}

void Button::SetPosition(glm::vec2 newPosition)
{
    position = glm::vec3(newPosition.x, newPosition.y, position.z);
}
