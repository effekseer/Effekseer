﻿using System;
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
		public const string Version = "1.50β3";

		public const string OptionFilePath = "config.option.xml";

		static Data.NodeBase selected_node = null;

		static Data.OptionValues option;

		static Data.EnvironmentValues environments;

		static Data.EffectBehaviorValues effectBehavior = new Data.EffectBehaviorValues();

		static Data.EffectCullingValues culling = new Data.EffectCullingValues();

		static Data.GlobalValues globalValues = new Data.GlobalValues();

		static Data.RecordingValues recording = new Data.RecordingValues();

		static Data.DynamicValues dynamic_ = new Data.DynamicValues();

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
		}

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
#if SCRIPT_ENABLED
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
#endif
		public static Data.OptionValues Option
		{
			get { return option; }
		}
		
		public static Data.RecordingValues Recording
		{
			get { return recording; }
		}

		public static Data.EnvironmentValues Environment
		{
			get { return environments; }
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

		public static Data.DynamicValues Dynamic
		{
			get { return dynamic_; }
		}

		/// <summary>
		/// Selected node
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
		/// Output logs
		/// </summary>
		public static Action<LogLevel, string> OnOutputLog;

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
#if SCRIPT_ENABLED
			CommandScripts = new Script.ScriptCollection<Script.CommandScript>();
			SelectedScripts = new Script.ScriptCollection<Script.SelectedScript>();
			ExportScripts = new Script.ScriptCollection<Script.ExportScript>();
			ImportScripts = new Script.ScriptCollection<Script.ImportScript>();
#endif
            // change a separator
            System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
            customCulture.NumberFormat.NumberDecimalSeparator = ".";
            System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;
            Language = Language.English;
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
				case Language.English:
					Thread.CurrentThread.CurrentUICulture = new CultureInfo("en-US");
					break;
				case Language.Japanese:
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
#if SCRIPT_ENABLED
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
#endif
		}

		public static void Dispose()
		{
#if SCRIPT_ENABLED
            Script.Compiler.Dispose();
#endif
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
				System.Environment.GetCommandLineArgs()[0]));
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

		public static string Copy(string elementName, object o)
		{
			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			var element = Data.IO.SaveObjectToElement(doc, elementName, o, true);

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

		public static bool Paste(string elementName, object o, string data)
		{
			if (o == null) return false;
			if (!IsValidXml(data)) return false;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			doc.LoadXml(data);

			if (doc.ChildNodes.Count == 0 || doc.ChildNodes[0].Name != elementName) return false;

			Command.CommandManager.StartCollection();
			Data.IO.LoadObjectFromElement(doc.ChildNodes[0] as System.Xml.XmlElement, ref o, true);
			Command.CommandManager.EndCollection();

			return true;
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

			if(recording.RecordingStorageTarget.Value == Data.RecordingStorageTargetTyoe.Local)
			{
				recording = new Data.RecordingValues();
			}

			dynamic_ = new Data.DynamicValues();

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

		public static System.Xml.XmlDocument SaveAsXmlDocument(string basePath)
		{
			basePath = System.IO.Path.GetFullPath(basePath);

			FullPath = basePath;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			var element = Data.IO.SaveObjectToElement(doc, "Root", Core.Root, false);

			var behaviorElement = Data.IO.SaveObjectToElement(doc, "Behavior", EffectBehavior, false);
			var cullingElement = Data.IO.SaveObjectToElement(doc, "Culling", Culling, false);
			var globalElement = Data.IO.SaveObjectToElement(doc, "Global", Global, false);
			var dynamicElement = Data.IO.SaveObjectToElement(doc, "Dynamic", Dynamic, false);

			System.Xml.XmlElement project_root = doc.CreateElement("EffekseerProject");

			project_root.AppendChild(element);

			if (behaviorElement != null) project_root.AppendChild(behaviorElement);
			if (cullingElement != null) project_root.AppendChild(cullingElement);
			if (globalElement != null) project_root.AppendChild(globalElement);
			if (dynamicElement != null) project_root.AppendChild(dynamicElement);

			// recording option (this option is stored in local or global)
			if (recording.RecordingStorageTarget.Value == Data.RecordingStorageTargetTyoe.Local)
			{
				var recordingElement = Data.IO.SaveObjectToElement(doc, "Recording", Recording, false);
				if (recordingElement != null) project_root.AppendChild(recordingElement);
			}

			project_root.AppendChild(doc.CreateTextElement("ToolVersion", Core.Version));
			project_root.AppendChild(doc.CreateTextElement("Version", 3));
			project_root.AppendChild(doc.CreateTextElement("StartFrame", StartFrame));
			project_root.AppendChild(doc.CreateTextElement("EndFrame", EndFrame));
			project_root.AppendChild(doc.CreateTextElement("IsLoop", IsLoop.ToString()));

			doc.AppendChild(project_root);

			var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
			doc.InsertBefore(dec, project_root);
	
			IsChanged = false;

			if (OnAfterSave != null)
			{
				OnAfterSave(null, null);
			}

			return doc;
		}

		public static void SaveTo(string path)
		{
			var loader = new IO.EfkEfc();
			loader.Save(path);
			return;
		}

		public static bool LoadFromXml(string xml, string basePath)
		{
			var doc = new System.Xml.XmlDocument();
			doc.LoadXml(xml);
			return LoadFromXml(doc, basePath);
		}
		public static bool LoadFromXml(System.Xml.XmlDocument doc, string basePath)
		{
			basePath = System.IO.Path.GetFullPath(basePath);

			SelectedNode = null;

			FullPath = basePath;

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
						case Language.English:
							throw new Exception("Version Error : \nThe file is created with a newer version of the tool.\nPlease use the latest version of the tool.");
							break;
						case Language.Japanese:
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
						for (int i = 0; i < node.ChildNodes.Count; i++)
						{
							replace(node.ChildNodes[i]);
						}
					}
				};

				replace(doc);
			}

			{
				Action<System.Xml.XmlNode> replace = null;
				replace = (node) =>
				{
					if ((node.Name == "RenderCommon") && node.ChildNodes.Count > 0)
					{
						if(node["Distortion"] != null && node["Distortion"].GetText() == "True")
						{
							node.RemoveChild(node["Material"]);
							node.AppendChild(doc.CreateTextElement("Material", (int)Data.RendererCommonValues.MaterialType.BackDistortion));
						}
					}
					else
					{
						for (int i = 0; i < node.ChildNodes.Count; i++)
						{
							replace(node.ChildNodes[i]);
						}
					}
				};

				replace(doc);
			}

			if (toolVersion < ParseVersion("1.50"))
			{
				List<System.Xml.XmlNode> nodes = new List<System.Xml.XmlNode>();

				Action<System.Xml.XmlNode> collectNodes = null;
				collectNodes = (node) =>
				{
					if (node.Name == "Node")
					{
						nodes.Add(node);
					}

					for (int i = 0; i < node.ChildNodes.Count; i++)
					{
						collectNodes(node.ChildNodes[i]);
					}
				};

				collectNodes(doc);

				foreach(var node in nodes)
				{
					var rendererCommon = node["RendererCommonValues"];
					var renderer = node["DrawingValues"];

					if (renderer != null && rendererCommon != null)
					{
						if (renderer["Type"] != null && renderer["Type"].GetTextAsInt() == (int)Data.RendererValues.ParamaterType.Model)
						{
							if (renderer["Model"]["Lighting"] == null || renderer["Model"]["Lighting"].GetText() == "True")
							{
								if (node["Material"] != null)
								{
									rendererCommon.RemoveChild(node["Material"]);
								}

								rendererCommon.AppendChild(doc.CreateTextElement("Material", (int)Data.RendererCommonValues.MaterialType.Lighting));
							}
						}
					}

					if (rendererCommon != null)
					{
						if (rendererCommon["Distortion"] != null && rendererCommon["Distortion"].GetText() == "True")
						{
							if(node["Material"] != null)
							{
								rendererCommon.RemoveChild(node["Material"]);
							}

							rendererCommon.AppendChild(doc.CreateTextElement("Material", (int)Data.RendererCommonValues.MaterialType.BackDistortion));
						}
					}

					if (renderer != null && rendererCommon != null)
					{
						if (renderer["Type"] != null && renderer["Type"].GetTextAsInt() == (int)Data.RendererValues.ParamaterType.Model)
						{
							if (renderer["Model"]["NormalTexture"] != null)
							{
								if(rendererCommon["Filter"] != null)
								{
									rendererCommon.AppendChild(doc.CreateTextElement("Filter2", rendererCommon["Filter"].GetText()));
								}

								if (rendererCommon["Wrap"] != null)
								{
									rendererCommon.AppendChild(doc.CreateTextElement("Wrap2", rendererCommon["Wrap"].GetText()));
								}
							}
						}
					}

					if (renderer != null && rendererCommon != null)
					{
						if (renderer["Type"] != null && renderer["Type"].GetTextAsInt() == (int)Data.RendererValues.ParamaterType.Model)
						{
							if (renderer["Model"]["NormalTexture"] != null)
							{
								rendererCommon.AppendChild(doc.CreateTextElement("NormalTexture", renderer["Model"]["NormalTexture"].GetText()));
							}
						}
					}
				}
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

			var dynamicElement = doc["EffekseerProject"]["Dynamic"];
			if (dynamicElement != null)
			{
				var o = dynamic_ as object;
				Data.IO.LoadObjectFromElement(dynamicElement as System.Xml.XmlElement, ref o, false);
			}

			// recording option (this option is stored in local or global)
			if (doc["EffekseerProject"]["Recording"] != null)
			{
				var o = recording as object;
				Data.IO.LoadObjectFromElement(doc["EffekseerProject"]["Recording"] as System.Xml.XmlElement, ref o, false);
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

			if (toolVersion < ParseVersion("1.50"))
			{
				Action<Data.NodeBase> convert = null;
				convert = (n) =>
				{
					var n_ = n as Data.Node;

					if (n_ != null && n_.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ring)
					{
						var rp = n_.DrawingValues.Ring;
						if (rp.ViewingAngle.Value != Data.RendererValues.RingParamater.ViewingAngleType.Fixed ||
							rp.ViewingAngle_Fixed.Value != 360)
						{
							var rc = rp.RingShape.Crescent;
							rp.RingShape.Type.SetValue(Data.RingShapeType.Crescent);
							rc.StartingAngle.SetValue((Data.FixedRandomEasingType)(int)rp.ViewingAngle.Value);
							rc.EndingAngle.SetValue((Data.FixedRandomEasingType)(int)rp.ViewingAngle.Value);

							rc.StartingAngle_Fixed.SetValue((360 - rp.ViewingAngle_Fixed.Value) / 2 + 90);
							rc.EndingAngle_Fixed.SetValue(360 - (360 - rp.ViewingAngle_Fixed.Value) / 2 + 90);

							rc.StartingAngle_Random.SetMax((360 - rp.ViewingAngle_Random.Min) / 2 + 90);
							rc.StartingAngle_Random.SetMin((360 - rp.ViewingAngle_Random.Max) / 2 + 90);

							rc.EndingAngle_Random.SetMax(360 - (360 - rp.ViewingAngle_Random.Max) / 2 + 90);
							rc.EndingAngle_Random.SetMin(360 - (360 - rp.ViewingAngle_Random.Min) / 2 + 90);

							rc.StartingAngle_Easing.Start.SetMax((360 - rp.ViewingAngle_Easing.Start.Min) / 2 + 90);
							rc.StartingAngle_Easing.Start.SetMin((360 - rp.ViewingAngle_Easing.Start.Max) / 2 + 90);
							rc.StartingAngle_Easing.End.SetMax((360 - rp.ViewingAngle_Easing.End.Min) / 2 + 90);
							rc.StartingAngle_Easing.End.SetMin((360 - rp.ViewingAngle_Easing.End.Max) / 2 + 90);

							rc.EndingAngle_Easing.Start.SetMax(360 - (360 - rp.ViewingAngle_Easing.Start.Max) / 2 + 90);
							rc.EndingAngle_Easing.Start.SetMin(360 - (360 - rp.ViewingAngle_Easing.Start.Min) / 2 + 90);
							rc.EndingAngle_Easing.End.SetMax(360 - (360 - rp.ViewingAngle_Easing.End.Max) / 2 + 90);
							rc.EndingAngle_Easing.End.SetMin(360 - (360 - rp.ViewingAngle_Easing.End.Min) / 2 + 90);
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

			if (toolVersion < ParseVersion("1.50"))
			{
				// Fcurve
				var fcurves = GetFCurveParameterNode();

				Action<ParameterTreeNode> convert = null;
				convert = (n) =>
				{
					foreach (var fcurve in n.Parameters)
					{
						var fcurve2 = fcurve.Item2 as Data.Value.FCurveVector2D;
						var fcurve3 = fcurve.Item2 as Data.Value.FCurveVector3D;
						var fcurvecolor = fcurve.Item2 as Data.Value.FCurveColorRGBA;

						if (fcurve2 != null)
						{
							fcurve2.Timeline.SetValue(Data.Value.FCurveTimelineMode.Time);
							if (!fcurve2.X.Sampling.IsValueAssigned) fcurve2.X.Sampling.SetValue(5);
							if (!fcurve2.Y.Sampling.IsValueAssigned) fcurve2.Y.Sampling.SetValue(5);
						}

						if (fcurve3 != null)
						{
							fcurve3.Timeline.SetValue(Data.Value.FCurveTimelineMode.Time);
							if (!fcurve3.X.Sampling.IsValueAssigned) fcurve3.X.Sampling.SetValue(5);
							if (!fcurve3.Y.Sampling.IsValueAssigned) fcurve3.Y.Sampling.SetValue(5);
							if (!fcurve3.Z.Sampling.IsValueAssigned) fcurve3.Z.Sampling.SetValue(5);
						}

						if (fcurvecolor != null)
						{
							fcurvecolor.Timeline.SetValue(Data.Value.FCurveTimelineMode.Time);
							if (!fcurvecolor.R.Sampling.IsValueAssigned) fcurvecolor.R.Sampling.SetValue(5);
							if (!fcurvecolor.G.Sampling.IsValueAssigned) fcurvecolor.G.Sampling.SetValue(5);
							if (!fcurvecolor.B.Sampling.IsValueAssigned) fcurvecolor.B.Sampling.SetValue(5);
							if (!fcurvecolor.A.Sampling.IsValueAssigned) fcurvecolor.A.Sampling.SetValue(5);
						}
					}

					foreach(var c in n.Children)
					{
						convert(c);
					}
				};

				convert(fcurves);
			}
			
			Command.CommandManager.Clear();
			IsChanged = false;

			if (OnAfterLoad != null)
			{
				OnAfterLoad(null, null);
			}

			return true;
		}

		public static bool LoadFrom(string path)
		{
			var fullpath = System.IO.Path.GetFullPath(path);

			if (!System.IO.File.Exists(fullpath)) return false;

			// new format?
			bool isNewFormat = false;
			{
				if (string.IsNullOrEmpty(path))
					return false;

				System.IO.FileStream fs = null;
				if (!System.IO.File.Exists(path)) return false;

				try
				{
					fs = System.IO.File.Open(path, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
				}
				catch (System.IO.FileNotFoundException e)
				{
					return false;
				}

				var br = new System.IO.BinaryReader(fs);

				var buf = new byte[1024];


				if (br.Read(buf, 0, 20) != 20)
				{
					fs.Dispose();
					br.Close();
					return false;
				}

				if (buf[0] != 'E' ||
					buf[1] != 'F' ||
					buf[2] != 'K' ||
					buf[3] != 'E')
				{
					isNewFormat = false;
				}
				else
				{
					isNewFormat = true;
				}
			}

			if(isNewFormat)
			{
				var loader = new IO.EfkEfc();
				return loader.Load(path);
			}

			var doc = new System.Xml.XmlDocument();
			doc.Load(path);

			return LoadFromXml(doc.InnerXml, path);
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
			environments = new Data.EnvironmentValues();

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

			var environment = doc["EffekseerProject"]["Environment"];
			if (environment != null) {
				var o = environments as object;
				Data.IO.LoadObjectFromElement(environment as System.Xml.XmlElement, ref o, false);
			}

			var recordingElement = doc["EffekseerProject"]["Recording"];
			if(recordingElement != null)
			{
				var o = recording as object;
				Data.IO.LoadObjectFromElement(recordingElement as System.Xml.XmlElement, ref o, false);
			}

			IsChanged = false;

            return res;
		}

		static public void SaveOption()
		{
			var path = System.IO.Path.Combine(GetEntryDirectory(), OptionFilePath);

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			var optionElement = Data.IO.SaveObjectToElement(doc, "Option", Option, false);
			var environmentElement = Data.IO.SaveObjectToElement(doc, "Environment", Environment, false);

			System.Xml.XmlElement project_root = doc.CreateElement("EffekseerProject");
			if(optionElement != null) project_root.AppendChild(optionElement);
			if(environmentElement != null) project_root.AppendChild(environmentElement);

			// recording option (this option is stored in local or global)
			if (recording.RecordingStorageTarget.Value == Data.RecordingStorageTargetTyoe.Global)
			{
				var recordingElement = Data.IO.SaveObjectToElement(doc, "Recording", Recording, false);
				if (recordingElement != null) project_root.AppendChild(recordingElement);
			}

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

			versionText = versionText.Replace(".", "");
			uint version = 0;

			uint.TryParse(versionText, out version);

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
				var index = targetParent.Children.Internal.Select((i, n) => Tuple35.Create(i, n)).FirstOrDefault(_ => _.Item1 == movedNode).Item2;

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
				var index = targetParent.Children.Internal.Select((i, n) => Tuple35.Create(i, n)).FirstOrDefault(_ => _.Item1 == movedNode).Item2;
				if(index < targetIndex)
				{
					targetIndex -= 1;
				}
			}

			Command.CommandManager.StartCollection();
		
			// because a node is unselected when removed
			var isNodeSelected = SelectedNode == movedNode;

			movedNode.Parent.RemoveChild(movedNode);
			targetParent.AddChild(movedNode, targetIndex);

			if(isNodeSelected)
			{
				SelectedNode = movedNode;
			}

			Command.CommandManager.EndCollection();
			return true;
		}

		/// <summary>
		/// Collect nodes contains enabled FCurve
		/// </summary>
		/// <returns></returns>
		public static Utl.ParameterTreeNode GetFCurveParameterNode()
		{
			// not smart

			Func<Data.Node, Tuple35<string, object>[]> getParameters = (node) =>
				{
					var list = new List<Tuple35<string, object>>();

					if (node.LocationValues.Type.Value == Data.LocationValues.ParamaterType.LocationFCurve)
					{
						var name = "Location";
						if(Language == Language.Japanese)
						{
							name = "位置";
						}

						list.Add(Tuple35.Create(name,(object)node.LocationValues.LocationFCurve.FCurve));
					}

					if (node.RotationValues.Type.Value == Data.RotationValues.ParamaterType.RotationFCurve)
					{
						var name = "Angle";
						if (Language == Language.Japanese)
						{
							name = "角度";
						}

						list.Add(Tuple35.Create(name, (object)node.RotationValues.RotationFCurve.FCurve));
					}

					if (node.ScalingValues.Type.Value == Data.ScaleValues.ParamaterType.FCurve)
					{
						var name = "Scaling Factor";
						if (Language == Language.Japanese)
						{
							name = "拡大率";
						}

						list.Add(Tuple35.Create(name, (object)node.ScalingValues.FCurve.FCurve));
					}

					if (node.RendererCommonValues.UV.Value == Data.RendererCommonValues.UVType.FCurve)
					{
						var name = "UV(Start)";
						if (Language == Language.Japanese)
						{
							name = "UV(始点)";
						}

						list.Add(Tuple35.Create(name, (object)node.RendererCommonValues.UVFCurve.Start));
					}

					if (node.RendererCommonValues.UV.Value == Data.RendererCommonValues.UVType.FCurve)
					{
						var name = "UV(Size)";
						if (Language == Language.Japanese)
						{
							name = "UV(大きさ)";
						}

						list.Add(Tuple35.Create(name, (object)node.RendererCommonValues.UVFCurve.Size));
					}

					if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Sprite &&
						node.DrawingValues.Sprite.ColorAll.Value == Data.StandardColorType.FCurve)
					{
						var name = "Sprite-Color all(RGBA)";
						if (Language == Language.Japanese)
						{
							name = "スプライト・全体色(RGBA)";
						}

						list.Add(Tuple35.Create(name, (object)node.DrawingValues.Sprite.ColorAll_FCurve.FCurve));
					}

					if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Model &&
						node.DrawingValues.Model.Color.Value == Data.StandardColorType.FCurve)
					{
						var name = "Model-Color(RGBA)";
						if (Language == Language.Japanese)
						{
							name = "モデル・色(RGBA)";
						}

						list.Add(Tuple35.Create(name, (object)node.DrawingValues.Model.Color_FCurve.FCurve));
					}

					if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Track)
					{
						if (node.DrawingValues.Track.ColorLeft.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Left(RGBA)";
							if (Language == Language.Japanese)
							{
								name = "軌跡・左(RGBA)";
							}

							list.Add(Tuple35.Create(name, (object)node.DrawingValues.Track.ColorLeft_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorLeftMiddle.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Left-Center(RGBA)";
							if (Language == Language.Japanese)
							{
								name = "軌跡・左中間(RGBA)";
							}

							list.Add(Tuple35.Create(name, (object)node.DrawingValues.Track.ColorLeftMiddle_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorCenter.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Center(RGBA)";
							if (Language == Language.Japanese)
							{
								name = "軌跡・中央(RGBA)";
							}

							list.Add(Tuple35.Create(name, (object)node.DrawingValues.Track.ColorCenter_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorCenterMiddle.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Center-Middle(RGBA)";
							if (Language == Language.Japanese)
							{
								name = "軌跡・中央中間(RGBA)";
							}

							list.Add(Tuple35.Create(name, (object)node.DrawingValues.Track.ColorCenterMiddle_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorRight.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Right(RGBA)";
							if (Language == Language.Japanese)
							{
								name = "軌跡・右(RGBA)";
							}

							list.Add(Tuple35.Create(name, (object)node.DrawingValues.Track.ColorRight_FCurve.FCurve));
						}

						if (node.DrawingValues.Track.ColorRightMiddle.Value == Data.StandardColorType.FCurve)
						{
							var name = "Track-Color,Right-Center(RGBA)";
							if (Language == Language.Japanese)
							{
								name = "軌跡・右中間(RGBA)";

							}
							list.Add(Tuple35.Create(name, (object)node.DrawingValues.Track.ColorRightMiddle_FCurve.FCurve));
						}
					}

					if (node.RendererCommonValues.CustomData1.CustomData.Value == Data.CustomDataType.FCurve2D)
					{
						var name = "CustomData1";
						if (Language == Language.Japanese)
						{
							name = "カスタムデータ1";
						}
						list.Add(Tuple35.Create(name, (object)node.RendererCommonValues.CustomData1.FCurve));
					}

					if (node.RendererCommonValues.CustomData2.CustomData.Value == Data.CustomDataType.FCurve2D)
					{
						var name = "CustomData2";
						if (Language == Language.Japanese)
						{
							name = "カスタムデータ2";
						}
						list.Add(Tuple35.Create(name, (object)node.RendererCommonValues.CustomData2.FCurve));
					}

					if (node.RendererCommonValues.CustomData1.CustomData.Value == Data.CustomDataType.FCurveColor)
					{
						var name = "CustomData1";
						if (Language == Language.Japanese)
						{
							name = "カスタムデータ1";
						}
						list.Add(Tuple35.Create(name, (object)node.RendererCommonValues.CustomData1.FCurveColor));
					}

					if (node.RendererCommonValues.CustomData2.CustomData.Value == Data.CustomDataType.FCurveColor)
					{
						var name = "CustomData2";
						if (Language == Language.Japanese)
						{
							name = "カスタムデータ2";
						}
						list.Add(Tuple35.Create(name, (object)node.RendererCommonValues.CustomData2.FCurveColor));
					}

					return list.ToArray();
				};

			// Generate tree
			Func<Data.NodeBase, Utl.ParameterTreeNode> getParameterTreeNodes = null;

			getParameterTreeNodes = (node) =>
				{
					Tuple35<string, object>[] parameters = null;

					var rootNode = node as Data.NodeRoot;
					var normalNode = node as Data.Node;

					if (rootNode != null)
					{
						parameters = new Tuple35<string, object>[0];
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

		/// <summary>
		/// Update resource paths in all nodes
		/// </summary>
		/// <param name="path"></param>
		public static void UpdateResourcePaths(string path)
		{
			Action<Data.NodeBase> convert = null;
			convert = (node) =>
			{
				var n = node as Data.Node;

				if(n != null)
				{
					if (n.RendererCommonValues.Material.Value == Data.RendererCommonValues.MaterialType.File && n.RendererCommonValues.MaterialFile.Path.GetAbsolutePath().Replace('\\', '/') == path)
					{
						Utl.MaterialInformation info = new Utl.MaterialInformation();
						info.Load(path);

						n.RendererCommonValues.MaterialFile.ApplyMaterial(info);
					}
				}

				for (int i = 0; i < node.Children.Count; i++)
				{
					convert(node.Children[i]);
				}
			};

			convert(Root as Data.NodeBase);
		}
	}
}
