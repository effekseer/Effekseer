import re
import codecs

def isValidLine(line):
	if re.search('include \"', line) == None or line.find('.PSVita') != -1 or line.find('.PS4') != -1 or line.find('.Switch') != -1 or line.find('.XBoxOne') != -1:
		return True
	return False

class CreateHeader:
	def __init__(self):
		self.lines = []

	def addLine(self,line):
		self.lines.append(line + '\n')

	def readLines(self,path):
		f = codecs.open(path, 'r','utf-8_sig')
		line = f.readline()
		while line:
			if isValidLine(line):
 	 			self.lines.append(line)
			line = f.readline()
		f.close()

	def output(self,path):
		f = codecs.open(path, 'w','utf-8_sig')
		for line in self.lines:
			f.write(line)
		f.close()


effekseerHeader = CreateHeader()
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Base.Pre.h')
effekseerHeader.readLines('Effekseer/Effekseer/Utils/Effekseer.CustomAllocator.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Vector2D.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Vector3D.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Color.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.RectF.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Matrix43.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Matrix44.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.File.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.DefaultFile.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Effect.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Manager.h')

effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.EffectLoader.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.TextureLoader.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.ModelLoader.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.MaterialLoader.h')

effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Model.h')

effekseerHeader.readLines('Effekseer/Effekseer/Sound/Effekseer.SoundPlayer.h')

effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.SoundLoader.h')

effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Setting.h')

effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Server.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Client.h')

effekseerHeader.output('Effekseer/Effekseer.h')

effekseerInternalHeader = CreateHeader()
effekseerInternalHeader.addLine('#pragma once')
effekseerInternalHeader.addLine('#include "Effekseer.h"')
effekseerInternalHeader.addLine('#include "Effekseer/SIMD/Effekseer.Vec2f.h"')
effekseerInternalHeader.addLine('#include "Effekseer/SIMD/Effekseer.Vec3f.h"')
effekseerInternalHeader.addLine('#include "Effekseer/SIMD/Effekseer.Vec4f.h"')
effekseerInternalHeader.addLine('#include "Effekseer/SIMD/Effekseer.Mat43f.h"')
effekseerInternalHeader.addLine('#include "Effekseer/SIMD/Effekseer.Mat44f.h"')
effekseerInternalHeader.addLine('#include "Effekseer/SIMD/Effekseer.SIMDUtils.h"')
effekseerInternalHeader.addLine('')
effekseerInternalHeader.addLine('// A header to access internal data of effekseer')
effekseerInternalHeader.addLine('')

effekseerInternalHeader.readLines('Effekseer/Effekseer/Parameter/Effekseer.Parameters.h')
effekseerInternalHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.SpriteRenderer.h')
effekseerInternalHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.RibbonRenderer.h')
effekseerInternalHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.RingRenderer.h')
effekseerInternalHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.ModelRenderer.h')
effekseerInternalHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.TrackRenderer.h')

effekseerInternalHeader.output('Effekseer/Effekseer.Internal.h')

effekseerRendererDX9Header = CreateHeader()
effekseerRendererDX9Header.readLines('EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Base.Pre.h')
effekseerRendererDX9Header.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererDX9Header.readLines('EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.h')
effekseerRendererDX9Header.output('EffekseerRendererDX9/EffekseerRendererDX9.h')

effekseerRendererDX11Header = CreateHeader()
effekseerRendererDX11Header.readLines('EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Base.Pre.h')
effekseerRendererDX11Header.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererDX11Header.readLines('EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h')
effekseerRendererDX11Header.output('EffekseerRendererDX11/EffekseerRendererDX11.h')

effekseerRendererDX12Header = CreateHeader()
effekseerRendererDX12Header.readLines('EffekseerRendererDX12/EffekseerRenderer/EffekseerRendererDX12.Base.Pre.h')
effekseerRendererDX12Header.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererDX12Header.readLines('EffekseerRendererDX12/EffekseerRenderer/EffekseerRendererDX12.Renderer.h')
effekseerRendererDX12Header.output('EffekseerRendererDX12/EffekseerRendererDX12.h')

effekseerRendererGLHeader = CreateHeader()
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Base.Pre.h')
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.DeviceObjectCollection.h')
effekseerRendererGLHeader.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h')
effekseerRendererGLHeader.output('EffekseerRendererGL/EffekseerRendererGL.h')

effekseerRendererMetalHeader = CreateHeader()
effekseerRendererMetalHeader.readLines('EffekseerRendererMetal/EffekseerRenderer/EffekseerRendererMetal.Base.Pre.h')
effekseerRendererMetalHeader.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererMetalHeader.readLines('EffekseerRendererMetal/EffekseerRenderer/EffekseerRendererMetal.Renderer.h')
effekseerRendererMetalHeader.output('EffekseerRendererMetal/EffekseerRendererMetal.h')
