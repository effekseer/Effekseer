#if SCRIPT_ENABLED
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.CSharp;
using System.CodeDom.Compiler;

using sh = Microsoft.Scripting.Hosting;
using ph = IronPython.Hosting;
using pr = IronPython.Runtime;

namespace Effekseer.Script
{
	class Compiler
	{
		enum ScriptLanguage
		{
			CSharp,
			Python,
			Ruby,
			Dll,
			Unknown,
		}

		static CSharpCodeProvider CSharpCodeProvider
		{
			get;
			set;
		}

		static CompilerParameters CompilerParameters
		{
			get;
			set;
		}

		static sh.ScriptEngine PythonEngine
		{
			get;
			set;
		}


		static public void Initialize()
		{
			var entryDirectory = Core.GetEntryDirectory() + "/";

			CSharpCodeProvider = new CSharpCodeProvider(new Dictionary<string, string> { { "CompilerVersion", "v4.0" } });
			CompilerParameters = new CompilerParameters();
			CompilerParameters.GenerateInMemory = true;
			CompilerParameters.GenerateExecutable = false;
			CompilerParameters.ReferencedAssemblies.Add("System.dll");
			CompilerParameters.ReferencedAssemblies.Add("System.Drawing.dll");
			CompilerParameters.ReferencedAssemblies.Add("System.Windows.Forms.dll");

			if(System.IO.File.Exists(entryDirectory + "Effekseer.exe"))
			{
				CompilerParameters.ReferencedAssemblies.Add(entryDirectory + "Effekseer.exe");
			}

			if (System.IO.File.Exists(entryDirectory + "EffekseerInterface.dll"))
            {
                CompilerParameters.ReferencedAssemblies.Add(entryDirectory + "EffekseerInterface.dll");
            }

			CompilerParameters.ReferencedAssemblies.Add(entryDirectory + "EffekseerCore.dll");
			PythonEngine = ph.Python.CreateEngine();
		}

		static public void Dispose()
		{
			if (CSharpCodeProvider != null)
			{
				CSharpCodeProvider.Dispose();
				CSharpCodeProvider = null;
			}

			if (PythonEngine != null)
			{
				PythonEngine.Runtime.Shutdown();
				PythonEngine = null;
			}
		}

		static ScriptLanguage Compile(string path, out object compiled, out string error)
		{ 
			if (!System.IO.File.Exists(path))
			{
				compiled = null;
				error = "Not found : " + path;
				return ScriptLanguage.Unknown;
			}

			string ext = System.IO.Path.GetExtension(path);


			if (ext == ".cs")
			{
				var entryDirectory = Core.GetEntryDirectory() + "/";

				// C#
				var lines = System.IO.File.ReadAllText(path);

				CompilerResults result = null;

				try
				{
					result = CSharpCodeProvider.CompileAssemblyFromSource(CompilerParameters, lines);
				}
				catch (Exception e)
				{
					compiled = null;
					error = "CSharp Compile Exception : " + path + "\n";
					error += e.ToString();
					return ScriptLanguage.Unknown;
				}

				if (result.Errors.HasErrors)
				{
					compiled = null;
					error = "CSharp Compile Error : " + path + "\n";
					for (int l = 0; l < result.Errors.Count; l++)
					{
						error += result.Errors[l] + "\n";
					}
					
					return ScriptLanguage.Unknown;
				}

				error = string.Empty;
				compiled = result;
				return ScriptLanguage.CSharp;
			}
			else if(ext == ".py")
			{
				// Python
				var scope = PythonEngine.CreateScope();
				try
				{
					var source = PythonEngine.CreateScriptSourceFromFile(path);
					var cc = source.Compile();
					cc.Execute(scope);
				}
				catch (Microsoft.Scripting.SyntaxErrorException e)
				{
					Microsoft.Scripting.Hosting.ExceptionOperations eo = null;

					eo = PythonEngine.GetService<Microsoft.Scripting.Hosting.ExceptionOperations>();

					compiled = null;
					error = "Python Compile Error : " + path + "\n";
					error += eo.FormatException(e);

					return ScriptLanguage.Unknown;
				}
				catch (Exception e)
				{
					compiled = null;
					error = e.ToString();
					return ScriptLanguage.Unknown;
				}
				
				error = string.Empty;
				compiled = scope;
				return ScriptLanguage.Python;
			}

			compiled = null;
			error = "Undefined Extention Error: " + path+ "\n";
			return ScriptLanguage.Unknown;
		}

		static public bool CompileScript<T>(string path, out T script, out string error) where T : ScriptBase
		{
			object compiled = null;
			var lang = Compile(path, out compiled, out error);
			if (lang == ScriptLanguage.Unknown)
			{
				script = null;
				return false;
			}

			if (lang == ScriptLanguage.CSharp || lang == ScriptLanguage.Dll)
			{
				System.Reflection.Assembly result = null;
				if (lang == ScriptLanguage.CSharp)
				{
					result = (compiled as CompilerResults).CompiledAssembly;
				}
				else if (lang == ScriptLanguage.Dll)
				{
					result = compiled as System.Reflection.Assembly;
				}
				
				try
				{
					Type type = result.GetType("Script");
					string uniquename = (string)type.GetProperty("UniqueName").GetValue(null, null);
					string author = (string)type.GetProperty("Author").GetValue(null, null);
					string title = (string)type.GetProperty("Title").GetValue(null, null);
					string description = (string)type.GetProperty("Description").GetValue(null, null);

					script = null;
					if (typeof(T) == typeof(CommandScript))
					{
						Action function = Delegate.CreateDelegate(typeof(Action), type.GetMethod("Call")) as Action;
						script = (new CommandScript(Script.ScriptPosition.External, uniquename, author, title, description, function)) as T;
					}
					if (typeof(T) == typeof(SelectedScript))
					{
						Action<Data.NodeBase, Object> function = Delegate.CreateDelegate(typeof(Action<Data.NodeBase, Object>), type.GetMethod("Call")) as Action<Data.NodeBase, Object>;
						script = (new SelectedScript(Script.ScriptPosition.External, uniquename, author, title, description, function)) as T;
					}
					if (typeof(T) == typeof(ExportScript))
					{
						string filter = (string)type.GetProperty("Filter").GetValue(null, null);
						Action<string> function = Delegate.CreateDelegate(typeof(Action<string>), type.GetMethod("Call")) as Action<string>;
						script = (new ExportScript(Script.ScriptPosition.External, uniquename, author, title, description, filter, function)) as T;
					}
					if (typeof(T) == typeof(ImportScript))
					{
						string filter = (string)type.GetProperty("Filter").GetValue(null, null);
						Action<string> function = Delegate.CreateDelegate(typeof(Action<string>), type.GetMethod("Call")) as Action<string>;
						script = (new ImportScript(Script.ScriptPosition.External, uniquename, author, title, description, filter, function)) as T;
					}

					error = string.Empty;
					return true;
				}
				catch(Exception e)
				{
					script = null;
					error = e.ToString();
					return false;
				}
			}
			if (lang == ScriptLanguage.Python)
			{
				var scope = compiled as sh.ScriptScope;
				
				try
				{
					string uniquename = scope.GetVariable<string>("UniqueName");
					string author = scope.GetVariable<string>("Author");
					string title = scope.GetVariable<string>("Title");
					string description = scope.GetVariable<string>("Description");

					script = null;
					if (typeof(T) == typeof(CommandScript))
					{
						var pyfunction = scope.GetVariable("Call") as pr.PythonFunction;
						Action function = () =>
						{
							PythonEngine.Operations.Invoke(pyfunction, new object[] { });
						};
						script = (new CommandScript(Script.ScriptPosition.External, uniquename, author, title, description, function)) as T;
					}
					if (typeof(T) == typeof(SelectedScript))
					{
						var pyfunction = scope.GetVariable("Call") as pr.PythonFunction;
						Action<Data.NodeBase, Object> function = (n,o) =>
						{
							PythonEngine.Operations.Invoke(pyfunction, new object[] {n,o });
						};
						script = (new SelectedScript(Script.ScriptPosition.External, uniquename, author, title, description, function)) as T;
					}
					if (typeof(T) == typeof(ExportScript))
					{
						string filter = scope.GetVariable<string>("Filter");
						var pyfunction = scope.GetVariable("Call") as pr.PythonFunction;
						Action<string> function = (s) =>
						{
							PythonEngine.Operations.Invoke(pyfunction, new object[] { s });
						};
						script = (new ExportScript(Script.ScriptPosition.External, uniquename, author, title, description, filter, function)) as T;
					}
					if (typeof(T) == typeof(ImportScript))
					{
						string filter = scope.GetVariable<string>("Filter");
						var pyfunction = scope.GetVariable("Call") as pr.PythonFunction;
						Action<string> function = (s) =>
						{
							PythonEngine.Operations.Invoke(pyfunction, new object[] { s });
						};
						script = (new ImportScript(Script.ScriptPosition.External, uniquename, author, title, description, filter, function)) as T;
					}

					error = string.Empty;
					return true;
				}
				catch (Exception e)
				{
					script = null;
					error = e.ToString();
					return false;
				}
			}

			throw new Exception();
		}
	}
}
#endif