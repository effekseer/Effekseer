using System;
using System.Collections.Generic;
using System.IO;
using TestCSharp.Misc;

namespace TestCSharp
{
	class MaterialCacheGeneratorTest
	{
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
			TestUtils.Assert(platformCount > 0, "Compiled material does not contain platform data.");

			var platforms = new HashSet<int>();
			for (int i = 0; i < platformCount; i++)
			{
				var platform = reader.ReadInt32();
				TestUtils.Assert(platforms.Add(platform), "Compiled material contains duplicated platform data.");
			}

			var originalDataSize = reader.ReadInt32();
			TestUtils.Assert(originalDataSize > 0, "Compiled material does not contain original material data.");
			stream.Seek(originalDataSize, SeekOrigin.Current);

			var chunkPlatforms = new HashSet<int>();
			while (stream.Position < stream.Length)
			{
				var platform = reader.ReadInt32();
				var chunkSize = reader.ReadInt32();
				var nextChunkPosition = stream.Position + chunkSize;
				TestUtils.Assert(platforms.Contains(platform), "Compiled material contains an undeclared platform chunk.");
				TestUtils.Assert(chunkPlatforms.Add(platform), "Compiled material contains duplicated platform chunks.");
				TestUtils.Assert(chunkSize >= sizeof(int) * 8, "Compiled material platform chunk is too small.");
				TestUtils.Assert(nextChunkPosition <= stream.Length, "Compiled material platform chunk exceeds file size.");

				var shaderSizes = new int[8];
				for (int i = 0; i < shaderSizes.Length; i++)
				{
					TestUtils.Assert(stream.Position + sizeof(int) <= nextChunkPosition, "Compiled material shader size exceeds chunk size.");
					shaderSizes[i] = reader.ReadInt32();
					TestUtils.Assert(shaderSizes[i] >= 0, "Compiled material shader size is invalid.");
					TestUtils.Assert(stream.Position + shaderSizes[i] <= nextChunkPosition, "Compiled material shader data exceeds chunk size.");
					stream.Seek(shaderSizes[i], SeekOrigin.Current);
				}

				TestUtils.Assert(shaderSizes[0] > 0, "Compiled material standard vertex shader is empty.");
				TestUtils.Assert(shaderSizes[1] > 0, "Compiled material standard pixel shader is empty.");
				TestUtils.Assert(shaderSizes[2] > 0, "Compiled material model vertex shader is empty.");
				TestUtils.Assert(shaderSizes[3] > 0, "Compiled material model pixel shader is empty.");
				TestUtils.Assert(stream.Position == nextChunkPosition, "Compiled material platform chunk size is inconsistent.");

				stream.Seek(nextChunkPosition, SeekOrigin.Begin);
			}

			TestUtils.Equals(platforms.Count, chunkPlatforms.Count, "Compiled material platform header and chunks are inconsistent.");
		}
	}
}
