using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

#if !DOTNET_STARNDARD
using System.Media;
#endif

namespace Effekseer.GUI.Component
{
	class PathForSound : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id3 = "";

		Data.Value.PathForSound binding = null;

		string filePath = string.Empty;
		string formatText1 = string.Empty;
		string formatText2 = string.Empty;
		bool isHovered = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.PathForSound Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;

				Read();
			}
		}

		public PathForSound()
		{
			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id3 = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.PathForSound;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void OnDisposed()
		{
#if !DOTNET_STARNDARD
			if (player != null)
			{
				player.Stop();
				player.Dispose();
				player = null;
			}
#endif
		}

		public override void OnDropped(string path, ref bool handle)
		{
			if(isHovered)
			{
				if (CheckExtension(path))
				{
					binding.SetAbsolutePath(path);
					Read();
				}

				handle = true;
			}
		}

		public override void Update()
		{
			isHovered = false;

			if (binding == null) return;

			string dd = null;

			float buttonSizeX = Manager.NativeManager.GetTextLineHeightWithSpacing() * 2;

			if(Manager.NativeManager.Button(Resources.GetString("Load") + id1, buttonSizeX))
			{
				btn_load_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateFileDst(DragAndDrops.FileType.Sound);

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			
			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(filePath);

			if (dd == null) dd = DragAndDrops.UpdateFileDst(DragAndDrops.FileType.Sound);

			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip(filePath);
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (filePath != string.Empty)
			{
				if (Manager.NativeManager.Button(Resources.GetString("Delete") + id2, buttonSizeX))
				{
					btn_delete_Click();
				}

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();

				Manager.NativeManager.SameLine();

				Manager.NativeManager.Text(formatText1);

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();

				if (Manager.NativeManager.Button(Resources.GetString("PlayString") + id3, buttonSizeX))
				{
					btn_play_Click();
				}

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();

				Manager.NativeManager.SameLine();

				Manager.NativeManager.Text(formatText2);

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			}

			if (dd != null)
			{
				Dropped(dd);
			}
		}

		public void Dropped(string path)
		{
			if (CheckExtension(path))
			{
				binding.SetAbsolutePath(path);
				Read();
			}
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

			var filter = Resources.GetString("SoundFilter");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;

				/*
				OpenFileDialog ofd = new OpenFileDialog();

				ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
				ofd.Filter = binding.Filter;
				ofd.FilterIndex = 2;
				ofd.Multiselect = false;

				if (ofd.ShowDialog() == DialogResult.OK)
				{
					var filepath = ofd.FileName;
					*/
				binding.SetAbsolutePath(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
			}
			else
			{
				return;
			}

			Read();
		}

		private void btn_delete_Click()
		{
#if !DOTNET_STARNDARD
			if (player != null)
			{
				player.Stop();
				player.Dispose();
				player = null;
			}
#endif
			binding.SetAbsolutePath("");
			Read();
		}

#if !DOTNET_STARNDARD
		SoundPlayer player = null;
#endif
		private void btn_play_Click()
		{
#if !DOTNET_STARNDARD
			if (player != null)
			{
				player.Stop();
				player.Dispose();
			}
			string path = binding.GetAbsolutePath();
			if (File.Exists(path))
			{
				player = new SoundPlayer(path);
				player.Play();
			}
#endif
		}

		void Read()
		{
			if (binding != null)
			{
				filePath = binding.GetRelativePath();
				if (filePath.Length > 0)
				{
					UpdateInfo();
				}
				else
				{
				}
			}
			else
			{
				filePath = string.Empty;
			}
		}

		void UpdateInfo()
		{
			string path = binding.GetAbsolutePath();

			try
			{
				var fs = new FileStream(path, FileMode.Open, FileAccess.Read);
				var reader = new BinaryReader(fs);

				if (reader.ReadUInt32() != 0x46464952)  // "RIFF"
					throw new Exception();
				reader.BaseStream.Seek(4, SeekOrigin.Current);
				if (reader.ReadUInt32() != 0x45564157)  // "WAVE"
					throw new Exception();

				ushort channels = 1;
				uint sampleRate = 44100;
				uint bytePerSec = sampleRate * 2;
				while (true)
				{
					uint chunk = reader.ReadUInt32();
					uint chunkSize = reader.ReadUInt32();

					if (chunk == 0x20746D66)        // "fmt "
					{
						reader.BaseStream.Seek(2, SeekOrigin.Current);
						channels = reader.ReadUInt16();
						sampleRate = reader.ReadUInt32();
						bytePerSec = reader.ReadUInt32();
						reader.BaseStream.Seek(-12, SeekOrigin.Current);
					}
					else if (chunk == 0x61746164)   // "data"
					{
						ulong time = (ulong)chunkSize * 1000 / bytePerSec;
						formatText1 = System.String.Format("[Format] {0}ch {1}Hz",
							channels, sampleRate);
						formatText2 = System.String.Format("[TotalTime] {0:00}:{1:00}.{2:00}",
							time / 1000 / 60, time / 1000 % 60, time % 1000 / 10);
						break;
					}
					reader.BaseStream.Seek(chunkSize, SeekOrigin.Current);
				}

				fs.Close();
				fs.Dispose();
			}
			catch (Exception)
			{
			}
		}

		private bool CheckExtension(string path)
		{
			var filters = binding.Filter.Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}
	}
}
