using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.IO;

namespace Effekseer.Utl
{
#if MATERIAL_ENABLED
	public class MaterialInformation
	{
		public TextureInformation[] Textures = new TextureInformation[0];

		public UniformInformation[] Uniforms = new UniformInformation[0];

		public void Load(string path)
		{
			if (string.IsNullOrEmpty(path))
				return;

			System.IO.FileStream fs = null;
			if (!System.IO.File.Exists(path)) return;

			try
			{
				fs = System.IO.File.Open(path, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
			}
			catch (System.IO.FileNotFoundException e)
			{
				return;
			}


			var br = new System.IO.BinaryReader(fs);

			var buf = new byte[1024];


			if (br.Read(buf, 0, 16) != 16)
			{
				fs.Dispose();
				br.Close();
				return;
			}

			if(buf[0] != 'e' ||
				buf[1] != 'f' ||
				buf[2] != 'k' ||
				buf[3] != 'M')
			{
				return;
			}

			int version = BitConverter.ToInt32(buf, 4);
			UInt64 guid = BitConverter.ToUInt64(buf, 8);

			while(true)
			{
				if (br.Read(buf, 0, 8) != 16)
				{
					fs.Dispose();
					br.Close();
					break;
				}


				if (buf[0] == 'g' &&
				buf[1] == 'e' &&
				buf[2] == 'n' &&
				buf[3] == 'e')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (fs.Read(temp, 0, temp.Length) != temp.Length) return;
				}

				if (buf[0] != 'd' ||
				buf[1] != 'a' ||
				buf[2] != 't' ||
				buf[3] != 'a')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (fs.Read(temp, 0, temp.Length) != temp.Length) return;

					var jsonText = Encoding.UTF8.GetString(temp);

					var parsed = JObject.Parse(jsonText);

					var params_ = parsed["Params"];
					var uniforms_ = params_["Uniforms"] as JArray;
					var textures_ = params_["Textures"] as JArray;

					List<TextureInformation> textures = new List<TextureInformation>();

					foreach (var texture in textures_)
					{
						var name = texture["Name"].Value<string>();
						var offset = texture["Index"].Value<double>();
						var defaultPath = texture["DefaultPath"].Value<double>();
						var isParam = texture["IsParam"].Value<bool>();

						var info = new TextureInformation();

						textures.Add(info);
					}

					Textures = textures.ToArray();

					List<UniformInformation> uniforms = new List<UniformInformation>();

					foreach (var uniform in uniforms_)
					{
						var name = uniform["Name"].Value<string>();
						var offset = uniform["Offset"].Value<double>();
						var type = uniform["Type"].Value<double>();
						var defaultValue1 = uniform["DefaultValue1"].Value<double>();
						var defaultValue2 = uniform["DefaultValue2"].Value<double>();
						var defaultValue3 = uniform["DefaultValue3"].Value<double>();
						var defaultValue4 = uniform["DefaultValue4"].Value<double>();

						var info = new UniformInformation();
						info.Name = name;
						info.Offset = (int)offset;
						info.Type = (int)type;
						info.DefaultValues[0] = (float)defaultValue1;
						info.DefaultValues[1] = (float)defaultValue2;
						info.DefaultValues[2] = (float)defaultValue3;
						info.DefaultValues[3] = (float)defaultValue4;

						uniforms.Add(info);
					}

					Uniforms = uniforms.ToArray();
				}
			}
		}

		public class TextureInformation
		{
			public string Name;
			public int Index;
			public string DefaultPath;
			public bool IsParam;
		}

		public class UniformInformation
		{
			public string Name;
			public int Offset;
			public int Type = 0;
			public float[] DefaultValues = new float[4];
		}
	}
#endif
}
