using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class CommonValues
	{
		[Name(language = Language.Japanese, value = "生成数")]
		[Description(language = Language.Japanese, value = "インスタンスの生成数")]
		public Value.IntWithInifinite MaxGeneration
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "位置への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの位置への影響")]
		public Value.Enum<ParentEffectType> LocationEffectType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "回転への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの回転への影響")]
		public Value.Enum<ParentEffectType> RotationEffectType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "拡大への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの拡大への影響")]
		public Value.Enum<ParentEffectType> ScaleEffectType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "寿命により削除")]
		public Value.Boolean RemoveWhenLifeIsExtinct
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "親削除時削除")]
		public Value.Boolean RemoveWhenParentIsRemoved
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "子が全て消滅時削除")]
		public Value.Boolean RemoveWhenAllChildrenAreRemoved
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生存時間")]
		[Description(language = Language.Japanese, value = "1インスタンスが生存する時間")]
		public Value.IntWithRandom Life
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生成時間")]
		[Description(language = Language.Japanese, value = "1インスタンスを生成するのに必要とする時間")]
		public Value.Float GenerationTime
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生成開始時間")]
		[Description(language = Language.Japanese, value = "このノードのインスタンスが生成されてから生成を開始するまでの時間")]
		public Value.Float GenerationTimeOffset
		{
			get;
			private set;
		}

		internal CommonValues()
		{
			MaxGeneration = new Value.IntWithInifinite(1, false, int.MaxValue, 0);
			LocationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RotationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			ScaleEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RemoveWhenLifeIsExtinct = new Value.Boolean(true);
			RemoveWhenParentIsRemoved = new Value.Boolean(false);
			RemoveWhenAllChildrenAreRemoved = new Value.Boolean(false);
			Life = new Value.IntWithRandom(100, int.MaxValue, 1);
			GenerationTime = new Value.Float(1.0f, float.MaxValue, 0.00001f);
			GenerationTimeOffset = new Value.Float(0.0f, float.MaxValue, 0.0f);
		}
	}
}
