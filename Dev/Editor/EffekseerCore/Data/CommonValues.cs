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
		[Name(language = Language.English, value = "Spawn Count")]
		[Description(language = Language.English, value = "Number of instances to generate")]
		public Value.IntWithInifinite MaxGeneration
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "位置への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの位置への影響")]
		[Name(language = Language.English, value = "Inherit Position")]
		[Description(language = Language.English, value = "When this instance should copy its parent node's position")]
		public Value.Enum<ParentEffectType> LocationEffectType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "回転への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの回転への影響")]
		[Name(language = Language.English, value = "Inherit Rotation")]
		[Description(language = Language.English, value = "When this instance should copy its parent node's rotation")]
		public Value.Enum<ParentEffectType> RotationEffectType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "拡大への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの拡大への影響")]
		[Name(language = Language.English, value = "Inherit Scale")]
		[Description(language = Language.English, value = "When this instance should copy its parent node's scale")]
		public Value.Enum<ParentEffectType> ScaleEffectType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "寿命により削除")]
		[Name(language = Language.English, value = "Destroy after time")]
		public Value.Boolean RemoveWhenLifeIsExtinct
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "親削除時削除")]
		[Name(language = Language.English, value = "Destroy with parent")]
		public Value.Boolean RemoveWhenParentIsRemoved
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "子が全て消滅時削除")]
		[Name(language = Language.English, value = "Destroy when no\nmore children")]
		public Value.Boolean RemoveWhenAllChildrenAreRemoved
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生存時間")]
		[Description(language = Language.Japanese, value = "1インスタンスが生存する時間")]
		[Name(language = Language.English, value = "Time to live")]
		[Description(language = Language.English, value = "Length of time each instance survives")]
		public Value.IntWithRandom Life
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生成時間")]
		[Description(language = Language.Japanese, value = "1インスタンスを生成するのに必要とする時間")]
		[Name(language = Language.English, value = "Spawn Rate")]
		[Description(language = Language.English, value = "Time between each instance generation")]
		public Value.FloatWithRandom GenerationTime
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生成開始時間")]
		[Description(language = Language.Japanese, value = "このノードのインスタンスが生成されてから生成を開始するまでの時間")]
		[Name(language = Language.English, value = "Initial Delay")]
		[Description(language = Language.English, value = "Amount of time that must elapse after instance spawns before it starts generating.")]
		public Value.FloatWithRandom GenerationTimeOffset
		{
			get;
			private set;
		}

		internal CommonValues()
		{
			MaxGeneration = new Value.IntWithInifinite(1, false, int.MaxValue, 1);
			LocationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RotationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			ScaleEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RemoveWhenLifeIsExtinct = new Value.Boolean(true);
			RemoveWhenParentIsRemoved = new Value.Boolean(false);
			RemoveWhenAllChildrenAreRemoved = new Value.Boolean(false);
			Life = new Value.IntWithRandom(100, int.MaxValue, 1);
			GenerationTime = new Value.FloatWithRandom(1.0f, float.MaxValue, 0.00001f);
			GenerationTimeOffset = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);

			// dynamic parameter
			MaxGeneration.CanSelectDynamicEquation = true;
			Life.CanSelectDynamicEquation = true;
			GenerationTime.CanSelectDynamicEquation = true;
			GenerationTimeOffset.CanSelectDynamicEquation = true;
		}
	}
}
