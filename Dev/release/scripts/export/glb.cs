
public class Script
{
	public static string UniqueName
	{
		get
		{
			return "glb";
		}
	}

	public static string Author
	{
		get
		{
			return "glb";
		}
	}

	public static string Title
	{
		get
		{
			if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "glb形式";
			return "glb format";
		}
	}

	public static string Description
	{
		get
		{
			if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "glb形式で出力する。";
			return "Export as glb format";
		}
	}

	public static string Filter
	{
		get
		{
			if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "glb形式 (*.glb)|*.glb";
			return "glb format (*.glb)|*.glb";
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

				var save_text = "Save";
				if (Effekseer.Core.Language == Effekseer.Language.Japanese) save_text = "保存";

				if (Effekseer.GUI.Manager.NativeManager.Button(save_text))
				{
					var option = new Effekseer.Exporter.glTFExporterOption();
					option.Scale = mag[0];
					option.Format = Effekseer.Exporter.glTFExporterFormat.glb;
					var exporter = new Effekseer.Exporter.glTFExporter();
					exporter.Export(path, option);
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
