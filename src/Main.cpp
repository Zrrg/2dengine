#define SDL_MAIN_HANDLED    // so Windows MinGW won't complain about winMain
#include "Application.h"


//+-------------------------------------------------------------------------+//
//| Entry point - Multiplatform SDL2 app
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