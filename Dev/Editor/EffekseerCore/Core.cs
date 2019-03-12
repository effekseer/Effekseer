using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;
using System.Globalization;
using System.Threading;

namespace Effekseer
{
	public class Core
	{
		public const string Version = "1.43b";

		public const string OptionFilePath = "config.option.xml";

		static Data.NodeBase selected_node = null;

		static Data.OptionValues option;

		static Data.EffectBehaviorValues effectBehavior = new Data.EffectBehaviorValues();

		static Data.EffectCullingValues culling = new Data.EffectCullingValues();

		static Data.GlobalValues globalValues = new Data.GlobalValues();

		static int start_frame = 0;

		static int end_frame = 160;

		static bool is_loop = false;

		/*
		public static IViewer Viewer
		{
			get;
			set;
		}
		*/

		public static object MainForm
		{
			get;
			set;
		}

		public static Language Language
		{
			get;
			private set;
		} = Language.English;

		public static Data.NodeRoot Root
		{
			get;
			private set;
		}

		public static string FullPath
		{
			get;
			private set;
		}

		public static bool IsChanged
		{
			get;
			private set;
		}

		/// <summary>
		/// 再生開始フレーム
		/// </summary>
		public static int StartFrame
		{
			get
			{
				return start_frame;
			}
			set
			{
				if (value < 0) return;
				if (start_frame == value) return;

				if (end_frame < value)
				{
					start_frame = end_frame;
				}
				else
				{
					start_frame = value;
				}

				if (OnAfterChangeStartFrame != null)
				{
					OnAfterChangeStartFrame(null, null);
				}
			}
		}

		/// <summary>
		/// 再生終了フレーム
		/// </summary>
		public static int EndFrame
		{
			get
			{
				return end_frame;
			}
			set
			{
				if (value < 0) return;
				if (end_frame == value) return;

				if (start_frame > value)
				{
					end_frame = start_frame;
				}
				else
				{
					end_frame = value;
				}

				if (OnAfterChangeEndFrame != null)
				{
					OnAfterChangeEndFrame(null, null);
				}
			}
		}

		/// <summary>
		/// 再生をループするか?
		/// </summary>
		public static bool IsLoop
		{
			get
			{
				return is_loop;
			}
			set
			{
				if (is_loop == value) return;

				is_loop = value;

				if (OnAfterChangeIsLoop != null)
				{
					OnAfterChangeIsLoop(null, null);
				}
			}
		}

		public static Script.ScriptCollection<Script.CommandScript> CommandScripts
		{
			get;
			private set;
		}

		public static Script.ScriptCollection<Script.SelectedScript> SelectedScripts
		{
			get;
			private set;
		}

		public static Script.ScriptCollection<Script.ExportScript> ExportScripts
		{
			get;
			private set;
		}

		public static Script.ScriptCollection<Script.ImportScript> ImportScripts
		{
			get;
			private set;
		}

		public static Data.OptionValues Option
		{
			get { return option; }
		}

		public static Data.EffectBehaviorValues EffectBehavior
		{
			get { return effectBehavior; }
		}

		public static Data.EffectCullingValues Culling
		{
			get { return culling; }
		}

		public static Data.GlobalValues Global
		{
			get { return globalValues; }
		}


		/// <summary>
		/// 選択中のノード
		/// </summary>
		public static Data.NodeBase SelectedNode
		{
			get
			{
				return selected_node;
			}

			set
			{
				if (selected_node == value) return;

				selected_node = value;
				if (OnAfterSelectNode != null)
				{
					OnAfterSelectNode(null, null);
				}
			}
		}

		/// <summary>
		/// Output message
		/// </summary>
		public static Action<string> OnOutputMessage;

		/// <summary>
		/// 選択中のノード変更後イベント
		/// </summary>
		public static event EventHandler OnAfterSelectNode;

		/// <summary>
		/// 新規作成前イベント
		/// </summary>
		public static event EventHandler OnBeforeNew;

		/// <summary>
		/// 新規作成後イベント
		/// </summary>
		public static event EventHandler OnAfterNew;

		/// <summary>
		/// 保存後イベント
		/// </summary>
		public static event EventHandler OnAfterSave;

		/// <summary>
		/// 読込前イベント
		/// </summary>
		public static event EventHandler OnBeforeLoad;

		/// <summary>
		/// 読込後イベント
		/// </summary>
		public static event EventHandler OnAfterLoad;

		/// <summary>
		/// 開始フレーム変更イベント
		/// </summary>
		public static event EventHandler OnAfterChangeStartFrame;

		/// <summary>
		/// 終了フレーム変更イベント
		/// </summary>
		public static event EventHandler OnAfterChangeEndFrame;

		/// <summary>
		/// ループ変更イベント
		/// </summary>
		public static event EventHandler OnAfterChangeIsLoop;

		/// <summary>
		/// 読込後イベント
		/// </summary>
		public static event EventHandler OnReload;

		static Core()
		{
			CommandScripts = new Script.ScriptCollection<Script.CommandScript>();
			SelectedScripts = new Script.ScriptCollection<Script.SelectedScript>();
			ExportScripts = new Script.ScriptCollection<Script.ExportScript>();
			ImportScripts = new Script.ScriptCollection<Script.ImportScript>();
		}

		public static void Initialize(Language? language = null)
		{
			var entryDirectory = GetEntryDirectory() + "/";

			Command.CommandManager.Changed += new EventHandler(CommandManager_Changed);
			FullPath = string.Empty;

			option = LoadOption(language);

			// Switch the language according to the loaded settings
			Language = Option.GuiLanguage;

			// Switch the culture according to the set language
			switch (Language)
			{
				case Effekseer.Language.English:
					Thread.CurrentThread.CurrentUICulture = new CultureInfo("en-US");
					break;
				case Effekseer.Language.Japanese:
					Thread.CurrentThread.CurrentUICulture = new CultureInfo("ja-JP");
					break;
			}

			New();

			if(!DoExecuteWithMLBundle())
			{
				InitializeScripts(entryDirectory);
			}
		}

		static void InitializeScripts(string entryDirectory)
		{
			// Load scripts
			System.IO.Directory.CreateDirectory(entryDirectory + "scripts");
			System.IO.Directory.CreateDirectory(entryDirectory + "scripts/import");
			System.IO.Directory.CreateDirectory(entryDirectory + "scripts/export");
			System.IO.Directory.CreateDirectory(entryDirectory + "scripts/command");
			System.IO.Directory.CreateDirectory(entryDirectory + "scripts/selected");

			Script.Compiler.Initialize();

			{
				var files = System.IO.Directory.GetFiles(entryDirectory + "scripts/command", "*.*", System.IO.SearchOption.AllDirectories);

				foreach (var file in files)
				{
					var ext = System.IO.Path.GetExtension(file);
					if (ext == ".cs" || ext == ".py")
					{
						Script.CommandScript script = null;
						string error = string.Empty;

						if (Script.Compiler.CompileScript<Script.CommandScript>(file, out script, out error))
						{
							CommandScripts.Add(script);
						}
						else
						{
							if (OnOutputMessage != null)
							{
								OnOutputMessage(error);
							}
						}
					}
				}
			}

			{
				var files = System.IO.Directory.GetFiles(entryDirectory + "scripts/selected", "*.*", System.IO.SearchOption.AllDirectories);
				foreach (var file in files)
				{
					var ext = System.IO.Path.GetExtension(file);
					if (ext == ".cs" || ext == ".py")
					{
						Script.SelectedScript script = null;
						string error = string.Empty;

						if (Script.Compiler.CompileScript<Script.SelectedScript>(file, out script, out error))
						{
							SelectedScripts.Add(script);
						}
						else
						{
							if (OnOutputMessage != null)
							{
								OnOutputMessage(error);
							}
						}
					}
				}
			}

			{
				var files = System.IO.Directory.GetFiles(entryDirectory + "scripts/export", "*.*", System.IO.SearchOption.AllDirectories);
				foreach (var file in files)
				{
					var ext = System.IO.Path.GetExtension(file);
					if (ext == ".cs" || ext == ".py")
					{
						Script.ExportScript script = null;
						string error = string.Empty;

						if (Script.Compiler.CompileScript<Script.ExportScript>(file, out script, out error))
						{
							ExportScripts.Add(script);
						}
						else
						{
							if (OnOutputMessage != null)
							{
								OnOutputMessage(error);
							}
						}

						Console.WriteLine(error);

					}
				}
			}

			{
				var files = System.IO.Directory.GetFiles(entryDirectory + "scripts/import", "*.*", System.IO.SearchOption.AllDirectories);
				foreach (var file in files)
				{
					var ext = System.IO.Path.GetExtension(file);
					if (ext == ".cs" || ext == ".py")
					{
						Script.ImportScript script = null;
						string error = string.Empty;

						if (Script.Compiler.CompileScript<Script.ImportScript>(file, out script, out error))
						{
							ImportScripts.Add(script);
						}
						else
						{
							if (OnOutputMessage != null)
							{
								OnOutputMessage(error);
							}
						}
					}
				}
			}
		}

		public static void Dispose()
		{
			Script.Compiler.Dispose();

			SaveOption();
		}

		/// <summary>
		/// Get a directory where the application exists
		/// </summary>
		/// <returns></returns>
		public static string GetEntryDirectory()
		{
			var myAssembly = System.Reflection.Assembly.GetEntryAssembly();
			string path = myAssembly.Location;
			var dir = System.IO.Path.GetDirectoryName(path);

			// for mkbundle
			if (dir == string.Empty)
			{
				dir = System.IO.Path.GetDirectoryName(
				System.IO.Path.GetFullPath(
				Environment.GetCommandLineArgs()[0]));
			}

			return dir;
		}

		public static bool DoExecuteWithMLBundle()
		{
			var myAssembly = System.Reflection.Assembly.GetEntryAssembly();
			string path = myAssembly.Location;
			var dir = System.IO.Path.GetDirectoryName(path);
			return dir == string.Empty;
		}

		public static string Copy(Data.NodeBase node)
		{
			if (node == null) return string.Empty;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			var element = Data.IO.SaveObjectToElement(doc, "CopiedNode", node, true);

			doc.AppendChild(element);

			return doc.InnerXml;
		}

		/// <summary>
		/// Check whether data is valid xml?
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		public static bool IsValidXml(string data)
		{
			try
			{
				System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
				doc.LoadXml(data);
				return true;
			}
			catch
			{
				return false;
			}
		}

		public static void Paste(Data.NodeBase node, string data)
		{
			if (node == null) return;
			if (!IsValidXml(data)) return;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			doc.LoadXml(data);

			if (doc.ChildNodes.Count == 0 || doc.ChildNodes[0].Name != "CopiedNode") return;

			Command.CommandManager.StartCollection();
			Data.IO.LoadFromElement(doc.ChildNodes[0] as System.Xml.XmlElement, node, true);
			Command.CommandManager.EndCollection();
		}

		/// <summary>
		/// New
		/// </summary>
		public static void New()
		{
			if (OnBeforeNew != null)
			{
				OnBeforeNew(null, null);
			}

			StartFrame = 0;
			EndFrame = 120;
			IsLoop = true;

			SelectedNode = null;
			Command.CommandManager.Clear();
			Root = new Data.NodeRoot();

			// Adhoc code
			effectBehavior.Reset();
			culling = new Data.EffectCullingValues();
			globalValues = new Data.GlobalValues();

            // Add a root node
            Root.AddChild();
			Command.CommandManager.Clear();
			FullPath = string.Empty;
			IsChanged = false;

			// Select child
			//SelectedNode = Root.Children[0];

			if (OnAfterNew != null)
			{
				OnAfterNew(null, null);
			}
		}

		public static void SaveTo(string path)
		{
			path = System.IO.Path.GetFullPath(path);

			FullPath = path;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			var element = Data.IO.SaveObjectToElement(doc, "Root", Core.Root, false);

			var behaviorElement = Data.IO.SaveObjectToElement(doc, "Behavior", EffectBehavior, false);
			var cullingElement = Data.IO.SaveObjectToElement(doc, "Culling", Culling, false);
			var globalElement = Data.IO.SaveObjectToElement(doc, "Global", Global, false);

			System.Xml.XmlElement project_root = doc.CreateElement("EffekseerProject");

			project_root.AppendChild(element);

			if(behaviorElement != null) project_root.AppendChild(behaviorElement);
			if (cullingElement != null) project_root.AppendChild(cullingElement);
			if (globalElement != null) project_root.AppendChild(globalElement);

			project_root.AppendChild(doc.CreateTextElement("ToolVersion", Core.Version));
			project_root.AppendChild(doc.CreateTextElement("Version", 3));
			project_root.AppendChild(doc.CreateTextElement("StartFrame", StartFrame));
			project_root.AppendChild(doc.CreateTextElement("EndFrame", EndFrame));
			project_root.AppendChild(doc.CreateTextElement("IsLoop", IsLoop.ToString()));

			doc.AppendChild(project_root);

			var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
			doc.InsertBefore(dec, project_root);
			doc.Save(path);
			IsChanged = false;

			if (OnAfterSave != null)
			{
				OnAfterSave(null, null);
			}
		}

		public static bool LoadFrom(string path)
		{
			path = System.IO.Path.GetFullPath(path);

			if (!System.IO.File.Exists(path)) return false;
			SelectedNode = null;

			FullPath = path;

			var doc = new System.Xml.XmlDocument();

			doc.Load(path);

			if (doc.ChildNodes.Count != 2) return false;
			if (doc.ChildNodes[1].Name != "EffekseerProject") return false;

			if (OnBeforeLoad != null)
			{
				OnBeforeLoad(null, null);
			}

			uint toolVersion = 0;
			if (doc["EffekseerProject"]["ToolVersion"] != null)
			{
				var fileVersion = doc["EffekseerProject"]["ToolVersion"].GetText();
				var currentVersion = Core.Version;

				toolVersion = ParseVersion(fileVersion);

				if (toolVersion > ParseVersion(currentVersion))
				{
                    switch (Language)
                    {
                        case Effekseer.Language.English:
                            throw new Exception("Version Error : \nThe file is created with a newer version of the tool.\nPlease use the latest version of the tool.");
                            break;
                        case Effekseer.Language.Japanese:
                            throw new Exception("Version Error : \nファイルがより新しいバージョンのツールで作成されています。\n最新バージョンのツールを使用してください。");
                            break;
                    }

                    
				}
			}

            // For compatibility
            {
                // Stripe→Ribbon
                var innerText = doc.InnerXml;
				innerText = innerText.Replace("<Stripe>", "<Ribbon>").Replace("</Stripe>", "</Ribbon>");
				doc = new System.Xml.XmlDocument();
				doc.LoadXml(innerText);
			}

            // For compatibility
            {
                // GenerationTime
                // GenerationTimeOffset

                Action<System.Xml.XmlNode> replace = null;
				replace = (node) =>
					{
						if ((node.Name == "GenerationTime" || node.Name == "GenerationTimeOffset") &&
							node.ChildNodes.Count > 0 &&
							node.ChildNodes[0] is System.Xml.XmlText)
						{
							var name = node.Name;
							var value = node.ChildNodes[0].Value;

							node.RemoveAll();

							var center = doc.CreateElement("Center");
							var max = doc.CreateElement("Max");
							var min = doc.CreateElement("Min");

							center.AppendChild(doc.CreateTextNode(value));
							max.AppendChild(doc.CreateTextNode(value));
							min.AppendChild(doc.CreateTextNode(value));

							node.AppendChild(center);
							node.AppendChild(max);
							node.AppendChild(min);
						}
						else
						{
							for(int i = 0; i < node.ChildNodes.Count; i++)
							{
								replace(node.ChildNodes[i]);
							}
						}
					};

				replace(doc);
			}

			var root = doc["EffekseerProject"]["Root"];
			if (root == null) return false;

			culling = new Data.EffectCullingValues();
			globalValues = new Data.GlobalValues();

			// Adhoc code
			effectBehavior.Reset();

			var behaviorElement = doc["EffekseerProject"]["Behavior"];
			if (behaviorElement != null)
			{
				var o = effectBehavior as object;
				Data.IO.LoadObjectFromElement(behaviorElement as System.Xml.XmlElement, ref o, false);
			}

			var cullingElement = doc["EffekseerProject"]["Culling"];
			if (cullingElement != null)
			{
				var o = culling as object;
				Data.IO.LoadObjectFromElement(cullingElement as System.Xml.XmlElement, ref o, false);
			}

			var globalElement = doc["EffekseerProject"]["Global"];
			if (globalElement != null)
			{
				var o = globalValues as object;
				Data.IO.LoadObjectFromElement(globalElement as System.Xml.XmlElement, ref o, false);
			}

			StartFrame = 0;
			EndFrame = doc["EffekseerProject"]["EndFrame"].GetTextAsInt();
			StartFrame = doc["EffekseerProject"]["StartFrame"].GetTextAsInt();
			IsLoop = bool.Parse(doc["EffekseerProject"]["IsLoop"].GetText());
			IsLoop = true;

			int version = 0;
			if (doc["EffekseerProject"]["Version"] != null)
			{
				version = doc["EffekseerProject"]["Version"].GetTextAsInt();
			}

			var root_node = new Data.NodeRoot() as object;
			Data.IO.LoadObjectFromElement(root as System.Xml.XmlElement, ref root_node, false);

            // For compatibility
            if (version < 3)
			{
				Action<Data.NodeBase> convert = null;
				convert = (n) =>
					{
						var n_ = n as Data.Node;

						if (n_ != null)
						{
							if (n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Sprite)
							{
								n_.RendererCommonValues.ColorTexture.SetAbsolutePathDirectly(n_.DrawingValues.Sprite.ColorTexture.AbsolutePath);
								n_.RendererCommonValues.AlphaBlend.SetValueDirectly(n_.DrawingValues.Sprite.AlphaBlend.Value);
							}
							else if (n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ring)
							{
								n_.RendererCommonValues.ColorTexture.SetAbsolutePathDirectly(n_.DrawingValues.Ring.ColorTexture.AbsolutePath);
								n_.RendererCommonValues.AlphaBlend.SetValueDirectly(n_.DrawingValues.Ring.AlphaBlend.Value);
							}
							else if (n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ribbon)
							{
								n_.RendererCommonValues.ColorTexture.SetAbsolutePathDirectly(n_.DrawingValues.Ribbon.ColorTexture.AbsolutePath);
								n_.RendererCommonValues.AlphaBlend.SetValueDirectly(n_.DrawingValues.Ribbon.AlphaBlend.Value);
							}
						}

						for (int i = 0; i < n.Children.Count; i++)
						{
							convert(n.Children[i]);
						}
					};

				convert(root_node as Data.NodeBase);
			}

			Root = root_node as Data.NodeRoot;
			Command.CommandManager.Clear();
			IsChanged = false;

			if (OnAfterLoad != null)
			{
				OnAfterLoad(null, null);
			}

			return true;
		}

		/// <summary>
		/// Load option parameters from config.option.xml
		/// If it failed, return default values.
		/// </summary>
		/// <param name="defaultLanguage"></param>
		/// <returns></returns>
		static public Data.OptionValues LoadOption(Language? defaultLanguage)
		{
            Data.OptionValues res = new Data.OptionValues();

			var path = System.IO.Path.Combine(GetEntryDirectory(), OptionFilePath);

			if (!System.IO.File.Exists(path))
			{
				if (defaultLanguage != null)
				{
					res.GuiLanguage.SetValueDirectly((Language)defaultLanguage.Value);
				}
				return res;
			}

			var doc = new System.Xml.XmlDocument();

			doc.Load(path);

            if (doc.ChildNodes.Count != 2) return res;
            if (doc.ChildNodes[1].Name != "EffekseerProject") return res;

			var optionElement = doc["EffekseerProject"]["Option"];
			if (optionElement != null)
			{
                var o = res as object;
				Data.IO.LoadObjectFromElement(optionElement as System.Xml.XmlElement, ref o, false);
			}

			IsChanged = false;

            return res;
		}

		static public void SaveOption()
		{
			var path = System.IO.Path.Combine(GetEntryDirectory(), OptionFilePath);

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			var optionElement = Data.IO.SaveObjectToElement(doc, "Option", Option, false);

			System.Xml.XmlElement project_root = doc.CreateElement("EffekseerProject");
			if(optionElement != null) project_root.AppendChild(optionElement);

			doc.AppendChild(project_root);

			var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
			doc.InsertBefore(dec, project_root);
			doc.Save(path);
			IsChanged = false;
		}

		static uint ParseVersion(string versionText)
		{
			versionText = versionText.Replace("CTP1", "");
			versionText = versionText.Replace("CTP2", "");
			versionText = versionText.Replace("CTP3", "");
			versionText = versionText.Replace("CTP4", "");
			versionText = versionText.Replace("CTP5", "");

			versionText = versionText.Replace("α1", "");
			versionText = versionText.Replace("α2", "");
			versionText = versionText.Replace("α3", "");
			versionText = versionText.Replace("α4", "");
			versionText = versionText.Replace("α5", "");

			versionText = versionText.Replace("β1", "");
			versionText = versionText.Replace("β2", "");
			versionText = versionText.Replace("β3", "");
			versionText = versionText.Replace("β4", "");
			versionText = versionText.Replace("β5", "");

			versionText = versionText.Replace("RC1", "");
			versionText = versionText.Replace("RC2", "");
			versionText = versionText.Replace("RC3", "");
			versionText = versionText.Replace("RC4", "");
			versionText = versionText.Replace("RC5", "");

			versionText = versionText.Replace("a", "");
			versionText = versionText.Replace("b", "");
			versionText = versionText.Replace("c", "");
			versionText = versionText.Replace("d", "");
			versionText = versionText.Replace("e", "");
			versionText = versionText.Replace("f", "");
			versionText = versionText.Replace("g", "");
			versionText = versionText.Replace("h", "");
			versionText = versionText.Replace("i", "");

			if (versionText.Length == 2) versionText += "000";
			if (versionText.Length == 3) versionText += "00";
			if (versionText.Length == 4) versionText += "0";

			uint version = 0;
			string[] list = versionText.Split('.');
			int count = Math.Min(list.Length, 4);
			for (int i = 0; i < count; i++) {
				int value = Math.Min(int.Parse(list[i]), 255);
				version |= (uint)value << ((3 - i) * 8);
			}
			return version;
		}

		static void CommandManager_Changed(object sender, EventArgs e)
		{
			IsChanged = true;
		}

		public static void Reload()
		{
			if (OnReload != null)
			{
				OnReload(null, null);
			}
		}

		public static bool MoveNode(Data.Node movedNode, Data.NodeBase targetParent, int targetIndex)
		{
			// Check
			if(movedNode.Parent == targetParent && targetIndex != int.MaxValue)
			{
				var index = targetParent.Children.Internal.Select((i, n) => Tuple.Create(i, n)).FirstOrDefault(_ => _.Item1 == movedNode).Item2;

				// Not changed.
				if (index == targetIndex || index + 1 == targetIndex)
				{
					return false;
				}
			}

			if(movedNode == targetParent)
			{
				return false;
			}

			Func<Data.Node, bool> isFound = null;
			
			isFound = (Data.Node n) =>
			{
				if(n.Children.Internal.Any(_=>_ == targetParent))
				{
					return true;
				}

				foreach(var n_ in n.Children.Internal)
				{
					if (isFound(n_)) return true;
				}

				return false;
			};

			if(isFound(movedNode))
			{
				return false;
			}

			// 
			if(targetParent == movedNode.Parent && targetIndex != int.MaxValue)
			{
				var index = targetParent.Children.Internal.Select((i, n) => Tuple.Create(i, n)).FirstOrDefault(_ => _.Item1 == movedNode).Item2;
				if(index < targetIndex)
				{
					targetIndex -= 1;
				}
			}

			Command.CommandManager.StartCollection();
			movedNode.Parent.RemoveChild(movedNode);
			targetParent.AddChild(movedNode, targetIndex);
			Command.CommandManager.EndCollection();
			return true;
		}

		/// <summary>
		/// 現在有効なFカーブを含めたノード情報を取得する。
		/// </summary>
		/// <returns></returns>
		public static Utl.ParameterTreeNode GetFCurveParameterNode()
		{
			// 実行速度を上げるために、全て力技で対応

			// 値を取得する
			Func<Data.Node, Tuple<string, object>[]> getParameters = (node) =>
				{
					var list = new List<Tuple<string, object>>();

					if (node.LocationValues.Type.Value == Data.LocationValues.ParamaterType.LocationFCurve)
					{
						var name = "Location";
						if(Language == Effekseer.Language.Japanese)
						{
							name = "位置";
						}

						list.Add(Tuple.Create(name,(object)node.LocationValues.LocationFCurve.FCurve));
					}

					if (node.RotationValues.Type.Value == Data.RotationValues.ParamaterType.RotationFCurve)
					{
						var name = "Angle";
						if (Language == Effekseer.Language.Japanese)
						{
							name = "角度";
						}

						list.Add(Tuple.Create(name, (object)node.RotationValues.RotationFCurve.FCurve));
					}

					if (node.ScalingValues.Type.Value == Data.ScaleValues.ParamaterType.FCurve)
					{
						var name = "Scaling Factor";
						if (Language == Effekseer.Language.Japanese)
						{
							name = "拡大率";
						}

						list.Add(Tuple.Create(name, (object)node.ScalingValues.FCurve.FCurve));
					}

					if (node.RendererCommonValues.UV.Value == Data.RendererCommonValues.UVType.FCurve)
					{
						var name = "UV(Start)";
						if (Language == Effekseer.Language.Japanese)
						{
							name = "UV(始点)";
						}

						list.Add(Tuple.Create(name, (object)node.RendererCommonValues.UVFCurve.Start));
					}

					if (node.RendererCommonValues.UV.Value == Data.RendererCommonValues.UVType.FCurve)
					{
						var name = "UV(Size)";
						if (Language == Effekseer.Language.Japanese)
						{
							name = "UV(大きさ)";
						}

						list.Add(Tuple.Create(name, (object)node.RendererCommonValues.UVFCurve.Size));
					}

					if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Sprite &&
						node.DrawingValues.Sprite.ColorAll.Value == Data.StandardColorType.FCurve)
					{
						var name = "Sprite-Color all(RGBA)";
						if (Language == Effekseer.Language.Japanese)
						{
							name = "スプライト・全体色(RGBA)";
						}

						list.Add(Tuple.Create(name, (object)node.DrawingValues.Sprite.ColorAll_FCurve.FCurve));
					}

					if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Model &&
						node.DrawingValues.Model.Color.Value == Data.StandardColorType.FCurve)
					{
						var name = "Model-Color(RGBA)";
						if (Language == Effekseer.Language.Japanese)
						{
							name = "モデル・色(RGBA)";
						}

						list.Add(Tuple.Create(name, (object)node.DrawingValues.Model.Color_FCurve.FCurve));
					}

					if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Track)
					{
						if (node.DrawingValues.Track.ColorLeft.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Left(RGBA)";
							if (Language == Effekseer.Language.Japanese)
							{
								name = "軌跡・左(RGBA)";
							}

							list.Add(Tuple.Create(name, (object)node.DrawingValues.Track.ColorLeft_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorLeftMiddle.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Left-Center(RGBA)";
							if (Language == Effekseer.Language.Japanese)
							{
								name = "軌跡・左中間(RGBA)";
							}

							list.Add(Tuple.Create(name, (object)node.DrawingValues.Track.ColorLeftMiddle_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorCenter.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Center(RGBA)";
							if (Language == Effekseer.Language.Japanese)
							{
								name = "軌跡・中央(RGBA)";
							}

							list.Add(Tuple.Create(name, (object)node.DrawingValues.Track.ColorCenter_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorCenterMiddle.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Center-Middle(RGBA)";
							if (Language == Effekseer.Language.Japanese)
							{
								name = "軌跡・中央中間(RGBA)";
							}

							list.Add(Tuple.Create(name, (object)node.DrawingValues.Track.ColorCenterMiddle_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorRight.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Right(RGBA)";
							if (Language == Effekseer.Language.Japanese)
							{
								name = "軌跡・右(RGBA)";
							}

							list.Add(Tuple.Create(name, (object)node.DrawingValues.Track.ColorRight_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorRightMiddle.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Right-Center(RGBA)";
							if (Language == Effekseer.Language.Japanese)
							{
								name = "軌跡・右中間(RGBA)";

							}
							list.Add(Tuple.Create(name, (object)node.DrawingValues.Track.ColorRightMiddle_FCurve.FCurve));
						}
					}

					return list.ToArray();
				};

			// Generate tree
			Func<Data.NodeBase, Utl.ParameterTreeNode> getParameterTreeNodes = null;

			getParameterTreeNodes = (node) =>
				{
					Tuple<string, object>[] parameters = null;

					var rootNode = node as Data.NodeRoot;
					var normalNode = node as Data.Node;

					if (rootNode != null)
					{
						parameters = new Tuple<string, object>[0];
					}
					else if (normalNode != null)
					{
						parameters = getParameters(normalNode);
					}

					List<Utl.ParameterTreeNode> children = new List<ParameterTreeNode>();
					for (int i = 0; i < node.Children.Count; i++)
					{
						children.Add(getParameterTreeNodes(node.Children[i]));
					}

					return new ParameterTreeNode(node, parameters, children.ToArray());
				};


			return getParameterTreeNodes(Root);
		}
	}
}
