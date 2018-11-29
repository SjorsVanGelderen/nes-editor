#include "samples.h"

const glm::vec2 frustumSize = App::GetFrustumSize();

const glm::vec2 Samples::size = glm::vec2(
    frustumSize.x * 2 / 3
    , frustumSize.x * 2 / 3 / 13 * 2
    );

glm::vec3 Samples::position = glm::vec3(
    frustumSize.x - size.x / 2
    , -frustumSize.y * App::GetAspect() + size.y / 2, 1.0f
    );

glm::mat4 Samples::model;

GLuint Samples::activeSample;
GLuint Samples::activeColor;

GLuint Samples::programId;
GLuint Samples::vertexBufferId;
GLuint Samples::indexBufferId;
GLuint Samples::paletteTextureId;
GLint  Samples::mvpUniformId;
GLint  Samples::samplesUniformId;
GLint  Samples::activeSampleUniformId;
GLint  Samples::activeColorUniformId;
GLint  Samples::mouseUniformId;

std::vector<GLfloat>     Samples::vertices;
std::vector<GLuint>      Samples::indices;
std::vector<std::string> Samples::filenames;

std::shared_ptr<std::vector<GLuint>> Samples::samples =
    std::make_shared<std::vector<GLuint>>
        ( std::vector<GLuint>
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0
                , 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 0
                }
        );

std::shared_ptr<SamplesDrawable> Samples::drawable;

AppStatus Samples::Start(GLuint textureId)
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

    filenames = { "samples.vert", "samples.frag" };

    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    
    const auto programResult = Media::LoadShaderProgram(filenames);
    if(programResult.first != AppStatus::Success) return programResult.first;
    
    programId        = programResult.second;
    paletteTextureId = textureId;
    model            = glm::translate(glm::mat4(1.0f), position);
    
    mvpUniformId          = glGetUniformLocation(programId, "mvp");
    mouseUniformId        = glGetUniformLocation(programId, "mouse");
    samplesUniformId      = glGetUniformLocation(programId, "samples");
    activeSampleUniformId = glGetUniformLocation(programId, "activeSample");
    activeColorUniformId  = glGetUniformLocation(programId, "activeColor");

    drawable = std::make_shared<SamplesDrawable>();
    
    return AppStatus::Success;
}

AppStatus Samples::Stop()
{
    //GLuint textureIds[] = { paletteTextureId };
    GLuint bufferIds[] = { vertexBufferId, indexBufferId };
    
    //glDeleteTextures(1, textureIds);
    glDeleteBuffers(2, bufferIds);
    glDeleteProgram(programId);

    return AppStatus::Success;
}

AppStatus Samples::Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
{
    const auto mvp = projection * view * model;

    mouse.y = 1.0f - mouse.y;
    
    glUseProgram(programId);

    glUniformMatrix4fv(mvpUniformId, 1, GL_FALSE, &mvp[0][0]);
    glUniform1uiv(samplesUniformId, samples->size(), samples->data());
    glUniform2fv(mouseUniformId, 1, &mouse[0]);
    glUniform1ui(activeSampleUniformId, activeSample);
    glUniform1ui(activeColorUniformId, activeColor);

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
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return AppStatus::Success;
}

bool Samples::Click(glm::vec2 mouse)
{
    const auto second = mouse.y > 0.5;
    const auto offset = second ? 13 : 0;
    
    activeColor = offset + mouse.x * samples->size() / 2;

    if(activeColor == samples->size()) activeColor--;

    if( activeColor != samples->size() / 2 - 1
     && activeColor != samples->size() - 1
      )
    {
        const auto groupOffset = second ? 4 : 0;
        activeSample = floor((activeColor % (samples->size() / 2)) / 3 + groupOffset);
    }
    
    return false;
}

bool Samples::Release(glm::vec2 mouse)
{
    return false;
}

glm::vec2 Samples::GetPosition()
{
    return position;
}

glm::vec2 Samples::GetSize()
{
    return size;
}

void Samples::SetColor(GLubyte paletteIndex)
{
    (*samples)[activeColor] = paletteIndex;
}

GLuint Samples::GetActiveSample()
{
    return activeSample;
}

GLuint Samples::GetActiveColor()
{
    return activeColor;
}

std::shared_ptr<IDrawable> Samples::GetDrawable()
{
    return drawable;
}

void Samples::SetSamples(std::vector<GLuint> newSamples)
{
    samples = std::make_shared<std::vector<GLuint>>(newSamples);
}

std::shared_ptr<std::vector<GLuint>> Samples::GetSamples()
{
    return samples;
}
