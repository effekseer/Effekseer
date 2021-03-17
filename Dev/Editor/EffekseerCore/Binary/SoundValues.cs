using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class SoundValues
	{
		public static byte[] GetBytes(Data.SoundValues value, SortedDictionary<string, int> wave_and_index)
		{
			List<byte[]> data = new List<byte[]>();

			if (value == null)
			{
				data.Add(((int)(Data.SoundValues.ParamaterType.None)).GetBytes());
			}
			else
			{
				data.Add(value.Type.GetValueAsInt().GetBytes());
			}
			

			if (value == null)
			{ 
			
			}
            else if (value.Type.Value == Data.SoundValues.ParamaterType.None)
			{
				
			}
			else if (value.Type.Value == Data.SoundValues.ParamaterType.Use)
			{
				var sound_parameter = value.Sound;

                // ウェーブ番号
                if (sound_parameter.Wave.RelativePath != string.Empty)
                {
                    data.Add(wave_and_index[sound_parameter.Wave.RelativePath].GetBytes());
                }
                else
                {
                    data.Add((-1).GetBytes());
                }

                data.Add(sound_parameter.Volume.Max.GetBytes());
                data.Add(sound_parameter.Volume.Min.GetBytes());
                data.Add(sound_parameter.Pitch.Max.GetBytes());
                data.Add(sound_parameter.Pitch.Min.GetBytes());
                data.Add(sound_parameter.PanType.GetValueAsInt().GetBytes());
                data.Add(sound_parameter.Pan.Max.GetBytes());
                data.Add(sound_parameter.Pan.Min.GetBytes());
                data.Add(sound_parameter.Distance.GetValue().GetBytes());
                data.Add(sound_parameter.Delay.Max.GetBytes());
                data.Add(sound_parameter.Delay.Min.GetBytes());
			}

			return data.ToArray().ToArray();
		}
	}
}
