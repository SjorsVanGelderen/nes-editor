#include "character_prop.h"

GLuint CharacterProp::programId;
GLuint CharacterProp::vertexBufferId;
GLuint CharacterProp::indexBufferId;
GLuint CharacterProp::pixelsTextureId;
GLint  CharacterProp::mvpUniformId;
GLint  CharacterProp::pixelsUniformId;
GLint  CharacterProp::mouseUniformId;

std::vector<GLfloat>     CharacterProp::vertices;
std::vector<GLuint>      CharacterProp::indices;
std::vector<std::string> CharacterProp::filenames;
std::vector<GLubyte>     CharacterProp::pixels;

glm::mat4 CharacterProp::model;

CharacterProp::~CharacterProp()
{
    GLuint textureIds[] = { pixelsTextureId };
    GLuint bufferIds[]  = { vertexBufferId, indexBufferId };
    
    glDeleteTextures(1, textureIds);
    glDeleteBuffers(2, bufferIds);
    glDeleteProgram(programId);
}

AppStatus CharacterProp::Setup(GLuint textureId)
{
    const GLfloat w = 48.0f;
    const GLfloat h = 48.0f;

    vertices = {
        -w, h, 1.0f,
        0.0f, 1.0f,
        
        w, h, 1.0f,
        1.0, 1.0f,
        
        w, -h, 1.0f,
        1.0f, 0.0f,
        
        -w, -h, 1.0f,
        0.0f, 0.0f
    };

    indices = { 0, 1, 2, 2, 3, 0 };

    filenames = { "character.vert", "character.frag" };

    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // TODO: Should use samples
    for(int i = 0; i < 256 * 256; i++)
    {
        for(int o = 0; o < 3; o++)
        {
            pixels.push_back(Palette::paletteRGB[i % 4 + o]);
        }
    }
    
    glGenTextures(1, &pixelsTextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pixelsTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, Palette::paletteRGB.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    auto programResult = Media::LoadShaderProgram(filenames);
    if(programResult.first != AppStatus::Success) return programResult.first;
    
    programId = programResult.second;
    
    mvpUniformId    = glGetUniformLocation(programId, "mvp");
    pixelsUniformId = glGetUniformLocation(programId, "pixels");
    mouseUniformId  = glGetUniformLocation(programId, "mouse");

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -15.0f, 2.0f));

    return AppStatus::Success;
}

AppStatus CharacterProp::Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
{
    auto mvp = projection * view * model;
    
    glUseProgram(programId);

    glUniformMatrix4fv(mvpUniformId, 1, GL_FALSE, &mvp[0][0]);
    glUniform2fv(mouseUniformId, 1, &mouse[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pixelsTextureId);
    
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

bool CharacterProp::Click(glm::vec2 mouse)
{
    return false;
}

// AppStatus CharacterProp::SyncPixels(std::vector<GLuint> inputPixels)
// {
//     pixels = inputPixels;
// }
