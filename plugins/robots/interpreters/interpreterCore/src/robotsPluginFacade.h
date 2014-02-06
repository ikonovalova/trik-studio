#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include <qrgui/toolPluginInterface/pluginConfigurator.h>

#include "customizer.h"
#include "managers/kitPluginManager.h"
#include "managers/robotModelManager.h"
#include "managers/titlesVisibilityManager.h"
#include "managers/actionsManager.h"
#include "managers/sensorsConfigurationManager.h"
#include "managers/graphicsWatcherManager.h"
#include "interpreter/interpreter.h"
#include "textLanguage/robotsBlockParser.h"
#include "ui/robotsSettingsPage.h"
#include "ui/sensorsConfigurationWidget.h"

namespace interpreterCore {

/// Responsible for initialization, interconnecting and keeping in sync core plugin subsystems.
/// \todo Document this.
class RobotsPluginFacade : public QObject
{
	Q_OBJECT

public:
	RobotsPluginFacade();

	~RobotsPluginFacade() override;

	void init(qReal::PluginConfigurator const &configurer);

	PreferencesPage *robotsSettingsPage() const;  // Transfers ownership.

	interpreterCore::Customizer &customizer();

	ActionsManager &actionsManager();

private:
	/// Customizer object for this plugin
	Customizer mCustomizer;

	textLanguage::RobotsBlockParser *mParser;  // Has ownership

	/// Main class for robot interpreter. Contains implementation of generic diagram interpreter.
	interpreter::InterpreterInterface *mInterpreter;  // Has ownership

	/// Page with plugin settings. Created here, but then ownership is passed to
	/// a caller of preferencesPage().
	ui::RobotsSettingsPage *mRobotSettingsPage;  // Does not have ownership

	KitPluginManager mKitPluginManager;
	RobotModelManager mRobotModelManager;
	ActionsManager mActionsManager;
	QScopedPointer<TitlesVisibilityManager> mTitlesVisibilityManager;
	SensorsConfigurationManager mSensorsConfigurationManager;

	ui::SensorsConfigurationWidget *mDockSensorsConfigurer;  // Does not have ownership
	utils::WatchListWindow *mWatchListWindow;  // Does not have ownership
	GraphicsWatcherManager *mGraphicsWatcherManager;  // Has ownership
};

}
