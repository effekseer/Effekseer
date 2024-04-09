using System;
using Effekseer.Data;

namespace Effekseer.GUI.Dock
{
	class Dynamic : DockPanel
	{
		readonly BindableComponent.ParameterList paramerterListInput = null;
		readonly BindableComponent.ParameterList paramerterList = null;

		string compileResult = string.Empty;

		public Dynamic()
		{
			Label = Icons.PanelDynamicParams + MultiLanguageTextProvider.GetText("DynamicParameter_Name") + "###DynamicParameter";
			DocPage = "dynamicParameter.html";

			paramerterListInput = new BindableComponent.ParameterList();
			paramerterList = new BindableComponent.ParameterList();

			Core.OnBeforeLoad += Core_OnBeforeLoad;
			Core.OnBeforeNew += Core_OnBeforeNew;
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.Dynamic.Equations.OnChanged += Vectors_OnChanged;
			Core.Dynamic.Inputs.Values[0].Input.OnChanged += Input_OnChanged;
			Core.Dynamic.Inputs.Values[1].Input.OnChanged += Input_OnChanged;
			Core.Dynamic.Inputs.Values[2].Input.OnChanged += Input_OnChanged;
			Core.Dynamic.Inputs.Values[3].Input.OnChanged += Input_OnChanged;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("DynamicParameter_Name");
		}

		private static void Input_OnChanged(object sender, ChangedValueEventArgs e)
		{
			Manager.Viewer.SetDynamicInput(
				Core.Dynamic.Inputs.Values[0].Input.Value,
				Core.Dynamic.Inputs.Values[1].Input.Value,
				Core.Dynamic.Inputs.Values[2].Input.Value,
				Core.Dynamic.Inputs.Values[3].Input.Value);
		}

		private static void Vectors_OnChanged(object sender, ChangedValueEventArgs e)
		{
		}

		public void FixValues()
		{
			paramerterListInput.FixValues();
			paramerterList.FixValues();
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.Dynamic.Equations.OnChanged -= Vectors_OnChanged;
		}

		protected override void UpdateInternal()
		{
			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("DynamicInput"));

			paramerterListInput.Update();

			Manager.NativeManager.Separator();

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("DynamicEquation"));

			float width = Manager.NativeManager.GetContentRegionAvail().X;

			Manager.NativeManager.PushItemWidth(width - Manager.NativeManager.GetTextLineHeight() * 5.5f);

			var nextParam = BindableComponent.ObjectCollection.Select("", "", Core.Dynamic.Equations.Selected, false, Core.Dynamic.Equations);

			if (Core.Dynamic.Equations.Selected != nextParam)
			{
				Core.Dynamic.Equations.Selected = nextParam;
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("DynamicAdd") + "###DynamicAdd"))
			{
				Core.Dynamic.Equations.New();
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("DynamicDelete") + "###DynamicDelete"))
			{
				Core.Dynamic.Equations.Delete(Core.Dynamic.Equations.Selected);
			}

			paramerterList.Update();

			if (Core.Dynamic.Equations.Selected != null
				&& Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("Compile") + "###DynamicCompile")
				&& Core.Dynamic.Equations.Selected is DynamicEquation selected)
			{
				var compiler = new InternalScript.Compiler();
				var result = compiler.Compile(selected.Code.Value);

				compileResult = result.Error != null ? Utils.CompileErrorGenerator.Generate(selected.Code.Value, result.Error) : "OK";
			}

			// show errors
			Manager.NativeManager.Text(compileResult);
		}

		private void Read()
		{
			paramerterListInput.SetValue(Core.Dynamic.Inputs);
			paramerterList.SetValue(Core.Dynamic.Equations);
		}

		private void Core_OnBeforeNew(object sender, EventArgs e)
		{
			Core.Dynamic.Inputs.Values[0].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Inputs.Values[1].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Inputs.Values[2].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Inputs.Values[3].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Equations.OnChanged -= Vectors_OnChanged;
			paramerterList.SetValue(null);
		}

		private void Core_OnBeforeLoad(object sender, EventArgs e)
		{
			Core.Dynamic.Inputs.Values[0].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Inputs.Values[1].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Inputs.Values[2].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Inputs.Values[3].Input.OnChanged -= Input_OnChanged;
			Core.Dynamic.Equations.OnChanged -= Vectors_OnChanged;
			paramerterList.SetValue(null);
		}

		private void OnAfterLoad(object sender, EventArgs e)
		{
			Core.Dynamic.Equations.OnChanged += Vectors_OnChanged;
			Core.Dynamic.Inputs.Values[0].Input.OnChanged += Input_OnChanged;
			Core.Dynamic.Inputs.Values[1].Input.OnChanged += Input_OnChanged;
			Core.Dynamic.Inputs.Values[2].Input.OnChanged += Input_OnChanged;
			Core.Dynamic.Inputs.Values[3].Input.OnChanged += Input_OnChanged;

			Read();
		}
	}
}