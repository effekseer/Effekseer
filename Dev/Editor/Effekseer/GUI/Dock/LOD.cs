using Effekseer.Data.Value;
using Effekseer.swig;
using Effekseer.Utils;

namespace Effekseer.GUI.Dock
{
	public class LOD : DockPanel
	{

		public static readonly int LevelCount = 4;
		public static readonly uint[] LevelColors = {
			GetColorU32Abgr(0xFF0BE36F),
			GetColorU32Abgr(0xFFC7E7FE),
			GetColorU32Abgr(0xFFE3C417),
			GetColorU32Abgr(0xFFE31F0B) };
		
		private bool isFirstUpdate = true;
		
		public LOD()
		{
			Label = Icons.PanelLOD + MultiLanguageTextProvider.GetText("LOD_Name") + "###LOD";
			DocPage = "levelsOfDetails.html";
		}

		static uint GetColorU32Abgr(uint argb){
			uint a = (argb >> 24) & 0xFF;
			uint r = (argb >> 16) & 0xFF;
			uint g = (argb >> 8) & 0xFF;
			uint b = (argb) & 0xFF;
			return (a << 24) | (b << 16) | (g << 8) | r;
		}
		
		private bool ShowLevel(int level, Data.Value.Boolean levelEnabled, Data.Value.Float levelDistance,
								float minDistance, float maxDistance)
		{
			Manager.NativeManager.PushStyleColor(ImGuiColFlags.Text, LevelColors[level]);
			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("LOD_Level") + " " + level);
			Manager.NativeManager.PopStyleColor();

			Manager.NativeManager.BeginDisabled(level == 0);
			
			Manager.NativeManager.NextColumn();
			bool[] value = { levelEnabled.Value };
			if (Manager.NativeManager.Checkbox("##level" + level, value))
			{
				levelEnabled.SetValue(value[0]);
			}

			if (levelEnabled.Value)
			{
				Manager.NativeManager.SameLine();
				float[] distance = { levelDistance.Value };

				
				Manager.NativeManager.DragFloat(MultiLanguageTextProvider.GetText("LOD_Level_Minimum_Distance") + "##distance" + level,
					distance, 1, minDistance, maxDistance, Core.Option.GetFloatFormat());
				levelDistance.SetValue(distance[0].Clipping(maxDistance, minDistance));
				
			}

			Manager.NativeManager.EndDisabled();
			
			return levelEnabled.Value;
		}

		protected override void UpdateInternal()
		{
			LevelParameter[] levels = new LevelParameter[]
			{
				new LevelParameter(0, Core.LodValues.Lod0Enabled, Core.LodValues.Distance0),
				new LevelParameter(1, Core.LodValues.Lod1Enabled, Core.LodValues.Distance1),
				new LevelParameter(2, Core.LodValues.Lod2Enabled, Core.LodValues.Distance2),
				new LevelParameter(3, Core.LodValues.Lod3Enabled, Core.LodValues.Distance3),
			};

			Manager.NativeManager.Columns(2);
			if (isFirstUpdate)
			{
				Manager.NativeManager.SetColumnWidth(0, 60 * Manager.GetUIScaleBasedOnFontSize());
				isFirstUpdate = false;
			}
			{
				bool showNextLevel = true;
				for (var i = 0; i < levels.Length && showNextLevel; i++)
				{
					var level = levels[i];

					float min = i <= 0 ? 0F : levels[i - 1].LevelDistance.Value;
					float max = i < levels.Length - 1 && levels[i + 1].IsEnabled.Value ? levels[i + 1].LevelDistance.Value : float.MaxValue ;
					
					showNextLevel = ShowLevel(level.LevelIndex, level.IsEnabled, level.LevelDistance, min, max);
					Manager.NativeManager.NextColumn();
				}
			}
			Manager.NativeManager.Columns(1);
		}

		struct LevelParameter
		{
			public readonly int LevelIndex;
			public readonly Data.Value.Boolean IsEnabled;
			public readonly Data.Value.Float LevelDistance;

			public LevelParameter(int levelIndex, Boolean isEnabled, Float levelDistance)
			{
				this.LevelIndex = levelIndex;
				IsEnabled = isEnabled;
				LevelDistance = levelDistance;
			}
		}
	}
}