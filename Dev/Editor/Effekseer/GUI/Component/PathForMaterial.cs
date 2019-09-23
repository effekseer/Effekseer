using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class PathForMaterial : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id3 = "";
		string id4 = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.PathForMaterial binding = null;

		string relativeFilePath = string.Empty;
		string absoluteFilePath = string.Empty;

		bool isHovered = false;

		Utl.MaterialInformation matInfo = new Utl.MaterialInformation();

		Utl.CompiledMaterialInformation compiledMatInfo = new Utl.CompiledMaterialInformation();

		public bool EnableUndo { get; set; } = true;

		public Data.Value.PathForMaterial Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;

				Read();
			}
		}

		public PathForMaterial(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id3 = "###" + Manager.GetUniqueID().ToString();
			id4 = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.PathForMaterial;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void OnDisposed()
		{
		}

		public override void OnDropped(string path, ref bool handle)
		{
			if (isHovered)
			{
				if (CheckExtension(path))
				{
					binding.SetAbsolutePath(path);
					Read();
				}

				handle = true;
			}
		}

		public override void Update()
		{
			isHovered = false;

			if (binding == null) return;

			float buttonSizeX = Manager.NativeManager.GetTextLineHeightWithSpacing() * 2;

			if (Manager.NativeManager.Button(Resources.GetString("Load") + id1, buttonSizeX))
			{
				btn_load_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			// show path
			Manager.NativeManager.Text(relativeFilePath);

			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip(relativeFilePath);
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			// 
			if (absoluteFilePath != string.Empty)
			{

				if (Manager.NativeManager.Button(Resources.GetString("Delete") + id2, buttonSizeX))
				{
					btn_delete_Click();
				}

				if (Manager.NativeManager.Button("Edit", buttonSizeX))
				{
					Process.MaterialEditor.Run();
					Process.MaterialEditor.OpenOrCreateMaterial(absoluteFilePath);
				}

				if (Manager.NativeManager.Button("GenCache" + id4, buttonSizeX))
				{
					GenerateCompiledMaterial();
				}

				if(Manager.NativeManager.IsItemHovered())
				{
					ShowInformation();
				}

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			}
			else
			{
				if (Manager.NativeManager.Button("Create", buttonSizeX))
				{
					var filter = Resources.GetString("MaterialFilter");
					var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (!string.IsNullOrEmpty(result))
					{
						var filepath = result;
						Process.MaterialEditor.Run();
						Process.MaterialEditor.OpenOrCreateMaterial(absoluteFilePath);

						// wait
						int counter = 0;
						while(counter < 50)
						{
							if (System.IO.File.Exists(absoluteFilePath))
								break;
							counter++;
							System.Threading.Thread.Sleep(100);
						}

						if (System.IO.File.Exists(absoluteFilePath))
						{
							LoadFile(filepath, false);
							Read();
						}
						
					}
				}
			}
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

			var filter = Resources.GetString("MaterialFilter");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;
				LoadFile(filepath, false);
			}
			else
			{
				return;
			}

			Read();
		}

		private void btn_delete_Click()
		{
			if (binding == null) return;
			binding.SetAbsolutePath(string.Empty);

			Read();
		}


		void Read()
		{
			if (binding != null)
			{
				absoluteFilePath = binding.GetAbsolutePath();
				relativeFilePath = binding.GetRelativePath();
			}
			else
			{
				absoluteFilePath = string.Empty;
				relativeFilePath = string.Empty;
			}

			UpdateInformation();
		}

		void GenerateCompiledMaterial()
		{
			var generator = new swig.CompiledMaterialGenerator();

			string appDirectory = GUI.Manager.GetEntryDirectory();
			string fullPath = System.IO.Path.Combine(appDirectory, "tools/");

			generator.Initialize(fullPath);

			if(absoluteFilePath != string.Empty)
			{
				generator.Compile(CreateBinaryFilePath(), binding.GetAbsolutePath());
			}

			generator.Dispose();

			UpdateInformation();
		}

		private bool CheckExtension(string path)
		{
			var filters = binding.Filter.Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}

		void LoadFile(string filepath, bool isReloading)
		{
			binding.SetAbsolutePath(filepath);

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

			Manager.Viewer.Reload(true);

			UpdateInformation();
		}

		void UpdateInformation()
		{
			if(binding == null || string.IsNullOrEmpty(binding.GetAbsolutePath()))
			{
				matInfo = null;
				compiledMatInfo = null;
				return;
			}

			matInfo = new Utl.MaterialInformation();
			matInfo.Load(binding.GetAbsolutePath());

			if(System.IO.File.Exists(CreateBinaryFilePath()))
			{
				compiledMatInfo = new Utl.CompiledMaterialInformation();
				compiledMatInfo.Load(CreateBinaryFilePath());
			}
			else
			{
				compiledMatInfo = null;
			}
		}

		void ShowInformation()
		{
			Manager.NativeManager.BeginTooltip();
			if(compiledMatInfo != null)
			{
				if(matInfo.GUID != compiledMatInfo.GUID)
				{
					Manager.NativeManager.Text("!!!Cache is too old.!!!");
				}
				else
				{
					foreach(var p in compiledMatInfo.Platforms)
					{
						Manager.NativeManager.Text(p.ToString());
					}
				}
			}
			else
			{
				Manager.NativeManager.Text("Cache is not generated.");
			}

			Manager.NativeManager.EndTooltip();
		}

		string CreateBinaryFilePath()
		{
			return binding.GetAbsolutePath() + "d";
		}
	}
}

