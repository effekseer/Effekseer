using System;
using Effekseer.Data;

namespace Effekseer.GUI.Dock
{
	class ProceduralModel : DockPanel
	{
		readonly BindableComponent.ParameterList paramerterList = null;

		public ProceduralModel()
		{
			Label = Icons.PanelProceduralModel + MultiLanguageTextProvider.GetText("ProceduralModel_Name") + "###ProceduralModel";
			DocPage = "proceduralModel.html";

			paramerterList = new BindableComponent.ParameterList();
			CopyAndPaste = new BindableComponent.CopyAndPaste("ProceduralModel", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnBeforeLoad += Core_OnBeforeLoad;
			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("ProceduralModel_Name");
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

			var nextParam = BindableComponent.ObjectCollection.Select("", "", Core.ProceduralModel.ProceduralModels.Selected, false, Core.ProceduralModel.ProceduralModels);

			if (Core.ProceduralModel.ProceduralModels.Selected != nextParam)
			{
				Core.ProceduralModel.ProceduralModels.Selected = nextParam;
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("DynamicAdd") + "###DynamicAdd"))
			{
				Core.ProceduralModel.ProceduralModels.New();
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("DynamicDelete") + "###DynamicDelete"))
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