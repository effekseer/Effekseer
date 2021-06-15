%module(directors="1") EffekseerNative

%{
#include "efk.Base.h"
#include "GUI/efk.ImageResource.h"
#include "dll.h"
#include "GUI/efk.GUIManager.h"
#include "GUI/efk.FileDialog.h"
#include "CompiledMaterialGenerator.h"
#include "../EditorCommon/Platform/PlatformMisc.h"
#include "../EditorCommon/GUI/MainWindow.h"
#include "../EditorCommon/IO/FileReader.h"
#include "../EditorCommon/IO/StaticFile.h"
#include "../EditorCommon/IO/IO.h"

%}

//-----------------------------------------------------------------------------------
// csharp
//-----------------------------------------------------------------------------------
%include "wchar.i"
%include "stdint.i"
%include "char16.i"
%include "u16string.i"
%include "typemaps.i"
%include "arrays_csharp.i"
%include <std_shared_ptr.i>

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

%apply bool *INOUT { bool* p_open }
%apply bool *INOUT { bool* p_selected }
%apply bool *INOUT { bool* p_checked }

%apply int INOUT[] { int* v_current_min }
%apply int INOUT[] { int* v_current_max }

%apply float INOUT[] { float* v_current_min }
%apply float INOUT[] { float* v_current_max }


// FCurve
%apply float INOUT[] { float* keys }
%apply float INOUT[] { float* values }
%apply float INOUT[] { float* leftHandleKeys }
%apply float INOUT[] { float* leftHandleValues }
%apply float INOUT[] { float* rightHandleKeys }
%apply float INOUT[] { float* rightHandleValues }
%apply bool INOUT[] { bool* kv_selected }
%apply int *INOUT { int* newCount }
%apply bool *INOUT { bool* newSelected }
%apply float *INOUT { float* movedX }
%apply float *INOUT { float* movedY }
%apply int *INOUT { int* changedType }
%apply int INOUT[] { int* interporations }
%apply int *INOUT { int* hovered }

%apply uint8_t INOUT[] { uint8_t* kv_selected }

%apply uint32_t *INOUT { uint32_t* outId1 }
%apply uint32_t *INOUT { uint32_t* outId2 }
%apply int *INOUT { int* size }
%apply uint8_t INOUT[] { uint8_t* data }
%apply uint8_t INOUT[] { uint8_t* data_output }

%feature("director") GUIManagerCallback;
%feature("director") IOCallback;

%shared_ptr(Effekseer::MainWindow);
%shared_ptr(Effekseer::StaticFile);
%shared_ptr(Effekseer::IO);
%shared_ptr(Effekseer::IOCallback);

// Timeline
%apply int *INOUT { int* frameMin }
%apply int *INOUT { int* frameMax }
%apply int *INOUT { int* currentFrame }
%apply int *INOUT { int* selectedEntry }
%apply int *INOUT { int* firstFrame }

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
%include "Cpp/Viewer/efk.Base.h"
%include "Cpp/Viewer/GUI/efk.Vec2.h"
%include "Cpp/Viewer/GUI/efk.ImageResource.h"
%include "Cpp/Viewer/ViewerEffectBehavior.h"
%include "Cpp/Viewer/dll.h"
%include "Cpp/Viewer/GUI/efk.GUIManager.h"
%include "Cpp/Viewer/GUI/efk.FileDialog.h"
%include "Cpp/Viewer/CompiledMaterialGenerator.h"

%include "Cpp/EditorCommon/Platform/PlatformMisc.h"
%include "Cpp/EditorCommon/GUI/MainWindow.h"

%include "Cpp/EditorCommon/IO/FileReader.h"
%include "Cpp/EditorCommon/IO/StaticFile.h"
%include "Cpp/EditorCommon/IO/IO.h"
