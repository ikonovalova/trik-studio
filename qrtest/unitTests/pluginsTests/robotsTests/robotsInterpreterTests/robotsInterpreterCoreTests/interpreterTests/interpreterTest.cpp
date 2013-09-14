#include "interpreterTest.h"

#include <src/interpreter/interpreter.h>

using namespace qrTest::robotsInterpreterCoreTests;

using namespace robotsInterpreterCore::interpreter;

void InterpreterTest::SetUp()
{
	mBlocksFactory = new DummyBlockFactory();

	mQrguiFacade = new QrguiFacade("unittests/testModel.qrs");

	mInterpreter = new Interpreter(
			mQrguiFacade->graphicalModelAssistInterface()
			, mQrguiFacade->logicalModelAssistInterface()
			, mQrguiFacade->mainWindowInterpretersInterface()
			, mQrguiFacade->projectManagementInterface()
			, mBlocksFactory
			, &mModel
			);
}

void InterpreterTest::TearDown()
{
	delete mInterpreter;
	delete mQrguiFacade;
}

TEST_F(InterpreterTest, sanityCheck)
{
}

TEST_F(InterpreterTest, interpret)
{
	mInterpreter->interpret();
}
