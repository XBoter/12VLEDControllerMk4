#include "Parameterhandler.h"

/**
 * @brief Constructor for the parameterhandler class
 */
Parameterhandler::Parameterhandler(){

};

/**
 * @brief Sets the needed refernce for the parameterhandler class
 */
void Parameterhandler::setReference(){

};

/**
 * @brief Initializes the parameterhandler component
 * 
 * @return True if the initialization was successful
 */
bool Parameterhandler::Init()
{
    if (!this->init)
    {
        this->init = true;
    }

    return this->init;
};

/**
 * @brief Runs the parameterhandler component
 * 
 */
void Parameterhandler::Run()
{
    if (!init)
    {
        Init();
        return;
    }
};
