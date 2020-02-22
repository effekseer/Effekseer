using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
    public class FCurveScalar
    {
        public Value.Enum<FCurveTimelineMode> Timeline = new Enum<FCurveTimelineMode>();
        public FCurve<float> S { get; private set; }

        public FCurveScalar(float MinValue = float.MinValue, float MaxValue = float.MaxValue)
        {
            Timeline = new Enum<FCurveTimelineMode>(FCurveTimelineMode.Percent);
            S = new FCurve<float>(0);
            S.DefaultValueRangeMin = MinValue;
            S.DefaultValueRangeMax = MaxValue;
        }

        public byte[] GetBytes(float mul = 1.0f)
        {
            List<byte[]> data = new List<byte[]>();
            data.Add(BitConverter.GetBytes((int)Timeline.Value));
            data.Add(S.GetBytes(mul));
            return data.SelectMany(_ => _).ToArray();
        }
    }
}
