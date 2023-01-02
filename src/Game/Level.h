#ifndef LEVEL_H
#define LEVEL_H

#include "Scene/Scene.h"

#include "Core/Image.h"
#include "Core/Foundations.h"


struct Level;

DECLARE_PTR_TYPE(Level);


struct Level
{
    ScenePtr scene;
    ImagePtr map;
    std::string floorName;

    inline static LevelPtr Create() { return std::make_shared<Level>(); }
};


#endif // LEVEL_H