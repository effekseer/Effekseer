using System;
using System.IO;
using TestCSharp.Misc;

namespace TestCSharp
{
	class MaterialCacheGeneratorTest
	{
		const int WebGPUPlatform = (int)Effekseer.Utils.CompiledMaterialPlatformType.WebGPU;

		public static void Test()
		{
			var testDirectory = Path.Combine(Directory.GetCurrentDirectory(), "MaterialCacheGeneratorTest");
			Directory.CreateDirectory(testDirectory);

			var sourcePath = Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), "../../TestData/Effects/Materials/UV1.efkmat"));
			var materialPath = Path.Combine(testDirectory, "UV1.efkmat");
			var compiledPath = Effekseer.IO.MaterialCacheGenerator.CreateBinaryFilePath(materialPath);

			if (File.Exists(materialPath))
			{
				File.Delete(materialPath);
			}

			if (File.Exists(compiledPath))
			{
				File.Delete(compiledPath);
			}

			File.Copy(sourcePath, materialPath);

			TestUtils.Assert(Effekseer.IO.MaterialCacheGenerator.GenerateMaterialCache(materialPath), "Failed to generate material cache.");
			TestUtils.Assert(File.Exists(compiledPath), "Compiled material cache was not created.");

			ValidateCompiledMaterial(File.ReadAllBytes(compiledPath));

			File.Delete(compiledPath);
			File.Delete(materialPath);
			Directory.Delete(testDirectory);
		}

		static void ValidateCompiledMaterial(byte[] data)
		{
			using var stream = new MemoryStream(data);
			using var reader = new BinaryReader(stream);

			var header = new string(reader.ReadChars(4));
			TestUtils.Assert(header == "eMCB", "Invalid compiled material header.");

			reader.ReadInt32();
			reader.ReadUInt64();

			var platformCount = reader.ReadInt32();
			var hasWebGPU = false;
			for (int i = 0; i < platformCount; i++)
			{
				hasWebGPU |= reader.ReadInt32() == WebGPUPlatform;
			}
			TestUtils.Assert(hasWebGPU, "Compiled material does not contain WebGPU platform data.");

			var originalDataSize = reader.ReadInt32();
			stream.Seek(originalDataSize, SeekOrigin.Current);

			while (stream.Position < stream.Length)
			{
				var platform = reader.ReadInt32();
				var chunkSize = reader.ReadInt32();
				var nextChunkPosition = stream.Position + chunkSize;

				if (platform == WebGPUPlatform)
				{
					var standardVSSize = reader.ReadInt32();
					stream.Seek(standardVSSize, SeekOrigin.Current);
					var standardPSSize = reader.ReadInt32();
					stream.Seek(standardPSSize, SeekOrigin.Current);
					var modelVSSize = reader.ReadInt32();
					stream.Seek(modelVSSize, SeekOrigin.Current);
					var modelPSSize = reader.ReadInt32();

					TestUtils.Assert(standardVSSize > 0, "WebGPU standard vertex shader is empty.");
					TestUtils.Assert(standardPSSize > 0, "WebGPU standard pixel shader is empty.");
					TestUtils.Assert(modelVSSize > 0, "WebGPU model vertex shader is empty.");
					TestUtils.Assert(modelPSSize > 0, "WebGPU model pixel shader is empty.");
					return;
				}

				stream.Seek(nextChunkPosition, SeekOrigin.Begin);
			}

			TestUtils.Assert(false, "WebGPU compiled material chunk was not found.");
		}
	}
}
