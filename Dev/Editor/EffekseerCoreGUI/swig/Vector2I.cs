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

public class Vector2I : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Vector2I(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Vector2I obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Vector2I() {
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
          EffekseerNativePINVOKE.delete_Vector2I(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public int X {
    set {
      EffekseerNativePINVOKE.Vector2I_X_set(swigCPtr, value);
    } 
    get {
      int ret = EffekseerNativePINVOKE.Vector2I_X_get(swigCPtr);
      return ret;
    } 
  }

  public int Y {
    set {
      EffekseerNativePINVOKE.Vector2I_Y_set(swigCPtr, value);
    } 
    get {
      int ret = EffekseerNativePINVOKE.Vector2I_Y_get(swigCPtr);
      return ret;
    } 
  }

  public Vector2I() : this(EffekseerNativePINVOKE.new_Vector2I__SWIG_0(), true) {
  }

  public Vector2I(int x, int y) : this(EffekseerNativePINVOKE.new_Vector2I__SWIG_1(x, y), true) {
  }

}

}
