using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Effekseer.Utl
{
	public enum CompiledMaterialPlatformType : int
	{
		DirectX9 = 0,
		// DirectX10 = 1,
		DirectX11 = 2,
		DirectX12 = 3,
		OpenGL = 10,
		Metal = 20,
		Vulkan = 30,
		PS4 = 40,
		Switch = 50,
		XBoxOne = 60,
	}

	public enum TextureType : int
	{
		Color,
		Value,
	}

	/// <summary>
	/// An information of file of compiled material's header
	/// </summary>
	public class CompiledMaterialInformation
	{
		public UInt64 GUID;
		public HashSet<CompiledMaterialPlatformType> Platforms = new HashSet<CompiledMaterialPlatformType>();

		public bool Load(string path)
		{
			if (string.IsNullOrEmpty(path))
				return false;

			System.IO.FileStream fs = null;
			if (!System.IO.File.Exists(path)) return false;

			try
			{
				fs = System.IO.File.Open(path, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
			}
			catch (System.IO.FileNotFoundException e)
			{
				return false;
			}


			var br = new System.IO.BinaryReader(fs);

			var buf = new byte[1024];


			if (br.Read(buf, 0, 20) != 20)
			{
				fs.Dispose();
				br.Close();
				return false;
			}

			if (buf[0] != 'e' ||
				buf[1] != 'M' ||
				buf[2] != 'C' ||
				buf[3] != 'B')
			{
				return false;
			}

			int version = BitConverter.ToInt32(buf, 4);
			GUID = BitConverter.ToUInt64(buf, 8);

			var platformCount = BitConverter.ToInt32(buf, 16);

			for(int i = 0; i < platformCount; i++)
			{
				if (br.Read(buf, 0, 4) != 4)
				{
					fs.Dispose();
					br.Close();
					return false;
				}

				var type = (CompiledMaterialPlatformType)BitConverter.ToInt32(buf, 0);
				Platforms.Add(type);
			}

			return true;
		}
	}

	public class MaterialInformation
	{
		public TextureInformation[] Textures = new TextureInformation[0];

		public UniformInformation[] Uniforms = new UniformInformation[0];

		public UInt64 GUID;

		public Dictionary<Language, string> Names = new Dictionary<Language, string>();

		public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();

		public bool Load(string path)
		{
			if (string.IsNullOrEmpty(path))
				return false;

			System.IO.FileStream fs = null;
			if (!System.IO.File.Exists(path)) return false;

			try
			{
				fs = System.IO.File.Open(path, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
			}
			catch (System.IO.FileNotFoundException e)
			{
				return false;
			}


			var br = new System.IO.BinaryReader(fs);

			var buf = new byte[1024];


			if (br.Read(buf, 0, 16) != 16)
			{
				fs.Dispose();
				br.Close();
				return false;
			}

			if(buf[0] != 'E' ||
				buf[1] != 'F' ||
				buf[2] != 'K' ||
				buf[3] != 'M')
			{
				return false;
			}

			int version = BitConverter.ToInt32(buf, 4);
			GUID = BitConverter.ToUInt64(buf, 8);

			while (true)
			{
				if (br.Read(buf, 0, 8) != 8)
				{
					fs.Dispose();
					br.Close();
					break;
				}

				if (buf[0] == 'D' &&
				buf[1] == 'E' &&
				buf[2] == 'S' &&
				buf[3] == 'C')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					int count = 0;
					reader.Get(ref count);

					for (int i = 0; i < count; i++)
					{
						int lang = 0;
						string name = null;
						string desc = null;
						reader.Get(ref lang);
						reader.Get(ref name, Encoding.UTF8);
						reader.Get(ref desc, Encoding.UTF8);
						Names.Add((Language)lang, name);
						Descriptions.Add((Language)lang, desc);
					}
				}

				if (buf[0] == 'P' &&
				buf[1] == 'R' &&
				buf[2] == 'M' &&
				buf[3] == '_')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					int shadingModel = 0;
					reader.Get(ref shadingModel);

					bool hasNormal = false;
					reader.Get(ref hasNormal);

					bool hasReflection = false;
					reader.Get(ref hasReflection);

					int customData1Count = 0;
					reader.Get(ref customData1Count);

					int customData2Count = 0;
					reader.Get(ref customData2Count);

					int textureCount = 0;
					reader.Get(ref textureCount);

					List<TextureInformation> textures = new List<TextureInformation>();

					for (int i = 0; i < textureCount; i++)
					{
						TextureInformation info = new TextureInformation();

						reader.Get(ref info.Name, Encoding.UTF8);
						reader.Get(ref info.DefaultPath, Encoding.UTF8);
						reader.Get(ref info.Index);
						reader.Get(ref info.Priority);
						reader.Get(ref info.IsParam);
						int textureType = 0;
						reader.Get(ref textureType);
						info.Type = (TextureType)textureType;
						int sampler = 0;
						reader.Get(ref sampler);

						// convert a path into absolute
						if (string.IsNullOrEmpty(info.DefaultPath))
						{
							info.DefaultPath = string.Empty;
						}
						else
						{
							Func<string, string> escape = (string s) =>
							{
								return s.Replace("%", "%25");
							};

							Func<string, string> unescape = (string s) =>
							{
								return s.Replace("%25", "%");
							};

							Uri basePath = new Uri(path);
							Uri targetPath = new Uri(basePath, info.DefaultPath);
							info.DefaultPath = targetPath.AbsolutePath;
						}

						textures.Add(info);
					}

					Textures = textures.ToArray();

					int uniformCount = 0;
					reader.Get(ref uniformCount);

					List<UniformInformation> uniforms = new List<UniformInformation>();

					for (int i = 0; i < uniformCount; i++)
					{
						UniformInformation info = new UniformInformation();

						reader.Get(ref info.Name, Encoding.UTF8);
						reader.Get(ref info.Offset);
						reader.Get(ref info.Priority);
						reader.Get(ref info.Type);
						reader.Get(ref info.DefaultValues[0]);
						reader.Get(ref info.DefaultValues[1]);
						reader.Get(ref info.DefaultValues[2]);
						reader.Get(ref info.DefaultValues[3]);
						uniforms.Add(info);
					}

					Uniforms = uniforms.ToArray();
				}

				if (buf[0] == 'P' &&
				buf[1] == 'R' &&
				buf[2] == 'M' &&
				buf[3] == '2')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					int textureCount = 0;
					reader.Get(ref textureCount);

					for (int j = 0; j < textureCount; j++)
					{
						int count = 0;
						reader.Get(ref count);

						for (int i = 0; i < count; i++)
						{
							int lang = 0;
							string name = null;
							string desc = null;
							reader.Get(ref lang);
							reader.Get(ref name, Encoding.UTF8);
							reader.Get(ref desc, Encoding.UTF8);
							Textures[j].Names.Add((Language)lang, name);
							Textures[j].Descriptions.Add((Language)lang, desc);
						}
					}

					int uniformCount = 0;
					reader.Get(ref uniformCount);

					for (int j = 0; j < uniformCount; j++)
					{
						int count = 0;
						reader.Get(ref count);

						for (int i = 0; i < count; i++)
						{
							int lang = 0;
							string name = null;
							string desc = null;
							reader.Get(ref lang);
							reader.Get(ref name, Encoding.UTF8);
							reader.Get(ref desc, Encoding.UTF8);
							Uniforms[j].Names.Add((Language)lang, name);
							Uniforms[j].Descriptions.Add((Language)lang, desc);
						}
					}
				}
			}

			return true;
		}

		public class TextureInformation
		{
			public string Name;
			public int Index;
			public string DefaultPath;
			public bool IsParam;
			public TextureType Type = TextureType.Color;
			public int Priority = 1;

			public Dictionary<Language, string> Names = new Dictionary<Language, string>();
			public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();
		}

		public class UniformInformation
		{
			public string Name;
			public int Offset;
			public int Type = 0;
			public float[] DefaultValues = new float[4];
			public int Priority = 1;

			public Dictionary<Language, string> Names = new Dictionary<Language, string>();
			public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();
		}
	}
}
