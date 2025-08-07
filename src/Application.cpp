#ifndef IMGUI_ENABLED
#define IMGUI_ENABLED false
#endif

#include "Application.h"
#include "./Physics/Constants.h"
#include "./Graphics.h"
#include "./Force.h"
#include "./Shape.h"
#if IMGUI_ENABLED
    #include "imgui.h"
    #include "imgui_impl_sdl2.h"
    #include "imgui_impl_sdlrenderer2.h"
#endif


bool Application::IsRunning() {
    return running;
}

//+-------------------------------------------------------------------------+//
//| Setup function - executes once 
//+-------------------------------------------------------------------------+//
void Application::Setup() {
    running = Graphics::OpenWindow();

#if IMGUI_ENABLED
    // ImGui integration - setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplSDL2_InitForSDLRenderer(Graphics::window, Graphics::renderer);
    ImGui_ImplSDLRenderer2_Init(Graphics::renderer);
#endif

    Body* bob = new Body(CircleShape(30), 500 , 500, 2.0);
    bodies.push_back(bob);

    Body* sam = new Body(CircleShape(20), 500 , 300, 2.0);
    bodies.push_back(sam);

    Body* petr = new Body(CircleShape(45), 500 , 700, 2.0);
    bodies.push_back(petr);

    Body* box = new Body(BoxShape(200,100), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 1.0);
    bodies.push_back(box);
}

//+-------------------------------------------------------------------------+//
//| Input processing - called several times per second
//+-------------------------------------------------------------------------+//
void Application::Input() {
    SDL_Event event;
    int pushConst = 50;
    while (SDL_PollEvent(&event)) {
    #if IMGUI_ENABLED
        ImGui_ImplSDL2_ProcessEvent(&event);
    #endif
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
                    Body* body = new Body(x, y, 1.0);
                    body->radius = 5;
                    bodies.push_back(body);
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
                    Vec2 impulseDirection = (bodies[0]->position - mouseCursor).UnitVector();
                    float impulseMagnitude = (bodies[0]->position - mouseCursor).Magnitude() * 5.0;
                    bodies[0]->velocity = impulseDirection * impulseMagnitude;
            }
            break;
        }
    }
}

//+-------------------------------------------------------------------------+//
//| Update function - called several times per second to update objects
//+-------------------------------------------------------------------------+//
void Application::Update() {
  #if IMGUI_ENABLED
    // Imgui integration - update
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
  #endif

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
    

    for (auto body : bodies) {
        Vec2 drag = Force::GenerateDragForce(*body, 0.002);
        Vec2 weight = Vec2(0.0, body->mass * GRAVITY * 100);

         body->AddForce(pushForce);
         body->AddForce(drag);     
        // body->AddForce(weight);

        float torque = 200;
        body->AddTorque(torque);
    }

//+-------------------------------------------------------------------------+//
// Integrate the acceleration and the velocity to find the new position
//+-------------------------------------------------------------------------+//

    for (auto body : bodies) {
        body->Update(deltaTime);
    }

//+-------------------------------------------------------------------------+//
// screen border bounce
//+-------------------------------------------------------------------------+//
    for (auto body : bodies) {
        if (body->shape->GetType() == CIRCLE) {
            CircleShape* circleShape = (CircleShape*)body->shape;
            if (body->position.y + circleShape->radius >= Graphics::Height()) { // bottom
                body->position.y = Graphics::Height() - circleShape->radius;
                body->velocity.y *= -BOUNCE_FACTOR;
            } else if (body->position.y <= 0) {                                    // top
                body->position.y = circleShape->radius;
                body->velocity.y *= -BOUNCE_FACTOR;
            }
            if (body->position.x + circleShape->radius >= Graphics::Width()) { // right
                body->position.x = Graphics::Width() - circleShape->radius;
                body->velocity.x *= -BOUNCE_FACTOR;
            } else if (body->position.x <= 0) {                                   // left
                body->position.x = circleShape->radius;
                body->velocity.x *= -BOUNCE_FACTOR;
            }
        }
    }
}

//+-------------------------------------------------------------------------+//
//| Render function - called several times per second to draw objects
//+-------------------------------------------------------------------------+//
void Application::Render () {
    Graphics::ClearScreen(0xFF0F0725);

#if IMGUI_ENABLED
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Graphics::renderer);
#endif

     if (leftMouseButtonDown) 
         Graphics::DrawLine(bodies[0]->position.x, bodies[0]->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);


     for (auto body : bodies) {
        if (body->shape->GetType() == CIRCLE) {
            CircleShape* circleShape = (CircleShape*) body->shape;
            Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, 0xFFFFFFFF);
        }
        if (body->shape->GetType() == BOX) {
            BoxShape* boxShape = (BoxShape*)body->shape;
            Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFFFFFFFF);
        }
    }
    Graphics::RenderFrame();
}

//+-------------------------------------------------------------------------+//
// Destroy function to delete objects and close the window
//+-------------------------------------------------------------------------+//
void Application::Destroy() {
    for (auto body: bodies) {
        delete body;
    }

  #if IMGUI_ENABLED
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  #endif
    Graphics::CloseWindow();
}