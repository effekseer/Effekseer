//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace Effekseer.swig {

public class GradientHDRState : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal GradientHDRState(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(GradientHDRState obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~GradientHDRState() {
    Dispose(false);
  }

  public void Dispose() {
    Dispose(true);
    global::System.GC.SuppressFinalize(this);
  }

  protected virtual void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          EffekseerNativePINVOKE.delete_GradientHDRState(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public int GetColorCount() {
    int ret = EffekseerNativePINVOKE.GradientHDRState_GetColorCount(swigCPtr);
    return ret;
  }

  public int GetAlphaCount() {
    int ret = EffekseerNativePINVOKE.GradientHDRState_GetAlphaCount(swigCPtr);
    return ret;
  }

  public float GetColorMarkerPosition(int index) {
    float ret = EffekseerNativePINVOKE.GradientHDRState_GetColorMarkerPosition(swigCPtr, index);
    return ret;
  }

  public ColorF GetColorMarkerColor(int index) {
    ColorF ret = new ColorF(EffekseerNativePINVOKE.GradientHDRState_GetColorMarkerColor(swigCPtr, index), true);
    return ret;
  }

  public float GetColorMarkerIntensity(int index) {
    float ret = EffekseerNativePINVOKE.GradientHDRState_GetColorMarkerIntensity(swigCPtr, index);
    return ret;
  }

  public float GetAlphaMarkerPosition(int index) {
    float ret = EffekseerNativePINVOKE.GradientHDRState_GetAlphaMarkerPosition(swigCPtr, index);
    return ret;
  }

  public float GetAlphaMarkerAlpha(int index) {
    float ret = EffekseerNativePINVOKE.GradientHDRState_GetAlphaMarkerAlpha(swigCPtr, index);
    return ret;
  }

  public GradientHDRState() : this(EffekseerNativePINVOKE.new_GradientHDRState(), true) {
  }

}

}
