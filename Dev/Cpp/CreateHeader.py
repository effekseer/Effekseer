import os
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
		self.lines.append(line)

	def readLines(self,path):
		f = codecs.open(path, 'r','utf-8_sig')
		line = f.readline()
		while line:
			if isValidLine(line):
 	 			self.lines.append(line.strip(os.linesep))
			line = f.readline()
		f.close()

	def output(self,path):
		f = codecs.open(path, 'w','utf-8_sig')
		for line in self.lines:
			f.write(line + os.linesep)
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
effekseerHeader.readLines('Effekseer/Effekseer/Backend/GraphicsDevice.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Resource.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Effect.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Manager.h')
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.Setting.h')
effekseerHeader.readLines('Effekseer/Effekseer/Network/Effekseer.Server.h')
effekseerHeader.readLines('Effekseer/Effekseer/Network/Effekseer.Client.h')
effekseerHeader.addLine('')
effekseerHeader.addLine('#include "Effekseer.Modules.h"')
effekseerHeader.addLine('')
effekseerHeader.output('Effekseer/Effekseer.h')

effekseerSimdHeader = CreateHeader()
effekseerSimdHeader.addLine('#pragma once')
effekseerSimdHeader.addLine('#include "Effekseer.h"')
effekseerSimdHeader.addLine('')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Base.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Float4_Gen.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Float4_NEON.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Float4_SSE.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Int4_Gen.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Int4_NEON.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Int4_SSE.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Bridge_Gen.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Bridge_NEON.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Bridge_SSE.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Vec2f.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Vec3f.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Vec4f.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Mat43f.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Mat44f.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Quaternionf.h')
effekseerSimdHeader.readLines('Effekseer/Effekseer/SIMD/Utils.h')
effekseerSimdHeader.output('Effekseer/Effekseer.SIMD.h')

effekseerModulesHeader = CreateHeader()
effekseerModulesHeader.addLine('#pragma once')
effekseerModulesHeader.addLine('')
effekseerModulesHeader.addLine('#include "Effekseer.h"')
effekseerModulesHeader.addLine('#include "Effekseer.SIMD.h"')
effekseerModulesHeader.addLine('')
effekseerModulesHeader.addLine('// A header to access internal data of effekseer')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Parameter/Effekseer.Parameters.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.SpriteRenderer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.RibbonRenderer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.RingRenderer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.ModelRenderer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.TrackRenderer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.GPUTimer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Effekseer.EffectLoader.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Effekseer.TextureLoader.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Model/Model.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Model/ModelLoader.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Effekseer.MaterialLoader.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Model/Model.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Effekseer.Curve.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Effekseer.CurveLoader.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Sound/Effekseer.SoundPlayer.h')
effekseerModulesHeader.readLines('Effekseer/Effekseer/Effekseer.SoundLoader.h')
effekseerModulesHeader.output('Effekseer/Effekseer.Modules.h')

effekseerRendererDX9Header = CreateHeader()
effekseerRendererDX9Header.readLines('EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Base.Pre.h')
effekseerRendererDX9Header.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererDX9Header.readLines('EffekseerRendererCommon/TextureLoader.h')
effekseerRendererDX9Header.readLines('EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.h')
effekseerRendererDX9Header.output('EffekseerRendererDX9/EffekseerRendererDX9.h')

effekseerRendererDX11Header = CreateHeader()
effekseerRendererDX11Header.readLines('EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Base.Pre.h')
effekseerRendererDX11Header.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererDX11Header.readLines('EffekseerRendererCommon/TextureLoader.h')
effekseerRendererDX11Header.readLines('EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h')
effekseerRendererDX11Header.output('EffekseerRendererDX11/EffekseerRendererDX11.h')

effekseerRendererDX12Header = CreateHeader()
effekseerRendererDX12Header.readLines('EffekseerRendererDX12/EffekseerRenderer/EffekseerRendererDX12.Base.Pre.h')
effekseerRendererDX12Header.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererDX12Header.readLines('EffekseerRendererCommon/TextureLoader.h')
effekseerRendererDX12Header.readLines('EffekseerRendererDX12/EffekseerRenderer/EffekseerRendererDX12.Renderer.h')
effekseerRendererDX12Header.readLines('EffekseerRendererLLGI/Common.h')
effekseerRendererDX12Header.output('EffekseerRendererDX12/EffekseerRendererDX12.h')

effekseerRendererVulkanHeader = CreateHeader()
effekseerRendererVulkanHeader.readLines('EffekseerRendererVulkan/EffekseerRenderer/EffekseerRendererVulkan.Base.Pre.h')
effekseerRendererVulkanHeader.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererVulkanHeader.readLines('EffekseerRendererCommon/TextureLoader.h')
effekseerRendererVulkanHeader.readLines('EffekseerRendererVulkan/EffekseerRenderer/EffekseerRendererVulkan.Renderer.h')
effekseerRendererVulkanHeader.readLines('EffekseerRendererLLGI/Common.h')
effekseerRendererVulkanHeader.output('EffekseerRendererVulkan/EffekseerRendererVulkan.h')

effekseerRendererGLHeader = CreateHeader()
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Base.Pre.h')
effekseerRendererGLHeader.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererGLHeader.readLines('EffekseerRendererCommon/TextureLoader.h')
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h')
effekseerRendererGLHeader.output('EffekseerRendererGL/EffekseerRendererGL.h')

effekseerRendererMetalHeader = CreateHeader()
effekseerRendererMetalHeader.readLines('EffekseerRendererMetal/EffekseerRenderer/EffekseerRendererMetal.Base.Pre.h')
effekseerRendererMetalHeader.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererMetalHeader.readLines('EffekseerRendererCommon/TextureLoader.h')
effekseerRendererMetalHeader.readLines('EffekseerRendererMetal/EffekseerRenderer/EffekseerRendererMetal.Renderer.h')
effekseerRendererMetalHeader.readLines('EffekseerRendererLLGI/Common.h')
effekseerRendererMetalHeader.output('EffekseerRendererMetal/EffekseerRendererMetal.h')
