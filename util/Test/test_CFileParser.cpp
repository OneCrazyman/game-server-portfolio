#include "gtest/gtest.h"
#include "Util/CFileParser.h"

TEST(CFileParser, ParserTest) {
	//[SectionOne] {
	//		"Int1": 1,
	//		"Int2" : 100,
	//		"String1" : "Hello1",
	//		"String2" : "Hello2",
	//}
	CFileParser parser;
	auto parse = parser.Parse("config.ini");
	EXPECT_TRUE(parse);
	if (!parse) {
		return;
	}
	EXPECT_TRUE(parser.SetSection("SectionOne"));
	int val1 = 0;
	EXPECT_TRUE(parser.GetIntValue("Int1",&val1));
	EXPECT_EQ(val1, 1);
	int val2 = 0;
	EXPECT_TRUE(parser.GetIntValue("Int2",&val2));
	EXPECT_EQ(val2, 100);
	char str1[256] = {};
	char str2[256] = {};
	EXPECT_TRUE(parser.GetStringValue("String1", str1));
	EXPECT_EQ(strcmp(str1, "Hello1"), 0);
	EXPECT_TRUE(parser.GetStringValue("String2", str2));
	EXPECT_EQ(strcmp(str2, "Hello2"), 0);

	EXPECT_FALSE(parser.GetIntValue("no exist word",&val1));

	//[SectionTwo] {
	//		"Int1": 2,
	//		"Int2" : 200,
	//		"String1" : "World1",
	//		"String2" : "World2",
	//}
	//EXPECT_TRUE(parser.Parse("test.ini"));
	EXPECT_TRUE(parser.SetSection("SectionTwo"));
	EXPECT_TRUE(parser.GetIntValue("Int1", &val1));
	EXPECT_EQ(val1, 2);
	EXPECT_TRUE(parser.GetIntValue("Int2", &val2));
	EXPECT_EQ(val2, 200);
	EXPECT_TRUE(parser.GetStringValue("String1", str1));
	EXPECT_EQ(strcmp(str1, "World1"), 0);
	EXPECT_TRUE(parser.GetStringValue("String2", str2));
	EXPECT_EQ(strcmp(str2,  "World2"), 0);
}