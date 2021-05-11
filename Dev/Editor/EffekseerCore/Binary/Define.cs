using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;
using Effekseer.Data;

namespace Effekseer.Binary
{
	enum NodeType : int
	{
		Root = -1,
		None = 0,
		Sprite = 2,
		Ribbon = 3,
		Ring = 4,
		Model = 5,
		Track = 6,
	};

	[StructLayout(LayoutKind.Sequential)]
	struct Vector3D
	{
		float X;
		float Y;
		float Z;

		public Vector3D(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}
	}

	class Utils
	{
		public static string GetModelPath(Data.Value.Path path)
		{
			var relativePath = path.RelativePath;
			var ext = System.IO.Path.GetExtension(relativePath);

			if (string.IsNullOrEmpty(ext))
			{
				return relativePath;
			}

			if (string.IsNullOrEmpty(System.IO.Path.GetDirectoryName(relativePath)))
			{
				relativePath = System.IO.Path.GetFileNameWithoutExtension(relativePath) + ".efkmodel";
			}
			else
			{
				relativePath = System.IO.Path.ChangeExtension(relativePath, ".efkmodel");
			}

			return relativePath;
		}

		public static void ExportEasing(FloatEasingParamater easingValue, float magnification, List<byte[]> destination, ExporterVersion version, bool exportSize)
		{
			var refBuf1_1 = easingValue.Start.DynamicEquationMax.Index.GetBytes();
			var refBuf1_2 = easingValue.Start.DynamicEquationMin.Index.GetBytes();
			var refBuf2_1 = easingValue.End.DynamicEquationMax.Index.GetBytes();
			var refBuf2_2 = easingValue.End.DynamicEquationMin.Index.GetBytes();

			List<byte[]> _data = new List<byte[]>();

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				_data.Add(refBuf1_1);
				_data.Add(refBuf1_2);
				_data.Add(refBuf2_1);
				_data.Add(refBuf2_2);
				_data.Add(easingValue.Start.GetBytes(magnification));
				_data.Add(easingValue.End.GetBytes(magnification));
			}

			// middle
			if (version >= ExporterVersion.Ver16Alpha1)
			{
				int isMiddleEnabled = easingValue.IsMiddleEnabled ? 1 : 0;
				_data.Add(isMiddleEnabled.GetBytes());
				if (easingValue.IsMiddleEnabled.Value)
				{
					var refBuf3_1 = easingValue.Middle.DynamicEquationMax.Index.GetBytes();
					var refBuf3_2 = easingValue.Middle.DynamicEquationMin.Index.GetBytes();
					_data.Add(refBuf3_1);
					_data.Add(refBuf3_2);
					_data.Add(easingValue.Middle.GetBytes(magnification));
				}
			}

			var type = easingValue.Type.Value;

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				_data.Add(BitConverter.GetBytes((int)type));

				if (type == EasingType.LeftRightSpeed)
				{
					var easing = Utl.MathUtl.Easing((float)easingValue.StartSpeed.Value, (float)easingValue.EndSpeed.Value);
					_data.Add(BitConverter.GetBytes(easing[0]));
					_data.Add(BitConverter.GetBytes(easing[1]));
					_data.Add(BitConverter.GetBytes(easing[2]));
				}
			}
			else
			{
				var easing = Utl.MathUtl.Easing((float)easingValue.StartSpeed.Value, (float)easingValue.EndSpeed.Value);
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
			}

			// Channel
			if (version >= ExporterVersion.Ver16Alpha1)
			{
				{
					int channel = 0;
					channel += (byte)0 << 0;
					_data.Add(BitConverter.GetBytes(channel));
				}
			}

			// Individual
			if (version >= ExporterVersion.Ver16Alpha1)
			{
				int individualTypeEnabled = 0;
				_data.Add(BitConverter.GetBytes(individualTypeEnabled));
			}


			var __data = _data.ToArray().ToArray();
			if (exportSize)
			{
				destination.Add(__data.Count().GetBytes());
			}
			destination.Add(__data);
		}

		public static void ExportEasing(Vector3DEasingParamater easingValue, float magnification, List<byte[]> destination, ExporterVersion version)
		{

			var refBuf1_1 = easingValue.Start.DynamicEquationMax.Index.GetBytes();
			var refBuf1_2 = easingValue.Start.DynamicEquationMin.Index.GetBytes();
			var refBuf2_1 = easingValue.End.DynamicEquationMax.Index.GetBytes();
			var refBuf2_2 = easingValue.End.DynamicEquationMin.Index.GetBytes();


			List<byte[]> _data = new List<byte[]>();
			_data.Add(refBuf1_1);
			_data.Add(refBuf1_2);
			_data.Add(refBuf2_1);
			_data.Add(refBuf2_2);
			_data.Add(easingValue.Start.GetBytes(magnification));
			_data.Add(easingValue.End.GetBytes(magnification));

			// middle
			if (version >= ExporterVersion.Ver16Alpha1)
			{
				int isMiddleEnabled = easingValue.IsMiddleEnabled.Value ? 1 : 0;
				_data.Add(isMiddleEnabled.GetBytes());
				if (easingValue.IsMiddleEnabled.Value)
				{
					var refBuf3_1 = easingValue.Middle.DynamicEquationMax.Index.GetBytes();
					var refBuf3_2 = easingValue.Middle.DynamicEquationMin.Index.GetBytes();
					_data.Add(refBuf3_1);
					_data.Add(refBuf3_2);
					_data.Add(easingValue.Middle.GetBytes(magnification));
				}
			}

			var type = easingValue.Type.Value;

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				_data.Add(BitConverter.GetBytes((int)type));

				if (type == EasingType.LeftRightSpeed)
				{
					var easing = Utl.MathUtl.Easing((float)easingValue.StartSpeed.Value, (float)easingValue.EndSpeed.Value);
					_data.Add(BitConverter.GetBytes(easing[0]));
					_data.Add(BitConverter.GetBytes(easing[1]));
					_data.Add(BitConverter.GetBytes(easing[2]));
				}
			}
			else
			{
				var easing = Utl.MathUtl.Easing((float)easingValue.StartSpeed.Value, (float)easingValue.EndSpeed.Value);
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
			}

			// Channel
			if (version >= ExporterVersion.Ver16Alpha1)
			{
				if (easingValue.IsRandomGroupEnabled.Value)
				{
					Dictionary<int, int> id2ind = new Dictionary<int, int>();

					var ids = new[] { easingValue.RandomGroupX.Value, easingValue.RandomGroupY.Value, easingValue.RandomGroupZ.Value };

					foreach (var id in ids)
					{
						if (!id2ind.ContainsKey(id))
						{
							id2ind.Add(id, id2ind.Count);
						}
					}

					int channel = 0;
					channel += (byte)id2ind[easingValue.RandomGroupX.Value] << 0;
					channel += (byte)id2ind[easingValue.RandomGroupY.Value] << 8;
					channel += (byte)id2ind[easingValue.RandomGroupZ.Value] << 16;
					_data.Add(BitConverter.GetBytes(channel));
				}
				else
				{
					int channel = 0;
					channel += (byte)0 << 0;
					channel += (byte)1 << 8;
					channel += (byte)2 << 16;
					_data.Add(BitConverter.GetBytes(channel));
				}
			}

			// Individual
			if (version >= ExporterVersion.Ver16Alpha1)
			{
				int individualTypeEnabled = easingValue.IsIndividualTypeEnabled.Value ? 1 : 0;
				_data.Add(BitConverter.GetBytes(individualTypeEnabled));
				if (easingValue.IsIndividualTypeEnabled.Value)
				{
					_data.Add(BitConverter.GetBytes((int)easingValue.TypeX.Value));
					_data.Add(BitConverter.GetBytes((int)easingValue.TypeY.Value));
					_data.Add(BitConverter.GetBytes((int)easingValue.TypeZ.Value));
				}
			}

			var __data = _data.ToArray().ToArray();
			destination.Add(__data.Count().GetBytes());
			destination.Add(__data);

		}
	}
}
