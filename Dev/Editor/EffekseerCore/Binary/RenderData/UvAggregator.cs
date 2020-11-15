using System.Collections.Generic;
using Effekseer.Data.Value;
using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class UvAggregator
	{
		public float Width { get; }
		public float Height { get; }
		private readonly List<byte[]> _data = new List<byte[]>();

		public IEnumerable<byte[]> CurrentData => _data;

		public UvAggregator(float width, float height)
		{
			Width = width;
			Height = height;
		}

		public void AddInt(int value)
		{
			_data.Add(value.GetBytes());
		}

		public void AddFloat(float value)
		{
			_data.Add(value.GetBytes());
		}

		public void Add(byte[] value)
		{
			_data.Add(value);
		}

		public void AddVector2D(Vector2D vector)
		{
			AddFloat(vector.X / Width);
			AddFloat(vector.Y / Height);
		}

		public void AddRandomVector2D(Vector2DWithRandom vector)
		{
			AddFloat(vector.X.Max / Width);
			AddFloat(vector.Y.Max / Height);
			AddFloat(vector.X.Min / Width);
			AddFloat(vector.Y.Min / Height);
		}
	}
}
