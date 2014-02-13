#pragma once

#include <interpreterBase/robotModel/robotParts/touchSensor.h>
#include "nxtInputDevice.h"

namespace nxtKitInterpreter {
namespace robotModel {
namespace real {
namespace parts {

class TouchSensor : public interpreterBase::robotModel::robotParts::TouchSensor, public NxtInputDevice
{
	Q_OBJECT

public:
	TouchSensor(interpreterBase::robotModel::PluggableDeviceInfo const &info
			, interpreterBase::robotModel::PortInfo const &port
			, utils::robotCommunication::RobotCommunicator &robotCommunicator);

	void read() override;

private:
	void sensorSpecificProcessResponse(QByteArray const &reading) override;
	void onDeviceConfigured() override;
};

}
}
}
}