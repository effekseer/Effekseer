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


		MultiLanguageString scale_name = new MultiLanguageString("Recording_Scale_Name");
		MultiLanguageString scale_desc = new MultiLanguageString("Recording_Scale_Desc");

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

			Label = Icons.PanelRecorder + Resources.GetString("Recorder") + "###Recorder";

			TabToolTip = Resources.GetString("Recorder");
		}

		protected override void UpdateInternal()
		{
			var viewerParameter = GUIManager.Viewer.GetViewerParamater();
            viewerParameter.GuideWidth = Core.Recording.RecordingWidth.Value;
            viewerParameter.GuideHeight = Core.Recording.RecordingHeight.Value;
            viewerParameter.RendersGuide = Core.Recording.IsRecordingGuideShown;

			float dpiScale = GUIManager.DpiScale;
			var w = new int [] { GUIManager.Viewer.GetViewerParamater().GuideWidth };
			var h = new int [] { GUIManager.Viewer.GetViewerParamater().GuideHeight };
			var showGuide = new bool[] { GUIManager.Viewer.GetViewerParamater().RendersGuide };

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
			{
				GUIManager.NativeManager.BeginChild("##RecordRes", new swig.Vec2(0, 140 * dpiScale), true, swig.WindowFlags.MenuBar);

				if (GUIManager.NativeManager.BeginMenuBar())
				{
					if (GUIManager.NativeManager.BeginMenu(areaTitle + "##RecordResTitle"))
					{
						GUIManager.NativeManager.EndMenu();
					}

					GUIManager.NativeManager.EndMenuBar();
				}

				GUIManager.NativeManager.Columns(2);
				GUIManager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

				GUIManager.NativeManager.Text(Resources.GetString("Width"));

				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.PushItemWidth(-1);
				if (GUIManager.NativeManager.DragInt("###w", w))
				{
					viewerParameter.GuideWidth = w[0];
					Core.Recording.RecordingWidth.SetValueDirectly(w[0]);
				}
				GUIManager.NativeManager.PopItemWidth();

				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.Text(Resources.GetString("Height"));

				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.PushItemWidth(-1);
				if (GUIManager.NativeManager.DragInt("###h", h))
				{
					viewerParameter.GuideHeight = h[0];
					Core.Recording.RecordingHeight.SetValueDirectly(h[0]);
				}
				GUIManager.NativeManager.PopItemWidth();

				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.Text(Resources.GetString("ShowGuide"));

				GUIManager.NativeManager.NextColumn();

				if (GUIManager.NativeManager.Checkbox("###sg", showGuide))
				{
					viewerParameter.RendersGuide = showGuide[0];
					Core.Recording.IsRecordingGuideShown.SetValueDirectly(showGuide[0]);
				}

				// Scale

				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.Text(scale_name.ToString());

				if(GUIManager.NativeManager.IsItemHovered())
				{
					GUIManager.NativeManager.SetTooltip(scale_desc.ToString());
				}

				GUIManager.NativeManager.NextColumn();

				var recordingScale = new int[] { Core.Recording.RecordingScale.Value };
				if (GUIManager.NativeManager.DragInt("###RecScale", recordingScale))
				{
					Core.Recording.RecordingScale.SetValue(recordingScale[0]);
				}

				GUIManager.NativeManager.Columns(1);

				GUIManager.NativeManager.EndChild();
			}

			// Recordingwindow
			GUIManager.NativeManager.BeginChild("##OutputFrame", new swig.Vec2(0, 130 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (GUIManager.NativeManager.BeginMenuBar())
			{
				if (GUIManager.NativeManager.BeginMenu(exportedFrameTitle + "##OutputFrameTitle"))
				{
					GUIManager.NativeManager.EndMenu();
				}

				GUIManager.NativeManager.EndMenuBar();
			}

			GUIManager.NativeManager.Columns(2);
			GUIManager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			GUIManager.NativeManager.Text(Resources.GetString("StartFrame"));

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.PushItemWidth(-1);
			if (GUIManager.NativeManager.DragInt("###sf", startingFrame_))
			{
                Core.Recording.RecordingStartingFrame.SetValueDirectly(startingFrame_[0]);
			}
			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.Text(Resources.GetString("EndFrame"));

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.PushItemWidth(-1);
			if (GUIManager.NativeManager.DragInt("###ef", endingFrame_))
			{
                Core.Recording.RecordingEndingFrame.SetValueDirectly(endingFrame_[0]);
			}
			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.Text(Resources.GetString("Frequency_Frame"));

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.PushItemWidth(-1);
			if (GUIManager.NativeManager.DragInt("###ff", freq_))
			{
                Core.Recording.RecordingFrequency.SetValueDirectly(freq_[0]);
			}
			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.Columns(1);

			GUIManager.NativeManager.EndChild();

			// ExportType
			float exportTypeChildHeight = (selectedTypeIndex == 0 ? 90 : 60) * dpiScale;
			GUIManager.NativeManager.BeginChild("##OutputType", new swig.Vec2(0, exportTypeChildHeight), true, swig.WindowFlags.MenuBar);
			if (GUIManager.NativeManager.BeginMenuBar())
			{
				if (GUIManager.NativeManager.BeginMenu(typeTitle + "##OutputTypeTitle"))
				{
					GUIManager.NativeManager.EndMenu();
				}

				GUIManager.NativeManager.EndMenuBar();
			}

			GUIManager.NativeManager.Columns(2);
			GUIManager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			GUIManager.NativeManager.Text(Resources.GetString("ExportedFrame"));

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.PushItemWidth(-1);
			if (GUIManager.NativeManager.BeginCombo("###extype", selectedExportTypes[selectedTypeIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedExportTypes.Length; i++)
				{
					if (GUIManager.NativeManager.Selectable(selectedExportTypes[i]))
					{
						selectedTypeIndex = i;
						Core.Recording.RecordingExporter.SetValueDirectly((Data.RecordingExporterType)i);
						GUIManager.NativeManager.SetItemDefaultFocus();
					}
				}

				GUIManager.NativeManager.EndCombo();
			}
			GUIManager.NativeManager.PopItemWidth();

			if (selectedTypeIndex == 0)
			{
				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.Text(Resources.GetString("XCount"));

				GUIManager.NativeManager.NextColumn();

				GUIManager.NativeManager.PushItemWidth(-1);
				if (GUIManager.NativeManager.DragInt("###tn", theNumberOfImageH_))
				{
					Core.Recording.RecordingHorizontalCount.SetValueDirectly(theNumberOfImageH_[0]);
				}
				GUIManager.NativeManager.PopItemWidth();
			}

			GUIManager.NativeManager.EndChild();

			// Recordingwindow
			GUIManager.NativeManager.BeginChild("##Option", new swig.Vec2(0, 60 * dpiScale), true, swig.WindowFlags.MenuBar);
			if (GUIManager.NativeManager.BeginMenuBar())
			{
				if (GUIManager.NativeManager.BeginMenu(optionsTitle + "##OptionTitle"))
				{
					GUIManager.NativeManager.EndMenu();
				}

				GUIManager.NativeManager.EndMenuBar();
			}

			GUIManager.NativeManager.Columns(2);
			GUIManager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			GUIManager.NativeManager.Text(Resources.GetString("MakeTransparent"));

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.PushItemWidth(-1);
			if (GUIManager.NativeManager.BeginCombo("###tr", selectedAlphaTypes[selectedAlphaIndex], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedAlphaTypes.Length; i++)
				{
					if (GUIManager.NativeManager.Selectable(selectedAlphaTypes[i]))
					{
						selectedAlphaIndex = i;
                        Core.Recording.RecordingTransparentMethod.SetValueDirectly((Data.RecordingTransparentMethodType)i);
                        GUIManager.NativeManager.SetItemDefaultFocus();
					}
				}

				GUIManager.NativeManager.EndCombo();
			}
			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.Columns(1);

			GUIManager.NativeManager.EndChild();

			// Storage
			GUIManager.NativeManager.BeginChild("##Storage", new swig.Vec2(0, 60 * dpiScale), true, swig.WindowFlags.MenuBar);
			GUIManager.NativeManager.Columns(2);
			GUIManager.NativeManager.SetColumnWidth(0, 120 * dpiScale);

			GUIManager.NativeManager.Text(Resources.GetString("StorageTarget"));

			GUIManager.NativeManager.NextColumn();

			GUIManager.NativeManager.PushItemWidth(-1);
			if (GUIManager.NativeManager.BeginCombo("###st", selectedStorageTargets[selectedStorageTarget], swig.ComboFlags.None))
			{
				for (int i = 0; i < selectedStorageTargets.Length; i++)
				{
					if (GUIManager.NativeManager.Selectable(selectedStorageTargets[i]))
					{
						selectedStorageTarget = i;
						Core.Recording.RecordingStorageTarget.SetValueDirectly((Data.RecordingStorageTargetTyoe)i);
						GUIManager.NativeManager.SetItemDefaultFocus();
					}
				}

				GUIManager.NativeManager.EndCombo();
			}
			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.Columns(1);

			GUIManager.NativeManager.EndChild();


			GUIManager.Viewer.SetViewerParamater(viewerParameter);

			float buttonWidth = 100 * dpiScale;
			GUIManager.NativeManager.SetCursorPosX(GUIManager.NativeManager.GetContentRegionAvail().X / 2 - buttonWidth / 2);

			if (GUIManager.NativeManager.Button(Resources.GetString("Record") + "###btn", buttonWidth))
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

				if (GUIManager.Viewer != null)
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

					var viewer = GUIManager.Viewer;
					var param = GUIManager.Viewer.GetViewerParamater();

					//if (viewer.LoadEffectFunc != null)
					{
						viewer.LoadEffectFunc();
					}

					var tempDirectory = System.IO.Directory.GetCurrentDirectory();
					System.IO.Directory.SetCurrentDirectory(Application.StartDirectory);

					Utils.Logger.Write(string.Format("SetCurrentDirectory : {0}", Application.StartDirectory));

					string errorMessage = string.Empty;

                    var startingFrame = Core.Recording.RecordingStartingFrame.Value;
                    var freq = Core.Recording.RecordingFrequency.Value;

                    var recordingParameter = new swig.RecordingParameter();

                    recordingParameter.Count = count;
                    recordingParameter.HorizontalCount = horizontalCount;
                    recordingParameter.Freq = freq;
                    recordingParameter.OffsetFrame = startingFrame;
                    recordingParameter.Transparence = (swig.TransparenceType)selectedAlphaIndex;
					recordingParameter.Scale = Core.Recording.RecordingScale.Value;

					if (Effekseer.Core.Language == Language.Japanese)
					{
						errorMessage = "保存に失敗しました。ファイルが他のアプリケーションで開かれている、もしくはスペックが足りません。";
					}
					else
					{
						errorMessage = "It failed to save. A file is opend by other application or lack of specification.";
					}

					bool recordResult = false;

					if (selectedTypeIndex == 0)
					{
						Utils.Logger.Write(string.Format("RecordSpriteSheet : {0}", filename));

						recordResult = viewer.RecordSpriteSheet(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 1)
					{
						Utils.Logger.Write(string.Format("RecordSprite : {0}", filename));

						recordResult = viewer.RecordSprite(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 2)
					{
						Utils.Logger.Write(string.Format("RecordAsGifAnimation : {0}", filename));

						recordResult = viewer.RecordAsGifAnimation(filename, recordingParameter);
					}
					else if (selectedTypeIndex == 3)
					{
						Utils.Logger.Write(string.Format("RecordAsAVI : {0}", filename));

						recordResult = viewer.RecordAsAVI(filename, recordingParameter);
					}

					if (recordResult)
					{
						GUIManager.NativeManager.OpenPopup("###RecorderProgress");
					}
					else
					{
						swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);

						Utils.Logger.Write(string.Format("Record : Failed : {0}", errorMessage));
					}

					System.IO.Directory.SetCurrentDirectory(tempDirectory);

				}
			}

			GUIManager.NativeManager.SetNextWindowSize(320, 0, swig.Cond.Always);

			UpdateProgressDialog();
		}

		private void UpdateProgressDialog()
		{
			if (GUIManager.NativeManager.BeginPopupModal(Resources.GetString("RecorderProgress") + "###RecorderProgress"))
			{
				float dpiScale = GUIManager.DpiScale;

				var viewer = GUIManager.Viewer;
				float progress;

				viewer.StepRecord(4);

				if (viewer.IsRecordCompleted())
				{
					viewer.EndRecord();
					progress = 1.0f;
					GUIManager.NativeManager.CloseCurrentPopup();
				}
				else
				{
					progress = viewer.GetRecordingProgress();
				}

				GUIManager.NativeManager.PushItemWidth(-1);
				
				GUIManager.NativeManager.ProgressBar(progress, new swig.Vec2(-1, 0));

				GUIManager.NativeManager.PopItemWidth();

				GUIManager.NativeManager.Spacing();

				float buttonWidth = 100 * dpiScale;
				GUIManager.NativeManager.SetCursorPosX(GUIManager.NativeManager.GetContentRegionAvail().X / 2 - buttonWidth / 2 + 8 * dpiScale);
				if (GUIManager.NativeManager.Button(Resources.GetString("RecorderAbort"), buttonWidth))
				{
					viewer.EndRecord();
					GUIManager.NativeManager.CloseCurrentPopup();
				}

				GUIManager.NativeManager.EndPopup();
			}
		}
	}
}
