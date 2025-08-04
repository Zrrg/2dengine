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

    anchor = Vec2(Graphics::Width() / 2.0 , 30);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle* bob = new Particle(anchor.x , anchor.y + (i * restLength), 2.0);
        bob->radius = 6;
        particles.push_back(bob);
    }
}
//+-------------------------------------------------------------------------+//
//| Input processing - called several times per second
//+-------------------------------------------------------------------------+//
void Application::Input() {
    SDL_Event event;
    int pushConst = 150;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
            break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = -pushConst * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = -pushConst * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = pushConst * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = pushConst * PIXELS_PER_METER;
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
                    int lastParticle = NUM_PARTICLES - 1;
                    Vec2 impulseDirection = (particles[lastParticle]->position - mouseCursor).UnitVector();
                    float impulseMagnitude = (particles[lastParticle]->position - mouseCursor).Magnitude() * 5.0;
                    particles[lastParticle]->velocity = impulseDirection * impulseMagnitude;
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
    particles[0]->AddForce(pushForce);

  for (auto particle : particles) {
    Vec2 drag = Force::GenerateDragForce(*particle, 0.002);
    particle->AddForce(drag);
    
    Vec2 weight = Vec2(0.0, particle->mass * GRAVITY * 100);
    particle->AddForce(weight);

  }

// Apply a spring force to the particle connceted to the anchor

   Vec2 springForce = Force::GenerateSpringForce(*particles[0], anchor, restLength, k);
   particles[0]->AddForce(springForce);

    for (int i = 1; i < NUM_PARTICLES; i++) {
        Vec2 springForce = Force::GenerateSpringForce(*particles[i], *particles[i-1], restLength, k);
        particles[i]->AddForce(springForce);
        particles[i-1]->AddForce(-springForce);
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

     if (leftMouseButtonDown) 
         Graphics::DrawLine(particles[NUM_PARTICLES-1]->position.x, particles[NUM_PARTICLES-1]->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);

   // Draw the anchor
    Graphics::DrawFillCircle(anchor.x, anchor.y, 5, 0xFF001155);
    

// Предполагая, что particles - это std::vector<Particle*> или аналогичный контейнер указателей
    for (int i = 0; i < NUM_PARTICLES; i++) {

    Particle* particle = particles[i];
    
    // Рисуем саму частицу
    Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
    
    if (i == 0) {
        // Для первой частицы - линия к anchor
        Graphics::DrawLine(anchor.x, anchor.y, particle->position.x, particle->position.y, 0xFF346622);
    } else {
        // Для остальных - линия к предыдущей частице
        Particle* prev_particle = particles[i-1];
        Graphics::DrawLine(prev_particle->position.x, prev_particle->position.y, 
                          particle->position.x, particle->position.y, 0xFF346622);
    }
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