#include "efk.Language.h"

#import <Foundation/NSLocale.h>

EfkLanguage GetEfkLanguage()
{
	NSString *localeIdentifier = [[NSLocale currentLocale] localeIdentifier];
	if([localeIdentifier isEqual: @"ja_JP"])
	{
		return EfkLanguage::Japanese;
	}
	else
	{
		return EfkLanguage::Enligsh;
	}
}