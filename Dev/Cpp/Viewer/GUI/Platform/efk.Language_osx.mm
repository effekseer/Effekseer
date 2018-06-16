#include "efk.Language.h"

#import <Foundation/NSLocale.h>
#import <Cocoa/Cocoa.h>

EfkLanguage GetEfkLanguage()
{
	NSString *localeIdentifier = [[NSLocale currentLocale] localeIdentifier];
    NSArray *languages = [NSLocale preferredLanguages];
    NSString *languageID = [languages objectAtIndex:0];
    
    //printf("%s", [languageID cStringUsingEncoding:NSUTF8StringEncoding]);
    
	if ([languageID hasPrefix:@"ja-"])
	{
		return EfkLanguage::Japanese;
	}
	else
	{
        return EfkLanguage::English;
	}
}
