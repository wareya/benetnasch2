#ifndef INCLUDE_BRENDERERS
#define INCLUDE_BRENDERERS

namespace Sys
{
    namespace Renderers
    {
        bool DrawTextured(float x, float y);
        bool DrawRotateTextured(float x, float y);
        bool DrawCharacterDebug(float x, float y);
        bool DrawBackground(float x, float y);
        bool DrawBullets(float x, float y);
        bool DrawBoxes(float x, float y);
        bool DrawSpeedometer(float x, float y);
        bool DrawScreenText(float x, float y);
        bool DrawTextWindows(float x, float y);
    }   
}

#endif
