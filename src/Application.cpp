#include "Application.h"
#include "./Physics/Constants.h"
#include "./Graphics.h"

bool Application::IsRunning() {
    return running;
}

//+-------------------------------------------------------------------------+//
//| Setup function - executes once
//+-------------------------------------------------------------------------+//
void Application::Setup() {
    running = Graphics::OpenWindow();

    Particle * smallBall = new Particle(200, 100, 1.0);
    smallBall->radius = 4;
    particles.push_back(smallBall);

    Particle * bigBall = new Particle(500, 100, 3.0);
    bigBall->radius = 12;
    particles.push_back(bigBall);
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
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = 0;
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
    if (deltaTime > DELTA_SAFEGUARD)
        deltaTime = DELTA_SAFEGUARD;


    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks64();
//+-------------------------------------------------------------------------+//
// ACTUALLY update the objects in the scene 
//+-------------------------------------------------------------------------+//
//   for (auto particle : particles) {
//     // Apply a "wind force"
//     Vec2 wind = Vec2(1.0 * PIXELS_PER_METER, 0.0);
//     particle->AddForce(wind);
//   }

  for (auto particle : particles) {
    // Apply a "weight force"
    Vec2 weight = Vec2(0.0, particle->mass * GRAVITY * PIXELS_PER_METER);
    particle->AddForce(weight);
  }

  for (auto particle : particles) {
    particle->AddForce(pushForce);
  }

  for (auto particle : particles) {
    // Integrate the acceleration and the velocity to find the new position
    particle->Integrate(deltaTime);
    }

    //+-------------------------------------------------------------------------+//
    // screen border bounce
    //+-------------------------------------------------------------------------+//
  for (auto particle : particles) {
    if (particle->position.y + particle->radius >= Graphics::windowHeight) { // bottom
        particle->position.y = Graphics::windowHeight - particle->radius;
        particle->velocity.y *= -BOUNCE_FACTOR;
     }
    else if (particle->position.y <= 0) {                                    // top
        particle->position.y = particle->radius;
        particle->velocity.y *= -BOUNCE_FACTOR;
    }
    if (particle->position.x + particle->radius >= Graphics::windowWidth) { // right
        particle->position.x = Graphics::windowWidth - particle->radius;
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
    Graphics::ClearScreen(0xFF0D327A);
    for (auto particle : particles) {
      Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
    }
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