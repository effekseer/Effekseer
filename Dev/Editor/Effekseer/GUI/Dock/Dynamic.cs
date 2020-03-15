using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Dynamic : DockPanel
	{
		Component.ParameterList paramerterListInput = null;

		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		string compileResult = string.Empty;

		public Dynamic()
		{
			Label = Resources.GetString("DynamicParameter_Name") + "###DynamicParameter";

			paramerterListInput = new Component.ParameterList();
			paramerterList = new Component.ParameterList();
			
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

			Icon = Images.GetIcon("PanelDynamicParameter");
			TabToolTip = Resources.GetString("DynamicParameter_Name");
		}

		private void Input_OnChanged(object sender, ChangedValueEventArgs e)
		{
			Manager.Viewer.SetDynamicInput(
				Core.Dynamic.Inputs.Values[0].Input.Value,
				Core.Dynamic.Inputs.Values[1].Input.Value,
				Core.Dynamic.Inputs.Values[2].Input.Value,
				Core.Dynamic.Inputs.Values[3].Input.Value);
		}

		private void Vectors_OnChanged(object sender, ChangedValueEventArgs e)
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
			if (isFiestUpdate)
			{
			}

			Manager.NativeManager.Text(Resources.GetString("DynamicInput"));

			paramerterListInput.Update();

			Manager.NativeManager.Separator();

			Manager.NativeManager.Text(Resources.GetString("DynamicEquation"));

			float width = Manager.NativeManager.GetContentRegionAvail().X;
			
			Manager.NativeManager.PushItemWidth(width - Manager.NativeManager.GetTextLineHeight() * 5.5f);
			
			var nextParam = Component.DynamicSelector.Select("", "", Core.Dynamic.Equations.Selected, false, true);

			if (Core.Dynamic.Equations.Selected != nextParam)
			{
				Core.Dynamic.Equations.Selected = nextParam;
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicAdd") + "###DynamicAdd"))
			{
				Core.Dynamic.Equations.Add();
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Resources.GetString("DynamicDelete") + "###DynamicDelete"))
			{
				Core.Dynamic.Equations.Delete(Core.Dynamic.Equations.Selected);
			}

			paramerterList.Update();

			if (Core.Dynamic.Equations.Selected != null)
			{
				// TODO make good GUI
				if (Manager.NativeManager.Button(Resources.GetString("Compile") + "###DynamicCompile"))
				{
					var selected = Core.Dynamic.Equations.Selected;
					if (selected != null)
					{
						var compiler = new InternalScript.Compiler();
						var result = compiler.Compile(selected.Code.Value);

						if (result.Error != null)
						{
							compileResult = Utils.CompileErrorGenerator.Generate(selected.Code.Value, result.Error);
						}
						else
						{
							compileResult = "OK";
						}
					}
				}
			}

			// show errors
			Manager.NativeManager.Text(compileResult);
		}

		void Read()
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

		void OnAfterLoad(object sender, EventArgs e)
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

