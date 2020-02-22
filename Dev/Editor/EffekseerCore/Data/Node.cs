using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class Node : NodeBase
	{

		[IO(Export = true)]
		public CommonValues CommonValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public LocationValues LocationValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public RotationValues RotationValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public ScaleValues ScalingValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public LocationAbsValues LocationAbsValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public GenerationLocationValues GenerationLocationValues
		{
			get;
			private set;
		}

        [IO(Export = true)]
        public DepthValues DepthValues
        {
            get;
            private set;
        }

        [IO(Export = true)]
		public RendererCommonValues RendererCommonValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public RendererValues DrawingValues
		{
			get;
			private set;
		}

        [IO(Export = true)]
        public SoundValues SoundValues
        {
            get;
            private set;
        }

#if __EFFEKSEER_BUILD_VERSION16__
		[IO(Export = true)]
		public AlphaCrunchValues AlphaCrunchValues
		{
			get;
			private set;
		}
#endif

		/// <summary>
		/// コンストラクタ
		/// </summary>
		internal Node(NodeBase parent)
			:base(parent)
		{
			Name.SetValueDirectly("Node");
			CommonValues = new Data.CommonValues();
			LocationValues = new Data.LocationValues();
			RotationValues = new Data.RotationValues();
			ScalingValues = new Data.ScaleValues();
			LocationAbsValues = new Data.LocationAbsValues();
			GenerationLocationValues = new Data.GenerationLocationValues();
            DepthValues = new DepthValues();
			RendererCommonValues = new Data.RendererCommonValues();
            DrawingValues = new RendererValues();
            SoundValues = new SoundValues();
#if __EFFEKSEER_BUILD_VERSION16__
			AlphaCrunchValues = new Data.AlphaCrunchValues();
#endif
		}
	}
}
