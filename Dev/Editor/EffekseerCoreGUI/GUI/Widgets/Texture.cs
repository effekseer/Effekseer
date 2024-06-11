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
			public string id1 = "###" + Manager.GetUniqueID().ToString();
			public string id2 = "###" + Manager.GetUniqueID().ToString();
			public string idPopup = "###" + Manager.GetUniqueID().ToString();
			public string idReset = "###" + Manager.GetUniqueID().ToString();
		}

		public static Inspector.InspectorWidgetResult Update(object value, Inspector.WidgetState state)
		{
			var ret = new Inspector.InspectorWidgetResult();

			var appendedState = state.UserData as State;
			if (appendedState == null)
			{
				appendedState = new State();
				state.UserData = appendedState;
			}

			var texture = (Asset.TextureAsset)value;
			var previousTexture = texture;
			var thumbnail = appendedState.thumbnail;
			appendedState.isHovered = false;
			appendedState.isPopupShown = false;

			string dd = null;

			var size = Manager.NativeManager.GetContentRegionAvail();
			float itemSpacing = 4;
			float lineHeight = Manager.NativeManager.GetFrameHeight();
			float lineSpacing = Manager.NativeManager.GetFrameHeightWithSpacing() - lineHeight;
			float imageSize = lineHeight * 2 + lineSpacing;
			float buttonSizeX = Manager.NativeManager.GetTextLineHeightWithSpacing() * 2;

			float cursorX = Manager.NativeManager.GetCursorPosX();
			float cursorY = Manager.NativeManager.GetCursorPosY();

			Manager.NativeManager.BeginGroup();

			{
				if (thumbnail != null && thumbnail.Image != null)
				{
					Manager.NativeManager.ImageData(thumbnail.Image, imageSize, imageSize);
				}
				else
				{
					float x = Manager.NativeManager.GetCursorScreenPosX();
					float y = Manager.NativeManager.GetCursorScreenPosY();
					Manager.NativeManager.AddRectFilled(x, y, x + imageSize, y + imageSize, 0xff383838, 0, 0);
				}
			}

			{
				Manager.NativeManager.SetCursorPosX(cursorX + imageSize + itemSpacing);
				Manager.NativeManager.SetCursorPosY(cursorY);

				Manager.NativeManager.SetNextItemWidth(size.X - imageSize - buttonSizeX - itemSpacing * 2 - 1);
				Manager.NativeManager.InputText(appendedState.id1, appendedState.filePath, swig.InputTextFlags.ReadOnly);

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(appendedState.filePath);
				}

				Manager.NativeManager.SameLine();

				Manager.NativeManager.SetCursorPosX(cursorX + size.X - buttonSizeX - 1);
				Manager.NativeManager.SetCursorPosY(cursorY);
				if (Manager.NativeManager.IconButton(Icons.FileView))
				{
					btn_load_Click(ref texture, appendedState);
				}
			}

			{
				Manager.NativeManager.SetCursorPosX(cursorX + imageSize + itemSpacing);
				Manager.NativeManager.SetCursorPosY(cursorY + lineHeight + lineSpacing);

				// subparts
				{
					float width = size.X - imageSize - buttonSizeX - itemSpacing * 2 - 1;
					Manager.NativeManager.SetNextItemWidth(width);
					//Manager.NativeManager.Text(appendedState.infoText + appendedState.id2);
					Manager.NativeManager.InputText(appendedState.id2, appendedState.infoText, swig.InputTextFlags.ReadOnly);
				}

				if (thumbnail != null)
				{
					Manager.NativeManager.SameLine();

					Manager.NativeManager.SetCursorPosX(cursorX + size.X - buttonSizeX - 1);
					Manager.NativeManager.SetCursorPosY(cursorY + lineHeight + lineSpacing);

					if (Manager.NativeManager.IconButton(Icons.Remove))
					{
						btn_delete_Click(ref texture, appendedState);
					}
				}
			}

			Manager.NativeManager.SetCursorPosX(cursorX);
			Manager.NativeManager.SetCursorPosY(cursorY + imageSize);

			Manager.NativeManager.EndGroup();

			Popup(ref texture, appendedState);

			if (dd == null) dd = DragAndDrops.UpdateFileDst(FileType.Texture);
			appendedState.isHovered = appendedState.isHovered || Manager.NativeManager.IsItemHovered();

			if (dd != null)
			{
				Dropped(ref texture, appendedState, dd);
			}

			if (texture != previousTexture)
			{
				ret.value = texture;
				ret.isEdited = true;
			}

			return ret;
		}


		static void btn_load_Click(ref Asset.TextureAsset texture, State guiState)
		{
			var filter = MultiLanguageTextProvider.GetText("ImageFilter");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var loaded = CoreContext.Environment.GetAsset(result) as Asset.TextureAsset;

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

		static void btn_delete_Click(ref Asset.TextureAsset texture, State guiState)
		{
			texture = null;
			Read(texture, guiState);
		}

		static void Read(Asset.TextureAsset texture, State guiState)
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

		static void UpdateInfo(Asset.TextureAsset texture, State guiState)
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
			var filters = MultiLanguageTextProvider.GetText("ImageFilter").Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}

		static void Popup(ref Asset.TextureAsset texture, State guiState)
		{
			if (guiState.isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(guiState.idPopup))
			{
				if (WidgetHelper.ShowResetButton(guiState.idReset))
				{
					texture = null;
				}

				Manager.NativeManager.EndPopup();

				guiState.isPopupShown = true;
			}
		}

		public static Inspector.InspectorWidgetResult Dropped(object value, string path, Inspector.WidgetState state)
		{
			var ret = new Inspector.InspectorWidgetResult();

			var guiState = state.UserData as State;
			if (guiState == null)
			{
				guiState = new State();
				state.UserData = guiState;
			}

			if (value is Asset.TextureAsset texture)
			{
				var previousTexture = texture;

				if (guiState.isHovered)
				{
					Dropped(ref texture, guiState, path);
					ret.isHovered = true;
				}

				if (texture != previousTexture)
				{
					ret.value = texture;
					ret.isEdited = true;
				}
			}

			return ret;
		}


		static void Dropped(ref Asset.TextureAsset texture, State guiState, string path)
		{
			if (CheckExtension(path))
			{
				var loaded = CoreContext.Environment.GetAsset(path) as Asset.TextureAsset;

				if (loaded != null)
				{
					texture = loaded;
					Read(texture, guiState);
				}
			}
		}
	}
}
