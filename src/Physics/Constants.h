#pragma once

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

const int PIXELS_PER_METER = 10000;
const float GRAVITY = 9.81;
const float ATTRACTION = 0.00000000006674; // gravitational attraction constant in real world 6.674*10^-11

const float BOUNCE_FACTOR = 0.9;
const float DELTA_SAFEGUARD = 0.016;
