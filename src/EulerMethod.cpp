//#include "Everything.h"

#include "LinearR3.h"
#include "LinearR4.h"
#include "MathMisc.h"
#include "MyDrone.h"
#include "DrawScene.h"

const double coefficientOfLift = 1;
const double airDensity = 1.225;
const double surfaceArea = PI2 * bladeLength * bladeWidth;
const double epsilon = 1e-6;

double velocity;

void EulerMethod(LinearMapR4 &droneMatrix, VectorR3 &currentVelocity, VectorR3 &currentAngularVelocity, double deltaTime) {
	VectorR3 totalForce = VectorR3(0.0, 0.0, 0.0);
	VectorR3 totalTorque = VectorR3(0.0, 0.0, 0.0);
	VectorR3 positionVec[4] = { VectorR3(-1.0, 0.0, 0.0), VectorR3(0.0, 0.0, 1.0), VectorR3(1.0, 0.0, 0.0), VectorR3(0.0, 0.0, -1.0) };
	for (int i = 0; i < 4; i++) {
		velocity = spinVelocity[i] * bladeLength / 2.0;
		double liftForce = coefficientOfLift * 0.5 * airDensity * abs(velocity) * velocity / 3.0 * surfaceArea;
		totalForce += VectorR3(0.0, liftForce, 0.0);
		totalTorque += positionVec[i] * VectorR3(0.0, liftForce, 0.0);
	}
	VectorR3 acceleration = totalForce / totalMass;
	VectorR3 angularAcceleration = momentOfInertia.Inverse() * totalTorque;
	currentVelocity += acceleration * deltaTime;
	currentAngularVelocity += angularAcceleration * deltaTime;
	droneMatrix.Mult_glTranslate(currentVelocity * deltaTime);
	double angularSpeed = currentAngularVelocity.Norm();
	if (angularSpeed > epsilon)
		droneMatrix.Mult_glRotate(angularSpeed * deltaTime, currentAngularVelocity);
}
