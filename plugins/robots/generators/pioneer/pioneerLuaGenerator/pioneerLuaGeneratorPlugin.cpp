/* Copyright 2017 QReal Research Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "pioneerLuaGeneratorPlugin.h"

#include <qrkernel/logging.h>
#include <qrkernel/settingsManager.h>
#include <pioneerKit/blocks/pioneerBlocksFactory.h>
#include <pioneerKit/constants.h>

#include "pioneerLuaMasterGenerator.h"
#include "communicator/controllerCommunicator.h"
#include "robotModel/pioneerGeneratorRobotModel.h"
#include "widgets/pioneerAdditionalPreferences.h"

using namespace pioneer;
using namespace pioneer::lua;
using namespace qReal;

PioneerLuaGeneratorPlugin::PioneerLuaGeneratorPlugin()
	: mGenerateCodeAction(new QAction(nullptr))
	, mUploadProgramAction(new QAction(nullptr))
	, mRunProgramAction(new QAction(nullptr))
	, mBlocksFactory(new blocks::PioneerBlocksFactory)
	, mGeneratorForRealCopterRobotModel(
			new PioneerGeneratorRobotModel(
					kitId()
					, "Pioneer"
					, modelNames::realCopter
					, tr("Pioneer model (real copter)")
					, 9
				)
		)
	, mGeneratorForSimulatorRobotModel(
		new PioneerGeneratorRobotModel(
				kitId()
				, "Pioneer"
				, modelNames::simulator
				, tr("Pioneer model (simulator)")
				, 10
			)
		)
{
	mAdditionalPreferences = new PioneerAdditionalPreferences;

	mGenerateCodeAction->setText(tr("Generate to Pioneer Lua"));
	mGenerateCodeAction->setIcon(QIcon(":/pioneer/lua/images/generateLuaCode.svg"));
	connect(mGenerateCodeAction, &QAction::triggered, this, &PioneerLuaGeneratorPlugin::generateCode);

	mUploadProgramAction->setText(tr("Upload generated program to Pioneer"));
	mUploadProgramAction->setIcon(QIcon(":/pioneer/lua/images/upload.svg"));
	connect(mUploadProgramAction, &QAction::triggered, this, &PioneerLuaGeneratorPlugin::uploadProgram);

	mRunProgramAction->setText(tr("Run program on a Pioneer"));
	mRunProgramAction->setIcon(QIcon(":/pioneer/lua/images/run.svg"));
	connect(mRunProgramAction, &QAction::triggered, this, &PioneerLuaGeneratorPlugin::runProgram);

	text::Languages::registerLanguage(text::LanguageInfo{ "lua"
			, tr("Lua language")
			, true
			, 4
			, nullptr
			, {}
	});
}

PioneerLuaGeneratorPlugin::~PioneerLuaGeneratorPlugin()
{
	if (mOwnsAdditionalPreferences) {
		delete mAdditionalPreferences;
	}
}

void PioneerLuaGeneratorPlugin::init(const kitBase::KitPluginConfigurator &configurator)
{
	generatorBase::RobotsGeneratorPluginBase::init(configurator);
	mControllerCommunicator.reset(
			new ControllerCommunicator(mMainWindowInterface->errorReporter(), mRobotModelManager)
	);

	connect(
			mControllerCommunicator.data()
			, &CommunicatorInterface::uploadCompleted
			, [this]() { setUploadAndRunActionsEnabled(true); }
	);

	connect(
			mControllerCommunicator.data()
			, &CommunicatorInterface::runCompleted
			, [this]() { setUploadAndRunActionsEnabled(true); }
	);
}


QList<ActionInfo> PioneerLuaGeneratorPlugin::customActions()
{
	const ActionInfo generateCodeActionInfo(mGenerateCodeAction, "generators", "tools");
	const ActionInfo uploadProgramActionInfo(mUploadProgramAction, "generators", "tools");
	const ActionInfo runProgramActionInfo(mRunProgramAction, "generators", "tools");
	return { generateCodeActionInfo, uploadProgramActionInfo, runProgramActionInfo };
}

QList<HotKeyActionInfo> PioneerLuaGeneratorPlugin::hotKeyActions()
{
	mGenerateCodeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
	mUploadProgramAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
	mRunProgramAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F5));

	HotKeyActionInfo generateActionInfo(
			"Generator.GeneratePioneerLua"
			, tr("Generate Lua script for Pioneer Quadcopter")
			, mGenerateCodeAction);

	HotKeyActionInfo uploadProgramInfo(
			"Generator.UploadPioneerLua"
			, tr("Upload Pioneer program")
			, mUploadProgramAction);

	HotKeyActionInfo runProgramInfo("Generator.RunPioneerLua", tr("Run Pioneer Program"), mRunProgramAction);

	return { generateActionInfo, uploadProgramInfo, runProgramInfo };
}

QIcon PioneerLuaGeneratorPlugin::iconForFastSelector(const kitBase::robotModel::RobotModelInterface &robotModel) const
{
	Q_UNUSED(robotModel)
	return QIcon(":/pioneer/lua/images/switchToPioneerGenerator.svg");
}

int PioneerLuaGeneratorPlugin::priority() const
{
	return 9;
}

QString PioneerLuaGeneratorPlugin::kitId() const
{
	return "pioneerKit";
}

QList<kitBase::robotModel::RobotModelInterface *> PioneerLuaGeneratorPlugin::robotModels()
{
	return { mGeneratorForRealCopterRobotModel.data()
#ifndef WIN32
			// Simulator does not work for Windows.
			, mGeneratorForSimulatorRobotModel.data()
#endif
	};
}

kitBase::blocksBase::BlocksFactoryInterface *PioneerLuaGeneratorPlugin::blocksFactoryFor(
		const kitBase::robotModel::RobotModelInterface *model)
{
	Q_UNUSED(model)
	return mBlocksFactory;
}

QList<kitBase::AdditionalPreferences *> PioneerLuaGeneratorPlugin::settingsWidgets()
{
	mOwnsAdditionalPreferences = false;
	return { mAdditionalPreferences };
}

QString PioneerLuaGeneratorPlugin::defaultSettingsFile() const
{
	return ":/pioneer/lua/pioneerLuaDefaultSettings.ini";
}

QString PioneerLuaGeneratorPlugin::defaultFilePath(const QString &projectName) const
{
	return QString("pioneer/%1/%1.lua").arg(projectName);
}

text::LanguageInfo PioneerLuaGeneratorPlugin::language() const
{
	return text::Languages::pickByExtension("lua");
}

QString PioneerLuaGeneratorPlugin::generatorName() const
{
	return "pioneer/lua";
}

generatorBase::MasterGeneratorBase *PioneerLuaGeneratorPlugin::masterGenerator()
{
	return new PioneerLuaMasterGenerator(*mRepo
			, *mMainWindowInterface->errorReporter()
			, *mParserErrorReporter
			, *mRobotModelManager
			, *mTextLanguage
			, mMainWindowInterface->activeDiagram()
			, generatorName());
}

void PioneerLuaGeneratorPlugin::regenerateExtraFiles(const QFileInfo &newFileInfo)
{
	Q_UNUSED(newFileInfo)
}

void PioneerLuaGeneratorPlugin::uploadProgram()
{
	const QFileInfo program = generateCodeForProcessing();
	setUploadAndRunActionsEnabled(false);
	mControllerCommunicator->uploadProgram(program);
}

void PioneerLuaGeneratorPlugin::runProgram()
{
	const QFileInfo program = generateCodeForProcessing();
	setUploadAndRunActionsEnabled(false);
	mControllerCommunicator->runProgram(program);
}

void PioneerLuaGeneratorPlugin::setUploadAndRunActionsEnabled(bool isEnabled)
{
	mUploadProgramAction->setEnabled(isEnabled);
	mRunProgramAction->setEnabled(isEnabled);
}
