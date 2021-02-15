using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.GUI;

namespace Effekseer.Plugin
{
	class ExportDefault
	{
		public static string UniqueName
		{
			get
			{
				return "Default";
			}
		}

		public static string Author
		{
			get
			{
				return "Default";
			}
		}

		public static string Title
		{
			get
			{
				return Resources.GetString("DefaultFormat");
			}
		}

		public static string Description
		{
			get
			{
				return Resources.GetString("ExportDefaultFormat");
			}
		}

		public static string Filter
		{
			get
			{
				return Resources.GetString("DefaultFormatExample");
			}
		}

		public static void Call(string path)
		{
			var window = new Magnification();
			window.Show(path);
		}

		class Magnification : Effekseer.GUI.IRemovableControl
		{
			string title = string.Empty;
			string message = string.Empty;
			string id = "###magnification";

			bool opened = true;

			bool isFirstUpdate = true;

			string path = string.Empty;
			float[] mag = new float[1];

			public bool ShouldBeRemoved { get; private set; }

			public Magnification()
			{
				ShouldBeRemoved = false;
			}

			public void Show(string path)
			{
				this.title = Resources.GetString("Maginification");

				this.path = path;
				mag[0] = Effekseer.Core.Option.ExternalMagnification.GetValue();
				GUIManager.AddControl(this);
			}

			public void Update()
			{
				if (isFirstUpdate)
				{
					GUIManager.NativeManager.OpenPopup(id);
					isFirstUpdate = false;
				}

				if (GUIManager.NativeManager.BeginPopupModal(title + id, ref opened, Effekseer.swig.WindowFlags.AlwaysAutoResize))
				{
					if (GUIManager.NativeManager.DragFloat(this.title, mag, 0.1f, 0, float.MaxValue))
					{

					}

					var save_text = MultiLanguageTextProvider.GetText("Save");
					var cancel_text = MultiLanguageTextProvider.GetText("Cancel");

					if (GUIManager.NativeManager.Button(save_text))
					{
						var binaryExporter = new Binary.Exporter();
						var binary = binaryExporter.Export(Core.Root, mag[0]);
						System.IO.File.WriteAllBytes(path, binary);
						Effekseer.Core.Option.ExternalMagnification.SetValue(mag[0]);
						ShouldBeRemoved = true;
					}

					if (GUIManager.NativeManager.Button(cancel_text))
					{
						ShouldBeRemoved = true;
					}

					GUIManager.NativeManager.EndPopup();
				}
				else
				{
					ShouldBeRemoved = true;
				}
			}
		}
	}
}
