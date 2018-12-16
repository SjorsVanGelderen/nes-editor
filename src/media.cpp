#include "media.h"

std::map<std::string, GLuint> Media::shaderPrograms;

const auto bankSize = pow(128, 2);

AppStatus Media::Start()
{
  ilInit();
  ilClearColour(255, 255, 255, 0);

  iluInit();

  ILenum error = ilGetError();
  if(error != IL_NO_ERROR)
  {
    return AppStatus::FailureDevILStart;
  }

  return AppStatus::Success;
}

AppStatus Media::Stop()
{
  return AppStatus::Success;
}

std::pair<AppStatus, GLuint> Media::LoadTexture(std::string path)
{
  ILuint id = 0;
  
  ilGenImages(1, &id);
  ilBindImage(id);

  if(ilLoadImage(path.c_str()) != IL_TRUE)
  {
    ILenum error = ilGetError();

    std::stringstream stream;

    stream << "Failed to load image with IL"
           << iluErrorString(error) 
           << std::endl;

    Debug::Log(LogLevel::Error, stream.str());
    
    return std::make_pair(AppStatus::FailureTextureLoad, 0);
  }

  if(ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) != IL_TRUE)
  {
    Debug::Log(LogLevel::Error, "Failed to convert image with IL");
    return std::make_pair(AppStatus::FailureTextureLoad, 0);
  }

  auto data   = (GLuint*)ilGetData();
  auto width  = (GLuint)ilGetInteger(IL_IMAGE_WIDTH);
  auto height = (GLuint)ilGetInteger(IL_IMAGE_HEIGHT);

  GLuint textureId;
  
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  
  glTexImage2D
    ( GL_TEXTURE_2D, 0, GL_RGBA, width, height
    , 0, GL_RGBA, GL_UNSIGNED_BYTE, data
    );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  ilDeleteImages(1, &id);
  
  return std::make_pair(AppStatus::Success, textureId);
}

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
    
  GLuint modeId 
    = modeString == ".vert"
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

// std::pair<AppStatus, std::vector<GLubyte>> Media::LoadSamples()
// {
//     return std::make_pair(AppStatus::Success, std::vector<GLubyte>());
// }

AppStatus Media::SaveSamples()
{
  auto samples = Samples::GetSamples();

  std::ofstream file("samples.sam", std::ios::out | std::ios::binary | std::ios::trunc);

  if(!file.is_open()) return AppStatus::Success;

  Debug::Log(LogLevel::Info, "Writing samples to file...");

  for(const auto &sample : *samples)
  {
    file.write((char*)&sample, sizeof(uint8_t));
  }

  Debug::Log(LogLevel::Info, "Finished writing sample file!");

  file.close();
    
  return AppStatus::Success;
}

AppStatus Media::LoadSamples()
{
  std::vector<GLuint> samples;

  std::ifstream file("samples.sam", std::ios::in | std::ios::binary);

  if(!file.is_open()) return AppStatus::Success;

  Debug::Log(LogLevel::Info, "Reading samples from file...");

  uint8_t sample = 0x00;
    
  while(file.read((char*)&sample, 1))
  {
    samples.push_back(sample);
  }

  Debug::Log(LogLevel::Info, "Finished reading samples from file!");
    
  file.close();

  Samples::SetSamples(std::move(samples));

  return AppStatus::Success;
}

AppStatus Media::SaveCharacter()
{
  const auto character = Character::GetCharacter();

  std::ofstream file("data.chr", std::ios::out | std::ios::binary | std::ios::trunc);

  if(!file.is_open()) return AppStatus::Success;

  Debug::Log(LogLevel::Info, "Writing character to file...");

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
          for(auto y = 0; y < 8; y++) // 8 rows per tile
          {
            uint8_t row = 0x00; // Byte buffer
                        
            for(auto x = 0; x < 8; x++) // 8 columns per tile
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
    
  Debug::Log(LogLevel::Info, "Finished writing character file!");

  file.close();
    
  return AppStatus::Success;
}

AppStatus Media::LoadCharacter()
{
  std::vector<GLubyte> buffer;
  std::vector<GLubyte> character(bankSize * 2);
    
  std::ifstream file("data.chr", std::ios::in | std::ios::binary);

  if(!file.is_open()) return AppStatus::Success;

  Debug::Log(LogLevel::Info, "Reading character from file...");

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
    
  Debug::Log(LogLevel::Info, "Finished reading character file!");

  file.close();

  Character::SetCharacter(std::move(character));
    
  return AppStatus::Success;
}
