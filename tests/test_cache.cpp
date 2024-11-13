#include <gtest/gtest.h>
#include "cache.h"

class CacheTest : public ::testing::Test {
  protected:
	CacheTest() {
		cache = new Cache();
	}

	~CacheTest() {
		delete cache;
	}

	Cache* cache;
};

// TEST_F(CacheTest, DISABLED_CheckEncache) {
// 	Msg msg1 = Msg::createOrderBookMsg("2333");
// 	Msg msg2 = Msg::createOrderBookMsg("6666");

// 	EXPECT_EQ(cache->encache(msg1), true);
// 	EXPECT_EQ(cache->size(), 1);

// 	EXPECT_EQ(cache->encache(msg1), false);
// 	EXPECT_EQ(cache->size(), 1);

// 	EXPECT_EQ(cache->encache(msg2), true);
// 	EXPECT_EQ(cache->size(), 2);
// }