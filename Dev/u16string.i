/* -----------------------------------------------------------------------------
 * std_wstring.i
 *
 * Typemaps for std::wstring and const std::wstring&
 * These are mapped to a C# String and are passed around by value.
 *
 * To use non-const std::wstring references use the following %apply.  Note 
 * that they are passed by value.
 * %apply const std::wstring & {std::wstring &};
 * ----------------------------------------------------------------------------- */

/*
	You may copy, modify, distribute, and make derivative works based on
	this software, in source code or object code form, without
	restriction. If you distribute the software to others, you may do
	so according to the terms of your choice. This software is offered as
	is, without warranty of any kind.
*/

namespace std {

%naturalvar u16string;

class u16string;

// u16string
%typemap(ctype, out="void *") u16string "char16_t *"
%typemap(imtype, inattributes="[global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPWStr)]") u16string "string"
%typemap(cstype) u16string "string"
%typemap(csdirectorin) u16string "$iminput"
%typemap(csdirectorout) u16string "$cscall"

%typemap(in, canthrow=1) u16string 
%{ if (!$input) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null u16string", 0);
    return $null;
   }
   $1.assign($input); %}
%typemap(out) u16string %{ $result = SWIG_csharp_wstring_callback((const wchar_t*)$1.c_str()); %}

%typemap(directorout, canthrow=1) u16string 
%{ if (!$input) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null u16string", 0);
    return $null;
   }
   $result.assign($input); %}

%typemap(directorin) u16string %{ $input = SWIG_csharp_wstring_callback((const wchar_t*)$1.c_str()); %}

%typemap(csin) u16string "$csinput"
%typemap(csout, excode=SWIGEXCODE) u16string {
    string ret = $imcall;$excode
    return ret;
  }

//%typemap(typecheck) u16string = achar *;

%typemap(throws, canthrow=1) u16string
%{ std::string message($1.begin(), $1.end());
   SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, message.c_str());
   return $null; %}

// const u16string &
%typemap(ctype, out="void *") const u16string & "char16_t *"
%typemap(imtype, inattributes="[global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPWStr)]") const u16string & "string"  
%typemap(cstype) const u16string & "string"

%typemap(csdirectorin) const u16string & "$iminput"
%typemap(csdirectorout) const u16string & "$cscall"

%typemap(in, canthrow=1) const u16string &
%{ if (!$input) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null u16string", 0);
    return $null;
   }
   ::std::u16string $1_str($input);
   $1 = &$1_str; %}
%typemap(out) const u16string & %{ $result = SWIG_csharp_wstring_callback((const wchar_t*)$1->c_str()); %}

%typemap(csin) const u16string & "$csinput"
%typemap(csout, excode=SWIGEXCODE) const u16string & {
    string ret = $imcall;$excode
    return ret;
  }

%typemap(directorout, canthrow=1, warning=SWIGWARN_TYPEMAP_THREAD_UNSAFE_MSG) const u16string &
%{ if (!$input) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null u16string", 0);
    return $null;
   }
   /* possible thread/reentrant code problem */
   static std::u16string $1_str;
   $1_str = $input;
   $result = &$1_str; %}

%typemap(directorin) const u16string & %{ $input = SWIG_csharp_wstring_callback((const wchar_t*)$1.c_str()); %}

%typemap(csvarin, excode=SWIGEXCODE2) const u16string & %{
    set {
      $imcall;$excode
    } %}
%typemap(csvarout, excode=SWIGEXCODE2) const u16string & %{
    get {
      string ret = $imcall;$excode
      return ret;
    } %}

//%typemap(typecheck) const u16string & = char16_t *;

%typemap(throws, canthrow=1) const u16string &
%{ std::string message($1.begin(), $1.end());
   SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, message.c_str());
   return $null; %}

}

