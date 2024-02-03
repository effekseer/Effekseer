using Effekseer.Data.Value;
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

		[IO(Export = true)]
		public AdvancedRenderCommonValues AdvancedRendererCommonValuesValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public KillRulesValues KillRulesValues
		{
			get;
			private set;
		}

		[IO(Export = true)]
		public CollisionsValues CollisionsValues
		{
			get;
			private set;
		} = new CollisionsValues();
		
		[IO(Export = true)]
		public GpuParticlesValues GpuParticles
		{
			get;
			private set;
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		internal Node(NodeBase parent)
			: base(parent)
		{
			Path basepath = GetRoot().GetPath();

			Name.SetValueDirectly("Node");
			CommonValues = new Data.CommonValues();
			LocationValues = new Data.LocationValues(basepath);
			RotationValues = new Data.RotationValues();
			ScalingValues = new Data.ScaleValues();
			LocationAbsValues = new Data.LocationAbsValues();
			GenerationLocationValues = new Data.GenerationLocationValues(basepath);
			DepthValues = new DepthValues();
			RendererCommonValues = new Data.RendererCommonValues(basepath);
			DrawingValues = new RendererValues(basepath);
			SoundValues = new SoundValues(basepath);
			AdvancedRendererCommonValuesValues = new Data.AdvancedRenderCommonValues(basepath);
			KillRulesValues = new KillRulesValues();
			GpuParticles = new GpuParticlesValues(basepath);
		}
	}
}