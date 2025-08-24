using System;
using System.Collections.Generic;
using System.Linq;

namespace Effekseer.GUI.BindableComponent
{
	class PathBase : Control, IParameterControl
	{
		protected string id1 = "";
		protected string id2 = "";
		protected string id_c = "";
		protected string id_reset = "";

		protected Data.Value.Path binding = null;

		protected string filter = "*";
		protected FileType fileType = FileType.Other;
		protected string filePath = string.Empty;
		protected bool isHovered = false;

		protected bool isPopupShown = false;

		protected Thumbnail thumbnail = null;
		public bool EnableUndo { get; set; } = true;

		public Data.Value.Path Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value)
				{
					return;
				}

				if (value == null)
				{
					binding.OnChanged -= Binding_OnChanged;
					binding = null;
					return;
				}

				binding = value;
				binding.OnChanged += Binding_OnChanged;
				Read();
			}
		}

		private void Binding_OnChanged(object sender, ChangedValueEventArgs e)
		{
			Read();
		}

		public PathBase()
		{
			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_reset = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Path;
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

		public void Dropped(string path)
		{
			if (CheckExtension(path))
			{
				binding.SetAbsolutePath(path);
				Read();

				LoadFile(path, false);
			}
		}

		public override void Update()
		{
			isHovered = false;
			isPopupShown = false;

			if (binding == null) return;

			string dd = null;

			var size = Manager.NativeManager.GetContentRegionAvail();
			float itemSpacing = 4;
			float lineHeight = Manager.NativeManager.GetFrameHeight();
			float lineSpacing = Manager.NativeManager.GetFrameHeightWithSpacing() - lineHeight;
			float imageSize = lineHeight * 2 + lineSpacing;
			float buttonSizeX = lineHeight;

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
				Manager.NativeManager.InputText(id1, filePath, swig.InputTextFlags.ReadOnly);

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(filePath);
				}

				Manager.NativeManager.SameLine();

				Manager.NativeManager.SetCursorPosX(cursorX + size.X - buttonSizeX - 1);
				Manager.NativeManager.SetCursorPosY(cursorY);
				if (Manager.NativeManager.IconButton(Icons.FileView))
				{
					btn_load_Click();
				}
			}

			{
				Manager.NativeManager.SetCursorPosX(cursorX + imageSize + itemSpacing);
				Manager.NativeManager.SetCursorPosY(cursorY + lineHeight + lineSpacing);

				UpdateSubParts(size.X - imageSize - buttonSizeX - itemSpacing * 2 - 1);

				if (thumbnail != null)
				{
					Manager.NativeManager.SameLine();

					Manager.NativeManager.SetCursorPosX(cursorX + size.X - buttonSizeX - 1);
					Manager.NativeManager.SetCursorPosY(cursorY + lineHeight + lineSpacing);

					if (Manager.NativeManager.IconButton(Icons.Remove))
					{
						btn_delete_Click();
					}
				}
			}

			Manager.NativeManager.SetCursorPosX(cursorX);
			Manager.NativeManager.SetCursorPosY(cursorY + imageSize);

			Manager.NativeManager.EndGroup();

			Popup();

			if (dd == null) dd = DragAndDrops.UpdateFileDst(fileType);
			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (dd != null)
			{
				Dropped(dd);
			}
		}

		protected virtual void UpdateSubParts(float width)
		{
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;
				binding.SetAbsolutePath(filepath);

				LoadFile(filepath, false);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
			}
			else
			{
				return;
			}

			Read();
		}

		private void btn_delete_Click()
		{
			binding.SetAbsolutePath("");
			Read();
		}

		protected void Read()
		{
			if (binding != null)
			{
				filePath = binding.GetRelativePath();
				UpdateInfo();
			}
			else
			{
				filePath = string.Empty;
				UpdateInfo();
			}
		}

		protected virtual void LoadFile(string filepath, bool isReloading)
		{
		}

		protected virtual void UpdateInfo()
		{
			string path = binding.GetAbsolutePath();

			if (System.IO.File.Exists(path))
			{
				thumbnail = ThumbnailManager.Load(path);
				if (thumbnail == null)
				{
					return;
				}
			}
			else
			{
				thumbnail = null;
			}
		}

		protected virtual bool CheckExtension(string path)
		{
			var filters = filter.Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				Functions.ShowReset(binding, id_reset);

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}