//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.1
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace Effekseer.swig {

public class MainWindowState : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal MainWindowState(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(MainWindowState obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~MainWindowState() {
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
          EffekseerNativePINVOKE.delete_MainWindowState(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public int PosX {
    set {
      EffekseerNativePINVOKE.MainWindowState_PosX_set(swigCPtr, value);
    } 
    get {
      int ret = EffekseerNativePINVOKE.MainWindowState_PosX_get(swigCPtr);
      return ret;
    } 
  }

  public int PosY {
    set {
      EffekseerNativePINVOKE.MainWindowState_PosY_set(swigCPtr, value);
    } 
    get {
      int ret = EffekseerNativePINVOKE.MainWindowState_PosY_get(swigCPtr);
      return ret;
    } 
  }

  public int Width {
    set {
      EffekseerNativePINVOKE.MainWindowState_Width_set(swigCPtr, value);
    } 
    get {
      int ret = EffekseerNativePINVOKE.MainWindowState_Width_get(swigCPtr);
      return ret;
    } 
  }

  public int Height {
    set {
      EffekseerNativePINVOKE.MainWindowState_Height_set(swigCPtr, value);
    } 
    get {
      int ret = EffekseerNativePINVOKE.MainWindowState_Height_get(swigCPtr);
      return ret;
    } 
  }

  public bool IsMaximumMode {
    set {
      EffekseerNativePINVOKE.MainWindowState_IsMaximumMode_set(swigCPtr, value);
    } 
    get {
      bool ret = EffekseerNativePINVOKE.MainWindowState_IsMaximumMode_get(swigCPtr);
      return ret;
    } 
  }

  public MainWindowState() : this(EffekseerNativePINVOKE.new_MainWindowState(), true) {
  }

}

}
