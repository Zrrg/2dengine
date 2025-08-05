#pragma once
#include "Vec2.h"
#include "Shape.h"

struct Body {
 
    // Linear motion
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    // Forces
    Vec2 sumForces;

    // Mass
    float mass;
    float invMass;

    // A pointer to the geometry shape of the rigid body (circle/box/polygon)
    Shape* shape = NULL; 

    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    void AddForce(const Vec2& force);
    void ClearForces();

    void Integrate(float dt);
};

