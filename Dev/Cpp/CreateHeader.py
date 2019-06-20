import re
import codecs

def isValidLine(line):
	if re.search('include \"', line) == None or line.find('.PSVita') != -1 or line.find('.PS4') != -1 or line.find('.Switch') != -1 or line.find('.XBoxOne') != -1:
		return True
	return False

class CreateHeader:
	def __init__(self):
		self.lines = []

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
effekseerHeader.readLines('Effekseer/Effekseer/Effekseer.CustomAllocator.h')
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

effekseerHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.SpriteRenderer.h')
effekseerHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.RibbonRenderer.h')
effekseerHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.RingRenderer.h')
effekseerHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.ModelRenderer.h')
effekseerHeader.readLines('Effekseer/Effekseer/Renderer/Effekseer.TrackRenderer.h')

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

effekseerRendererGLHeader = CreateHeader()
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Base.Pre.h')
effekseerRendererGLHeader.readLines('EffekseerRendererCommon/EffekseerRenderer.Renderer.h')
effekseerRendererGLHeader.readLines('EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h')
effekseerRendererGLHeader.output('EffekseerRendererGL/EffekseerRendererGL.h')
