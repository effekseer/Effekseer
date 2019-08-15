#define _CRT_SECURE_NO_WARNINGS 1

/*
  Native File Dialog

  http://www.frogtoss.com/labs
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "nfd_common.h"

static char g_errorstr[NFD_MAX_STRLEN] = {0};

/* public routines */

const char *NFD_GetError( void )
{
    return g_errorstr;
}

size_t NFD_PathSet_GetCount( const nfdpathset_t *pathset )
{
    assert(pathset);
    return pathset->count;
}

nfdchar_t *NFD_PathSet_GetPath( const nfdpathset_t *pathset, size_t num )
{
    assert(pathset);
    assert(num < pathset->count);
    
    return pathset->buf + pathset->indices[num];
}

void NFD_PathSet_Free( nfdpathset_t *pathset )
{
    assert(pathset);
    NFDi_Free( pathset->indices );
    NFDi_Free( pathset->buf );
}

/* internal routines */

void *NFDi_Malloc( size_t bytes )
{
    void *ptr = malloc(bytes);
    if ( !ptr )
        NFDi_SetError("NFDi_Malloc failed.");

    return ptr;
}

void NFDi_Free( void *ptr )
{
    assert(ptr);
    free(ptr);
}

void NFDi_SetError( const char *msg )
{
    int bTruncate = NFDi_SafeStrncpy( g_errorstr, msg, NFD_MAX_STRLEN );
    assert( !bTruncate );  _NFD_UNUSED(bTruncate);
}


int NFDi_SafeStrncpy( char *dst, const char *src, size_t maxCopy )
{
    size_t n = maxCopy;
    char *d = dst;

    assert( src );
    assert( dst );
    
    while ( n > 0 && *src != '\0' )    
    {
        *d++ = *src++;
        --n;
    }

    /* Truncation case -
       terminate string and return true */
    if ( n == 0 )
    {
        dst[maxCopy-1] = '\0';
        return 1;
    }

    /* No truncation.  Append a single NULL and return. */
    *d = '\0';
    return 0;
}


/* adapted from microutf8 */
size_t NFDi_UTF8_Strlen( const nfdchar_t *str )
{
	/* This function doesn't properly check validity of UTF-8 character 
	sequence, it is supposed to use only with valid UTF-8 strings. */
    
	size_t character_count = 0;
	size_t i = 0; /* Counter used to iterate over string. */
	nfdchar_t maybe_bom[4];
	
	/* If there is UTF-8 BOM ignore it. */
	if (strlen(str) > 2)
	{
		strncpy(maybe_bom, str, 3);
		maybe_bom[3] = 0;
		if (strcmp(maybe_bom, (nfdchar_t*)NFD_UTF8_BOM) == 0)
			i += 3;
	}
	
	while(str[i])
	{
		int c = (unsigned char)str[i];
		int bytes = 0;
		if ((c >= 0x00) && (c <= 0x7f)) {
			bytes = 1;
		} else if ((c >= 0xc2) && (c <= 0xdf)) {
			bytes = 2;
		} else if ((c >= 0xe0) && (c <= 0xef)) {
			bytes = 3;
		} else if ((c >= 0xf0) && (c <= 0xf7)) {
			bytes = 4;
		} else if ((c >= 0xf8) && (c <= 0xfb)) {
			bytes = 5;
		} else if ((c >= 0xfc) && (c <= 0xfd)) {
			bytes = 6;
		}
		++character_count;
		i += bytes;
	}

	return character_count;	
}

int NFDi_IsFilterSegmentChar( char ch )
{
    return (ch==','||ch==';'||ch=='\0');
}

