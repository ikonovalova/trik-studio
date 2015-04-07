#pragma once

#include <QtCore/QString>
#include <QtCore/QSharedPointer>

#include <qrgui/plugins/interpretedPluginInterface/interpretedPluginInterface.h>

#include "ast/node.h"

#include "variablesTable.h"

namespace generationRules {
namespace generator {

class CommonGenerator
{
public:
	static QString generatedResult(QSharedPointer<simpleParser::ast::Node> node
			, qReal::LogicalModelAssistInterface *logicalModelInterface
			, VariablesTable tableOfVariables
			, qReal::EditorManagerInterface *editorManagerInterface
			, const qReal::Id elementId = qReal::Id::rootId()
			, const QString &elementType = ""
			, const QString &elementName = ""
			);
};
}
}
