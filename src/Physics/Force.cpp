#include "./Force.h"
#include <algorithm>

Vec2 Force::GenerateDragForce(const Body& body, float k) {
    Vec2 dragForce = Vec2(0, 0);
    if (body.velocity.MagnitudeSquared() > 0) {
        Vec2 dragDirection = body.velocity.UnitVector() * -1.0;
        float dragMagnitude = k * body.velocity.MagnitudeSquared();
        dragForce = dragDirection * dragMagnitude;
    }
    return dragForce;
}

Vec2 Force::GenerateFrictionForce(const Body& body, float k) {
    Vec2 frictionForce = Vec2(0, 0);
    Vec2 frictionDirection = body.velocity.UnitVector() * -1.0;
    float frictionMagnitude = k;
    frictionForce = frictionDirection * frictionMagnitude;
    return frictionForce;
}

Vec2 Force::GenerateSpringForce (const Body& body, Vec2 anchor, float restLength, float k) {
    // Calculate the distance between the anchor and the object
    Vec2 d = body.position - anchor;

    // Find the spring displacement considering the rest length
    float displacement = d.Magnitude() - restLength;

    // Calculate the direction and the magnitude of the spring force
    Vec2 springDirection = d.UnitVector();
    float springMagnitude = -k * displacement;

    // Calculate the final resulting spring force vectore
    Vec2 springForce = springDirection * springMagnitude;
    return springForce;
}

Vec2 Force::GenerateSpringForce (const Body& a, const Body& b, float restLength, float k) {
    // Calculate the distance between the anchor and the object
    Vec2 d = a.position - b.position;

    // Find the spring displacement considering the rest length
    float displacement = d.Magnitude() - restLength;

    // Calculate the direction and the magnitude of the spring force
    Vec2 springDirection = d.UnitVector();
    float springMagnitude = -k * displacement;

    // Calculate the final resulting spring force vectore
    Vec2 springForce = springDirection * springMagnitude;
    return springForce;
}

Vec2 Force::GenerateGravitationalForce (const Body& a, const Body& b, float G, float minDistance, float maxDistance) {
    Vec2 d = (b.position - a.position);
    float distanceSquared = d.MagnitudeSquared();
    distanceSquared = std::clamp(distanceSquared, minDistance, maxDistance); // clamp values of the distance
    Vec2 attractionDirection = d.UnitVector();
    float attractionMagnitude = G * (a.mass * b.mass) / distanceSquared;
    Vec2 attractionForce = attractionDirection * attractionMagnitude;
    return attractionForce;
}
