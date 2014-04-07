#include "colorSensorGreen.h"

using namespace nxtKitInterpreter::robotModel::real::parts;

ColorSensorGreen::ColorSensorGreen(interpreterBase::robotModel::DeviceInfo const &info
		, interpreterBase::robotModel::PortInfo const &port
		, utils::robotCommunication::RobotCommunicator &robotCommunicator)
	: ColorSensor(info, port, robotCommunicator, enums::lowLevelSensorType::COLORGREEN)
{
}