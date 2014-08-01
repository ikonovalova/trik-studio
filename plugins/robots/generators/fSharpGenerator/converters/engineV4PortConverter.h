#pragma once

#include <generatorBase/simpleGenerators/binding.h>

namespace fSharp {
namespace converters {

class PowerV4MotorPortConverter : public generatorBase::simple::Binding::ConverterInterface
{
public:
	static QString splitRegexp();

	QString convert(QString const &data) const override;
};

}
}
