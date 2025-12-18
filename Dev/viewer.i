%module(directors="1") EffekseerNative

%{
#include "Parameters.h"
#include "ProcessConnector.h"
#include "Recorder/EffectRecorder.h"
#include "3D/ViewPointController.h"
#include "3D/Effect.h"
#include "3D/EffectSetting.h"
#include "3D/EffectFactory.h"
#include "3D/MainScreenEffectRenderer.h"
#include "Network/Network.h"
#include "Sound/SoundDevice.h"
#include "GUI/efk.GUIManager.h"
#include "GUI/efk.FileDialog.h"
#include "GUI/Image.h"
#include "GUI/ReloadableImage.h"
#include "GUI/RenderImage.h"
#include "GUI/GradientHDRState.h"
#include "Utils/Logger.h"
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
%include <std_vector.i>

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

%template(ProfileSampleManagerVector) std::vector<Effekseer::Tool::ProfileSample::Manager>;
%template(ProfileSampleEffectVector) std::vector<Effekseer::Tool::ProfileSample::Effect>;
%template(ViewerExternalModelVector) std::vector<Effekseer::Tool::ViewerExternalModel>;

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

// Plot
%apply double INPUT[] { const double* xValues }
%apply double INPUT[] { const double* yValues }

%feature("director") GUIManagerCallback;
%feature("director") IOCallback;
%feature("director") EffectRendererCallback;

%shared_ptr(Effekseer::MainWindow);
%shared_ptr(Effekseer::StaticFile);
%shared_ptr(Effekseer::IO);
%shared_ptr(Effekseer::IOCallback);
%shared_ptr(Effekseer::Tool::EffectRecorder);
%shared_ptr(Effekseer::Tool::Image);
%shared_ptr(Effekseer::Tool::RenderImage);
%shared_ptr(Effekseer::Tool::ReloadableImage);
%shared_ptr(Effekseer::Tool::ViewPointController);
%shared_ptr(Effekseer::Tool::Effect);
%shared_ptr(Effekseer::Tool::EffectFactory);
%shared_ptr(Effekseer::Tool::EffectSetting);
%shared_ptr(Effekseer::Tool::GraphicsDevice);
%shared_ptr(Effekseer::Tool::SoundDevice);

// Timeline
%apply int *INOUT { int* frameMin }
%apply int *INOUT { int* frameMax }
%apply int *INOUT { int* currentFrame }
%apply int *INOUT { int* selectedEntry }
%apply int *INOUT { int* firstFrame }

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
%include "Cpp/Viewer/Math/Matrix44F.h"
%include "Cpp/Viewer/Math/Vector2I.h"
%include "Cpp/Viewer/Math/Vector3F.h"

%include "Cpp/Viewer/Parameters.h"
%include "Cpp/Viewer/ProcessConnector.h"
%include "Cpp/Viewer/GUI/efk.Vec2.h"
%include "Cpp/Viewer/GUI/efk.Vec4.h"
%include "Cpp/Viewer/Recorder/RecordingParameter.h"
%include "Cpp/Viewer/Recorder/EffectRecorder.h"

%include "Cpp/Viewer/Graphics/Color.h"
%include "Cpp/Viewer/Graphics/GraphicsDevice.h"
%include "Cpp/Viewer/Sound/SoundDevice.h"

%include "Cpp/Viewer/3D/ViewPointController.h"
%include "Cpp/Viewer/3D/Effect.h"
%include "Cpp/Viewer/3D/EffectSetting.h"
%include "Cpp/Viewer/3D/EffectFactory.h"
%include "Cpp/Viewer/3D/EffectRenderer.h"
%include "Cpp/Viewer/3D/MainScreenEffectRenderer.h"

%include "Cpp/Viewer/Network/ProfileSample.h"
%include "Cpp/Viewer/Network/Network.h"

%include "Cpp/Viewer/Utils/Logger.h"

%include "Cpp/Viewer/GUI/efk.GUIManager.h"
%include "Cpp/Viewer/GUI/efk.FileDialog.h"
%include "Cpp/Viewer/GUI/Image.h"
%include "Cpp/Viewer/GUI/GradientHDRState.h"
%include "Cpp/Viewer/GUI/ReloadableImage.h"
%include "Cpp/Viewer/GUI/RenderImage.h"
%include "Cpp/Viewer/CompiledMaterialGenerator.h"

%include "Cpp/EditorCommon/Platform/PlatformMisc.h"
%include "Cpp/EditorCommon/GUI/MainWindow.h"

%include "Cpp/EditorCommon/IO/FileReader.h"
%include "Cpp/EditorCommon/IO/StaticFile.h"
%include "Cpp/EditorCommon/IO/IO.h"
