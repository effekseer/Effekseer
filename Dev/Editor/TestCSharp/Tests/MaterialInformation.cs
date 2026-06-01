using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using TestCSharp.Misc;

namespace TestCSharp
{
	class MaterialInformation
	{
		public static void Test()
		{
			TestMaterialFile("../../TestData/Effects/Materials_Serialized/CustomData1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials_Serialized/DepthFade.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials_Serialized/Parameter23.efkmat");
		}

		static void TestMaterialFile(string path)
		{
			string fullpath = Path.Combine(Directory.GetCurrentDirectory(), path);
			var material = new Effekseer.Utils.MaterialInformation();

			byte[] bytes = File.ReadAllBytes(fullpath);
			if (bytes == null)
			{
				TestUtils.Assert(false, "Failed to load material file: " + path);
			}

			if (!material.Load(bytes))
			{
				TestUtils.Assert(false, "Failed to load material file: " + path);
			}

			byte[] savedBytes = material.SaveToBytes();
			// File.WriteAllBytes(fullpath + ".testout.efkmat", savedBytes);

			var savedMaterial = new Effekseer.Utils.MaterialInformation();
			if (!savedMaterial.Load(savedBytes))
			{
				TestUtils.Assert(false, "Failed to load saved material bytes: " + path);
			}

			CompareMaterial(material, savedMaterial);

			byte[] savedAgainBytes = savedMaterial.SaveToBytes();
			TestUtils.CompareBytes(RemoveDataChunk(savedBytes), RemoveDataChunk(savedAgainBytes));
		}

		static void CompareMaterial(Effekseer.Utils.MaterialInformation expected, Effekseer.Utils.MaterialInformation actual)
		{
			TestUtils.Assert(expected.GUID == actual.GUID);
			TestUtils.Equals(expected.ShadingModel, actual.ShadingModel);
			TestUtils.Assert(expected.HasNormal == actual.HasNormal);
			TestUtils.Assert(expected.HasRefraction == actual.HasRefraction);
			TestUtils.Equals(expected.CustomData1Count, actual.CustomData1Count);
			TestUtils.Equals(expected.CustomData2Count, actual.CustomData2Count);
			TestUtils.Equals(expected.Name, actual.Name);
			TestUtils.Equals(expected.Description, actual.Description);
			TestUtils.Equals(expected.Code, actual.Code);
			TestUtils.Assert(expected.RequiredFunctionTypes.SequenceEqual(actual.RequiredFunctionTypes));

			TestUtils.Equals(expected.Textures.Length, actual.Textures.Length);
			for (int i = 0; i < expected.Textures.Length; i++)
			{
				CompareTexture(expected.Textures[i], actual.Textures[i]);
			}

			TestUtils.Equals(expected.Uniforms.Length, actual.Uniforms.Length);
			for (int i = 0; i < expected.Uniforms.Length; i++)
			{
				CompareUniform(expected.Uniforms[i], actual.Uniforms[i]);
			}

			TestUtils.Equals(expected.CustomData.Length, actual.CustomData.Length);
			for (int i = 0; i < expected.CustomData.Length; i++)
			{
				CompareCustomData(expected.CustomData[i], actual.CustomData[i]);
			}

			TestUtils.Equals(expected.Gradients.Length, actual.Gradients.Length);
			for (int i = 0; i < expected.Gradients.Length; i++)
			{
				CompareGradient(expected.Gradients[i], actual.Gradients[i]);
			}

			TestUtils.Equals(expected.FixedGradients.Length, actual.FixedGradients.Length);
			for (int i = 0; i < expected.FixedGradients.Length; i++)
			{
				CompareGradient(expected.FixedGradients[i], actual.FixedGradients[i]);
			}

			TestUtils.Assert(Newtonsoft.Json.Linq.JToken.DeepEquals(expected.EditorData, actual.EditorData));
		}

		static void CompareTexture(Effekseer.Utils.MaterialInformation.TextureInformation expected, Effekseer.Utils.MaterialInformation.TextureInformation actual)
		{
			TestUtils.Equals(expected.Name, actual.Name);
			TestUtils.Equals(expected.UniformName, actual.UniformName);
			TestUtils.Equals(expected.Index, actual.Index);
			TestUtils.Equals(expected.DefaultPath, actual.DefaultPath);
			TestUtils.Assert(expected.IsParam == actual.IsParam);
			TestUtils.Equals(expected.Sampler, actual.Sampler);
			TestUtils.Assert(expected.Type == actual.Type);
			TestUtils.Equals(expected.Priority, actual.Priority);
			TestUtils.Equals(expected.Summary, actual.Summary);
			TestUtils.Equals(expected.Description, actual.Description);
		}

		static void CompareUniform(Effekseer.Utils.MaterialInformation.UniformInformation expected, Effekseer.Utils.MaterialInformation.UniformInformation actual)
		{
			TestUtils.Equals(expected.Name, actual.Name);
			TestUtils.Equals(expected.UniformName, actual.UniformName);
			TestUtils.Equals(expected.Offset, actual.Offset);
			TestUtils.Equals(expected.Type, actual.Type);
			TestUtils.Assert(expected.DefaultValues.SequenceEqual(actual.DefaultValues));
			TestUtils.Equals(expected.Priority, actual.Priority);
			TestUtils.Equals(expected.Summary, actual.Summary);
			TestUtils.Equals(expected.Description, actual.Description);
		}

		static void CompareCustomData(Effekseer.Utils.MaterialInformation.CustomDataInformation expected, Effekseer.Utils.MaterialInformation.CustomDataInformation actual)
		{
			TestUtils.Equals(expected.Summary, actual.Summary);
			TestUtils.Equals(expected.Description, actual.Description);
			TestUtils.Assert(expected.DefaultValues.SequenceEqual(actual.DefaultValues));
		}

		static void CompareGradient(Effekseer.Utils.MaterialInformation.GradientInformation expected, Effekseer.Utils.MaterialInformation.GradientInformation actual)
		{
			TestUtils.Equals(expected.Name, actual.Name);
			TestUtils.Equals(expected.UniformName, actual.UniformName);
			TestUtils.Equals(expected.Offset, actual.Offset);
			TestUtils.Equals(expected.Priority, actual.Priority);
			TestUtils.Equals(expected.Summary, actual.Summary);
			TestUtils.Equals(expected.Description, actual.Description);
			TestUtils.Assert(expected.Data.Equals(actual.Data));
		}

		static byte[] RemoveDataChunk(byte[] bytes)
		{
			var stream = new MemoryStream(bytes);
			var reader = new BinaryReader(stream);
			var buf = new byte[16];

			reader.Read(buf, 0, 16);

			while (stream.Position < stream.Length)
			{
				reader.Read(buf, 0, 4);
				uint size = reader.ReadUInt32();

				if (buf[0] == 'D' &&
					buf[1] == 'A' &&
					buf[2] == 'T' &&
					buf[3] == 'A')
				{
					return bytes.Take((int)stream.Position - 8).ToArray();
				}

				stream.Seek(stream.Position + size, SeekOrigin.Begin);
			}
			return bytes;
		}
	}
}
