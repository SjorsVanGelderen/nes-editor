#include "media.h"

std::map<std::string, GLuint> Media::shaderPrograms;

const auto bankSize = pow(128, 2);

std::pair<AppStatus, GLuint> Media::LoadShaderProgram(std::vector<std::string> filenames)
{
    auto existing = shaderPrograms.find(filenames.at(0));
    if(existing != shaderPrograms.end()) return std::make_pair(Success, existing->second);
    
    GLuint programId = glCreateProgram();

    std::vector<GLuint> shaders;
    
    for(auto filename : filenames)
    {
        auto shaderResult = LoadShader(filename);
        
        if(shaderResult.first != AppStatus::Success)
        {
            return std::make_pair(shaderResult.first, 0);
        }
        
        glAttachShader(programId, shaderResult.second);
        shaders.push_back(shaderResult.second);
    }

    glLinkProgram(programId);

    GLint       result        = GL_FALSE;
    int         infoLogLength = 0;
    // const char* sourcePointer = source.c_str();

    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if(infoLogLength > 0)
    {
        std::vector<char> messageBuffer(infoLogLength + 1);
        
        glGetProgramInfoLog(programId, infoLogLength, NULL, &messageBuffer[0]);

        std::string message(messageBuffer.begin(), messageBuffer.end());

        Debug::Log(LogLevel::Error, message);

        return std::pair(AppStatus::FailureShaderProgramLoad, 0);
    }

    for(auto shader : shaders)
    {
        glDetachShader(programId, shader);
        glDeleteShader(shader);
    }
    
    return std::make_pair(AppStatus::Success, programId);
}

std::pair<AppStatus, GLuint> Media::LoadShader(std::string filename)
{
    auto modeString = filename.substr(filename.find("."));

    if(modeString != ".vert" && modeString != ".frag")
    {
        return std::make_pair(AppStatus::FailureShaderLoad, 0);
    }
    
    GLuint modeId = modeString == ".vert"
        ? GL_VERTEX_SHADER
        : GL_FRAGMENT_SHADER;

    GLuint shaderId = glCreateShader(modeId);

    std::ifstream     sourceStream(filename, std::ios::in);
    std::stringstream stringStream;

    if(!sourceStream.is_open())
    {
        return std::make_pair(AppStatus::FailureShaderLoad, 0);
    }

    stringStream << sourceStream.rdbuf();
    
    std::string source = stringStream.str();

    sourceStream.close();

    GLint       result        = GL_FALSE;
    int         infoLogLength = 0;
    const char* sourcePointer = source.c_str();

    glShaderSource(shaderId, 1, &sourcePointer, NULL);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if(infoLogLength > 0)
    {
        std::vector<char> messageBuffer(infoLogLength + 1);
        
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, &messageBuffer[0]);

        std::string message(messageBuffer.begin(), messageBuffer.end());

        Debug::Log(LogLevel::Error, message);

        return std::make_pair(AppStatus::FailureShaderLoad, 0);
    }

    return std::make_pair(AppStatus::Success, shaderId);
}

std::pair<AppStatus, std::vector<GLubyte>> Media::LoadSamples()
{
    return std::make_pair(AppStatus::Success, std::vector<GLubyte>());
}

AppStatus Media::SaveSamples()
{
    return AppStatus::Success;
}

AppStatus Media::SaveCharacter()
{
    const auto character = Character::GetCharacter();

    std::ofstream file("data.chr", std::ios::out | std::ios::binary | std::ios::trunc);

    if(!file.is_open()) return AppStatus::Success;

    std::cout << "Writing character to file..." << std::endl;

    for(auto bank = 0; bank < 2; bank++)
    {
        const auto bo = bank * bankSize; // Bank offset
        
        for(auto ty = 0; ty < 16; ty++) // 16 x 16 Tiles = 1 bank
        {
            for(auto tx = 0; tx < 16; tx++)
            {
                const auto to = ty * 128 * 8 + tx * 8; // Tile offset
                
                for(auto pass = 0; pass < 2; pass++) // Two passes to register color bits
                {
                    for(int y = 0; y < 8; y++) // 8 rows per tile
                    {
                        uint8_t row = 0x00; // Byte buffer
                        
                        for(int x = 0; x < 8; x++) // 8 columns per tile
                        {
                            const auto p = bo + to + y * 128 + x;

                            // Read number into byte buffer here
                            switch(character[p])
                            {
                            case 1:
                                if(pass == 0)
                                {
                                    row |= 0b10000000 >> x;
                                }
                                break;

                            case 2:
                                if(pass == 1)
                                {
                                    row |= 0b10000000 >> x;
                                }
                                break;

                            case 3:
                                row |= 0b10000000 >> x;
                                break;

                            default:
                                break;
                            }
                        }
                        
                        file.write((char*)&row, sizeof(uint8_t));
                    }
                }
            }
        }
    }
    
    std::cout << std::endl << "Finished writing character file!" << std::endl;

    file.close();
    
    return AppStatus::Success;
}

AppStatus Media::LoadCharacter()
{
    std::vector<GLubyte> buffer;
    std::vector<GLubyte> character(bankSize * 2);
    
    std::ifstream file("mario.chr", std::ios::in | std::ios::binary);

    if(!file.is_open()) return AppStatus::Success;

    std::cout << "Reading character from file..." << std::endl;

    for(auto tile = 0; tile < 512; tile++) // 512 tiles
    {
        std::vector<GLubyte> tileBuffer(64); // 64 pixels (8 x 8) 
        
        for(auto pass = 0; pass < 2; pass++) // 16 rows define 1 tile, 1 pass is 8 rows
        {
            for(auto y = 0; y < 8; y++) // Rows
            {
                uint8_t row = 0x00;
                
                file.read((char*)&row, 1); // Read 1 byte
                
                for(auto x = 0; x < 8; x++) // Columns
                {
                    if(pass == 0 && row & (0b10000000 >> x))
                    {
                        tileBuffer[y * 8 + x] = 1;
                    }
                    else if(pass == 1 && row & (0b10000000 >> x))
                    {
                        if(tileBuffer[y * 8 + x] == 0)
                        {
                            tileBuffer[y * 8 + x] = 2;
                        }
                        else
                        {
                            tileBuffer[y * 8 + x] = 3;
                        }
                    }
                }
            }
        }

        // Insert 1 full tile
        buffer.insert(buffer.end(), tileBuffer.begin(), tileBuffer.end());
    }

    // Read 1 bank of tiles into the character, changing the format for rendering
    for(auto tile = 0; tile < 512; tile++)
    {
        const auto bo = tile * 64; // Offset for each tile in the buffer
        const auto co = floor(tile / 16) * 128 * 8 + tile % 16 * 8; // Character offset
        
        for(auto i = 0; i < 64; i++)
        {
            const auto z  = co + floor(i / 8) * 128 + i % 8;
            character[z]  = buffer[bo + i];
        }
    }
    
    std::cout << std::endl << "Finished reading character file!" << std::endl;

    file.close();

    Character::SetCharacter(std::move(character));
    
    return AppStatus::Success;
}
