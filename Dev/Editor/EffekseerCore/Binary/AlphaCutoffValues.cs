using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
    class AlphaCutoffValues
    {
        public static byte[] GetBytes(Data.AlphaCutoffParameter value, ExporterVersion version)
        {
            List<byte[]> data = new List<byte[]>();
            data.Add(value.Type.GetValueAsInt().GetBytes());

            if (value.Type == Data.AlphaCutoffParameter.ParameterType.Fixed)
            {
                var refBuf = value.Fixed.Threshold.DynamicEquation.Index.GetBytes();
                var mainBuf = value.Fixed.Threshold.GetBytes();
                data.Add( (mainBuf.Count() + refBuf.Count()).GetBytes() );
                data.Add(refBuf);
                data.Add(mainBuf);
            }
            else if (value.Type == Data.AlphaCutoffParameter.ParameterType.FourPointInterpolation)
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
            else if (value.Type == Data.AlphaCutoffParameter.ParameterType.Easing)
            {
				Utils.ExportEasing(value.Easing, 1.0f, data, version);
			}
            else if (value.Type == Data.AlphaCutoffParameter.ParameterType.FCurve)
            {
                var _data = value.FCurve.GetBytes();
                data.Add(_data.Count().GetBytes());
                data.Add(_data);
            }

			data.Add(value.EdgeParam.EdgeThreshold.GetBytes());
			data.Add((byte[])value.EdgeParam.EdgeColor);
			data.Add(BitConverter.GetBytes(value.EdgeParam.EdgeColorScaling));

            return data.ToArray().ToArray();
        }
    }
}
