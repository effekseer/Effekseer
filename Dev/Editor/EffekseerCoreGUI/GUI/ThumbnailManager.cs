using Effekseer.swig;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace Effekseer.GUI
{
	class Thumbnail
	{
		public swig.ReloadableImage Image = null;
	}

	class ThumbnailManager
	{
		class IOCallback : swig.IOCallback
		{
			public override void OnFileChanged(StaticFileType fileType, string path)
			{
				if (thumbnails.ContainsKey(path))
				{
					thumbnails[path].Image.Invalidate();
					thumbnails[path].Image.Validate();
				}
			}
		}

		static IOCallback callback = new IOCallback();
		static Dictionary<string, Thumbnail> thumbnails = new Dictionary<string, Thumbnail>();
		public static Thumbnail Load(string path)
		{
			if (!System.IO.Path.IsPathRooted(path))
				throw new Exception("Not root");

			if (thumbnails.ContainsKey(path))
			{
				return thumbnails[path];
			}

			ReloadableImage image = null;

			switch (Path.GetExtension(path).ToLower())
			{
				case ".efkproj":
				case ".efkefc":
					image = Images.Icons["FileEfkefc128"];
					break;
				case ".png":
					image = ReloadableImage.Create(Manager.HardwareDevice.GraphicsDevice, path);
					break;
				case ".wav":
					image = Images.Icons["FileSound128"];
					break;
				case ".efkmodel":
				case ".fbx":
				case ".mqo":
					image = Images.Icons["FileModel128"];
					break;
				case ".efkmat":
					image = Images.Icons["FileEfkmat128"];
					break;
				case ".efkcurve":
					image = Images.Icons["FileCurve128"];
					break;
			}

			if (image == null)
			{
				return null;
			}

			Thumbnail thumbnail = new Thumbnail();

			thumbnail.Image = image;

			thumbnails.Add(path, thumbnail);

			return thumbnail;
		}

		internal static void Initialize()
		{
			Manager.IO.AddCallback(callback);
		}

		internal static void Terminate()
		{
			foreach (var th in thumbnails)
			{
				th.Value.Image.Dispose();
			}

			thumbnails.Clear();
		}
	}
}