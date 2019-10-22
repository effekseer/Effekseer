#include "../PlatformMisc.h"

#import <Foundation/NSLocale.h>
#import <Cocoa/Cocoa.h>

namespace Effekseer
{

SystemLanguage GetSystemLanguage()
{
    NSString *localeIdentifier = [[NSLocale currentLocale] localeIdentifier];
    NSArray *languages = [NSLocale preferredLanguages];
    NSString *languageID = [languages objectAtIndex:0];
    
    //printf("%s", [languageID cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if ([languageID hasPrefix:@"ja-"])
    {
        return SystemLanguage::Japanese;
    }
    else
    {
        return SystemLanguage::English;
    }
}

}
