using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Effekseer.GUI.Dock
{
	class FileViewer : DockPanel
	{
		string currentPath = null;
		List<FileItem> items = new List<FileItem>();
		int selectedIndex = -1;

		public FileViewer()
		{
			Label = Resources.GetString("FileViewer");
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;

			if (!String.IsNullOrEmpty(Core.FullPath)) {
				UpdateFileList(Path.GetDirectoryName(Core.FullPath));
			}
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
		}

		protected override void UpdateInternal()
		{
			Manager.NativeManager.PushItemWidth(-1);

			// Address bar
			{
				if (Manager.NativeManager.Button("↑") &&
					!String.IsNullOrEmpty(currentPath))
				{
					UpdateFileList(Path.GetDirectoryName(currentPath));
				}
			
				Manager.NativeManager.SameLine();

				// Display current directory
				if (Manager.NativeManager.InputText("", (currentPath != null) ? currentPath : ""))
				{
					string path = Manager.NativeManager.GetInputTextResult();
					UpdateFileList(path);
				}
			}

			Manager.NativeManager.Separator();

			// Display all files
			for(int i = 0; i < items.Count; i++)
			{
				var item = items[i];

				// Todo: ファイルタイプに適したアイコンを割り当てる
				
				string caption = Path.GetFileName(item.FilePath);
				if (Manager.NativeManager.Selectable(caption, i == selectedIndex, swig.SelectableFlags.AllowDoubleClick))
				{
					selectedIndex = i;

					if(Manager.NativeManager.IsMouseDoubleClicked(0))
					{
						DoubleClick();
					}
				}
				
				// D&D
				DragAndDrops.UpdateImageSrc(caption);
			}
			
			Manager.NativeManager.PopItemWidth();
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			UpdateFileList(Path.GetDirectoryName(Core.FullPath));
		}

		/// <summary>
		/// A file in the list
		/// </summary>
		public class FileItem
		{
			/// <summary>
			/// File path
			/// </summary>
			public string FilePath { get; set; }

			public FileItem(string name, string filePath)
			{
				FilePath = filePath;
			}
		}

		/// <summary>
		/// Update list
		/// </summary>
		private void UpdateFileList(string path)
		{
			if (!Directory.Exists(path))
			{
				return;
			}

			selectedIndex = -1;
			currentPath = path;
			items.Clear();

			//string parentDirectory = System.IO.Path.GetDirectoryName(path);
			//if (parentDirectory != null)
			//{
			//	items.Add(new FileItem("Parent directory", Path.GetDirectoryName(path)));
			//}

			// add directories
			foreach (string dirPath in Directory.EnumerateDirectories(path))
			{
				//int imageIndex = GetImageIndexFileIcon(dirPath);
				var dirNode = new FileItem(Path.GetFileName(dirPath), dirPath);
				items.Add(dirNode);
			}

			// add files
			foreach (string filePath in Directory.EnumerateFiles(path))
			{
				//int imageIndex = GetImageIndexFileIcon(filePath);
				var fileNode = new FileItem(Path.GetFileName(filePath), filePath);
				items.Add(fileNode);
			}
		}

		void DoubleClick()
		{
			if (items.Count == 0)
			{
				return;
			}

			if (selectedIndex == -1) return;

			var fileItem = items[selectedIndex];
			if (Path.GetExtension(fileItem.FilePath) == ".efkproj")
			{
				// efkproj is opened internal function
				Commands.Open(fileItem.FilePath);
			}
			else if (Directory.Exists(fileItem.FilePath))
			{
				// move directory
				UpdateFileList(fileItem.FilePath);
			}
			else
			{
				// open process
				System.Diagnostics.Process.Start(fileItem.FilePath);
			}
		}
	}
}

/*
 using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Effekseer.GUI
{
	public partial class DockFileViewer : DockContent
	{
		private string currentPath;
		private Dictionary<string, int> extensionsIcon = new Dictionary<string, int>();

		public DockFileViewer()
		{
			InitializeComponent();

            Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconFileViewer).GetHicon());
        }
		
		class Win32
		{
			[StructLayout(LayoutKind.Sequential)]
			public struct SHFILEINFO 
			{
				public IntPtr hIcon;
				public IntPtr iIcon;
				public uint dwAttributes;
				[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
				public string szDisplayName;
				[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
				public string szTypeName;
			};

			public const uint SHGFI_ICON = 0x100;
			public const uint SHGFI_LARGEICON = 0x0; // 'Large icon
			public const uint SHGFI_SMALLICON = 0x1; // 'Small icon
		
			[DllImport("shell32.dll")]
			public static extern IntPtr SHGetFileInfo(string pszPath, uint dwFileAttributes, ref SHFILEINFO psfi, uint cbSizeFileInfo, uint uFlags);
			
			[StructLayoutAttribute(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
			public struct SHSTOCKICONINFO
			{
			  public Int32 cbSize;
			  public IntPtr hIcon;
			  public Int32 iSysImageIndex;
			  public Int32 iIcon;
			  [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
			  public string szPath;
			}
			[DllImport("shell32.dll")]
			public static extern int SHGetStockIconInfo(UInt32 siid, UInt32 uFlags, ref SHSTOCKICONINFO sii);
		}

		// Windowsシェルアイコンの取得
		private Icon GetStockIcon(uint siid) {
			var shinfo = new Win32.SHSTOCKICONINFO();
			shinfo.cbSize = Marshal.SizeOf(shinfo);
			int result = Win32.SHGetStockIconInfo(siid, Win32.SHGFI_ICON, ref shinfo);
			if (result == 0) {
				return Icon.FromHandle(shinfo.hIcon);
			}
			return null;
		}
		
		// ファイルに関連付くアイコンの取得
		private Icon GetFileIcon(string path) {
			var shinfo = new Win32.SHFILEINFO();
			IntPtr hSuccess = Win32.SHGetFileInfo(path, 0, ref shinfo, (uint)Marshal.SizeOf(shinfo), Win32.SHGFI_ICON);
			if (hSuccess != IntPtr.Zero) {
				return Icon.FromHandle(shinfo.hIcon);
			}
			return null;
		}

		// ファイルに関連付くアイコンの取得
		private Icon GetImagePreview(string path) {
			Image image = Image.FromFile(path);
			if (image != null) {
				return Icon.FromHandle(((Bitmap)image).GetHicon());
			}
			return null;
		}
		
		private int GetImageIndexFileIcon(string path)
		{
			string ext = Path.GetExtension(path);

			int imageIndex = -1;
			if (ext == ".png") {
				// プレビュー画像を取得(変更があるかもしれないので毎回取得する)
				Icon icon = GetImagePreview(path);
				if (icon != null) {
					imageIndex = imageList.Images.Count;
					imageList.Images.Add(icon);
					if (extensionsIcon.ContainsKey(path)) {
						extensionsIcon[path] = imageIndex;
					} else {
						extensionsIcon.Add(path, imageIndex);
					}
				}
			} else {
				// その他のファイルタイプはアイコンを表示
				if (extensionsIcon.ContainsKey(ext)) {
					imageIndex = extensionsIcon[ext];
				} else {
					// アイコンを取得
					Icon icon = GetFileIcon(path);
					if (icon != null) {
						imageIndex = imageList.Images.Count;
						imageList.Images.Add(icon);
						extensionsIcon.Add(ext, imageIndex);
					}
				}
			}
			return imageIndex;
		}	
		
		// リストビューのファイルアイテム
		public class FileItem : ListViewItem
		{
			// ファイルパス
			public string FilePath {get; set;}

			public FileItem(string name, string filePath, int imageIndex)
				: base(name, imageIndex)
			{
				FilePath = filePath;
			}
		}
		
		private void UpdateFileList()
		{
			if (String.IsNullOrEmpty(Core.FullPath)) {
				fileView.Items.Clear();
				currentPath = null;
				return;
			}

			// ディレクトリ、ファイルを列挙
			UpdateFileListItems(Path.GetDirectoryName(Core.FullPath));
		}
		
		private void UpdateFileListItems(string path)
		{
			currentPath = path;
			
			fileView.Items.Clear();
			string parentDirectory = Path.GetDirectoryName(path);
			if (parentDirectory != null) {
				fileView.Items.Add(new FileItem("Parent directory", Path.GetDirectoryName(path), 0));
			}
			// ディレクトリを追加
			foreach (string dirPath in Directory.EnumerateDirectories(path)) {
				int imageIndex = GetImageIndexFileIcon(dirPath);
				var dirNode = new FileItem(Path.GetFileName(dirPath), dirPath, 1);
				fileView.Items.Add(dirNode);
			}
			// ファイルを追加
			foreach (string filePath in Directory.EnumerateFiles(path)) {
				int imageIndex = GetImageIndexFileIcon(filePath);
				var fileNode = new FileItem(Path.GetFileName(filePath), filePath, imageIndex);
				fileView.Items.Add(fileNode);
			}
		}

		private void DockFileViewer_Load(object sender, EventArgs e)
		{
			contextMenuStrip.ResumeLayout();
			contextMenuStrip.PerformLayout();

			imageList.Images.Add(GetStockIcon(45));		// To Parent Directory
			imageList.Images.Add(GetStockIcon(3));		// Directory

			Core.OnAfterLoad += Core_OnAfterLoad;
			Core.OnAfterSave += Core_OnAfterSave;
			Core.OnAfterNew += Core_OnAfterNew;
			Core.OnReload += Core_OnReload;

			UpdateFileList();
		}

		private void DockFileViewer_FormClosed(object sender, FormClosedEventArgs e)
		{
			Core.OnAfterLoad -= Core_OnAfterLoad;
			Core.OnAfterSave -= Core_OnAfterSave;
			Core.OnAfterNew -= Core_OnAfterNew;
			Core.OnReload -= Core_OnReload;
		}

		private void fileView_DoubleClick(object sender, EventArgs e)
		{
			if (fileView.SelectedItems.Count == 0) {
				return;
			}

			var item = fileView.SelectedItems[0];
			if (item.GetType() != typeof(FileItem)) {
				return;
			}

			var fileItem = (FileItem)item;
			if (Path.GetExtension(fileItem.FilePath) == ".efkproj") {
				// efkprojのときはこのプロセスで開く
				Commands.Open(fileItem.FilePath);
			} else if (Directory.Exists(fileItem.FilePath)) {
				// ディレクトリの場合は移動
				UpdateFileListItems(fileItem.FilePath);
			} else {
				// その他の場合はシェルで開く
				System.Diagnostics.Process.Start(fileItem.FilePath);
			}
		}
		
		void Core_OnAfterLoad(object sender, EventArgs e)
		{
			UpdateFileList();
		}

		void Core_OnAfterSave(object sender, EventArgs e)
		{
			UpdateFileList();
		}

		void Core_OnAfterNew(object sender, EventArgs e)
		{
			UpdateFileList();
		}
		
		void Core_OnReload(object sender, EventArgs e)
		{
			UpdateFileList();
		}

		private void fileView_ItemDrag(object sender, ItemDragEventArgs e)
		{
			fileView.DoDragDrop(e.Item, DragDropEffects.Link);
		}
	}
}

	 */
