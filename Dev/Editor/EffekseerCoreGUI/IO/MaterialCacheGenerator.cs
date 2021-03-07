using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.IO
{
	public class MaterialCacheGenerator
	{
		public static bool GenerateMaterialCaches()
		{
			var exporter = new Binary.Exporter();
			exporter.Export(Core.Root);

			foreach(var path in exporter.Materials)
			{
				var materialPath = Utils.Misc.GetAbsolutePath(Core.Root.GetPath().GetAbsolutePath(), path);
				if(!GenerateMaterialCache(materialPath))
				{
					return false;
				}
			}

			return true;
		}
		
		public static bool GenerateMaterialCache(string absolutePath)
		{
			if (string.IsNullOrEmpty(absolutePath))
				return true;

			var generator = new swig.CompiledMaterialGenerator();

			string appDirectory = GUI.Manager.GetEntryDirectory();
			string fullPath = System.IO.Path.Combine(appDirectory, "tools/");

			generator.Initialize(fullPath);

			var ret = generator.Compile(CreateBinaryFilePath(absolutePath), absolutePath);

			generator.Dispose();

			return ret;
		}
		public static string CreateBinaryFilePath(string absolutePath)
		{
			return absolutePath + "d";
		}
	}
}

