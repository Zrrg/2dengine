#pragma once
#include "Vec2.h"
#include "Shape.h"

struct Body {
 
    // Linear motion
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    // Angular motion
    float rotation;
    float angularVelocity;
    float angularAcceleration;

    // Forces and torque
    Vec2 sumForces;
    float sumTorque;

    // Mass and moment of inertia
    float mass;
    float invMass;
    float I;
    float invI;

    // A pointer to the geometry shape of the rigid body (circle/box/polygon)
    Shape* shape = nullptr; 

    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    void AddForce(const Vec2& force);
    void AddTorque(float torque);
    void ClearForces();
    void ClearTorque();

    void IntegrateLinear(float dt);
    void IntegrateAngular(float dt);

    void Update(float dt);
};

