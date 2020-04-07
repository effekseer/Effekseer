
public class Script
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
			if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "標準形式";
			return "Default format";
		}
	}

	public static string Description
	{
		get
		{
			if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "標準形式で出力する。";
			return "Export as default format";
		}
	}

	public static string Filter
	{
		get
		{
			if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "標準形式 (*.efk)|*.efk";
			return "Default format (*.efk)|*.efk";
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
			if (Effekseer.Core.Language == Effekseer.Language.Japanese)
			{
				this.title = "拡大率";
			}
			else
			{
				this.title = "Maginification";
			}

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

				var save_text = Effekseer.MultiLanguageTextProvider.GetText("Save");
				var cancel_text = Effekseer.MultiLanguageTextProvider.GetText("Cancel");

				if (Effekseer.GUI.Manager.NativeManager.Button(save_text))
				{
                    var exporter = new Effekseer.Binary.Exporter();
                    var binary = exporter.Export(mag[0]);
					System.IO.File.WriteAllBytes(path, binary);
					Effekseer.Core.Option.ExternalMagnification.SetValue(mag[0]);
					ShouldBeRemoved = true;
				}

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
