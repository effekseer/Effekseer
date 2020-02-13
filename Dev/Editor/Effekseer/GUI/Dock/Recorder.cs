using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Recorder : DockPanel
	{
		int selectedTypeIndex = 0;
		int selectedAlphaIndex = 0;
		int selectedStorageTarget = 0;

		string[] selectedExportTypes = new string[]
		{
			"1",
			"2",
			"3",
			"4",
		};

		string[] selectedAlphaTypes = new string[]
		{
			"1",
			"2",
			"3",
			"4",
		};

		string[] selectedStorageTargets = new string[]
		{
			"1",
			"2",
		};


		public Recorder()
		{
			selectedExportTypes[0] = Resources.GetString("ExportAsSingleImage");
			selectedExportTypes[1] = Resources.GetString("ExportAsImages");
			selectedExportTypes[2] = Resources.GetString("ExportAsGifAnimation");
			selectedExportTypes[3] = Resources.GetString("ExportAsAvi");

			selectedAlphaTypes[0] = Resources.GetString("None");
			selectedAlphaTypes[1] = Resources.GetString("UseOriginalImage");
			selectedAlphaTypes[2] = Resources.GetString("GenerateAlpha");
			selectedAlphaTypes[3] = Resources.GetString("GenerateAlpha2");

			selectedStorageTargets[0] = Resources.GetString("StorageGlobal");
			selectedStorageTargets[1] = Resources.GetString("StorageLocal");

			Label = Resources.GetString("Recorder") + "###Recorder";

			Icon = Images.GetIcon("PanelRecorder");
			TabToolTip = Resources.GetString("Recorder");
		}

		protected override void UpdateInternal()
		{
			var viewerParameter = Manager.Viewer.GetViewerParamater();
            viewerParameter.GuideWidth = Core.Recording.RecordingWidth.Value;
            viewerParameter.GuideHeight = Core.Recording.RecordingHeight.Value;
            viewerParameter.RendersGuide = Core.Recording.IsRecordingGuideShown;

			float dpiScale = Manager.DpiScale;
			var w = new int [] { Manager.Viewer.GetViewerParamater().GuideWidth };
			var h = new int [] { Manager.Viewer.GetViewerParamater().GuideHeight };
			var showGuide = new bool[] { Manager.Viewer.GetViewerParamater().RendersGuide };

            var startingFrame_ = new int[] { Core.Recording.RecordingStartingFrame.Value };
			var endingFrame_ = new int[] { Core.Recording.RecordingEndingFrame.Value };
			var freq_ = new int[] { Core.Recording.RecordingFrequency.Value };
			var theNumberOfImageH_ = new int[] { Core.Recording.RecordingHorizontalCount.Value };
			
			var areaTitle = Resources.GetString("Resolution");
			var exportedFrameTitle = Resources.GetString("ExportedFrame");
			var typeTitle = Resources.GetString("Format");
			var optionsTitle = Resources.GetString("Options");

            selectedTypeIndex = Core.Recording.RecordingExporter.GetValueAsInt();
            selectedAlphaIndex = Core.Recording.RecordingTransparentMethod.GetValueAsInt();
			selectedStorageTarget = Core.Recording.RecordingStorageTarget.GetValueAsInt();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##RecordRes", new swig.Vec2(0, 120 * dpiScale), true, swig.WindowFlags.MenuBar);
			
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(areaTitle + "##RecordResTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(Resources.GetString("Width"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###w", w))
			{
				viewerParameter.GuideWidth = w[0];
                Core.Recording.RecordingWidth.SetValueDirectly(w[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("Height"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###h", h))
			{
				viewerParameter.GuideHeight = h[0];
                Core.Recording.RecordingHeight.SetValueDirectly(h[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("ShowGuide"));

			Manager.NativeManager.NextColumn();

			if (Manager.NativeManager.Checkbox("###sg", showGuide))
			{
				viewerParameter.RendersGuide = showGuide[0];
                Core.Recording.IsRecordingGuideShown.SetValueDirectly(showGuide[0]);
			}

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##OutputFrame", new swig.Vec2(0, 130 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(exportedFrameTitle + "##OutputFrameTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(Resources.GetString("StartFrame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###sf", startingFrame_))
			{
                Core.Recording.RecordingStartingFrame.SetValueDirectly(startingFrame_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("EndFrame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###ef", endingFrame_))
			{
                Core.Recording.RecordingEndingFrame.SetValueDirectly(endingFrame_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("Frequency_Frame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###ff", freq_))
			{
                Core.Recording.RecordingFrequency.SetValueDirectly(freq_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##OutputType", new swig.Vec2(0, 100 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(typeTitle + "##OutputTypeTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(Resources.GetString("XCount"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.DragInt("###tn", theNumberOfImageH_))
			{
                Core.Recording.RecordingHorizontalCount.SetValueDirectly(theNumberOfImageH_[0]);
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Text(Resources.GetString("ExportedFrame"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.BeginCombo("###extype", selectedExportTypes[selectedTypeIndex], swig.ComboFlags.None))
			{
				for(int i = 0; i < selectedExportTypes.Length; i++)
				{
					if(Manager.NativeManager.Selectable(selectedExportTypes[i]))
					{
						selectedTypeIndex = i;
                        Core.Recording.RecordingExporter.SetValueDirectly((Data.RecordingExporterType)i);
                        Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Recordingwindow
			Manager.NativeManager.BeginChild("##Option", new swig.Vec2(0, 60 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (Manager.NativeManager.BeginMenuBar())
			{
				if (Manager.NativeManager.BeginMenu(optionsTitle + "##OptionTitle"))
				{
					Manager.NativeManager.EndMenu();
				}

				Manager.NativeManager.EndMenuBar();
			}

			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(Resources.GetString("MakeTransparent"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.BeginCombo("###tr", selectedAlphaTypes[selectedAlphaIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedAlphaTypes.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedAlphaTypes[i]))
					{
						selectedAlphaIndex = i;
                        Core.Recording.RecordingTransparentMethod.SetValueDirectly((Data.RecordingTransparentMethodType)i);
                        Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();

			// Storage
			Manager.NativeManager.BeginChild("##Storage", new swig.Vec2(0, 60 * dpiScale), true, swig.WindowFlags.MenuBar);
			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			Manager.NativeManager.Text(Resources.GetString("StorageTarget"));

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.PushItemWidth(-1);
			if (Manager.NativeManager.BeginCombo("###st", selectedStorageTargets[selectedStorageTarget], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedStorageTargets.Length; i++)
				{
					if (Manager.NativeManager.Selectable(selectedStorageTargets[i]))
					{
						selectedStorageTarget = i;
						Core.Recording.RecordingStorageTarget.SetValueDirectly((Data.RecordingStorageTargetTyoe)i);
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChild();


			Manager.Viewer.SetViewerParamater(viewerParameter);

			if(Manager.NativeManager.Button(Resources.GetString("Record") + "###btn"))
			{
				var during = Core.Recording.RecordingEndingFrame.Value - Core.Recording.RecordingStartingFrame.Value;
				if (during < 0)
				{
                    swig.GUIManager.show("出力フレームが存在しません。", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
                        
					//var mb = new Dialog.MessageBox();
					//mb.Show("Error", "出力フレームが存在しません。");
				}

				var count = during / Core.Recording.RecordingFrequency.Value + 1;
				var horizontalCount = Core.Recording.RecordingHorizontalCount.Value;

				if (Manager.Viewer != null)
				{
					var filter = string.Empty;

					if (selectedTypeIndex == 0)
                    {
						filter = "png";
                    }
                    else if (selectedTypeIndex == 1)
                    {
						filter = "png";
                    }
                    else if (selectedTypeIndex == 2)
                    {
						filter = "gif";
                    }
                    else if (selectedTypeIndex == 3)
                    {
						filter = "avi";
                    }


                    var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (string.IsNullOrEmpty(result)) return;
                                   
					var filename = result;

					if (System.IO.Path.GetExtension(filename) != "." + filter)
                    {
						filename += "." + filter;
                    }

					var viewer = Manager.Viewer;
					var param = Manager.Viewer.GetViewerParamater();

					//if (viewer.LoadEffectFunc != null)
					{
						viewer.LoadEffectFunc();
					}

					var tempDirectory = System.IO.Directory.GetCurrentDirectory();
					System.IO.Directory.SetCurrentDirectory(Program.StartDirectory);

					string errorMessage = string.Empty;

                    var startingFrame = Core.Recording.RecordingStartingFrame.Value;
                    var freq = Core.Recording.RecordingFrequency.Value;

                    var recordingParameter = new swig.RecordingParameter();

                    recordingParameter.Count = count;
                    recordingParameter.HorizontalCount = horizontalCount;
                    recordingParameter.Freq = freq;
                    recordingParameter.OffsetFrame = startingFrame;
                    recordingParameter.Transparence = (swig.TransparenceType)selectedAlphaIndex;

					if (Effekseer.Core.Language == Language.Japanese)
					{
						errorMessage = "保存に失敗しました。ファイルが他のアプリケーションで開かれている、もしくはスペックが足りません。";
					}
					else
					{
						errorMessage = "It failed to save. A file is opend by other application or lack of specification.";
					}

					if (selectedTypeIndex == 0)
					{
						if (!viewer.RecordSpriteSheet(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 1)
					{
						if (!viewer.RecordSprite(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 2)
					{
						if (!viewer.RecordAsGifAnimation(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					else if (selectedTypeIndex == 3)
					{
						if (!viewer.RecordAsAVI(filename, recordingParameter))
						{
                            swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}

					System.IO.Directory.SetCurrentDirectory(tempDirectory);

				}
			}
		}
	}
}
