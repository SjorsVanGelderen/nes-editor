#include "nametable.h"

const glm::vec2 frustumSize = App::GetFrustumSize();

const glm::vec2 Nametable::size = glm::vec2(frustumSize.x * 2, frustumSize.x * 2);
const GLfloat   Nametable::maxZoom = 24.0f;

glm::vec3 Nametable::position;

GLfloat Nametable::zoom = 0;

GLuint Nametable::programId;
GLuint Nametable::vertexBufferId;
GLuint Nametable::indexBufferId;
GLuint Nametable::paletteTextureId;
GLint  Nametable::mvpUniformId;
GLint  Nametable::mouseUniformId;
GLint  Nametable::tilesUniformId;

std::vector<GLfloat>     Nametable::vertices;
std::vector<GLuint>      Nametable::indices;
std::vector<std::string> Nametable::filenames;
std::vector<GLuint>      Nametable::tiles;

glm::mat4 Nametable::model;

AppStatus Nametable::Start(GLuint textureId)
{
    vertices = {
        -size.x / 2, size.y / 2, -1.0f,
        0.0f, 1.0f,
        
        size.x / 2, size.y / 2, -1.0f,
        1.0, 1.0f,
        
        size.x / 2, -size.y / 2, -1.0f,
        1.0f, 0.0f,
        
        -size.x / 2, -size.y / 2, -1.0f,
        0.0f, 0.0f
    };

    indices   = { 0, 1, 2, 2, 3, 0 };
    filenames = { "nametable.vert", "nametable.frag" };

    for(int y = 0; y < 16; y++)
    {
        for(int x = 0; x < 16; x++)
        {
            tiles.push_back(y * x);
        }
    }

    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    const auto programResult = Media::LoadShaderProgram(filenames);
    if(programResult.first != AppStatus::Success) return programResult.first;
    
    programId = programResult.second;

    paletteTextureId = textureId;
    mvpUniformId     = glGetUniformLocation(programId, "mvp");
    mouseUniformId   = glGetUniformLocation(programId, "mouse");
    tilesUniformId   = glGetUniformLocation(programId, "tiles");

    zoom     = 1.0f;
    position = glm::vec3(0.0f, 0.0f, -3.0f);
    model    = glm::translate(glm::mat4(1.0f), position);

    return AppStatus::Success;
}

AppStatus Nametable::Stop()
{
    // GLuint textureIds[] = {};
    GLuint bufferIds[]  = { vertexBufferId, indexBufferId };

    // glDeleteTextures(1, textureIds);
    glDeleteBuffers(2, bufferIds);
    glDeleteProgram(programId);

    return AppStatus::Success;
}

AppStatus Nametable::Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
{    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * zoom, position.y * zoom, position.z));
    
    const auto mvp          = projection * view * glm::scale(model, glm::vec3(zoom));
    // const auto samples      = Samples::GetSamples();
    // const auto activeSample = Samples::GetActiveSample();
    // const auto activeColor  = Samples::GetActiveColor();

    mouse.y = 1.0f - mouse.y;
    
    glUseProgram(programId);

    glUniformMatrix4fv(mvpUniformId, 1, GL_FALSE, &mvp[0][0]);
    glUniform2fv(mouseUniformId, 1, &mouse[0]);
    glUniform1uiv(tilesUniformId, tiles.size(), tiles.data());
    // glUniform1ui(activeSampleUniformId, activeSample);
    // glUniform1ui(activeColorUniformId, activeColor);
    // glUniform1i(paletteTextureUniformId, 0);
    // glUniform1i(characterTextureUniformId, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, paletteTextureId);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)vertexPositionOffset);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)vertexUvOffset);
    
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    return AppStatus::Success;
}

bool Nametable::Click(glm::vec2 mouse)
{
    return false;
}

void Nametable::Zoom(GLfloat x)
{   
    const auto newZoom = zoom + x * 0.05f;
    
    zoom = newZoom < 1.0f    ? 1.0f
         : newZoom > maxZoom ? maxZoom
         : newZoom;
}

glm::vec2 Nametable::GetSize()
{
    return size;
}

glm::vec2 Nametable::GetPosition()
{
    return glm::vec2(position.x, position.y);
}

GLfloat Nametable::GetZoom()
{
    return zoom;
}
