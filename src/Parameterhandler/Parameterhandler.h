#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <ArduinoOTA.h>

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency

// ================================ CLASS ================================ //
/**
 * @brief Handles the management off all the diffrent parameters.
 * 
 */
class Parameterhandler : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Parameterhandler();
    void setReference();
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
public:
    // ================ Methods ================ //
private:
public:
};
