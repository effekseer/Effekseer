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

public class EffectFactory : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  private bool swigCMemOwnBase;

  internal EffectFactory(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwnBase = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(EffectFactory obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~EffectFactory() {
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
          EffekseerNativePINVOKE.delete_EffectFactory(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public EffectFactory(EffectSetting setting) : this(EffekseerNativePINVOKE.new_EffectFactory(EffectSetting.getCPtr(setting)), true) {
  }

  public Effect LoadEffect(System.IntPtr data, int size, string path) {
    global::System.IntPtr cPtr = EffekseerNativePINVOKE.EffectFactory_LoadEffect(swigCPtr, data, size, path);
    Effect ret = (cPtr == global::System.IntPtr.Zero) ? null : new Effect(cPtr, true);
    if (EffekseerNativePINVOKE.SWIGPendingException.Pending) throw EffekseerNativePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void ReloadAllResources() {
    EffekseerNativePINVOKE.EffectFactory_ReloadAllResources(swigCPtr);
    if (EffekseerNativePINVOKE.SWIGPendingException.Pending) throw EffekseerNativePINVOKE.SWIGPendingException.Retrieve();
  }

}

}
