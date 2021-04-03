using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Path : IResettableValue
	{
		Path _basepath;
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

		internal Path(Path basepath, string filter, bool isRelativeSaved = true, string abspath = "")
		{
			//if (basepath == null && isRelativeSaved)
			//{
			//	throw new Exception("required basepath");
			//
			//}

			Filter = filter;
			IsRelativeSaved = isRelativeSaved;
			_basepath = basepath;
			_abspath = abspath;

			DefaultValue = _abspath;

#if DEBUG
			if(abspath != string.Empty && !System.IO.Path.IsPathRooted(abspath))
			{
				throw new Exception(string.Format("{0} is not absolute path", abspath));
			}
#endif
		}

		public string GetAbsolutePath()
		{
			return _abspath;
		}

		public void SetAbsolutePath(string abspath)
		{
			if (abspath == _abspath) return;

			// Replace separators
			abspath = abspath.Replace('\\', '/');

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
			if (_basepath == null || _basepath.AbsolutePath == string.Empty) return _abspath;
			return Utils.Misc.GetRelativePath(_basepath.AbsolutePath, _abspath);
		}

		public void SetRelativePath(string relative_path)
		{
			// Replace separators
			relative_path = relative_path.Replace('\\', '/');

			try
			{
				if (_basepath == null || _basepath.AbsolutePath == string.Empty)
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
					SetAbsolutePath(Utils.Misc.GetAbsolutePath(_basepath.AbsolutePath, relative_path));
				}
			}
			catch (FileLoadPermissionException e)
			{
				throw e;
			}
			catch (Exception e)
			{
				throw new Exception(e.ToString() + " relative_path = " + relative_path);
			}
		}

		public void ResetValue()
		{
			SetAbsolutePath(DefaultValue);
		}

		internal void SetDefaultAbsolutePath(string abspath)
		{
			DefaultValue = abspath;
		}

		internal void SetAbsolutePathDirectly(string abspath, bool skipCallback = false)
		{
			if (_abspath == abspath) return;

			_abspath = abspath;
			if (!skipCallback && OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(_abspath, ChangedValueType.Execute));
			}
		}

		internal void SetRelativePathDirectly(string relative_path, bool skipCallback = false)
		{
			if (_basepath.AbsolutePath == string.Empty) SetAbsolutePathDirectly(relative_path);
			Uri basepath = new Uri(_basepath.AbsolutePath);
			Uri path = new Uri(basepath, relative_path);
			var absolute_path = path.LocalPath;
			SetAbsolutePathDirectly(absolute_path, skipCallback);
		}
	}
}
