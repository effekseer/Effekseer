using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Path
	{
		string _abspath = string.Empty;

		/// <summary>
		/// 相対パスで保存されるか?
		/// </summary>
		public bool IsRelativeSaved
		{
			get;
			private set;
		}

		public string Filter
		{
			get;
			private set;
		}

		public string AbsolutePath
		{
			get
			{
				return GetAbsolutePath();
			}

			set
			{
				SetAbsolutePath(value);
			}
		}

		public string RelativePath
		{
			get
			{
				return GetRelativePath();
			}
			set
			{
				SetRelativePath(value);
			}
		}

		public event ChangedValueEventHandler OnChanged;

		public string DefaultValue { get; private set; }

		internal Path(string filter, bool isRelativeSaved = true, string abspath = "")
		{
			Filter = filter;
			IsRelativeSaved = isRelativeSaved;
			_abspath = abspath;

			DefaultValue = _abspath;
		}

		public string GetAbsolutePath()
		{
			return _abspath;
		}

		public void SetAbsolutePath(string abspath)
		{
			if (abspath == _abspath) return;

			var old_value = _abspath;
			var new_value = abspath;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_abspath = new_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					_abspath = old_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		public string GetRelativePath()
		{
			if (_abspath == string.Empty) return _abspath;
			if (Core.FullPath == string.Empty) return _abspath;

			Uri basepath = new Uri(Core.FullPath);
			Uri path = new Uri(_abspath);
			var relative_path = basepath.MakeRelativeUri(path).ToString();

            return relative_path;
		}

		public void SetRelativePath(string relative_path)
		{
			try
			{
				if (Core.FullPath == string.Empty)
				{
					SetAbsolutePath(relative_path);
					return;
				}

				if (relative_path == string.Empty)
				{
					SetAbsolutePath(string.Empty);
				}
				else
				{
					Uri basepath = new Uri(Core.FullPath);
					Uri path = new Uri(basepath, relative_path);
					var absolute_path = path.LocalPath;
					SetAbsolutePath(absolute_path);
				}
			}
			catch (Exception e)
			{
				throw new Exception(e.ToString() + " Core.FullPath = " + Core.FullPath );
			}
		}

		internal void SetAbsolutePathDirectly(string abspath)
		{
			if (_abspath == abspath) return;

			_abspath = abspath;
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(_abspath, ChangedValueType.Execute));
			}
		}

		internal void SetRelativePathDirectly(string relative_path)
		{
			if (Core.FullPath == string.Empty) SetAbsolutePathDirectly(relative_path);
			Uri basepath = new Uri(Core.FullPath);
			Uri path = new Uri(basepath, relative_path);
			var absolute_path = path.LocalPath;
			SetAbsolutePathDirectly(absolute_path);
		}
	}
}
