#pragma once

// Interface
class IBaseClass
{

    // ## Functions ## //
private:
public:
    virtual ~IBaseClass() {}
    /*
            Gets called once in setup 
            @return true if init was successfull, false if not
        */
    virtual bool Init() = 0;
    /*
            Gets called every loop cycle 
        */
    virtual void Run() = 0;
};
