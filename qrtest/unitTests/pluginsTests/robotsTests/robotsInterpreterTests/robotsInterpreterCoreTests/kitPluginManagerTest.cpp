#include "kitPluginManagerTest.h"

#include <src/kitPluginManager.h>

using namespace qrTest::robotsInterpreterCoreTests;
using namespace robotsInterpreterCore;

TEST_F(KitPluginManagerTest, pluginsLoadingTest)
{
	KitPluginManager kitPluginManager("plugins/unittests");

	ASSERT_EQ(2, kitPluginManager.kitIds().size());
	ASSERT_EQ("MockKitPlugin1", kitPluginManager.kitIds()[0]);
	ASSERT_EQ("MockKitPlugin2", kitPluginManager.kitIds()[1]);
}
