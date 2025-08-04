#include "Application.h"
#include "./Physics/Constants.h"
#include "./Graphics.h"
#include "./Force.h"

bool Application::IsRunning() {
    return running;
}

//+-------------------------------------------------------------------------+//
//| Setup function - executes once 
//+-------------------------------------------------------------------------+//
void Application::Setup() {
    running = Graphics::OpenWindow();

    Particle * smallPlanet = new Particle(200, 200, 1.0);
    smallPlanet->radius = 6;
    particles.push_back(smallPlanet);

    Particle * bigPlanet = new Particle(500, 500, 20.0);
    bigPlanet->radius = 20;
    particles.push_back(bigPlanet);

    // liquid.x = 0;
    // liquid.y = Graphics::Height() /2;
    // liquid.w = Graphics::Width();
    // liquid.h = Graphics::Height() /2;
 }

//+-------------------------------------------------------------------------+//
//| Input processing - called several times per second
//+-------------------------------------------------------------------------+//
void Application::Input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
            break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = -50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = -50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 50 * PIXELS_PER_METER;
            break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = 0;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 0;
            break;
            #if 0
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    Particle* particle = new Particle(x, y, 1.0);
                    particle->radius = 5;
                    particles.push_back(particle);
                }
                break;
            #endif
            case SDL_MOUSEMOTION:
                mouseCursor.x = event.motion.x;
                mouseCursor.y = event.motion.y;
            break;
            case SDL_MOUSEBUTTONDOWN:
                if (!leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) {
                    leftMouseButtonDown = true;
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    mouseCursor.x = x;
                    mouseCursor.y = y;
            }
            break;
            case SDL_MOUSEBUTTONUP:
                if (leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) {
                    leftMouseButtonDown = false;
                    Vec2 impulseDirection = (particles[0]->position - mouseCursor).UnitVector();
                    float impulseMagnitude = (particles[0]->position - mouseCursor).Magnitude() * 0.9;
                    particles[0]->velocity = impulseDirection * impulseMagnitude;
            }
            break;
        }
    }
}

//+-------------------------------------------------------------------------+//
//| Update function - called several times per second to update objects
//+-------------------------------------------------------------------------+//
void Application::Update() {
    // Wait some time until the reach the target frame time in milliseconds
    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks64() - timePreviousFrame); 
    if  (timeToWait > 0) 
        SDL_Delay(timeToWait);
    // Delta time - time difference between frames in seconds                        彡ﾟ◉ω◉ )つー☆*:・ﾟ:｡･:*:･ﾟ’★,｡･:*:♪･ﾟ’☆*:.｡. .｡.:*･゜ﾟ･*☆
    float deltaTime = (SDL_GetTicks64() - timePreviousFrame) / 1000.0f;
    if (deltaTime > DELTA_SAFEGUARD) deltaTime = DELTA_SAFEGUARD;
    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks64();
//+-------------------------------------------------------------------------+//
// ACTUALLY update the objects in the scene 
//+-------------------------------------------------------------------------+//
  for (auto particle : particles) {
    Vec2 wind = Vec2(0.0 * PIXELS_PER_METER, 0.0);
    Vec2 weight = Vec2(0.0, particle->mass * GRAVITY * PIXELS_PER_METER);
    //particle->AddForce(wind);
    //particle->AddForce(weight);  
    particle->AddForce(pushForce);
    // if(particle->position.y >= liquid.y) {
    //     Vec2 drag = Force::GenerateDragForce(*particle, 0.03);
    //     particle->AddForce(drag);
    // }
    Vec2 friction = Force::GenerateFrictionForce(* particle, 10.0 * PIXELS_PER_METER);
    //particle->AddForce(friction);
    Vec2 attraction = Force::GenerateGravitationalForce(*particles[0], *particles[1], 2500.0, 5 , 200);
    particles[0]->AddForce(attraction);
    particles[1]->AddForce(-attraction);
  }


// Integrate the acceleration and the velocity to find the new position
  for (auto particle : particles) {
    particle->Integrate(deltaTime);
    }

    //+-------------------------------------------------------------------------+//
    // screen border bounce
    //+-------------------------------------------------------------------------+//
  for (auto particle : particles) {
    if (particle->position.y + particle->radius >= Graphics::Height()) { // bottom
        particle->position.y = Graphics::Height() - particle->radius;
        particle->velocity.y *= -BOUNCE_FACTOR;
     }
    else if (particle->position.y <= 0) {                                    // top
        particle->position.y = particle->radius;
        particle->velocity.y *= -BOUNCE_FACTOR;
    }
    if (particle->position.x + particle->radius >= Graphics::Width()) { // right
        particle->position.x = Graphics::Width() - particle->radius;
        particle->velocity.x *= -BOUNCE_FACTOR;
    }
    else if (particle->position.x <= 0) {                                   // left
        particle->position.x = particle->radius;
        particle->velocity.x *= -BOUNCE_FACTOR;
    }
  }
}

//+-------------------------------------------------------------------------+//
//| Render function - called several times per second to draw objects
//+-------------------------------------------------------------------------+//
void Application::Render () {
    Graphics::ClearScreen(0xFF0F0725);

    // Draw the liquid in the scene
    // Graphics::DrawFillRect(liquid.x + liquid.w /2, liquid.y + liquid.h /2, liquid.w, liquid.h, 0xFF6EE3713);

      Graphics::DrawFillCircle(particles[0]->position.x, particles[0]->position.y, particles[0]->radius, 0xFFAA3300);
      Graphics::DrawFillCircle(particles[1]->position.x, particles[1]->position.y, particles[1]->radius, 0xFF00FFFF);
    
    Graphics::RenderFrame();
}

//+-------------------------------------------------------------------------+//
// Destroy function to delete objects and close the window
//+-------------------------------------------------------------------------+//
void Application::Destroy() {
    for (auto particle: particles) {
        delete particle;
    }
    Graphics::CloseWindow();
}