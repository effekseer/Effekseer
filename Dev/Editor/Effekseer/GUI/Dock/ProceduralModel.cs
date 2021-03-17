using System;
using Effekseer.Data;

namespace Effekseer.GUI.Dock
{
	class ProceduralModel : DockPanel
	{
		readonly Component.ParameterList paramerterList = null;
		Component.CopyAndPaste candp = null;

		public ProceduralModel()
		{
			Label = Icons.PanelProceduralModel + Resources.GetString("ProceduralModel_Name") + "###ProceduralModel";

			paramerterList = new Component.ParameterList();
			candp = new Component.CopyAndPaste("ProceduralModel", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnBeforeLoad += Core_OnBeforeLoad;
			Read();

			TabToolTip = Resources.GetString("ProceduralModel_Name");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		object GetTargetObject()
		{
			return Core.ProceduralModel.ProceduralModels.Selected;
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.OnBeforeLoad -= Core_OnBeforeLoad;
		}

		protected override void UpdateInternal()
		{
			float width = Manager.NativeManager.GetContentRegionAvail().X;

			Manager.NativeManager.PushItemWidth(width - Manager.NativeManager.GetTextLineHeight() * 5.5f);

			var nextParam = Component.ObjectCollection.Select("", "", Core.ProceduralModel.ProceduralModels.Selected, false, Core.ProceduralModel.ProceduralModels);

			if (Core.ProceduralModel.ProceduralModels.Selected != nextParam)
			{
				Core.ProceduralModel.ProceduralModels.Selected = nextParam;
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicAdd") + "###DynamicAdd"))
			{
				Core.ProceduralModel.ProceduralModels.New();
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicDelete") + "###DynamicDelete"))
			{
				Core.ProceduralModel.ProceduralModels.Delete(Core.ProceduralModel.ProceduralModels.Selected);
			}

			candp.Update();
			paramerterList.Update();
		}

		private void Read()
		{
			paramerterList.SetValue(Core.ProceduralModel.ProceduralModels);
		}

		private void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		private void Core_OnBeforeLoad(object sender, EventArgs e)
		{
			paramerterList.SetValue(null);
		}

	}
}

