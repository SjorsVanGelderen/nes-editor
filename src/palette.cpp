#include "palette.h"

const glm::vec2 frustumSize = App::GetFrustumSize();

const glm::vec2 Palette::size = glm::vec2(frustumSize.x * 2 / 3, frustumSize.x * 2 / 3 / 16 * 4);

glm::vec3 Palette::position = glm::vec3
    ( -frustumSize.x * 2 / 2 + size.x / 2
    , -frustumSize.y * App::GetAspect() * 2 / 2 + size.y / 2
    , -1.0f
    );

GLuint Palette::programId;
GLuint Palette::vertexBufferId;
GLuint Palette::indexBufferId;
GLuint Palette::paletteTextureId;
GLint  Palette::mvpUniformId;
GLint  Palette::mouseUniformId;

std::vector<GLfloat>     Palette::vertices;
std::vector<GLuint>      Palette::indices;
std::vector<std::string> Palette::filenames;

glm::mat4 Palette::model;

std::shared_ptr<PaletteDrawable> Palette::drawable;

AppStatus Palette::Start()
{
    vertices =
        { -size.x / 2, size.y / 2, 1.0f
        , 0.0f, 1.0f
        
        , size.x / 2, size.y / 2, 1.0f
        , 1.0, 1.0f
        
        , size.x / 2, -size.y / 2, 1.0f
        , 1.0f, 0.0f
        
        , -size.x / 2, -size.y / 2, 1.0f
        , 0.0f, 0.0f
        };

    indices = { 0, 1, 2, 2, 3, 0 };

    filenames = { "palette.vert", "palette.frag" };

    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    const auto programResult = Media::LoadShaderProgram(filenames);
    if(programResult.first != AppStatus::Success) return programResult.first;
    
    programId = programResult.second;

    std::vector<GLfloat> pixels;

    for(auto &x : paletteRGB)
    {
        pixels.push_back(x / 255.0f);
    }

    glGenTextures(1, &paletteTextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, paletteTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, Palette::paletteRGB.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    mvpUniformId   = glGetUniformLocation(programId, "mvp");
    mouseUniformId = glGetUniformLocation(programId, "mouse");

    model = glm::translate(glm::mat4(1.0f), position);

    drawable = std::make_shared<PaletteDrawable>();

    return AppStatus::Success;
}

AppStatus Palette::Stop()
{
    GLuint textureIds[] = { paletteTextureId };
    GLuint bufferIds[]  = { vertexBufferId, indexBufferId };
    
    glDeleteTextures(1, textureIds);
    glDeleteBuffers(2, bufferIds);
    glDeleteProgram(programId);

    return AppStatus::Success;
}

AppStatus Palette::Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
{
    const auto mvp = projection * view * model;
    
    mouse.y = 1.0f - mouse.y;
    
    glUseProgram(programId);

    glUniformMatrix4fv(mvpUniformId, 1, GL_FALSE, &mvp[0][0]);
    glUniform2fv(mouseUniformId, 1, &mouse[0]);
    
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

bool Palette::Click(glm::vec2 mouse)
{
    GLuint paletteIndex = floor(mouse.y * 4) * 16 + mouse.x * 16;
    
    Samples::SetColor(paletteIndex);
    
    return false;
}

glm::vec2 Palette::GetPosition()
{
    return position;
}

glm::vec2 Palette::GetSize()
{
    return size;
}

GLuint Palette::GetPaletteTextureId()
{
    return paletteTextureId;
}

std::shared_ptr<IDrawable> Palette::GetDrawable()
{
    return drawable;
}

const std::vector<GLubyte> Palette::paletteRGB =
    { 101, 101, 101
    ,   3,  47, 103
    ,  21,  35, 125
    ,  60,  26, 122
    ,  95,  18,  97
    , 114,  14,  55
    , 112,  16,  13
    ,  89,  26,   5
    ,  52,  40,   3
    ,  13,  51,   3
    ,   3,  59,   4
    ,   4,  60,  19
    ,   3,  56,  63
    ,   0,   0,   0
    ,   0,   0,   0
    ,   0,   0,   0
    
    , 174, 174, 174
    ,  15,  99, 179
    ,  64,  81, 208
    , 120,  65, 204
    , 167,  54, 169
    , 192,  52, 112
    , 189,  60,  48
    , 159,  74,   0
    , 109,  92,   0
    ,  54, 109,   0
    ,   7, 119,   4
    ,   0, 121,  61
    ,   0, 114, 125
    ,   0,   0,   0
    ,   0,   0,   0
    ,   0,   0,   0

    , 254, 254, 255
    ,  93, 179, 255
    , 143, 161, 255
    , 200, 144, 255
    , 247, 133, 250
    , 255, 131, 192
    , 255, 138, 127
    , 239, 154,  73
    , 189, 172,  44
    , 133, 188,  47
    ,  85, 199,  83
    ,  60, 201, 140
    ,  62, 194, 205
    ,  78,  78,  78
    ,   0,   0,   0
    ,   0,   0,   0

    , 254, 254, 255
    , 188, 223, 255
    , 209, 216, 255
    , 232, 209, 255
    , 251, 205, 253
    , 255, 204, 229
    , 255, 207, 202
    , 248, 213, 180
    , 228, 220, 168
    , 204, 227, 169
    , 185, 232, 184
    , 174, 232, 208
    , 175, 229, 234
    , 182, 182, 182
    ,   0,   0,   0
    ,   0,   0,   0
    };
