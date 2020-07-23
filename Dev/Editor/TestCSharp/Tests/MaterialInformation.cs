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
			TestMaterialFile("../../TestData/Effects/Materials/CustomData1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Fresnel.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/FresnelLike.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Img6.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Lighting1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Math1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Normal.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Parameter23.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/PolarCoords.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Refraction.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Refraction_Lighting.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Rotar.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/Sampler1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/UV1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/UV2.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/VertexColor.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials/WorldPositionOffset.efkmat");
		}

		static void TestMaterialFile(string path)
		{
			string fullpath = Path.Combine(Directory.GetCurrentDirectory(), path);
			var material = new Effekseer.Utl.MaterialInformation();

			if (!material.Load(fullpath))
			{
				TestUtils.Assert(false, "Failed to load material file: " + path);
			}
		}
	}
}
