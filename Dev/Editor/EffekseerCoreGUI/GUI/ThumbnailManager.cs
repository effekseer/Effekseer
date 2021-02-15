using Effekseer.swig;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.GUI
{
	public class Thumbnail
	{
		public swig.ImageResource Image = null;
	}

	public class ThumbnailManager
	{
		class IOCallback : swig.IOCallback
		{
			public override void OnFileChanged(StaticFileType fileType, string path)
			{
				if(thumbnails.ContainsKey(path))
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

			if(thumbnails.ContainsKey(path))
			{
				return thumbnails[path];
			}

			var image = GUIManager.Native.LoadImageResource(path);
			if (image == null) return null;

			Thumbnail thumbnail = new Thumbnail();
			thumbnail.Image = image;

			thumbnails.Add(path, thumbnail);

			return thumbnail;
		}

		public static void Initialize()
		{
			GUIManager.IO.AddCallback(callback);
		}

		public static void Terminate()
		{
			foreach(var th in thumbnails)
			{
				th.Value.Image.Dispose();
			}

			thumbnails.Clear();
		}
	}
}
