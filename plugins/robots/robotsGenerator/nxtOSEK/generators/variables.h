#pragma once

#include <QtCore/QMap>
#include <QtCore/QStringList>

#include "../../../../../qrrepo/repoApi.h"

namespace robots {
namespace generator {

enum VariableType
{
	unknown = 0
	, floatType
	, intType
};

class Variables : QMap<QString, VariableType>
{
public:
	Variables();

	/// Tries to infer types for all variables declared in all function blocks
	/// on the specified diagram
	void reinit(qrRepo::RepoApi *api);
	QString generateVariableString() const;

private:
	QMap<QString, VariableType> nonGenerableReservedVariables() const;
	QMap<QString, int> intConstants() const;
	QMap<QString, float> floatConstants() const;
	QMap<QString, VariableType> reservedVariables() const;

	void inferTypes(QStringList const &expressions);

	/// Groups given expressions by variable names
	QMap<QString, QStringList> variablesExpressionsMap(QStringList const &expressions) const;

	/// Assigns to the given variable specified type if it is wider than old one
	void assignType(QString const &name, VariableType type);

	/// Adds to the target list all the variable names participating
	/// in the expression. Returns int or float type if expression has
	/// pre-determined type (for example, contains only int constants or one float)
	/// or unknown else
	VariableType participatingVariables(QString const &expression, QStringList &currentNames);

	/// Invokes inference process
	void startDeepInference(QMap<QString, QStringList> &dependencies);
	QStringList dependentFrom(QMap<QString, QStringList> const &dependencies
			, QString const variable) const;
	bool removeDependenciesFrom(QMap<QString, QStringList> &dependencies
			, QString const variable);

	bool isIdentifier(QString const &token);
	bool isDigit(QChar const &c) const;
	bool isLetter(QChar const &c) const;
};

}
}
