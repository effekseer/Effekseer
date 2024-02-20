using System;
using System.Collections.Generic;
using System.Linq;

namespace Effekseer.GUI.BindableComponent
{
	class PathForMaterial : PathBase
	{
		Utils.MaterialInformation matInfo = new Utils.MaterialInformation();

		Utils.CompiledMaterialInformation compiledMatInfo = new Utils.CompiledMaterialInformation();
		Utils.CompiledMaterialInformationErrorCode errorCode = Utils.CompiledMaterialInformationErrorCode.OK;

		public PathForMaterial()
		{
			fileType = FileType.Material;
			filter = MultiLanguageTextProvider.GetText("MaterialFilter");
		}

		protected override void UpdateSubParts(float width)
		{
			// 
			if (filePath != string.Empty)
			{
				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("Material_Edit_Name")))
				{
					if (Process.MaterialEditor.Run())
					{
						Process.MaterialEditor.OpenOrCreateMaterial(binding.AbsolutePath);
					}
				}

				if (Functions.CanShowTip())
				{
					Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Material_Edit_Desc"));
				}

				Manager.NativeManager.SameLine();

				var isValidCashe = IsValidCashe();

				if (isValidCashe.HasValue && !isValidCashe.Value)
				{
					Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0xff0000ff);
				}

				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("Material_GenCache_Name")))
				{
					GenerateCompiledMaterial();
				}

				if (isValidCashe.HasValue && !isValidCashe.Value)
				{
					Manager.NativeManager.PopStyleColor();
				}

				if (Functions.CanShowTip())
				{
					ShowInformation();
				}

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			}
			else
			{
				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("Material_Create_Name")))
				{
					var filter = MultiLanguageTextProvider.GetText("MaterialFilter");
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
								LoadFile(filepath, false);
								Read();
							}
						}
					}
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Material_Create_Desc"));
				}
			}
		}

		void GenerateCompiledMaterial()
		{
			IO.MaterialCacheGenerator.GenerateMaterialCache(binding.GetAbsolutePath());

			UpdateInformation();
		}

		protected override void LoadFile(string filepath, bool isReloading)
		{
			binding.SetAbsolutePath(filepath);

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

			Manager.Viewer.Reload(true);

			UpdateInformation();
		}

		void UpdateInformation()
		{
			if (binding == null || string.IsNullOrEmpty(binding.GetAbsolutePath()))
			{
				matInfo = null;
				compiledMatInfo = null;
				return;
			}

			matInfo = new Utils.MaterialInformation();
			matInfo.Load(binding.GetAbsolutePath());

			var binaryPath = IO.MaterialCacheGenerator.CreateBinaryFilePath(binding.GetAbsolutePath());
			if (System.IO.File.Exists(binaryPath))
			{
				compiledMatInfo = new Utils.CompiledMaterialInformation();
				errorCode = compiledMatInfo.Load(binaryPath);
			}
			else
			{
				compiledMatInfo = null;
			}
		}

		void ShowInformation()
		{
			Manager.NativeManager.BeginTooltip();

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Material_GenCache_Desc"));

			if (compiledMatInfo != null)
			{
				if (matInfo.GUID != compiledMatInfo.GUID)
				{
					Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Material_OldCache"));
				}
				else if (errorCode == Utils.CompiledMaterialInformationErrorCode.TooOldFormat)
				{
					Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Material_OldFomatCache"));
				}
				else if (errorCode == Utils.CompiledMaterialInformationErrorCode.TooNewFormat)
				{
					Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Material_NewFomatCache"));
				}
				else
				{
					foreach (var p in compiledMatInfo.Platforms)
					{
						Manager.NativeManager.Text(p.ToString());
					}
				}
			}
			else
			{
				Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Material_NoneCache"));
			}

			Manager.NativeManager.EndTooltip();
		}

		bool? IsValidCashe()
		{
			if (compiledMatInfo != null)
			{
				if (matInfo.GUID != compiledMatInfo.GUID)
				{
					return false;
				}
				else if (errorCode == Utils.CompiledMaterialInformationErrorCode.TooOldFormat)
				{
					return false;
				}
				else if (errorCode == Utils.CompiledMaterialInformationErrorCode.TooNewFormat)
				{
					return false;
				}
				else
				{
					return true;
				}
			}

			return null;
		}
	}
}