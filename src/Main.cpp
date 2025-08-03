#include "Application.h"

//+-------------------------------------------------------------------------+//
//| Entry point - Multiplatform for SDL2 app
//+-------------------------------------------------------------------------+//

#ifdef _WIN32
  int WINAPI WinMain() 
#else
  int main(int argc, char * argv[]) 
#endif
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