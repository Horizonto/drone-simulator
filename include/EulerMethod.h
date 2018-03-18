#pragma once

const double gravityAcceleration = 9.8;

void EulerMethod(LinearMapR4 &droneMatrix, VectorR3 &currentVelocity, VectorR3 &currentAngularVelocity, double deltaTime);
