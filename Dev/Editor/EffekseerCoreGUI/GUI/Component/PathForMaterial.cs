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

		Data.Value.PathForMaterial binding = null;

		string relativeFilePath = string.Empty;
		string absoluteFilePath = string.Empty;

		bool isHovered = false;

		Utl.MaterialInformation matInfo = new Utl.MaterialInformation();

		Utl.CompiledMaterialInformation compiledMatInfo = new Utl.CompiledMaterialInformation();
		Utl.CompiledMaterialInformationErrorCode errorCode = Utl.CompiledMaterialInformationErrorCode.OK;

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

		public PathForMaterial()
		{
			id1 = "###" + GUIManager.GetUniqueID().ToString();
			id2 = "###" + GUIManager.GetUniqueID().ToString();
			id3 = "###" + GUIManager.GetUniqueID().ToString();
			id4 = "###" + GUIManager.GetUniqueID().ToString();
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

			string dd = null;

			float buttonSizeX = GUIManager.NativeManager.GetTextLineHeightWithSpacing() * 2;

			if (GUIManager.NativeManager.Button(Resources.GetString("Load") + id1, buttonSizeX))
			{
				btn_load_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateFileDst(DragAndDrops.FileType.Material);

			isHovered = isHovered || GUIManager.NativeManager.IsItemHovered();

			GUIManager.NativeManager.SameLine();

			// show path
			GUIManager.NativeManager.Text(relativeFilePath);

			if (dd == null) dd = DragAndDrops.UpdateFileDst(DragAndDrops.FileType.Material);

			if (GUIManager.NativeManager.IsItemHovered())
			{
				GUIManager.NativeManager.SetTooltip(relativeFilePath);
			}

			isHovered = isHovered || GUIManager.NativeManager.IsItemHovered();

			// 
			if (absoluteFilePath != string.Empty)
			{

				if (GUIManager.NativeManager.Button(Resources.GetString("Delete") + id2, buttonSizeX))
				{
					btn_delete_Click();
				}

				GUIManager.NativeManager.SameLine();

				if (GUIManager.NativeManager.Button(Resources.GetString("Material_Edit_Name") + id3, buttonSizeX))
				{
					if(Process.MaterialEditor.Run())
					{
						Process.MaterialEditor.OpenOrCreateMaterial(absoluteFilePath);
					}
				}

				if (Functions.CanShowTip())
				{
					GUIManager.NativeManager.SetTooltip(Resources.GetString("Material_Edit_Desc"));
				}

				GUIManager.NativeManager.SameLine();

				if (GUIManager.NativeManager.Button(Resources.GetString("Material_GenCache_Name") + id4, buttonSizeX * 2.2f))
				{
					GenerateCompiledMaterial();
				}

				if (Functions.CanShowTip())
				{
					ShowInformation();
				}

				isHovered = isHovered || GUIManager.NativeManager.IsItemHovered();
			}
			else
			{
				if (GUIManager.NativeManager.Button(Resources.GetString("Material_Create_Name"), buttonSizeX))
				{
					var filter = Resources.GetString("MaterialFilter");
					var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (!string.IsNullOrEmpty(result))
					{
						var filepath = Functions.GetFilepathWithExtentions(result, ".efkmat");

						if (Process.MaterialEditor.Run())
						{
							Process.MaterialEditor.OpenOrCreateMaterial(filepath);

							// wait
							int counter = 0;
							while (counter < 50)
							{
								if (System.IO.File.Exists(filepath))
									break;
								counter++;
								System.Threading.Thread.Sleep(100);
							}

							if (System.IO.File.Exists(filepath))
							{
								LoadFile(filepath);
								Read();
							}
						}
					}
				}

				if (GUIManager.NativeManager.IsItemHovered())
				{
					GUIManager.NativeManager.SetTooltip(Resources.GetString("Material_Create_Desc"));
				}
			}

			if (dd != null)
			{
				Dropped(dd);
			}
		}

		public void Dropped(string path)
		{
			if (CheckExtension(path))
			{
				LoadFile(path);
				Read();
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
				LoadFile(filepath);
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
			IO.MaterialCacheGenerator.GenerateMaterialCache(binding.GetAbsolutePath());

			UpdateInformation();
		}

		private bool CheckExtension(string path)
		{
			var filters = Resources.GetString("MaterialFilter").Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}

		void LoadFile(string filepath)
		{
			binding.SetAbsolutePath(filepath);

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

			GUIManager.Viewer.Reload(true);

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

			var binaryPath = IO.MaterialCacheGenerator.CreateBinaryFilePath(binding.GetAbsolutePath());
			if (System.IO.File.Exists(binaryPath))
			{
				compiledMatInfo = new Utl.CompiledMaterialInformation();
				errorCode = compiledMatInfo.Load(binaryPath);
			}
			else
			{
				compiledMatInfo = null;
			}
		}

		void ShowInformation()
		{
			GUIManager.NativeManager.BeginTooltip();

			GUIManager.NativeManager.Text(Resources.GetString("Material_GenCache_Desc"));

			if (compiledMatInfo != null)
			{
				if(matInfo.GUID != compiledMatInfo.GUID)
				{
					GUIManager.NativeManager.Text(Resources.GetString("Material_OldCache"));
				}
				else if(errorCode == Utl.CompiledMaterialInformationErrorCode.TooOldFormat)
				{
					GUIManager.NativeManager.Text(Resources.GetString("Material_OldFomatCache"));
				}
				else
				{
					foreach(var p in compiledMatInfo.Platforms)
					{
						GUIManager.NativeManager.Text(p.ToString());
					}
				}
			}
			else
			{
				GUIManager.NativeManager.Text(Resources.GetString("Material_NoneCache"));
			}

			GUIManager.NativeManager.EndTooltip();
		}
	}
}

