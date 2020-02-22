using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
    class AlphaCrunchValues
    {
        public static byte[] GetBytes(Data.AlphaCrunchValues value)
        {
            List<byte[]> data = new List<byte[]>();
            data.Add(value.Type.GetValueAsInt().GetBytes());

            if (value.Type == Data.AlphaCrunchValues.ParameterType.Fixed)
            {
                var refBuf = value.Fixed.Threshold.DynamicEquation.Index.GetBytes();
                var mainBuf = value.Fixed.Threshold.GetBytes();
                data.Add( (mainBuf.Count() + refBuf.Count()).GetBytes() );
                data.Add(refBuf);
                data.Add(mainBuf);
            }
            else if (value.Type == Data.AlphaCrunchValues.ParameterType.FourPointInterpolation)
            {
                List<byte[]> _data = new List<byte[]>();
                _data.Add(value.FourPointInterpolation.BeginThreshold.Max.GetBytes());
                _data.Add(value.FourPointInterpolation.BeginThreshold.Min.GetBytes());
                _data.Add(value.FourPointInterpolation.TransitionFrameNum.Max.GetBytes());
                _data.Add(value.FourPointInterpolation.TransitionFrameNum.Min.GetBytes());
                _data.Add(value.FourPointInterpolation.No2Threshold.Max.GetBytes());
                _data.Add(value.FourPointInterpolation.No2Threshold.Min.GetBytes());
                _data.Add(value.FourPointInterpolation.No3Threshold.Max.GetBytes());
                _data.Add(value.FourPointInterpolation.No3Threshold.Min.GetBytes());
                _data.Add(value.FourPointInterpolation.TransitionFrameNum2.Max.GetBytes());
                _data.Add(value.FourPointInterpolation.TransitionFrameNum2.Min.GetBytes());
                _data.Add(value.FourPointInterpolation.EndThreshold.Max.GetBytes());
                _data.Add(value.FourPointInterpolation.EndThreshold.Min.GetBytes());

                var __data = _data.ToArray().ToArray();

                data.Add(__data.Count().GetBytes());
                data.Add(__data);
            }
            else if (value.Type == Data.AlphaCrunchValues.ParameterType.Easing)
            {
                var easing = Utl.MathUtl.Easing((float)value.Easing.StartSpeed.Value, (float)value.Easing.EndSpeed.Value);

                var refBuf1_1 = value.Easing.Start.DynamicEquationMax.Index.GetBytes();
                var refBuf1_2 = value.Easing.Start.DynamicEquationMin.Index.GetBytes();
                var refBuf2_1 = value.Easing.End.DynamicEquationMax.Index.GetBytes();
                var refBuf2_2 = value.Easing.End.DynamicEquationMin.Index.GetBytes();

                List<byte[]> _data = new List<byte[]>();
                _data.Add(refBuf1_1);
                _data.Add(refBuf1_2);
                _data.Add(refBuf2_1);
                _data.Add(refBuf2_2);
                _data.Add(value.Easing.Start.Max.GetBytes());
                _data.Add(value.Easing.Start.Min.GetBytes());
                _data.Add(value.Easing.End.Max.GetBytes());
                _data.Add(value.Easing.End.Min.GetBytes());
                _data.Add(BitConverter.GetBytes(easing[0]));
                _data.Add(BitConverter.GetBytes(easing[1]));
                _data.Add(BitConverter.GetBytes(easing[2]));
                var __data = _data.ToArray().ToArray();

                data.Add(__data.Count().GetBytes());
                data.Add(__data);
            }
            else if (value.Type == Data.AlphaCrunchValues.ParameterType.FCurve)
            {
                var _data = value.FCurve.GetBytes();
                data.Add(_data.Count().GetBytes());
                data.Add(_data);
            }

            return data.ToArray().ToArray();
        }
    }
}
