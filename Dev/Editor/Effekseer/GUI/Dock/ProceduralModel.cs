using System;
using Effekseer.Data;

namespace Effekseer.GUI.Dock
{
	class ProceduralModel : DockPanel
	{
		readonly Component.ParameterList paramerterList = null;

		public ProceduralModel()
		{
			Label = Icons.PanelProceduralModel + Resources.GetString("ProceduralModel_Name") + "###ProceduralModel";
			DocPage = "proceduralModel.html";

			paramerterList = new Component.ParameterList();
			CopyAndPaste = new Component.CopyAndPaste("ProceduralModel", GetTargetObject, Read);

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
			Manager.NativeManager.Separator();

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