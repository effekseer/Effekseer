using System;
using System.Text;

namespace Effekseer.GUI.Dock
{
	class GpuParticlesValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public GpuParticlesValues()
		{
			Label = Icons.PanelGpuParticles + MultiLanguageTextProvider.GetText("GpuParticles") + "###GpuParticles";
			DocPage = "gpu_particles.html";

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.GpuParticlesValues));
			CopyAndPaste = new BindableComponent.CopyAndPaste("GpuParticles", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Controls.Add(paramerterList);

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("GpuParticles");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
			}
		}

		void Read()
		{
			paramerterList.SetValue(GetTargetObject());
		}

		object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).GpuParticles;
				}
			}
			return null;
		}


		void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}
	}
}