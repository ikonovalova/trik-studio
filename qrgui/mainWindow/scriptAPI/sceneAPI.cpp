#include "sceneAPI.h"

#include <editor/editorView.h>
#include <editor/editorViewScene.h>

#include "mainWindow.h"
#include "palette/draggableElement.h"
#include "scriptAPI.h"
#include "virtualCursor.h"

using namespace qReal;
using namespace gui;

SceneAPI::SceneAPI(ScriptAPI &scriptAPI, MainWindow &mainWindow)
	: mScriptAPI(scriptAPI)
	, mMainWindow(mainWindow)
{
}

void SceneAPI::drawLink(const QString &fromElementId, const QString &toElementId, int duration)
{
	if (!mMainWindow.getCurrentTab()) {
		return;
	}

	const EditorViewScene *scene = &mMainWindow.getCurrentTab()->editorViewScene();
	const EditorView *sceneView = mMainWindow.getCurrentTab();

	const NodeElement *toNode = scene->getNodeById(Id::loadFromString(toElementId));
	const NodeElement *fromNode = scene->getNodeById(Id::loadFromString(fromElementId));

	if (toNode && fromNode) {
		mScriptAPI.virtualCursor().sceneMoveTo(sceneView->viewport()
				, duration / 2
				, sceneView->mapFromScene(fromNode->pos()).x()
				, sceneView->mapFromScene(fromNode->pos()).y());

		mScriptAPI.virtualCursor().rightButtonPress(sceneView->viewport());

		mScriptAPI.virtualCursor().sceneMoveTo(sceneView->viewport()
				, duration / 2
				, sceneView->mapFromScene(toNode->pos()).x()
				, sceneView->mapFromScene(toNode->pos()).y());

		mScriptAPI.virtualCursor().rightButtonRelease(sceneView->viewport(), 50);
	}
}

QString SceneAPI::createBlockOnScene(const DraggableElement *paletteElement
		, int xSceneCoord, int ySceneCoord)
{
	if (EditorView * const currentTab = mMainWindow.getCurrentTab()) {
		const Id elementId(paletteElement->id(), QUuid::createUuid().toString());
		const QMimeData *mimeData = paletteElement->mimeData(elementId);
		currentTab->mutableScene().createElement(
				paletteElement->mimeData(elementId)
				, currentTab->mapToScene(QPoint(xSceneCoord, ySceneCoord))
				, false
				, nullptr
				, true);

		QByteArray itemData = mimeData->data("application/x-real-uml-data");
		QDataStream inStream(&itemData, QIODevice::ReadOnly);
		QString uuid;
		inStream >> uuid;
		return uuid;
	}

	return QString();
}

QStringList SceneAPI::nodeList(const QString &diagram, const QString &element)
{
	if (!mMainWindow.getCurrentTab()) {
		return {};
	}

	/// @todo: Rewrite it using models
	const QList<QGraphicsItem *> items = mMainWindow.getCurrentTab()->editorViewScene().items();
	QStringList result;
	for (QGraphicsItem * const item : items) {
		const NodeElement *node = dynamic_cast<NodeElement*>(item);
		if (node && node->id().diagram() == diagram && node->id().element() == element) {
			result << node->id().toString();
		}
	}

	return result;
}
