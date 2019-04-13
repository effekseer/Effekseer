using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.GUI
{
	class Network
	{
		public string Target { get; set; }
		public int Port { get; set; }
		public bool AutoConnect { get; set; }
		public bool SendOnLoad { get; set; }
		public bool SendOnEdit { get; set; }
		public bool SendOnSave { get; set; }

		int time = 0;

		string target;
		int port;
		swig.Native native = null;

		public Action Loaded;

		public Network(swig.Native native)
		{
			Target = "127.0.0.1";
			Port = 60000;
			AutoConnect = false;
			SendOnLoad = true;
			SendOnEdit = false;
			SendOnSave = true;

			this.native = native;
		}

		public void Connect()
		{

			if (native.IsConnectingNetwork() && Target == target && Port == port) return;

			if (native.IsConnectingNetwork())
			{
				Disconnect();
			}

			if (native.StartNetwork(Target, (ushort)Port))
			{
				target = Target;
				port = Port;
			}
		}

		public void Disconnect()
		{
			if (!native.IsConnectingNetwork()) return;
			native.StopNetwork();
		}

		public bool IsConnected()
		{
			return native.IsConnectingNetwork();
		}

		public void Update()
		{
			if (AutoConnect && time % 60 * 15 == 0)
			{
				native.StartNetwork(Target, (ushort)Port);
			}

			time++;
		}

		public unsafe void Send()
		{
			if (!native.IsConnectingNetwork()) return;

			var binaryExporter = new Binary.Exporter();
			var data = binaryExporter.Export(Core.Option.Magnification);
			fixed (byte* p = &data[0])
			{
				native.SendDataByNetwork(System.IO.Path.GetFileNameWithoutExtension(Core.FullPath), new IntPtr(p), data.Length, Core.FullPath);
			}
		}

		public void Load(string path)
		{
			if (System.IO.File.Exists(path))
			{
				System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

				doc.Load(path);

				Target = doc["Root"]["Target"].ChildNodes[0].Value;

				Port = int.Parse(doc["Root"]["Port"].ChildNodes[0].Value);

				AutoConnect = bool.Parse(doc["Root"]["AutoConnect"].ChildNodes[0].Value);
				SendOnLoad = bool.Parse(doc["Root"]["SendOnLoad"].ChildNodes[0].Value);
				SendOnEdit = bool.Parse(doc["Root"]["SendOnEdit"].ChildNodes[0].Value);
				SendOnSave = bool.Parse(doc["Root"]["SendOnSave"].ChildNodes[0].Value);

				target = Target;
				port = Port;

				if(Loaded != null)
				{
					Loaded();
				}
			}
		}

		public void Save(string path)
		{
			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
			System.Xml.XmlElement project_root = doc.CreateElement("Root");

			{
				var e = doc.CreateElement("Target");
				e.AppendChild(doc.CreateTextNode(Target.ToString()));
				project_root.AppendChild(e);
			}

			{
				var e = doc.CreateElement("Port");
				e.AppendChild(doc.CreateTextNode(Port.ToString()));
				project_root.AppendChild(e);
			}

			{
				var e = doc.CreateElement("AutoConnect");
				e.AppendChild(doc.CreateTextNode(AutoConnect.ToString()));
				project_root.AppendChild(e);
			}

			{
				var e = doc.CreateElement("SendOnLoad");
				e.AppendChild(doc.CreateTextNode(SendOnLoad.ToString()));
				project_root.AppendChild(e);
			}

			{
				var e = doc.CreateElement("SendOnEdit");
				e.AppendChild(doc.CreateTextNode(SendOnEdit.ToString()));
				project_root.AppendChild(e);
			}

			{
				var e = doc.CreateElement("SendOnSave");
				e.AppendChild(doc.CreateTextNode(SendOnSave.ToString()));
				project_root.AppendChild(e);
			}

			doc.AppendChild(project_root);

			var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
			doc.InsertBefore(dec, project_root);

			// For failing to save
			try
			{
				doc.Save(path);
			}
			catch
			{

			}
		}
	}
}
