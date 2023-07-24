#include "../TestHelper.h"
#include <Effekseer.h>
#include <iostream>

#define TEST_EQUAL_STR(x, y)                                    \
	if ((x) != (y))                                             \
	{                                                           \
		std::cout << "Test failed : " << __LINE__ << std::endl; \
		abort();                                                \
	}

void FixedSizeStringTest()
{
	TEST_EQUAL_STR((std::u16string(u"1234")), (Effekseer::FixedSizeString<char16_t, 4>(u"12345").data()));

	TEST_EQUAL_STR((std::u16string(u"12345").substr(0)), (Effekseer::FixedSizeString<char16_t, 5>(u"12345").substr(0).data()));
	TEST_EQUAL_STR((std::u16string(u"12345").substr(0, 2)), (Effekseer::FixedSizeString<char16_t, 5>(u"12345").substr(0, 2).data()));
	TEST_EQUAL_STR((std::u16string(u"12345").substr(1)), (Effekseer::FixedSizeString<char16_t, 5>(u"12345").substr(1).data()));
	TEST_EQUAL_STR((std::u16string(u"12345").substr(1, 2)), (Effekseer::FixedSizeString<char16_t, 5>(u"12345").substr(1, 2).data()));

	TEST_EQUAL_STR((std::u16string(u"1234").back()), (Effekseer::FixedSizeString<char16_t, 4>(u"1234").back()));

	{
		auto v1 = std::u16string(u"1234");
		auto v2 = Effekseer::FixedSizeString<char16_t, 8>(u"1234");
		v1 += '5';
		v2 += '5';
		TEST_EQUAL_STR(v1, v2.data());
	}

	{
		auto v1 = std::u16string(u"1234");
		auto v2 = Effekseer::FixedSizeString<char16_t, 4>(u"1234");
		v2 += '5';
		TEST_EQUAL_STR(v1, v2.data());
	}

	{
		auto v1 = std::u16string(u"1234");
		auto v2 = Effekseer::FixedSizeString<char16_t, 8>(u"1234");
		v1 += u"5678";
		v2 += u"5678";
		TEST_EQUAL_STR(v1, v2.data());
	}

	{
		auto v1 = std::u16string(u"1234");
		auto v2 = Effekseer::FixedSizeString<char16_t, 6>(u"1234");
		v1 += u"56";
		v2 += u"5678";
		TEST_EQUAL_STR(v1, v2.data());
	}
}

void StringAndPathHelperTest()
{
	if (Effekseer::StringHelper::To<char16_t>("hoge") != std::u16string(u"hoge"))
		throw "";

	if (Effekseer::StringHelper::To<char32_t>("hoge") != std::u32string(U"hoge"))
		throw "";

	if (Effekseer::PathHelper::Normalize(std::u16string(u"/a/../b/c")) != std::u16string(u"/b/c"))
		throw "";

	if (Effekseer::PathHelper::Normalize(std::u16string(u"a/../b/c")) != std::u16string(u"b/c"))
		throw "";

	if (Effekseer::PathHelper::Normalize(std::u16string(u"../b/c")) != std::u16string(u"../b/c"))
		throw "";

	if (Effekseer::PathHelper::Absolute(std::u16string(u"d/c"), std::u16string(u"/a/b/c")) != std::u16string(u"/a/b/d/c"))
		throw "";

	if (Effekseer::PathHelper::Absolute(std::u16string(u"../d/c"), std::u16string(u"/a/b/c")) != std::u16string(u"/a/d/c"))
		throw "";

	if (Effekseer::PathHelper::Relative(std::u16string(u"/a/b/e"), std::u16string(u"/a/b/c")) != std::u16string(u"e"))
		throw "";

	if (Effekseer::PathHelper::GetDirectoryName(std::u16string(u"/a/b/e")) != std::u16string(u"/a/b/"))
		throw "";

	if (Effekseer::PathHelper::GetDirectoryName(Effekseer::FixedSizeString<char16_t, 256>(u"/a/b/e")) != Effekseer::FixedSizeString<char16_t, 256>(u"/a/b/"))
		throw "";

	if (Effekseer::PathHelper::GetDirectoryName(std::u16string(u"/a/b/e/")) != std::u16string(u"/a/b/e/"))
		throw "";

	if (Effekseer::PathHelper::GetDirectoryName(std::u16string(u"/a")) != std::u16string(u"/"))
		throw "";

	if (Effekseer::PathHelper::GetDirectoryName(std::u16string(u"")) != std::u16string(u""))
		throw "";

	if (Effekseer::PathHelper::GetFilenameWithoutExt(std::u16string(u"")) != std::u16string(u""))
		throw "";

	if (Effekseer::PathHelper::GetFilenameWithoutExt(std::u16string(u"a")) != std::u16string(u"a"))
		throw "";

	if (Effekseer::PathHelper::GetFilenameWithoutExt(std::u16string(u"/a")) != std::u16string(u"a"))
		throw "";

	if (Effekseer::PathHelper::GetFilenameWithoutExt(std::u16string(u"/b/a")) != std::u16string(u"a"))
		throw "";

	if (Effekseer::PathHelper::GetFilenameWithoutExt(std::u16string(u"a.a.a")) != std::u16string(u"a.a"))
		throw "";

	if (Effekseer::PathHelper::GetFilenameWithoutExt(std::u16string(u"/b/a.a.a")) != std::u16string(u"a.a"))
		throw "";

	TEST_EQUAL_STR((std::u16string(u"a.a")), (Effekseer::PathHelper::GetFilenameWithoutExt(Effekseer::FixedSizeString<char16_t, 128>(u"/b/a.a.a")).data()));

	TEST_EQUAL_STR((std::u16string(u"b/a")), (Effekseer::PathHelper::Combine(std::u16string(u"b"), std::u16string(u"a"))));

	TEST_EQUAL_STR((std::u16string(u"b/a")), (Effekseer::PathHelper::Combine(std::u16string(u"b/"), std::u16string(u"a"))));

	TEST_EQUAL_STR((std::u16string(u"/b/a")), (Effekseer::PathHelper::Combine(std::u16string(u"/b"), std::u16string(u"a"))));

	TEST_EQUAL_STR((std::u16string(u"/b/a/")), (Effekseer::PathHelper::Combine(std::u16string(u"/b/"), std::u16string(u"a/"))));

	TEST_EQUAL_STR((std::u16string(u"/b/a/")), (Effekseer::PathHelper::Combine(Effekseer::FixedSizeString<char16_t, 128>(u"/b/"), Effekseer::FixedSizeString<char16_t, 128>(u"a/")).data()));
}

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Runtime_FixedSizeStringTest("Runtime.FixedSizeStringTest", []() -> void
										 { FixedSizeStringTest(); });

TestRegister Runtime_StringAndPathHelperTest("Runtime.StringAndPathHelperTest", []() -> void
											 { StringAndPathHelperTest(); });

#endif