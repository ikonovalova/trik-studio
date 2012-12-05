#include <QtCore/QDebug>

#include "methodsTester.h"

MethodsTester::MethodsTester(EditorInterface *qrmcGeneratedPlugin, EditorInterface *qrxcGeneratedPlugin)
{
	mQrmcGeneratedPlugin = qrmcGeneratedPlugin;
	mQrxcGeneratedPlugin = qrxcGeneratedPlugin;
}

class MethodsTester::StringGenerator {
public:
	virtual QString generateString(EditorInterface *editorInterface) const {
		QString resultStr = "";

		foreach (QString const &elementOfList, generateList(editorInterface)) {
			resultStr += elementOfList + " ";
		}

		return resultStr;
	}

	virtual QString methodName() const = 0;

protected:
	virtual QStringList generateList(EditorInterface *editorInterface) const = 0;

	virtual QStringList callMethod(EditorInterface *editorInterface
			, QString const &diagram
			, QString const &element = ""
			, QString const &property = "") const = 0;
};

class MethodsTester::StringGeneratorForDiagrams : public MethodsTester::StringGenerator {
	virtual QStringList generateList(EditorInterface *editorInterface) const {
		QStringList resultList;

		foreach (QString const &diagram, editorInterface->diagrams()) {
			QStringList additionalList = callMethod(editorInterface, diagram);
			resultList += additionalList;
		}

		return resultList;
	}
};

class MethodsTester::StringGeneratorForElements : public MethodsTester::StringGenerator {
	virtual QStringList generateList(EditorInterface *editorInterface) const {
		QStringList resultList;

		foreach (QString const &diagram, editorInterface->diagrams()) {
			foreach (QString const &element, editorInterface->elements(diagram)) {
				QStringList additionalList = callMethod(editorInterface, diagram, element);
				resultList += additionalList;
			}
		}

		return resultList;
	}
};

class MethodsTester::StringGeneratorForProperties : public MethodsTester::StringGenerator {
	virtual QStringList generateList(EditorInterface *editorInterface) const {
		QStringList resultList;

		foreach (QString const &diagram, editorInterface->diagrams()) {
			foreach (QString const &element, editorInterface->elements(diagram)) {
				foreach (QString const &property, editorInterface->getPropertyNames(diagram, element)) {
					QStringList additionalList = callMethod(editorInterface, diagram, element, property);
					resultList += additionalList;
				}
			}
		}

		return resultList;
	}
};

class MethodsTester::StringGeneratorForGroups : public MethodsTester::StringGenerator {
	virtual QStringList generateList(EditorInterface *editorInterface) const {
		QStringList resultList;

		foreach (QString const &diagram, editorInterface->diagrams()) {
			foreach (QString const &group, editorInterface->diagramPaletteGroups(diagram)) {
				QStringList additionalList = callMethod(editorInterface, diagram, group);
				resultList += additionalList;
			}
		}

		return resultList;
	}
};

class MethodsTester::PropertiesWithDefaultValuesStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Properties with default values";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(diagram);
		Q_UNUSED(property);
		return editorInterface->getPropertiesWithDefaultValues(element);
	}
};

class MethodsTester::TypesContainedByStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Types contained by";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(diagram);
		Q_UNUSED(property);
		return editorInterface->getTypesContainedBy(element);
	}
};

class MethodsTester::ConnectedTypesStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Connected types";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(diagram);
		Q_UNUSED(property);
		return editorInterface->getConnectedTypes(element);
	}
};

class MethodsTester::UsedTypesStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Used types";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(diagram);
		Q_UNUSED(property);
		return editorInterface->getUsedTypes(element);
	}
};

class MethodsTester::GetPossibleEdgesStringGenerator : public MethodsTester::StringGeneratorForElements {

	typedef QPair<QPair<QString, QString>, QPair<bool, QString> > PossibleEdgesType;

	virtual QString methodName() const {
		return "Get possible edges";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(diagram);
		Q_UNUSED(property);
		QStringList result;

		foreach (PossibleEdgesType const &pair, editorInterface->getPossibleEdges(element)) {
			QString const &firstElement = pair.first.first;
			QString const &secondElement = pair.first.second;

			QString const &thirdElement = QString(pair.second.first);
			QString const &fourthElement = pair.second.second;

			result << firstElement << secondElement << thirdElement << fourthElement;
		}

		return result;
	}
};

class MethodsTester::IsNodeOrEdgeStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Is node or edge";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		QStringList result;

		Q_UNUSED(diagram);
		Q_UNUSED(property);
		int const isNodeOrEdge = editorInterface->isNodeOrEdge(element);
		result.append(element);
		result.append(QString("%1").arg(isNodeOrEdge));

		return result;
	}
};

class MethodsTester::GetPropertyNamesStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Get property names";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		Q_UNUSED(property);
		return editorInterface->getPropertyNames(diagram, element);
	}
};

class MethodsTester::GetReferencePropertiesStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Get reference properties";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		Q_UNUSED(property);
		return editorInterface->getReferenceProperties(diagram, element);
	}
};

class MethodsTester::GetPropertyTypesStringGenerator : public MethodsTester::StringGeneratorForProperties {
	virtual QString methodName() const {
		return "Get property types";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		Q_UNUSED(diagram);
		QStringList result;
		result.append(editorInterface->getPropertyType(element, property));
		return result;
	}
};

class MethodsTester::GetPropertyDefaultValueStringGenerator : public MethodsTester::StringGeneratorForProperties {
	virtual QString methodName() const {
		return "Get property default values";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		Q_UNUSED(diagram);
		QStringList result;
		result.append(editorInterface->getPropertyDefaultValue(element, property));
		return result;
	}
};

class MethodsTester::GetParentsOfStringGenerator : public MethodsTester::StringGeneratorForElements {
	typedef QPair<QString, QString> ParentsOfType;

	virtual QString methodName() const {
		return "Get parents of";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		Q_UNUSED(property);
		QStringList result;

		result << element << ": ";
		foreach (ParentsOfType const &pair, editorInterface->getParentsOf(diagram, element)) {
			QString const &firstElement = pair.first;
			QString const &secondElement = pair.second;

			result << firstElement << secondElement;
		}
		result << "\n";
		return result;
	}
};

class MethodsTester::DiagramNameStringGenerator : public MethodsTester::StringGeneratorForDiagrams {
	virtual QString methodName() const {
		return "Diagram name";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const {
		Q_UNUSED(property);
		Q_UNUSED(element)
		QStringList result;

		result.append(editorInterface->diagramName(diagram));
		return result;
	}
};

class MethodsTester::DiagramNodeNameStringGenerator : public MethodsTester::StringGeneratorForDiagrams {
	virtual QString methodName() const {
		return "Diagram node name";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(property);
		Q_UNUSED(element)
		QStringList result;

		result.append(editorInterface->diagramNodeName(diagram));
		return result;
	}
};

class MethodsTester::ElementNameStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Element name";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(property);
		QStringList result;
		result.append(editorInterface->elementName(diagram, element));
		return result;
	}
};

class MethodsTester::ElementMouseGestureStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Element mouse gesture";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(property);
		QStringList result;
		result.append(editorInterface->elementMouseGesture(diagram, element));
		return result;
	}
};

class MethodsTester::ElementDescriptionStringGenerator : public MethodsTester::StringGeneratorForElements {
	virtual QString methodName() const {
		return "Element description";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(property);
		QStringList result;
		result.append(editorInterface->elementDescription(diagram, element));
		return result;
	}
};

class MethodsTester::PropertyDescriptionStringGenerator : public MethodsTester::StringGeneratorForProperties {
	virtual QString methodName() const {
		return "Property description";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		QStringList result;
		result.append(editorInterface->propertyDescription(diagram, element, property));
		return result;
	}
};

class MethodsTester::PropertyDisplayedNameStringGenerator : public MethodsTester::StringGeneratorForProperties {
	virtual QString methodName() const {
		return "Property displayed name";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		QStringList result;
		result.append(editorInterface->propertyDisplayedName(diagram, element, property));
		return result;
	}
};

class MethodsTester::DiagramPaletteGroupListStringGenerator : public MethodsTester::StringGeneratorForGroups {
	virtual QString methodName() const {
		return "Diagram palette group list";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(property);
		return editorInterface->diagramPaletteGroupList(diagram, element);
	}
};

class MethodsTester::DiagramPaletteGroupDescriptionStringGenerator : public MethodsTester::StringGeneratorForGroups {
	virtual QString methodName() const {
		return "Diagram palette group description";
	}

	virtual QStringList callMethod(EditorInterface *editorInterface
			, const QString &diagram
			, const QString &element
			, const QString &property) const
	{
		Q_UNUSED(property);
		QStringList result;
		result.append(editorInterface->diagramPaletteGroupDescription(diagram, element));
		return result;
	}
};

void MethodsTester::testMethod(StringGenerator const &stringGenerator)
{
	qDebug() << "Testing: " << stringGenerator.methodName();

	QString const &qrmcResult = stringGenerator.generateString(mQrmcGeneratedPlugin);
	qDebug() << qrmcResult;
	QString const &qrxcResult = stringGenerator.generateString(mQrxcGeneratedPlugin);
	qDebug() << qrxcResult;

	if (qrmcResult == qrxcResult) {
		qDebug() << "Method is OK";
	} else {
		qDebug() << "Method is not OK";
		qDebug() << "For qrmc: " << qrmcResult;
		qDebug() << "For qrxc: " << qrxcResult;
	}

	qDebug() << "\n";
}

void MethodsTester::testMethods()
{
	testMethod(PropertiesWithDefaultValuesStringGenerator());
	testMethod(UsedTypesStringGenerator());
	testMethod(TypesContainedByStringGenerator());
	testMethod(ConnectedTypesStringGenerator());
	testMethod(GetPossibleEdgesStringGenerator());
	testMethod(IsNodeOrEdgeStringGenerator());

	testMethod(GetPropertyNamesStringGenerator());
	testMethod(GetReferencePropertiesStringGenerator());
	testMethod(GetParentsOfStringGenerator());

	testMethod(GetPropertyTypesStringGenerator());
	testMethod(GetPropertyDefaultValueStringGenerator());

	testMethod(DiagramNameStringGenerator());
	testMethod(DiagramNodeNameStringGenerator());

	testMethod(ElementNameStringGenerator());
	testMethod(ElementMouseGestureStringGenerator());
	testMethod(ElementDescriptionStringGenerator());

	testMethod(PropertyDisplayedNameStringGenerator());
	testMethod(PropertyDescriptionStringGenerator());

	testMethod(DiagramPaletteGroupListStringGenerator());
	testMethod(DiagramPaletteGroupDescriptionStringGenerator());
}
