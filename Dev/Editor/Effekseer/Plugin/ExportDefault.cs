using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
				Effekseer.GUI.Manager.AddControl(this);
			}

			public void Update()
			{
				if (isFirstUpdate)
				{
					Effekseer.GUI.Manager.NativeManager.OpenPopup(id);
					isFirstUpdate = false;
				}

				if (Effekseer.GUI.Manager.NativeManager.BeginPopupModal(title + id, ref opened, Effekseer.swig.WindowFlags.AlwaysAutoResize))
				{
					if (Effekseer.GUI.Manager.NativeManager.DragFloat(this.title, mag, 0.1f, 0, float.MaxValue))
					{

					}

					var save_text = "Save";
					if (Effekseer.Core.Language == Effekseer.Language.Japanese) save_text = "保存";

					if (Effekseer.GUI.Manager.NativeManager.Button(save_text))
					{
						var binaryExporter = new Binary.Exporter();
						var binary = binaryExporter.Export(mag[0]);
						System.IO.File.WriteAllBytes(path, binary);
						Effekseer.Core.Option.ExternalMagnification.SetValue(mag[0]);
						ShouldBeRemoved = true;
					}

					var cancel_text = "Cancel";
					if (Effekseer.Core.Language == Effekseer.Language.Japanese) cancel_text = "キャンセル";

					if (Effekseer.GUI.Manager.NativeManager.Button(cancel_text))
					{
						ShouldBeRemoved = true;
					}

					Effekseer.GUI.Manager.NativeManager.EndPopup();
				}
				else
				{
					ShouldBeRemoved = true;
				}
			}
		}
	}
}
