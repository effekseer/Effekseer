%module(directors="1") EffekseerNative

%{
#include "GUI/efk.ImageResource.h"
#include "dll.h"
#include "GUI/efk.GUIManager.h"
#include "GUI/efk.FileDialog.h"
%}

//-----------------------------------------------------------------------------------
// csharp
//-----------------------------------------------------------------------------------
%include "wchar.i"
%include "stdint.i"
%include "char16.i"
%include "arrays_csharp.i"

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

%apply int INOUT[] { int* v }
%apply bool INOUT[] { bool* v }
%apply float INOUT[] { float* v }

%apply float INOUT[] { float* col }

%apply bool INOUT[] { bool* p_open }
%apply bool INOUT[] { bool* p_selected }

%apply int INOUT[] { int* v_current_min }
%apply int INOUT[] { int* v_current_max }

%apply float INOUT[] { float* v_current_min }
%apply float INOUT[] { float* v_current_max }

%feature("director") GUIManagerCallback;

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
%include "Cpp/Viewer/GUI/efk.Vec2.h"
%include "Cpp/Viewer/GUI/efk.ImageResource.h"
%include "Cpp/Viewer/dll.h"
%include "Cpp/Viewer/GUI/efk.GUIManager.h"
%include "Cpp/Viewer/GUI/efk.FileDialog.h"

