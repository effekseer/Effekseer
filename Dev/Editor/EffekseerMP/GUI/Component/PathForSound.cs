using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Media;
using System.IO;
using System.Text.RegularExpressions;

namespace Effekseer.GUI.Component
{
	class PathForSound : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

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

		public PathForSound(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
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
			if (player != null)
			{
				player.Stop();
				player.Dispose();
				player = null;
			}
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
		
			if(Manager.NativeManager.Button("読込"))
			{
				btn_load_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			
			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(filePath);

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (Manager.NativeManager.Button("解除"))
			{
				btn_delete_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(formatText1);

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (Manager.NativeManager.Button("再生"))
			{
				btn_play_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(formatText2);

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();
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
			if (player != null)
			{
				player.Stop();
				player.Dispose();
				player = null;
			}
			binding.SetAbsolutePath("");
			Read();
		}

		SoundPlayer player = null;

		private void btn_play_Click()
		{
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
			catch (Exception e)
			{
			}
		}

		private bool CheckExtension(string path)
		{
			Match match = Regex.Match(binding.Filter, "\\*(\\.[a-zA-Z0-9]*)");
			string extension = match.Value.Substring(1);
			return System.IO.Path.GetExtension(path) == extension;
		}
	}
}
