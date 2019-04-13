using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.Exporter
{
	public enum glTFExporterFormat
	{
		glTF,
		glb,
	}

	public class glTFExporterOption
	{
		public glTFExporterFormat Format = glTFExporterFormat.glTF;
		public bool IsContainedTextureAsBinary = false;
		public float Scale = 1.0f;
	}

	public class glTFExporter
	{
		List<byte> internalBuffer = new List<byte>();
		Dictionary<string, Buffer> buffers = new Dictionary<string, Buffer>();
		Dictionary<string, BufferView> bufferViews = new Dictionary<string, BufferView>();

		public bool Export(string path, glTFExporterOption option = null)
		{
			if(option == null)
			{
				option = new glTFExporterOption();
			}

			if(option.Format == glTFExporterFormat.glb)
			{
				option.IsContainedTextureAsBinary = true;
			}

			var gltf = new Dictionary<string, object>();

			gltf.Add("extensionsUsed", new object[] { "Effekseer" });

			var extentions = new Dictionary<string, object>();

			gltf.Add("extensions", extentions);

			var effekseerExtention = new EffekseerExtention();

			var effect = CreateEffect(option.Scale, option.IsContainedTextureAsBinary);

			effekseerExtention.effects.Add(effect.effectName, effect);

			extentions.Add("Effekseer", effekseerExtention);

			// buffer
			Buffer buffer = new Buffer();
			buffer.byteLength = internalBuffer.Count;
            buffer.uri = System.IO.Path.ChangeExtension(path, "bin");
			buffers.Add("effectBuffer", buffer);
			gltf.Add("buffers", buffers);

			// bufferview
			foreach (var bufferView in bufferViews)
			{
				bufferView.Value.buffer = "effectBuffer";
			}

			gltf.Add("bufferViews", bufferViews);

			string json = JsonConvert.SerializeObject(gltf, Formatting.Indented);

			if(option.Format == glTFExporterFormat.glTF)
			{
				System.IO.File.WriteAllText(System.IO.Path.ChangeExtension(path, "json"), json);
				System.IO.File.WriteAllBytes(System.IO.Path.ChangeExtension(path, "bin"), internalBuffer.ToArray());
			}
			else
			{
				var jsonBuffer = Encoding.ASCII.GetBytes(json);
				List<byte[]> exported = new List<byte[]>();
				exported.Add(Encoding.ASCII.GetBytes("glTF").Take(4).ToArray());
				exported.Add(BitConverter.GetBytes(2));
				exported.Add(BitConverter.GetBytes(12 + 8 + jsonBuffer.Length + 8 + internalBuffer.Count));

				exported.Add(BitConverter.GetBytes(jsonBuffer.Length));
				exported.Add(Encoding.ASCII.GetBytes("JSON").Take(4).ToArray());
				exported.Add(jsonBuffer);

				exported.Add(BitConverter.GetBytes(internalBuffer.Count));
				exported.Add(Encoding.ASCII.GetBytes("BIN").Concat(new byte [] {0}).ToArray());
				exported.Add(internalBuffer.ToArray());

				System.IO.File.WriteAllBytes(System.IO.Path.ChangeExtension(path, "glb"), exported.SelectMany(_=>_).ToArray());
			}

			return true;
		}

		void AddBufferView(string name, byte[] buffer)
		{
			int offset = internalBuffer.Count;
			int length = buffer.Length;

			foreach (var b in buffer)
			{
				internalBuffer.Add(b);
			}

			var bufferView = new BufferView();
			bufferView.byteOffset = offset;
			bufferView.byteLength = length;
			bufferViews.Add(name, bufferView);
		}

		EffekseerEffect CreateEffect(float scale, bool isContainedTextureAsBinary)
		{
			EffekseerEffect effect = new EffekseerEffect();

			var name = System.IO.Path.GetFileNameWithoutExtension(Core.FullPath);
			if (string.IsNullOrEmpty(name))
			{
				name = "effect";
			}

			var bodyName = name + "_body";

			effect.nodeName = name;
			effect.effectName = name;

			var binaryExporter = new Binary.Exporter();
			var binary = binaryExporter.Export(scale);

			AddBufferView(bodyName, binary);

			effect.body = bodyName;

			SortedSet<string> textures = new SortedSet<string>();

			foreach (var texture in binaryExporter.UsedTextures)
			{
				textures.Add(texture);
			}

			foreach (var texture in binaryExporter.UsedNormalTextures)
			{
				textures.Add(texture);
			}

			foreach (var texture in binaryExporter.UsedDistortionTextures)
			{
				textures.Add(texture);
			}

			if(isContainedTextureAsBinary)
			{
				foreach (var texture in textures)
				{
					var buf = System.IO.File.ReadAllBytes(texture);
					AddBufferView(texture, buf);
					effect.images.Add(texture, CreateImageAsBufferView(texture));
				}
			}
			else
			{
				foreach (var texture in textures)
				{
					effect.images.Add(texture, CreateImageAsURI(texture));
				}
			}

			return effect;
		}

		class Buffer
		{
			public int byteLength = 0;
			public string type = "arraybuffer";
			public string uri = "";
		}

		class BufferView
		{
			public object buffer = null;
			public int byteLength = 0;
			public int byteOffset = 0;
		}

		class EffekseerExtention
		{
			public Dictionary<string, EffekseerEffect> effects = new Dictionary<string, EffekseerEffect>();
		}

		class EffekseerEffect
		{
			public string nodeName = "";

			public string effectName = "";

			public object body = "";

			public Dictionary<string, object> images = new Dictionary<string, object>();
		}

		Dictionary<string, object> CreateImageAsBufferView(string bufferview)
		{
			var ret = new Dictionary<string, object>();
			ret.Add("bufferview", bufferview);
			return ret;
		}

		Dictionary<string, object> CreateImageAsURI(string uri)
		{
			var ret = new Dictionary<string, object>();
			ret.Add("uri", uri);
			return ret;
		}
	}
}
