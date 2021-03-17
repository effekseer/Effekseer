using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class LocationAbsValues
	{
		public static byte[] GetBytes(Data.LocationAbsValues value, Data.ParentEffectType parentEffectType, ExporterVersion version)
		{
			List<byte[]> data = new List<byte[]>();

			// Force field
			List<Data.LocalForceField> lffs = new List<Data.LocalForceField>
			{
				value.LocalForceField1,
				value.LocalForceField2,
				value.LocalForceField3,
				value.LocalForceField4,
			};

			data.Add((lffs.Count).GetBytes());

			foreach(var lff in lffs)
			{
				if(version >= ExporterVersion.Ver16Alpha1)
				{
					data.Add(lff.Type.GetValueAsInt().GetBytes());

					var pi = (float)Math.PI;

					data.Add(BitConverter.GetBytes(lff.Power));
					data.Add(BitConverter.GetBytes(lff.Position.X));
					data.Add(BitConverter.GetBytes(lff.Position.Y));
					data.Add(BitConverter.GetBytes(lff.Position.Z));
					data.Add(BitConverter.GetBytes(lff.Rotation.X / 180.0f * pi));
					data.Add(BitConverter.GetBytes(lff.Rotation.Y / 180.0f * pi));
					data.Add(BitConverter.GetBytes(lff.Rotation.Z / 180.0f * pi));

					if (lff.Type.Value == Data.LocalForceFieldType.Force)
					{
						data.Add((lff.Force.Gravitation.GetValue() ? 1 : 0).GetBytes());
					}

					if (lff.Type.Value == Data.LocalForceFieldType.Wind)
					{
					}

					if (lff.Type.Value == Data.LocalForceFieldType.Vortex)
					{
						var ftype = lff.Vortex.VortexType.Value;
						data.Add(((int)ftype).GetBytes());
					}

					if (lff.Type.Value == Data.LocalForceFieldType.Turbulence)
					{
						var ftype = lff.Turbulence.TurbulenceType.Value;
						data.Add(((int)ftype).GetBytes());
						data.Add(lff.Turbulence.Seed.Value.GetBytes());
						data.Add(lff.Turbulence.FieldScale.Value.GetBytes());
						data.Add(lff.Turbulence.Octave.Value.GetBytes());
					}

					if (lff.Type.Value == Data.LocalForceFieldType.Drag)
					{
					}

					if (lff.Type.Value == Data.LocalForceFieldType.Gravity)
					{
						data.Add((byte[])lff.Gravity.Gravity);
					}

					if (lff.Type.Value == Data.LocalForceFieldType.AttractiveForce)
					{
						//data.Add(BitConverter.GetBytes(lff.AttractiveForce.Force.GetValue()));
						data.Add(BitConverter.GetBytes(lff.AttractiveForce.Control.GetValue()));
						data.Add(BitConverter.GetBytes(lff.AttractiveForce.MinRange.GetValue()));
						data.Add(BitConverter.GetBytes(lff.AttractiveForce.MaxRange.GetValue()));
					}

					data.Add(lff.Falloff.Type.GetValueAsInt().GetBytes());

					if(lff.Falloff.Type.Value != Data.ForceFieldFalloffType.None)
					{
						data.Add(lff.Falloff.Power.GetBytes());
						data.Add(lff.Falloff.MaxDistance.GetBytes());
						data.Add(lff.Falloff.MinDistance.GetBytes());

						if(lff.Falloff.Type.Value == Data.ForceFieldFalloffType.Tube)
						{
							data.Add(lff.Falloff.Tube.RadiusPower.GetBytes());
							data.Add(lff.Falloff.Tube.MaxRadius.GetBytes());
							data.Add(lff.Falloff.Tube.MinRadius.GetBytes());
						}

						if (lff.Falloff.Type.Value == Data.ForceFieldFalloffType.Cone)
						{
							data.Add(lff.Falloff.Cone.AnglePower.GetBytes());
							data.Add(lff.Falloff.Cone.MaxAngle.GetBytes(1.0f / 180.0f * pi));
							data.Add(lff.Falloff.Cone.MinAngle.GetBytes(1.0f / 180.0f * pi));
						}
					}
				}
				else
				{
					// 1.5 or later
					if(lff.Type.Value == Data.LocalForceFieldType.Turbulence)
					{
						data.Add(lff.Type.GetValueAsInt().GetBytes());
					}
					else
					{
						data.Add(((int)(Data.LocalForceFieldType.None)).GetBytes());
					}

					if (lff.Type.Value == Data.LocalForceFieldType.Turbulence)
					{
						var strength = lff.Power.Value / 10.0f;

						data.Add(lff.Turbulence.Seed.Value.GetBytes());
						data.Add(lff.Turbulence.FieldScale.Value.GetBytes());
						data.Add(strength.GetBytes());
						data.Add(lff.Turbulence.Octave.Value.GetBytes());
					}
				}
			}

			// For compatibility
			if(version < ExporterVersion.Ver16Alpha2)
			{
				var type = 0;
				data.Add(((int)type).GetBytes());
				data.Add(((int)0).GetBytes());
			}

			return data.ToArray().ToArray();
		}
	}
}
