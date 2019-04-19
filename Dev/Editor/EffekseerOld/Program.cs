using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace Effekseer
{
	class Program
	{
		/// <summary>
		/// 開始時のカレントディレクトリ
		/// </summary>
		public static string StartDirectory
		{
			get;
			private set;
		}

		[STAThread]
		static void Main(string[] args)
		{
			var resources = new System.Resources.ResourceManager("Effekseer.Properties.Resources", Assembly.GetExecutingAssembly());
			Effekseer.Resources.SetResourceManager(resources);

			StartDirectory = System.IO.Directory.GetCurrentDirectory();

			bool gui = true;
			string input = string.Empty;
			string output = string.Empty;
			string export = string.Empty;
			float magnification = 0.0f;

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
                else
                {
                    input = args[i];
                }
			}

			if (System.Diagnostics.Debugger.IsAttached)
			{
				Exec(gui, input, output, export, magnification);
			}
			else
			{
				try
				{
					Exec(gui, input, output, export, magnification);
				}
				catch (Exception e)
				{
					System.IO.File.WriteAllText("error.txt", e.ToString());
				}
			}
		}

		static void Exec(bool gui, string input, string output, string export, float magnification)
		{
			Core.OnOutputMessage += new Action<string>(Core_OnOutputMessage);
			Core.Initialize();

			if (gui)
			{
				GUI.GUIManager.Initialize();
			}

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

					var binaryExporter = new Binary.Exporter();
					var binary = binaryExporter.Export(magnification);
					System.IO.File.WriteAllBytes(export, binary);
				}
			}
			catch (Exception e)
			{
				System.Console.Error.WriteLine(e.Message);
			}

			if (gui)
			{
				while (GUI.GUIManager.Update())
				{

				}
			}

			Core.Dispose();
		}

		static void Core_OnOutputMessage(string obj)
		{
			MessageBox.Show(obj);
		}
	}
}