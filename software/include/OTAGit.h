#pragma once

// Includes
#include <Arduino.h>

// Interface
#include "../interface/IBaseClass.h"

// Classes
class OTAGit : public IBaseClass
{
    // ## Constructor ## //
public:
    OTAGit();

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
public:
    // ## Functions ## //
private:
public:
};
