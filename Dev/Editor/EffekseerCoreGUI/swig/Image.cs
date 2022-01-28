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

public class Image : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  private bool swigCMemOwnBase;

  internal Image(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwnBase = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Image obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Image() {
    Dispose(false);
  }

  public void Dispose() {
    Dispose(true);
    global::System.GC.SuppressFinalize(this);
  }

  protected virtual void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwnBase) {
          swigCMemOwnBase = false;
          EffekseerNativePINVOKE.delete_Image(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public int GetWidth() {
    int ret = EffekseerNativePINVOKE.Image_GetWidth(swigCPtr);
    if (EffekseerNativePINVOKE.SWIGPendingException.Pending) throw EffekseerNativePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int GetHeight() {
    int ret = EffekseerNativePINVOKE.Image_GetHeight(swigCPtr);
    if (EffekseerNativePINVOKE.SWIGPendingException.Pending) throw EffekseerNativePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Image() : this(EffekseerNativePINVOKE.new_Image(), true) {
  }

}

}
