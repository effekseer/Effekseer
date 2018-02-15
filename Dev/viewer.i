%module EffekseerNative

%{
#include "dll.h"
%}

//-----------------------------------------------------------------------------------
// csharp
//-----------------------------------------------------------------------------------
%include "wchar.i"
%include "stdint.i"
%include "char16.i"

%pragma(csharp) imclassclassmodifiers="
[System.Security.SuppressUnmanagedCodeSecurity]
class"

// csharp define void
%typemap(ctype) void * "void *"
%typemap(imtype) void * "System.IntPtr"
%typemap(cstype) void * "System.IntPtr"
%typemap(in) void * { $1 = $input; }
%typemap(out) void * { $result = $1; }
%typemap(csin) void * "$csinput"
%typemap(csout) void * { return $imcall; }
%typemap(csvarin) void * { set { $imcall; } }
%typemap(csvarout) void * { get { return $imcall; } } 

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
%typemap(csclassmodifiers) ViewerParamater "public class"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
%include "Cpp/Viewer/dll.h"
