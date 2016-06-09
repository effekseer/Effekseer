include $(CLEAR_VARS)

LOCAL_PATH := $(call my-dir)

LOCAL_ARM_MODE  := arm
LOCAL_PATH      := $(NDK_PROJECT_PATH)
LOCAL_MODULE    := libEffekseerUnity
LOCAL_CFLAGS    := -O2 -D__EFFEKSEER_RENDERER_GLES2__
LOCAL_LDLIBS    := -landroid -lEGL -lGLESv2

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../Effekseer \
	$(LOCAL_PATH)/../../EffekseerRendererGL

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/../common/EffekseerPluginCommon.cpp \
	$(LOCAL_PATH)/../common/EffekseerPluginModel.cpp \
	$(LOCAL_PATH)/../common/EffekseerPluginSound.cpp \
	$(LOCAL_PATH)/../opengl/EffekseerPluginGL.cpp \
	$(LOCAL_PATH)/../opengl/EffekseerPluginLoaderGL.cpp

LIB_SRC_PATH := $(LOCAL_PATH)/../../Effekseer/Effekseer
LOCAL_SRC_FILES += \
	$(LIB_SRC_PATH)/Effekseer.Client.cpp \
	$(LIB_SRC_PATH)/Effekseer.Color.cpp \
	$(LIB_SRC_PATH)/Effekseer.CriticalSection.cpp \
	$(LIB_SRC_PATH)/Effekseer.DefaultEffectLoader.cpp \
	$(LIB_SRC_PATH)/Effekseer.DefaultFile.cpp \
	$(LIB_SRC_PATH)/Effekseer.Effect.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNode.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNodeModel.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNodeRibbon.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNodeRing.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNodeRoot.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNodeSprite.cpp \
	$(LIB_SRC_PATH)/Effekseer.EffectNodeTrack.cpp \
	$(LIB_SRC_PATH)/Effekseer.FCurves.cpp \
	$(LIB_SRC_PATH)/Effekseer.Instance.cpp \
	$(LIB_SRC_PATH)/Effekseer.InstanceContainer.cpp \
	$(LIB_SRC_PATH)/Effekseer.InstanceGlobal.cpp \
	$(LIB_SRC_PATH)/Effekseer.InstanceGroup.cpp \
	$(LIB_SRC_PATH)/Effekseer.Manager.cpp \
	$(LIB_SRC_PATH)/Effekseer.Matrix43.cpp \
	$(LIB_SRC_PATH)/Effekseer.Matrix44.cpp \
	$(LIB_SRC_PATH)/Effekseer.RectF.cpp \
	$(LIB_SRC_PATH)/Effekseer.Server.cpp \
	$(LIB_SRC_PATH)/Effekseer.Setting.cpp \
	$(LIB_SRC_PATH)/Effekseer.Socket.cpp \
	$(LIB_SRC_PATH)/Effekseer.Thread.cpp \
	$(LIB_SRC_PATH)/Effekseer.Vector2D.cpp \
	$(LIB_SRC_PATH)/Effekseer.Vector3D.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.Grid.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.Layer.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.Matrix44.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.ObjectInternal.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.ReferenceObject.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.Vector3DF.cpp \
	$(LIB_SRC_PATH)/Culling/Culling3D.WorldInternal.cpp

LIB_SRC_PATH := $(LOCAL_PATH)/../../EffekseerRendererCommon
LOCAL_SRC_FILES += \
	$(LIB_SRC_PATH)/EffekseerRenderer.IndexBufferBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.ModelRendererBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.Renderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.RenderStateBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.RibbonRendererBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.RingRendererBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.SpriteRendererBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.TrackRendererBase.cpp \
	$(LIB_SRC_PATH)/EffekseerRenderer.VertexBufferBase.cpp

LIB_SRC_PATH := $(LOCAL_PATH)/../../EffekseerRendererGL/EffekseerRenderer
LOCAL_SRC_FILES += \
	$(LIB_SRC_PATH)/EffekseerRendererGL.DeviceObject.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.GLExtension.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.IndexBuffer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.ModelLoader.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.ModelRenderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.Renderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.RenderState.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.RibbonRenderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.RingRenderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.Shader.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.SpriteRenderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.TextureLoader.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.TrackRenderer.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.VertexArray.cpp \
	$(LIB_SRC_PATH)/EffekseerRendererGL.VertexBuffer.cpp

include $(BUILD_SHARED_LIBRARY)