// Include guard
#ifndef INFORMATION_H_INCLUDE
#define INFORMATION_H_INCLUDE

#include <Arduino.h>
#include "Network.h"

namespace LedControllerSoftwareMk5
{

    class Information
    {
        // ## Data ## //
    private:
        Network *network;               // Pointer to used network instance
        Network memNetwork = Network(); // Memory version of network instance
        bool init = false;
        const String symbol = "=";
        const uint8_t spacerLength = 40;    // x * Spaces
        const uint8_t insertLength = 4;     // x * Spaces

    public:

        // ## Functions ## //
    private:
        void FormatPrintSingle( String name, 
                                String paramterValue);

        void FormatPrintTime(   String name, 
                                String hour, 
                                String minute);

        void FormatPrintLEDStrip(   String name, 
                                    String power, 
                                    String brightness,
                                    String cw,
                                    String ww,
                                    String red,
                                    String green,
                                    String blue,
                                    String effect);

        void FormatPrintMotion( String name, 
                                String motionPower, 
                                String red,
                                String green,
                                String blue,
                                String timeout);
        void TopSpacerPrint();
        void BottomSpacerPrint();
        void InsertPrint();
        String BoolToString(bool b);

    public:
        Information(Network *network);
        void Init();
        void Run();
    };

};

#endif