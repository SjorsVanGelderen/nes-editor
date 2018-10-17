#include "character.h"

const glm::vec2 frustumSize = App::GetFrustumSize();
const GLfloat   aspect      = App::GetAspect();

const glm::vec2 textureSize = glm::vec2(256, 128);

const glm::vec2 Character::size    = glm::vec2(frustumSize.x * 2, frustumSize.x);
const GLfloat   Character::maxZoom = 24.0f;

GLfloat Character::zoom;
GLfloat Character::nametableZoom;

glm::vec3 Character::position;
glm::vec3 Character::nametablePosition;
glm::mat4 Character::model;

GLuint Character::programId;
GLuint Character::vertexBufferId;
GLuint Character::indexBufferId;
GLuint Character::paletteTextureId;
GLuint Character::characterTextureId;

GLint Character::mvpUniformId;
GLint Character::mouseUniformId;
GLint Character::samplesUniformId;
GLint Character::activeSampleUniformId;
GLint Character::activeColorUniformId;
GLint Character::paletteTextureUniformId;
GLint Character::characterTextureUniformId;

std::vector<GLfloat>     Character::vertices;
std::vector<GLuint>      Character::indices;
std::vector<std::string> Character::filenames;
std::vector<GLubyte>     Character::character;
std::vector<GLubyte>     Character::pixels;

Character::~Character()
{
    GLuint textureIds[] = { characterTextureId };
    GLuint bufferIds[]  = { vertexBufferId, indexBufferId };
    
    glDeleteTextures(1, textureIds);
    glDeleteBuffers(2, bufferIds);
    glDeleteProgram(programId);
}

AppStatus Character::Setup(GLuint textureId)
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

    indices = { 0, 1, 2, 2, 3, 0 };

    filenames = { "character.vert", "character.frag" };
    
    for(int y = 0; y < textureSize.y; y++)
    {
        for(int x = 0; x < textureSize.x; x++)
        {
            character.push_back(0);
            pixels.push_back(0);
        }
    }

    CharacterToTexture();
    
    glGenTextures(1, &characterTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, characterTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureSize.x, textureSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
    
    mvpUniformId              = glGetUniformLocation(programId, "mvp");
    samplesUniformId          = glGetUniformLocation(programId, "samples");
    activeSampleUniformId     = glGetUniformLocation(programId, "activeSample");
    activeColorUniformId      = glGetUniformLocation(programId, "activeColor");
    mouseUniformId            = glGetUniformLocation(programId, "mouse");
    paletteTextureUniformId   = glGetUniformLocation(programId, "paletteTexture");
    characterTextureUniformId = glGetUniformLocation(programId, "characterTexture");

    zoom          = 1.0f;
    nametableZoom = 0.5f;
    
    position = glm::vec3(0.0f, 0.0f, 1.0f);
    
    nametablePosition =
        glm::vec3(
            -frustumSize.x, //+ (size.x / 2) * nametableZoom * 2,
            -(frustumSize.y * aspect) + ((size.y * aspect) / 2) * nametableZoom,
            1.0f
            );
    
    model = glm::translate(glm::mat4(1.0f), position);
    
    return AppStatus::Success;
}

AppStatus Character::Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
{
    glm::mat4 mvp;
    
    if(App::GetMode() == AppMode::CharacterMode)
    {
        mvp   = projection * view * glm::scale(model, glm::vec3(zoom));
        model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * zoom, position.y * zoom, position.z));
    }
    else
    {
        mvp   = projection * view * glm::scale(model, glm::vec3(nametableZoom));
        model = glm::translate(glm::mat4(1.0f), nametablePosition);
    }
    
    const auto samples      = Samples::GetSamples();
    const auto activeSample = Samples::GetActiveSample();
    const auto activeColor  = Samples::GetActiveColor();

    mouse.y = 1.0f - mouse.y;
    
    glUseProgram(programId);

    glUniformMatrix4fv(mvpUniformId, 1, GL_FALSE, &mvp[0][0]);
    glUniform2fv(mouseUniformId, 1, &mouse[0]);
    glUniform1uiv(samplesUniformId, samples->size(), samples->data());
    glUniform1ui(activeSampleUniformId, activeSample);
    glUniform1ui(activeColorUniformId, activeColor);
    glUniform1i(paletteTextureUniformId, 0);
    glUniform1i(characterTextureUniformId, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, paletteTextureId);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, characterTextureId);

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

bool Character::Click(glm::vec2 mouse)
{
    const auto mode = App::GetMode();
    
    if(mode == AppMode::CharacterMode)
    {
        // TODO: Try to simplify this
        const float mouseX      = (mouse.x > 0.5f ? mouse.x - 0.5f : mouse.x) * 2;
        const uint  bankOffset  = mouse.x > 0.5f ? pow(128, 2) : 0;
        const uint  cIndex      = bankOffset + floor(mouse.y * 128) * 128 + floor(mouseX * 128);
        const uint  pIndex      = floor(mouse.y * textureSize.y) * textureSize.x + floor(mouse.x * textureSize.x);
        const uint  activeColor = Samples::GetActiveColor();
        const uint  color       = activeColor == 12 || activeColor == 24 ? 0 : activeColor % 3 + 1; // Requires review
        
        character[cIndex] = color;
        pixels[pIndex]    = GLubyte(255 / 3 * color);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, characterTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureSize.x, textureSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    }
    else if(mode == AppMode::NametableMode)
    {
        
    }
        
    return false;
}

glm::vec2 Character::GetPosition()
{
    return position;
}

glm::vec2 Character::GetSize()
{
    return size;
}

AppStatus Character::SetCharacter(std::vector<GLubyte> newCharacter)
{
    character = newCharacter;

    CharacterToTexture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, characterTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureSize.x, textureSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    
    return AppStatus::Success;
}

std::vector<GLubyte> Character::GetCharacter()
{
    return character;
}

std::vector<GLubyte> Character::GetPixels()
{    
    return pixels;
}

void Character::Move(glm::vec2 p)
{
    if(App::GetMode() == AppMode::CharacterMode)
    {
        position = position + glm::vec3(p.x, p.y, 0.0) / zoom;
    }
}

float Character::GetZoom()
{
    return zoom;
}

void Character::CharacterToTexture()
{
    pixels = std::vector<GLubyte>(pow(128, 2) * 2);

    for(uint bank = 0; bank < 2; bank++)
    {
        for(uint y = 0; y < 128; y++)
        {
            for(uint x = 0; x < 128; x++)
            {
                const auto i = y * textureSize.x + bank * 128 + x;
                const auto c = 255 / 3 * character[bank * pow(128, 2) + y * 128 + x];
                
                pixels[i] = c;
            }
        }
    }
}

void Character::Zoom(GLfloat x)
{   
    const auto newZoom = zoom + x * 0.05f;
    
    zoom = newZoom < 1.0f    ? 1.0f
         : newZoom > maxZoom ? maxZoom
         : newZoom;
}
