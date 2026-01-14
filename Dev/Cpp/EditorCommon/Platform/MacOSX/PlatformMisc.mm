#include "../PlatformMisc.h"

#import <Cocoa/Cocoa.h>
#import <Foundation/NSLocale.h>

namespace Effekseer
{

SystemLanguage GetSystemLanguage()
{
	NSString* localeIdentifier = [[NSLocale currentLocale] localeIdentifier];
	NSArray* languages = [NSLocale preferredLanguages];
	NSString* languageID = [languages objectAtIndex:0];

	// printf("%s", [languageID cStringUsingEncoding:NSUTF8StringEncoding]);

	if ([languageID hasPrefix:@"ja-"])
	{
		return SystemLanguage::Japanese;
	}
	else if ([languageID hasPrefix:@"zh-"])
	{
		return SystemLanguage::SimplifiedChinese;
	}
	else
	{
		return SystemLanguage::English;
	}
}

} // namespace Effekseer
