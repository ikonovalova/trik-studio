#include "trikKitInterpreterCommon/trikEmulation/trikmotoremu.h"

trik::TrikMotorEmu::TrikMotorEmu(kitBase::robotModel::robotParts::Motor *motor) : mMotor(motor)
{
}

void trik::TrikMotorEmu::setPower(int power, bool constrain)
{
	mMotor->on(constrain ? qBound(minControl(), power, maxControl()) : power);
}

int trik::TrikMotorEmu::power() const
{
	return mMotor->power();
}

void trik::TrikMotorEmu::powerOff()
{
	mMotor->off();
}

void trik::TrikMotorEmu::forceBreak(int)
{
	powerOff(); // should be the same for 2D model, since there is no inertia
}
