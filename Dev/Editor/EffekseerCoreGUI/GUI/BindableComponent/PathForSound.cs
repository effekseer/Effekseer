using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace Effekseer.GUI.BindableComponent
{
	class PathForSound : PathBase
	{
		string formatText = string.Empty;

		public PathForSound()
		{
			fileType = FileType.Sound;
			filter = MultiLanguageTextProvider.GetText("SoundFilter");
		}

		protected override void UpdateSubParts(float width)
		{
			Manager.NativeManager.SetNextItemWidth(width);
			Manager.NativeManager.InputText(id2, formatText, swig.InputTextFlags.ReadOnly);
		}

		protected override void UpdateInfo()
		{
			base.UpdateInfo();

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
						formatText = System.String.Format("{0}ch, {1}Hz, {2:00}:{3:00}.{4:00}",
							channels, sampleRate, time / 1000 / 60, time / 1000 % 60, time % 1000 / 10);
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
	}
}