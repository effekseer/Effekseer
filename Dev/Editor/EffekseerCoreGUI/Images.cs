
using System.Collections.Generic;
using System.IO;

namespace Effekseer.GUI
{
	public class Images
	{
		public static swig.ReloadableImage Play;
		public static swig.ReloadableImage Stop;
		public static swig.ReloadableImage Pause;
		public static swig.ReloadableImage Step;
		public static swig.ReloadableImage BackStep;

		public static Dictionary<string, swig.ReloadableImage> Icons = new Dictionary<string, swig.ReloadableImage>();

		public static Dictionary<string, swig.ReloadableImage> tempImages = new Dictionary<string, swig.ReloadableImage>();

		public static swig.ReloadableImage Load(swig.GraphicsDevice graphicsDevice, string path, bool isRequiredToReload = false)
		{
			if (tempImages.ContainsKey(path) && !isRequiredToReload)
			{
				return tempImages[path];
			}
			else
			{
				if (tempImages.ContainsKey(path))
				{
					tempImages[path].Invalidate();
					tempImages[path].Validate();
					return tempImages[path];
				}

				var img = swig.ReloadableImage.Create(graphicsDevice, path);
				if (img != null)
				{
					tempImages.Add(path, img);
				}
				return img;
			}
		}

		public static swig.ReloadableImage LoadAppResource(swig.GraphicsDevice graphicsDevice, string path)
		{
			string appDirectory = GUI.Manager.GetEntryDirectory();
			string fullPath = Path.Combine(appDirectory, path);
			return swig.ReloadableImage.Create(graphicsDevice, fullPath);
		}

		public static void Load(swig.GraphicsDevice graphicsDevice)
		{
			Play = LoadAppResource(graphicsDevice, "resources/Play.png");
			Stop = LoadAppResource(graphicsDevice, "resources/Stop.png");
			Pause = LoadAppResource(graphicsDevice, "resources/Pause.png");
			Step = LoadAppResource(graphicsDevice, "resources/Step.png");
			BackStep = LoadAppResource(graphicsDevice, "resources/BackStep.png");

			Icons["Copy"] = LoadAppResource(graphicsDevice, "resources/icons/Copy.png");
			Icons["Paste"] = LoadAppResource(graphicsDevice, "resources/icons/Paste.png");

			Icons["AppIcon"] = LoadAppResource(graphicsDevice, "resources/icon.png");

			Icons["VisibleShow"] = LoadAppResource(graphicsDevice, "resources/icons/Visible_Show.png");
			Icons["VisibleHide"] = LoadAppResource(graphicsDevice, "resources/icons/Visible_Hide.png");

			Icons["EnlargeAnchor"] = LoadAppResource(graphicsDevice, "resources/icons/EnlargeAnchor.png");
			Icons["ShrinkAnchor"] = LoadAppResource(graphicsDevice, "resources/icons/ShrinkAnchor.png");

			Icons["AutoZoom_On"] = LoadAppResource(graphicsDevice, "resources/icons/AutoZoom_On.png");
			Icons["AutoZoom_Off"] = LoadAppResource(graphicsDevice, "resources/icons/AutoZoom_Off.png");

			Icons["FileViewer_Directory"] = LoadAppResource(graphicsDevice, "resources/icons/FileViewer_Directory.png");
			Icons["FileViewer_EffekseerProj"] = LoadAppResource(graphicsDevice, "resources/icons/FileViewer_EffekseerProj.png");

			Icons["ButtonMin"] = LoadAppResource(graphicsDevice, "resources/icons/Button_Min.png");
			Icons["ButtonMax"] = LoadAppResource(graphicsDevice, "resources/icons/Button_Max.png");
			Icons["ButtonMaxCancel"] = LoadAppResource(graphicsDevice, "resources/icons/Button_MaxCancel.png");
			Icons["ButtonClose"] = LoadAppResource(graphicsDevice, "resources/icons/Button_Close.png");
		}

		public static void Unload()
		{
			Play.Dispose();
			Stop.Dispose();
			Pause.Dispose();
			Step.Dispose();
			BackStep.Dispose();
			foreach (var keyValue in Icons)
			{
				if (keyValue.Value != null)
				{
					keyValue.Value.Dispose();
				}
			}

			foreach (var keyValue in tempImages)
			{
				if (keyValue.Value != null)
				{
					keyValue.Value.Dispose();
				}
			}
		}

		public static swig.ReloadableImage GetIcon(string resourceName)
		{
			return Icons.ContainsKey(resourceName) ? Icons[resourceName] : null;
		}
	}
}
