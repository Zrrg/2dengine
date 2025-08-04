#define SDL_MAIN_HANDLED
#include "Application.h"

//+-------------------------------------------------------------------------+//
//| Entry point - Multiplatform for SDL2 app
//+-------------------------------------------------------------------------+//



int main(int argc, char * argv[]) 
{
    Application app;

    app.Setup();
    
    while (app.IsRunning()) {   // Main loop
        app.Input();
        app.Update();
        app.Render();
    }

    app.Destroy();
    return 0;
}