using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Media;
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

		public bool EnableUndo { get; set; } = true;

		public Data.Value.PathForImage Binding
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

		public override void Update()
		{
			isHovered = false;

			if (binding == null) return;

			string dd = null;

			if (Manager.NativeManager.Button("読込"))
			{
				btn_load_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(filePath);

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (Manager.NativeManager.Button("解除"))
			{
				btn_delete_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(infoText);

			if (dd == null) dd = DragAndDrops.UpdateImageDst();

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if(dd != null)
			{
				bool handled = false;
				OnDropped(dd, ref handled);
			}
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

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
				if (filePath.Length > 0)
				{
					UpdateInfo();
				}
				else
				{
				}
			}
			else
			{
				filePath = string.Empty;
			}
		}

		void UpdateInfo()
		{
			string path = binding.GetAbsolutePath();

			try
			{
				Console.WriteLine("Not implemented.");
			}
			catch (Exception e)
			{
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
