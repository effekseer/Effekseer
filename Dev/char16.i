/* -----------------------------------------------------------------------------
 * achar.i
 *
 * Typemaps for the char16_t type
 * These are mapped to a C# String and are passed around by value.
 *
 * Support code for wide strings can be turned off by defining SWIG_CSHARP_NO_WSTRING_HELPER
 *
 * ----------------------------------------------------------------------------- */

 /*
	You may copy, modify, distribute, and make derivative works based on
	this software, in source code or object code form, without
	restriction. If you distribute the software to others, you may do
	so according to the terms of your choice. This software is offered as
	is, without warranty of any kind.
 */

#if !defined(SWIG_CSHARP_NO_WSTRING_HELPER)
#if !defined(SWIG_CSHARP_WSTRING_HELPER_)
#define SWIG_CSHARP_WSTRING_HELPER_
%insert(runtime) %{
/* Callback for returning strings to C# without leaking memory */

#include <stdint.h>
#include <string>

typedef void * (SWIGSTDCALL* SWIG_CSharpWStringHelperCallback)(const char16_t *);
static SWIG_CSharpWStringHelperCallback SWIG_csharp_wstring_callback = NULL;
%}

%pragma(csharp) imclasscode=%{
  protected class SWIGWStringHelper {

    public delegate string SWIGWStringDelegate(System.IntPtr message);
    static SWIGWStringDelegate wstringDelegate = new SWIGWStringDelegate(CreateWString);

    [global::System.Runtime.InteropServices.DllImport("$dllimport", EntryPoint="SWIGRegisterWStringCallback_$module")]
    public static extern void SWIGRegisterWStringCallback_$module(SWIGWStringDelegate wstringDelegate);

    static string CreateWString([global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPWStr)]global::System.IntPtr cString) {
      return System.Runtime.InteropServices.Marshal.PtrToStringUni(cString);
    }

    static SWIGWStringHelper() {
      SWIGRegisterWStringCallback_$module(wstringDelegate);
    }
  }

  static protected SWIGWStringHelper swigWStringHelper = new SWIGWStringHelper();
%}

%insert(runtime) %{
#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterWStringCallback_$module(SWIG_CSharpWStringHelperCallback callback) {
  SWIG_csharp_wstring_callback = callback;
}
%}
#endif // SWIG_CSHARP_WSTRING_HELPER_
#endif // SWIG_CSHARP_NO_WSTRING_HELPER


// char16_t
%typemap(ctype) char16_t "char16_t"
%typemap(imtype) char16_t "char"
%typemap(cstype) char16_t "char"

%typemap(csin) char16_t "$csinput"
%typemap(csout, excode=SWIGEXCODE) char16_t {
    char ret = $imcall;$excode
    return ret;
  }
%typemap(csvarin, excode=SWIGEXCODE2) char16_t %{
    set {
      $imcall;$excode
    } %}
%typemap(csvarout, excode=SWIGEXCODE2) char16_t %{
    get {
      char ret = $imcall;$excode
      return ret;
    } %}

%typemap(in) char16_t %{ $1 = ($1_ltype)$input; %}
%typemap(out) char16_t %{ $result = (char16_t)$1; %}

//%typemap(typecheck) char16_t = char;

// char16_t *
%typemap(ctype) char16_t * "char16_t *"
%typemap(imtype, inattributes="[global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPWStr)]", out="System.IntPtr" ) char16_t * "string"
%typemap(cstype) char16_t * "string"

%typemap(csin) char16_t * "$csinput"
%typemap(csout, excode=SWIGEXCODE) char16_t * {
    string ret = System.Runtime.InteropServices.Marshal.PtrToStringUni($imcall);$excode
    return ret;
  }
%typemap(csvarin, excode=SWIGEXCODE2) char16_t * %{
    set {
      $imcall;$excode
    } %}
%typemap(csvarout, excode=SWIGEXCODE2) char16_t * %{
    get {
      string ret = $imcall;$excode
      return ret;
    } %}

%typemap(in) char16_t * %{ $1 = ($1_ltype)$input; %}
%typemap(out) char16_t * %{ $result = (char16_t *)$1; %}

//%typemap(typecheck) char16_t * = char *;
