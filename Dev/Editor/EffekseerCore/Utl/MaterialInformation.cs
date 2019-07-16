using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Effekseer.Utl
{
	public class MaterialInformation
	{
		public TextureInformation[] Textures = new TextureInformation[0];

		public UniformInformation[] Uniforms = new UniformInformation[0];

		public bool Load(string path)
		{
			if (string.IsNullOrEmpty(path))
				return false;

			System.IO.FileStream fs = null;
			if (!System.IO.File.Exists(path)) return false;

#if MATERIAL_ENABLED
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

			if(buf[0] != 'e' ||
				buf[1] != 'f' ||
				buf[2] != 'k' ||
				buf[3] != 'M')
			{
				return false;
			}

			int version = BitConverter.ToInt32(buf, 4);
			UInt64 guid = BitConverter.ToUInt64(buf, 8);

			while(true)
			{
				if (br.Read(buf, 0, 8) != 8)
				{
					fs.Dispose();
					br.Close();
					break;
				}


				if (buf[0] == 'p' &&
				buf[1] == 'a' &&
				buf[2] == 'r' &&
				buf[3] == 'a')
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

					int textureCount = 0;
					reader.Get(ref textureCount);

					List<TextureInformation> textures = new List<TextureInformation>();

					for(int i = 0; i < textureCount; i++)
					{
						TextureInformation info = new TextureInformation();

						reader.Get(ref info.Name);
						reader.Get(ref info.DefaultPath);
						reader.Get(ref info.Index);
						reader.Get(ref info.IsParam);
						reader.Get(ref info.IsValueTexture);

						// convert a path into absolute
						if (string.IsNullOrEmpty(info.DefaultPath))
						{
							info.DefaultPath = string.Empty;
						}
						else
						{
							Uri basePath = new Uri(path);
							Uri targetPath = new Uri(basePath, info.DefaultPath);
							info.DefaultPath = targetPath.ToString();
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

						reader.Get(ref info.Name);
						reader.Get(ref info.Offset);
						reader.Get(ref info.Type);
						reader.Get(ref info.DefaultValues[0]);
						reader.Get(ref info.DefaultValues[1]);
						reader.Get(ref info.DefaultValues[2]);
						reader.Get(ref info.DefaultValues[3]);
						uniforms.Add(info);
					}

					Uniforms = uniforms.ToArray();
				}
			}
#endif
			return true;
		}

		public class TextureInformation
		{
			public string Name;
			public int Index;
			public string DefaultPath;
			public bool IsParam;
			public bool IsValueTexture;
		}

		public class UniformInformation
		{
			public string Name;
			public int Offset;
			public int Type = 0;
			public float[] DefaultValues = new float[4];
		}
	}
}
