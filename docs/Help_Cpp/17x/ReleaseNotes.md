## 1.7

### Broken changes

#### Effekseer::Server, Effekseer::Client

Classes inherited ReferenceObject.

#### Effekseer::FileInterface, Effekseer::FileReader and Effekseer::FileWriter were changed.

Classes inherit ReferenceObject.
These class should be creted with MakeRefPtr and deleteed automatically.

#### Culling system was replaced.

You need to specify Effekseer::Manager::DrawParameter's ViewProjectionMatrix, ZNear and ZFar when Effekseer::Manager::Draw is called.

#### Effekseer::Backend::TextureParameter was changed.

It was combined to RenderTextureParameter and DepthTextureParameter.
It supported Texture3D and Texture2DArray.

#### Allocators were changed

These types were changed.
Getter and Setter in Manager were removed.

#### Callbacks were changed

These types were changed.

#### RandFunc were changed

A type was changed.

## 1.6

### Broken changes

Some classes are move from "Effekseer.h" to "Effekseer.Modules.h"

#### Effekseer::RefPtr

Almost pointers are replaced into SmartPointer. You don't need to consider about releasing memories.

Please replace

Effekseer::Effect* -> Effekseer::EffekseerRef

You have not to release Effekseer::EffectRef yourself.
If you want to release it, Please call Reset.

#### Effekseer::Effect

Effect::Reload without manager were abolished. Please use Effect::Reload with manager

#### Effekseer::Backend::GraphicsDevice

EffekseerRenderer::GraphicsDevice and DeviceObjectCollection were abolished and replaced.

We implemented Effekseer::Backend::GraphicsDevice, which contains platform devices.
Some methods require an instance of GraphicsDevice.

For example,

CreateTextureLoader

The instance are create with follow functions

EffekseerRendererDX11::CreateGraphicsDevice
EffekseerRendererGL::CreateGraphicsDevice
etc...

#### Effekseer::TextureLoader

Added an argument about mipmap

#### EffekseerRenderer::DistortionCallback

Added renderer as an argument

#### Effekseer::Manager::DrawPrameter::CameraDirection

It was change into Effekseer::Manager::DrawPrameter::CameraFrontDirection and flipped a direction. It means that the direction is normalize(focus - position)

## 1.52k

- Changed the timing of sending the model's buffer to the GPU before rendering, because in some environments the buffer generation process was not thread-safe. 

## 1.52i

- Fixed many bugs
- Added Metal sample

## 1.52h

- (Broken changes) Depth format now can be specified.
- Fixed many bugs
