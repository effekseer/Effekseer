using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Runtime.ExceptionServices;
using EfkN = Effekseer.swig.EffekseerNative;
using Effekseer.GUI;

namespace Effekseer
{
	class Program
	{
		[STAThread]
		[HandleProcessCorruptedStateExceptions]
		static void Main(string[] args)
		{
			// HACK code for mac
			// To load libMonoPosix at first
			try
			{
				var dummy = Effekseer.Utils.Zlib.Decompress(new byte[] { 1, 2, 3, 4 });
			}
			catch(Exception e)
			{
				Application.ExportError(e);
				return;
			}

			bool gui = true;
			string input = string.Empty;
			string output = string.Empty;
			string format = "efk";
			string export = string.Empty;
			float magnification = 0.0f;
			bool materialCache = false;

			for (int i = 0; i < args.Length; i++)
			{
				if (args[i] == "-cui")
				{
					gui = false;
				}
				else if (args[i] == "-in")
				{
					i++;
					if (i < args.Length)
					{
						input = args[i];
					}
				}
				else if (args[i] == "-o")
				{
					i++;
					if (i < args.Length)
					{
						output = args[i];
					}
				}
				else if (args[i] == "-f")
				{
					i++;
					if (i < args.Length)
					{
						format = args[i];
					}
				}
				else if (args[i] == "-e")
				{
					i++;
					if (i < args.Length)
					{
						export = args[i];
					}
				}
				else if (args[i] == "-m")
				{
					i++;
					if (i < args.Length)
					{
						float.TryParse(args[i], out magnification);
					}
				}
				else if (args[i] == "--materialcache")
				{
					materialCache = true;
				}
				else
				{
					input = args[i];
				}
			}

			if (System.Diagnostics.Debugger.IsAttached)
			{
				Exec(gui, input, output, export, format, magnification, materialCache);
			}
			else
			{
				try
				{
					Exec(gui, input, output, export, format, magnification, materialCache);
				}
				catch (Exception e)
				{
					Application.ExportError(e);
				}
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="gui"></param>
		/// <param name="input">CLI 用</param>
		/// <param name="output">CLI 用</param>
		/// <param name="export">CLI 用</param>
		/// <param name="format">CLI 用</param>
		/// <param name="magnification">CLI 用</param>
		/// <param name="materialCache">CLI 用</param>
		static void Exec(bool gui, string input, string output, string export, string format, float magnification, bool materialCache)
		{
			var app = new App();
			app.Initialize(gui);


			try
			{
				if (input != string.Empty)
				{
					Core.LoadFrom(input);
				}

				if (output != string.Empty)
				{
					Core.SaveTo(output);
				}

				if (export != string.Empty)
				{
					if (magnification == 0.0f)
					{
						magnification = Core.Option.Magnification;
					}

					if(format == "gltf")
					{
						var option = new Effekseer.Exporter.glTFExporterOption();
						option.Scale = magnification;
						var exporter = new Effekseer.Exporter.glTFExporter();
						exporter.Export(export, option);
					}
					else if (format == "glb")
					{
						var option = new Effekseer.Exporter.glTFExporterOption();
						option.Scale = magnification;
						option.Format = Exporter.glTFExporterFormat.glb;
						var exporter = new Effekseer.Exporter.glTFExporter();
						exporter.Export(export, option);
					}
					else
					{
						var binaryExporter = new Binary.Exporter();
						var binary = binaryExporter.Export(Core.Root, magnification);
						System.IO.File.WriteAllBytes(export, binary);
					}
				}
			}
			catch (Exception e)
			{
				System.Console.Error.WriteLine(e.Message);
			}

			try
			{
				if (materialCache)
				{
					IO.MaterialCacheGenerator.GenerateMaterialCaches();
				}
			}
			catch (Exception e)
			{
				System.Console.Error.WriteLine(e.Message);
			}

			if (gui)
			{
				app.Run();
			}

			Core.Dispose();
		}

		private static void ChangeLanguage()
		{
			MultiLanguageTextProvider.RootDirectory = Application.EntryDirectory + "/";
			MultiLanguageTextProvider.Reset();
			MultiLanguageTextProvider.LoadCSV("Base.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Options.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_BasicSettings.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Position.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Rotation.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Scale.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_BasicRenderSettings.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_RenderSettings.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_LocalForceField.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_AdvancedRenderCommon.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Environment.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_ProcedualModel.csv");

			Manager.UpdateFont();
		}
	}
}
