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
		public float ExternalScale = 1.0f;

        /// <summary>
        /// if bindingNode is int, an effect is binding with node index.
        /// if bindingNode is str, an effect is binding with node name.
        /// if bindingNode is null, an effect is binding with default node name.
        /// </summary>
        public object bindingNode = null;
	}

	public class glTFExporter
	{
		List<byte> internalBuffer = new List<byte>();
		List<Buffer> buffers = new List<Buffer>();
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

			var effect = CreateEffect(option.Scale, option.ExternalScale, option.bindingNode, option.IsContainedTextureAsBinary);

			effekseerExtention.effects.Add(effect);

			extentions.Add("Effekseer", effekseerExtention);

			// buffer
			Buffer buffer = new Buffer();
			buffer.byteLength = internalBuffer.Count;
            buffer.uri = System.IO.Path.GetFileName(System.IO.Path.ChangeExtension(path, "bin"));
			buffers.Add(buffer);
			gltf.Add("buffers", buffers);

			// bufferview
			foreach (var bufferView in bufferViews)
			{
				bufferView.Value.buffer = 0;
			}

			gltf.Add("bufferViews", bufferViews);

			string json = JsonConvert.SerializeObject(gltf, Formatting.Indented);

			if(option.Format == glTFExporterFormat.glTF)
			{
				System.IO.File.WriteAllText(System.IO.Path.ChangeExtension(path, "gltf"), json);
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

        EffekseerEffect CreateEffect(float scale, float externalScale, object bindingNode, bool isContainedAsBinary)
		{
			var effect = new EffekseerEffect();

			var name = System.IO.Path.GetFileNameWithoutExtension(Core.FullPath);
			if (string.IsNullOrEmpty(name))
			{
				name = "effect";
			}

			effect.Add("scale", externalScale);

			var bodyName = name + "_body";

            if (bindingNode == null)
            {
                effect.Add("nodeName", name);
            }
            else if (bindingNode is string)
            {
                effect.Add("nodeName", (string)bindingNode);
            }
            else if (bindingNode is int)
            {
                effect.Add("nodeIndex", (int)bindingNode);
            }

            effect.Add("effectName", name);

			var binaryExporter = new Binary.Exporter();
			var binary = binaryExporter.Export(scale, Binary.ExporterVersion.Latest);

			AddBufferView(bodyName, binary);

			effect.Add("body", CreateeBodyAsBufferView(bodyName));

			HashSet<string> textures = new HashSet<string>();
			
			foreach (var texture in binaryExporter.UsedTextures.ToList().OrderBy(_ => _))
			{
				textures.Add(texture);
			}

			HashSet<string> normalTextures = new HashSet<string>();

			foreach (var texture in binaryExporter.UsedNormalTextures.ToList().OrderBy(_ => _))
			{
				normalTextures.Add(texture);
			}

			HashSet<string> distortionTextures = new HashSet<string>();

			foreach (var texture in binaryExporter.UsedDistortionTextures.ToList().OrderBy(_ => _))
			{
				distortionTextures.Add(texture);
			}

            List<object> images = new List<object>();
			List<object> normalImages = new List<object>();
			List<object> distortionImages = new List<object>();

			if (isContainedAsBinary)
			{
				foreach (var texture in textures.ToList().OrderBy(_=>_))
				{
					Uri u1 = new Uri(System.IO.Path.GetDirectoryName(Core.FullPath) + System.IO.Path.DirectorySeparatorChar.ToString());
					Uri u2 = new Uri(u1, texture);

					var buf = System.IO.File.ReadAllBytes(u2.LocalPath);
					AddBufferView(texture, buf);

					var mimeType = "dds";
					if (System.IO.Path.GetExtension(texture).ToLower().Contains("png"))
						mimeType = "png";

					images.Add(CreateImageAsBufferView(texture, mimeType));
				}

				foreach (var texture in normalTextures.ToList().OrderBy(_ => _))
				{
					Uri u1 = new Uri(System.IO.Path.GetDirectoryName(Core.FullPath) + System.IO.Path.DirectorySeparatorChar.ToString());
					Uri u2 = new Uri(u1, texture);

					var buf = System.IO.File.ReadAllBytes(u2.LocalPath);
					AddBufferView(texture, buf);

					var mimeType = "dds";
					if (System.IO.Path.GetExtension(texture).ToLower().Contains("png"))
						mimeType = "png";

					normalImages.Add(CreateImageAsBufferView(texture, mimeType));
				}

				foreach (var texture in distortionTextures.ToList().OrderBy(_ => _))
				{
					Uri u1 = new Uri(System.IO.Path.GetDirectoryName(Core.FullPath) + System.IO.Path.DirectorySeparatorChar.ToString());
					Uri u2 = new Uri(u1, texture);

					var buf = System.IO.File.ReadAllBytes(u2.LocalPath);
					AddBufferView(texture, buf);

					var mimeType = "dds";
					if (System.IO.Path.GetExtension(texture).ToLower().Contains("png"))
						mimeType = "png";

					distortionImages.Add(CreateImageAsBufferView(texture, mimeType));
				}
			}
			else
			{
				foreach (var texture in textures.ToList().OrderBy(_ => _))
				{
					images.Add(CreateImageAsURI(texture));
				}

				foreach (var texture in normalTextures.ToList().OrderBy(_ => _))
				{
					normalImages.Add(CreateImageAsURI(texture));
				}

				foreach (var texture in distortionTextures.ToList().OrderBy(_ => _))
				{
					distortionImages.Add(CreateImageAsURI(texture));
				}
			}

            effect.Add("images", images);
			effect.Add("normalImages", normalImages);
			effect.Add("distortionImages", distortionImages);

			List<object> sounds = new List<object>();

			if (isContainedAsBinary)
			{
				foreach (var sound in binaryExporter.Sounds.ToList().OrderBy(_ => _))
				{
					Uri u1 = new Uri(System.IO.Path.GetDirectoryName(Core.FullPath) + System.IO.Path.DirectorySeparatorChar.ToString());
					Uri u2 = new Uri(u1, sound);

					var buf = System.IO.File.ReadAllBytes(u2.LocalPath);
					AddBufferView(sound, buf);
					sounds.Add(CreateAsBufferView(sound));
				}
			}
			else
			{
				foreach (var sound in binaryExporter.Sounds.ToList().OrderBy(_ => _))
				{
					sounds.Add(CreateImageAsURI(sound));
				}
			}

			effect.Add("sounds", sounds);

			List<object> models = new List<object>();

			if (isContainedAsBinary)
			{
				foreach (var model in binaryExporter.Models.ToList().OrderBy(_ => _))
				{
					Uri u1 = new Uri(System.IO.Path.GetDirectoryName(Core.FullPath) + System.IO.Path.DirectorySeparatorChar.ToString());
					Uri u2 = new Uri(u1, model);

					var buf = System.IO.File.ReadAllBytes(u2.LocalPath);
					AddBufferView(model, buf);
					models.Add(CreateAsBufferView(model));
				}
			}
			else
			{
				foreach (var model in binaryExporter.Models.ToList().OrderBy(_ => _))
				{
					models.Add(CreateImageAsURI(model));
				}
			}

			effect.Add("models", models);

			List<object> materials = new List<object>();

			if (isContainedAsBinary)
			{
				foreach (var material in binaryExporter.Materials.ToList().OrderBy(_ => _))
				{
					Uri u1 = new Uri(System.IO.Path.GetDirectoryName(Core.FullPath) + System.IO.Path.DirectorySeparatorChar.ToString());
					Uri u2 = new Uri(u1, material);

					var buf = System.IO.File.ReadAllBytes(u2.LocalPath);
					AddBufferView(material, buf);
					materials.Add(CreateAsBufferView(material));
				}
			}
			else
			{
				foreach (var material in binaryExporter.Materials.ToList().OrderBy(_ => _))
				{
					materials.Add(CreateImageAsURI(material));
				}
			}

			effect.Add("materials", materials);

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
			public int buffer = 0;
			public int byteLength = 0;
			public int byteOffset = 0;
		}

		class EffekseerExtention
		{
			public List<EffekseerEffect> effects = new List<EffekseerEffect>();
		}

        class EffekseerEffect : Dictionary<string, object>
        { }

		Dictionary<string, object> CreateeBodyAsBufferView(string bufferview)
		{
			var ret = new Dictionary<string, object>();
			ret.Add("bufferview", bufferview);
			return ret;
		}

		Dictionary<string, object> CreateBodyAsURI(string uri)
		{
			var ret = new Dictionary<string, object>();
			ret.Add("uri", uri);
			return ret;
		}

		Dictionary<string, object> CreateAsBufferView(string bufferview)
		{
			var ret = new Dictionary<string, object>();
			ret.Add("bufferview", bufferview);
			return ret;
		}

		Dictionary<string, object> CreateImageAsBufferView(string bufferview, string mimetype)
		{
			var ret = new Dictionary<string, object>();
			ret.Add("bufferview", bufferview);
			ret.Add("mimeType", "image/" + mimetype);
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
