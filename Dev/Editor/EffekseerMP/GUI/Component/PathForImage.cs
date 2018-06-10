using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace Effekseer.GUI.Component
{
	class PathForImage : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.PathForImage binding = null;

		string filePath = string.Empty;
		string infoText = string.Empty;
		bool isHovered = false;

		swig.ImageResource image = null;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.PathForImage Binding
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

				if(value == null)
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

		public PathForImage(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.PathForImage;
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
			}
		}

		public override void Update()
		{
			isHovered = false;

			if (binding == null) return;

			string dd = null;

			if (Manager.NativeManager.Button(Resources.GetString("Load")))
			{
				btn_load_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(filePath);

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (Manager.NativeManager.Button(Resources.GetString("Delete")))
			{
				btn_delete_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(infoText);

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if(image != null)
			{
				Manager.NativeManager.Image(image, 128, 128);

				if (dd == null) dd = DragAndDrops.UpdateImageDst();

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			}

			if (dd != null)
			{
				Dropped(dd);
			}
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

			var filter = Resources.GetString("ImageFilter");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;
				binding.SetAbsolutePath(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
			}
			else
			{
				return;
			}

				/*
				OpenFileDialog ofd = new OpenFileDialog();

				ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
				ofd.Filter = binding.Filter;
				ofd.FilterIndex = 2;
				ofd.Multiselect = false;

				if (ofd.ShowDialog() == DialogResult.OK)
				{
					var filepath = ofd.FileName;
					binding.SetAbsolutePath(filepath);

					System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
				}
				else
				{
					return;
				}
				*/

				Read();
		}

		private void btn_delete_Click()
		{
			binding.SetAbsolutePath("");
			Read();
		}

		void Read()
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

		void UpdateInfo()
		{
			string path = binding.GetAbsolutePath();

			if(System.IO.File.Exists(path))
			{
				image = Images.Load(Manager.Native, path);
			}
			else
			{
				image = null;
			}
		}

		private bool CheckExtension(string path)
		{
			Match match = Regex.Match(binding.Filter, "\\*(\\.[a-zA-Z0-9]*)");
			string extension = match.Value.Substring(1);
			return System.IO.Path.GetExtension(path) == extension;
		}
	}
}
