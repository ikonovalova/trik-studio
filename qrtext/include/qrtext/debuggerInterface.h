#pragma once

#include <QtCore/QString>
#include <QtCore/QVariant>

namespace qrtext {

/// Interface for debuggers, watchers and other tools requiring direct access to text language interpterer.
class DebuggerInterface
{
public:
	virtual ~DebuggerInterface() {}

	/// A list of identifiers currently known to interpreter.
	virtual QStringList identifiers() const = 0;

	/// A value of identifier with given name.
	template<typename T>
	T value(const QString &identifier) const
	{
		return value(identifier).value<T>();
	}

	/// Sets a value of given identifier in interpreter to given value.
	template<typename T>
	void setVariableValue(const QString &name, T value)
	{
		setVariableValue(name, QString("%1 = %2").arg(name).arg(value), QVariant(value));
	}

	/// Sets a value of given identifier in interpreter to given vector value.
	template<typename T>
	void setVectorVariableValue(const QString &name, const QVector<T> &value)
	{
		QStringList valueStringList;
		for (const T &valueItem : value) {
			valueStringList.append(QString("%1").arg(valueItem));
		}

		const QString valueString = QString("{ %1 }").arg(valueStringList.join(", "));
		QVariant variant;
		variant.setValue(value);

		setVariableValue(name, QString("%1 = %2").arg(name).arg(valueString), variant);
	}

private:
	/// Non-generic variant of value(), returns identifier value as QVariant.
	virtual QVariant value(const QString &identifier) const = 0;

	/// Non-generic variant of setVariableValue(), sets value as QVariant. Also requires textual code for initialization
	/// of a variable if it is not known to an interpreter. This code will be parsed and interpreted before value will
	/// be set.
	virtual void setVariableValue(const QString &name, const QString &initCode, QVariant const &value) = 0;
};

}
