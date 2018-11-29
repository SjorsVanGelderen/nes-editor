#ifndef IDRAWABLE_H
#define IDRAWABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct IDrawable
{
    virtual ~IDrawable() {}

    virtual AppStatus Draw(glm::mat4 projection, glm::mat4 view, glm::vec2 mouse)
    {
        return AppStatus::Success;
    }

    virtual glm::vec2 GetPosition()
    {
        return glm::vec2(0, 0);
    }

    virtual glm::vec2 GetSize() 
    {
        return glm::vec2(0, 0);
    }

    // TODO: Fix the definition of idrawable.. maybe iupdateable / ientity? :)
    virtual bool Click(glm::vec2 mouse)
    {
        return false;
    }

    virtual bool Release(glm::vec2 mouse)
    {
        return false;
    }
};

#endif
