using Effekseer.GUI;
using Effekseer;
using Effekseer.GUI.Inspector;
using PartsTreeSystem;
using System.Reflection.Emit;

namespace Effekseer.DockPanels
{
	public class Dynamic : GUI.Dock.DockPanel
	{
		Inspector equationInspector;

		int selectedEquaitionIndex = 0;

		string compileResult = string.Empty;

		public Dynamic()
		{
			equationInspector = new Inspector();
			Label = Icons.PanelDynamicParams + "Inspector_DynamicParameter###Inspector_DynamicParameter";

			TabToolTip = Resources.GetString("DynamicParameter_Name");
			DocPage = "dynamicParameter.html";
		}

		protected override void UpdateInternal()
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null ||
				CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			// TODO: Remove this when remove the previous version
			Manager.NativeManager.PushID(999);

			Manager.NativeManager.Text(Resources.GetString("DynamicInput"));

			// TODO insert inspector of DynamicInput here


			Manager.NativeManager.Separator();

			Manager.NativeManager.Text(Resources.GetString("DynamicEquation"));

			float width = Manager.NativeManager.GetContentRegionAvail().X;

			Manager.NativeManager.PushItemWidth(width - Manager.NativeManager.GetTextLineHeight() * 5.5f);

			var selectedEquation = CoreContext.SelectedEffect.Asset.DynamicEquations.Count > selectedEquaitionIndex ? CoreContext.SelectedEffect.Asset.DynamicEquations[selectedEquaitionIndex] : null;
			
			// combo of DynamicEquations
			GUI.Widgets.DynamicSelector.EquationsCombo("", "DockPanelDynamic", ref selectedEquaitionIndex, CoreContext.SelectedEffect.Asset.DynamicEquations);

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicAdd") + "###DynamicAdd"))
			{
				var newEquation = new Asset.DynamicEquation();
				newEquation.Name = "New Expression";
				newEquation.Code = "@O.x = @In0\n@O.y = @In1";
				CoreContext.SelectedEffect.Asset.DynamicEquations.Add(newEquation);
				selectedEquaitionIndex = CoreContext.SelectedEffect.Asset.DynamicEquations.Count - 1;
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicDelete") + "###DynamicDelete"))
			{
				CoreContext.SelectedEffect.Asset.DynamicEquations.Remove(selectedEquation);
			}

			// inspector of DynamicEquations
			if (selectedEquation != null)
			{
				CoreContext.SelectedEffect.Context.CommandManager.StartEditFields(
				selectedEquation, CoreContext.Environment);

				equationInspector.Update(CoreContext.SelectedEffect.Context, selectedEquation);

				CoreContext.SelectedEffect.Context.CommandManager.EndEditFields(
					selectedEquation, CoreContext.Environment);


				if (Manager.NativeManager.Button(Resources.GetString("Compile") + "###DynamicCompile"))
				{
					var compiler = new InternalScript.Compiler();
					var result = compiler.Compile(selectedEquation.Code);

					compileResult = result.Error != null ? Utils.CompileErrorGenerator.Generate(selectedEquation.Code, result.Error) : "OK";
				}
				// show errors
				Manager.NativeManager.Text(compileResult);
			}

			// TODO: Remove this when remove the previous version
			Manager.NativeManager.PopID();
		}

		public override void DispatchDropped(string path, ref bool handle)
		{
			if (CoreContext.SelectedEffect.Context.NodeTree == null || CoreContext.SelectedEffectNode == null)
			{
				return;
			}

			CoreContext.SelectedEffect.Context.CommandManager.StartEditFields(
				CoreContext.SelectedEffect.Asset.NodeTreeAsset,
				CoreContext.SelectedEffect.Context.NodeTree,
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);

			if (equationInspector.Drop(path, CoreContext.SelectedEffect.Context,
				CoreContext.SelectedEffectNode,
				typeof(Asset.Effect.RotationParameter)))
			{
				handle = true;
			}

			CoreContext.SelectedEffect.Context.CommandManager.EndEditFields(
				CoreContext.SelectedEffectNode,
				CoreContext.Environment);


			base.DispatchDropped(path, ref handle);
		}
	}
}
