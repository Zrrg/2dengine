#include "Shape.h"
#include <iostream>

CircleShape::CircleShape(float radius) {
    this->radius = radius;
    std::cout << "CircleShape constructor called!\n";
};

CircleShape::~CircleShape() {
    std::cout << "CircleShape destructor called!\n";
};

Shape* CircleShape::Clone() const {
    return new CircleShape(radius);
};

ShapeType CircleShape::GetType() const {
      return CIRCLE;
};

PolygonShape::PolygonShape(const std::vector<Vec2> vertices) {
    // todo 
    std::cout << "PolygonShape constructor called!\n";
};

PolygonShape::~PolygonShape() {
    // todo 
    std::cout << "PolygonShape destructor called!\n";
};

ShapeType PolygonShape::GetType() const {
      return POLYGON;
};

Shape* PolygonShape::Clone() const {
    return new PolygonShape(vertices);
};

BoxShape::BoxShape(float width, float height) {
    // todo 
    std::cout << "PolygonShape constructor called!\n";
};

BoxShape::~BoxShape() {
    // todo 
    std::cout << "PolygonShape destructor called!\n";
};

Shape* BoxShape::Clone() const {
    return new BoxShape(width, height);
};

ShapeType BoxShape::GetType() const {
      return BOX;
};

