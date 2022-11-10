using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

namespace Effekseer.GUI.Widgets
{
	class Texture
	{
		class State
		{
			public bool isHovered = false;
			public bool isPopupShown = false;
			public string filePath = string.Empty;
			public string infoText = string.Empty;
			public Thumbnail thumbnail = null;
			public string ID_Popup = "###" + Manager.GetUniqueID().ToString();
			public string ID_Reset = "###" + Manager.GetUniqueID().ToString();
		}

		public static Inspector.InspectorGuiResult Update(object value, Inspector.InspectorGuiState state)
		{
			var ret = new Inspector.InspectorGuiResult();

			var guiState = state.UserData as State;
			if (guiState == null)
			{
				guiState = new State();
				state.UserData = guiState;
			}

			var texture = (EffectAsset.TextureAsset)value;

			{
				var previousTexture = texture;
				guiState.isHovered = false;
				guiState.isPopupShown = false;

				string dd = null;

				float buttonSizeX = Manager.NativeManager.GetTextLineHeightWithSpacing() * 2;

				if (Manager.NativeManager.Button(Resources.GetString("Load") + state.Id + "_Load", buttonSizeX))
				{
					btn_load_Click(ref texture, guiState);
				}

				Popup(ref texture, guiState);

				if (dd == null) dd = DragAndDrops.UpdateFileDst(FileType.Texture);

				guiState.isHovered = guiState.isHovered || Manager.NativeManager.IsItemHovered();

				Manager.NativeManager.SameLine();

				Manager.NativeManager.Text(guiState.filePath);

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(guiState.filePath);
				}

				if (dd == null) dd = DragAndDrops.UpdateFileDst(FileType.Texture);

				guiState.isHovered = guiState.isHovered || Manager.NativeManager.IsItemHovered();

				if (guiState.thumbnail != null)
				{
					if (Manager.NativeManager.Button(Resources.GetString("Delete") + state.Id + "_Delete", buttonSizeX))
					{
						btn_delete_Click(ref texture, guiState);
					}

					Popup(ref texture, guiState);

					if (dd == null) dd = DragAndDrops.UpdateFileDst(FileType.Texture);

					guiState.isHovered = guiState.isHovered || Manager.NativeManager.IsItemHovered();

					Manager.NativeManager.SameLine();

					Manager.NativeManager.Text(guiState.infoText);

					Popup(ref texture, guiState);

					if (dd == null) dd = DragAndDrops.UpdateFileDst(FileType.Texture);

					guiState.isHovered = guiState.isHovered || Manager.NativeManager.IsItemHovered();

					if (guiState.thumbnail != null)
					{
						var image = guiState.thumbnail.Image;
						float imageSizeX = image.GetWidth();
						float imageSizeY = image.GetHeight();
						if (imageSizeX < imageSizeY)
						{
							Manager.NativeManager.ImageData(image, 128 * imageSizeX / imageSizeY, 128);
						}
						else if (imageSizeX > imageSizeY)
						{
							Manager.NativeManager.ImageData(image, 128, 128 * imageSizeY / imageSizeX);
						}
						else
						{
							Manager.NativeManager.ImageData(image, 128, 128);
						}

						Popup(ref texture, guiState);
					}

					if (dd == null) dd = DragAndDrops.UpdateFileDst(FileType.Texture);

					guiState.isHovered = guiState.isHovered || Manager.NativeManager.IsItemHovered();
				}

				if (dd != null)
				{
					Dropped(ref texture, guiState, dd);
				}

				if (texture != previousTexture)
				{
					ret.value = texture;
					ret.isEdited = true;
				}
			}

			return ret;
		}


		static void btn_load_Click(ref EffectAsset.TextureAsset texture, State guiState)
		{
			var filter = Resources.GetString("ImageFilter");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var loaded = CoreContext.Environment.GetAsset(result) as EffectAsset.TextureAsset;

				if (loaded != null)
				{
					var filepath = result;
					System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
					texture = loaded;
				}
			}
			else
			{
				texture = null;
			}

			Read(texture, guiState);
		}

		static void btn_delete_Click(ref EffectAsset.TextureAsset texture, State guiState)
		{
			texture = null;
			Read(texture, guiState);
		}

		static void Read(EffectAsset.TextureAsset texture, State guiState)
		{
			if (texture != null)
			{
				guiState.filePath = CoreContext.Environment.GetAssetPath(texture);
				UpdateInfo(texture, guiState);
			}
			else
			{
				guiState.filePath = string.Empty;
				UpdateInfo(texture, guiState);
			}
		}

		static void UpdateInfo(EffectAsset.TextureAsset texture, State guiState)
		{
			string path = CoreContext.Environment.GetAssetPath(texture);

			if (System.IO.File.Exists(path))
			{
				guiState.thumbnail = ThumbnailManager.Load(path);
				if (guiState.thumbnail == null)
				{
					guiState.infoText = "";
					return;
				}

				var image = guiState.thumbnail.Image;
				guiState.infoText = "" + image.GetWidth() + "x" + image.GetHeight();
			}
			else
			{
				guiState.thumbnail = null;
				guiState.infoText = "";
			}
		}

		static bool CheckExtension(string path)
		{
			var filters = Resources.GetString("ImageFilter").Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}

		static void Popup(ref EffectAsset.TextureAsset texture, State guiState)
		{
			if (guiState.isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(guiState.ID_Popup))
			{
				if (WidgetHelper.ShowResetButton(guiState.ID_Reset))
				{
					texture = null;
				}

				Manager.NativeManager.EndPopup();

				guiState.isPopupShown = true;
			}
		}

		public static Inspector.InspectorGuiResult Dropped(object value, string path, Inspector.InspectorGuiState state)
		{
			var ret = new Inspector.InspectorGuiResult();

			var guiState = state.UserData as State;
			if (guiState == null)
			{
				guiState = new State();
				state.UserData = guiState;
			}

			if (value is EffectAsset.TextureAsset texture)
			{
				var previousTexture = texture;

				if (guiState.isHovered)
				{
					Dropped(ref texture, guiState, path);
				}

				if (texture != previousTexture)
				{
					ret.value = texture;
					ret.isEdited = true;
				}
			}

			return ret;
		}


		static void Dropped(ref EffectAsset.TextureAsset texture, State guiState, string path)
		{
			if (CheckExtension(path))
			{
				var loaded = CoreContext.Environment.GetAsset(path) as EffectAsset.TextureAsset;

				if (loaded != null)
				{
					texture = loaded;
					Read(texture, guiState);
				}
			}
		}
	}
}
