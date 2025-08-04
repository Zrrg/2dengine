#pragma once
#include "Vec2.h"

struct Body {
    int radius; 
    float mass;
    float invMass;

    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 sumForces;
     
    Body(float x, float y, float mass);
    ~Body();

    void AddForce(const Vec2& force);
    void ClearForces();
    void Integrate(float dt);
};

