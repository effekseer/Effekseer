using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Effekseer.IO
{
	/// <summary>
	/// Chunck to save or load
	/// </summary>
	public class Chunk
	{
		public List<ChunkBlock> Blocks = new List<ChunkBlock>();

		public void AddChunk(string chunk, byte[] buffer)
		{
			var block = new ChunkBlock();
			block.Chunk = chunk;
			block.Buffer = buffer;
			Blocks.Add(block);
		}

		public byte[] Save()
		{
			List<byte[]> data = new List<byte[]>();

			foreach (var binary in Blocks)
			{
				var chunk = new byte[4] { 0, 0, 0, 0 };
				var chararray = Encoding.UTF8.GetBytes(binary.Chunk);
				for (int i = 0; i < Math.Min(chunk.Length, chararray.Length); i++)
				{
					chunk[i] = chararray[i];
				}

				data.Add(chunk);
				data.Add(BitConverter.GetBytes((UInt32)binary.Buffer.Length));
				data.Add(binary.Buffer);
			}

			return data.SelectMany(_ => _).ToArray();
		}

		public bool Load(byte[] data)
		{
			Blocks.Clear();

			int pos = 0;

			while (pos < data.Length)
			{
				var chunkdata = data.Skip(pos).Take(4).ToArray();
				var chunk = Encoding.UTF8.GetString(chunkdata, 0, 4);
				pos += 4;

				var size = BitConverter.ToInt32(data, pos);
				pos += 4;

				var buffer = data.Skip(pos).Take(size).ToArray();
				pos += size;

				var binary = new ChunkBlock();
				binary.Chunk = chunk;
				binary.Buffer = buffer;
				Blocks.Add(binary);
			}

			if (pos != data.Length) return false;

			return true;
		}
	}

	public class ChunkBlock
	{
		public string Chunk;

		public byte[] Buffer;
	}

	class EfkEfc
	{
		const int Version = 0;


		class Element
		{
			public string Name = null;
			public UInt16 NameIndex = 0;
			public object Value = null;
			public List<Element> Children = null;
		}

		byte[] Compress(System.Xml.XmlDocument xml)
		{
			// visit all nodes
			Func<System.Xml.XmlNodeList, List<Element>> visit = null;
			visit = (System.Xml.XmlNodeList xmlNodes) =>
			{
				var elements = new List<Element>();

				for (int i = 0; i < xmlNodes.Count; i++)
				{
					if (xmlNodes[i].NodeType == System.Xml.XmlNodeType.XmlDeclaration)
						continue;

					var element = new Element();

					element.Name = xmlNodes[i].Name;

					if (xmlNodes[i].ChildNodes.Count == 1 &&
					xmlNodes[i].ChildNodes[0].NodeType == System.Xml.XmlNodeType.Text)
					{
						var value = xmlNodes[i].ChildNodes[0].Value;
						element.Value = value;
					}
					else
					{
						element.Children = visit(xmlNodes[i].ChildNodes);
					}

					elements.Add(element);
				}

				return elements;
			};

			var visits = visit(xml.ChildNodes);

			// compress
			var keys = new Dictionary<string, Int16>();
			var values = new Dictionary<string, Int16>();
			Func<List<Element>, byte[]> comp = null;
			comp = (elements) =>
			{
				Utils.BinaryWriter res = new Utils.BinaryWriter();

				res.Push((Int16)elements.Count);
				foreach (var item in elements)
				{
					if (!keys.ContainsKey(item.Name))
						keys[item.Name] = (Int16)keys.Count();
					res.Push(keys[item.Name]);

					var valueStr = item.Value as string;
					bool isHaveValue = !string.IsNullOrEmpty(valueStr);
					res.Push(isHaveValue);
					if (isHaveValue)
					{
						if (!values.ContainsKey(valueStr))
							values[valueStr] = (Int16)values.Count();
						res.Push(values[valueStr]);
					}

					bool isHaveChildren = item.Children != null && item.Children.Count != 0;
					res.Push(isHaveChildren);
					if (isHaveChildren)
						res.PushDirectly(comp(item.Children));
				}
				return res.GetBinary();
			};

			var binary = new Utils.BinaryWriter();

			var valueBuf = comp(visits);

			binary.Push((Int16)keys.Count);
			foreach (var item in keys)
			{
				binary.Push(item.Key, Encoding.UTF8, false, 2);
				binary.Push(item.Value);
			}

			binary.Push((Int16)values.Count);
			foreach (var item in values)
			{
				binary.Push(item.Key, Encoding.UTF8, false, 2);
				binary.Push(item.Value);
			}

			binary.PushDirectly(valueBuf);

			return Utils.Zlib.Compress(binary.GetBinary());
		}

		System.Xml.XmlDocument Decompress(byte[] buffer)
		{
			var decompressBuffer = Utils.Zlib.Decompress(buffer);

			var doc = new System.Xml.XmlDocument();
			var reader = new Utl.BinaryReader(decompressBuffer);

			var keys = new Dictionary<Int16, string>();
			Int16 keySize = -1;
			reader.Get(ref keySize);
			for (int i = 0; i < keySize; i++)
			{
				Int16 key = -1;
				string name = "";
				reader.Get(ref name, Encoding.UTF8, false, 2);
				reader.Get(ref key);
				keys.Add(key, name);
			}

			var values = new Dictionary<Int16, string>();
			Int16 valueSize = -1;
			reader.Get(ref valueSize);
			for (int i = 0; i < valueSize; i++)
			{
				Int16 key = -1;
				string value = "";
				reader.Get(ref value, Encoding.UTF8, false, 2);
				reader.Get(ref key);
				values.Add(key, value);
			}

			Action<System.Xml.XmlNode> decomp = null;
			decomp = (node) =>
			{
				Int16 elementSize = 0;
				reader.Get(ref elementSize);
				for (int i = 0; i < elementSize; i++)
				{
					Int16 nameKey = -1;
					reader.Get(ref nameKey);
					var element = doc.CreateElement(keys[nameKey]);
					node.AppendChild(element);

					bool isHaveValue = false;
					reader.Get(ref isHaveValue);
					if (isHaveValue)
					{
						Int16 value = -1;
						reader.Get(ref value);
						var valueNode = doc.CreateNode(System.Xml.XmlNodeType.Text, "", "");
						valueNode.Value = values[value];
						element.AppendChild(valueNode);
					}

					bool isHaveChildren = false;
					reader.Get(ref isHaveChildren);
					if (isHaveChildren)
						decomp(element);
				}
			};


			var declare = doc.CreateXmlDeclaration("1.0", "UTF-8", null);
			doc.AppendChild(declare);
			decomp(doc);

			return doc;
		}

		byte[] GetBinaryStr(string str)
		{
			List<byte[]> data = new List<byte[]>();

			var path = Encoding.Unicode.GetBytes(str);
			data.Add(BitConverter.GetBytes((path.Count() + 2) / 2));
			data.Add(path);
			data.Add(new byte[] { 0, 0 });

			return data.SelectMany(_ => _).ToArray();
		}

		public bool Save(string path, Data.NodeRoot rootNode, XmlDocument editorData)
		{
			Utils.Logger.Write(string.Format("Save : Start : {0}", path));

			var allData = Save(rootNode, editorData);

			try
			{
				System.IO.File.WriteAllBytes(path, allData);
			}
			catch (Exception e)
			{
				string messeage = "";
				if (Core.Language == Language.English)
				{
					messeage = "Failed to save a file " + path + "\nThis error is \n";
				}
				else
				{
					messeage = "保存に失敗しました。 " + path + "\nエラーは \n";
				}

				messeage += e.ToString();

				if (Core.OnOutputMessage != null)
				{
					Core.OnOutputMessage(messeage);
				}

				Utils.Logger.Write(string.Format("Save : Failed : {0}", e.ToString()));

				return false;
			}

			Utils.Logger.Write(string.Format("Save : End"));

			return true;
		}

		public byte[] Save(Data.NodeRoot rootNode, XmlDocument editorData)
		{
			// binary data
			var binaryExporter = new Binary.Exporter();
			var binaryDataLatest = binaryExporter.Export(rootNode, 1, Binary.ExporterVersion.Latest);  // TODO change magnification

			// info data
			byte[] infoData = null;
			{
				var data = new List<byte[]>();
				Action<HashSet<string>> exportStrs = (HashSet<string> strs) =>
				{
					data.Add(BitConverter.GetBytes(strs.Count));
					foreach (var exportedPath in strs)
					{
						data.Add(GetBinaryStr(exportedPath));
					}
				};

				int infoVersion = (int)Binary.ExporterVersion.Latest;
				data.Add(BitConverter.GetBytes(infoVersion));

				exportStrs(binaryExporter.UsedTextures);
				exportStrs(binaryExporter.UsedNormalTextures);
				exportStrs(binaryExporter.UsedDistortionTextures);
				exportStrs(binaryExporter.Models);
				exportStrs(binaryExporter.Sounds);
				exportStrs(binaryExporter.Materials);
				exportStrs(binaryExporter.Curves);

				infoData = data.SelectMany(_ => _).ToArray();
			}

			// header
			byte[] headerData = null;
			{
				var data = new List<byte[]>();
				data.Add(Encoding.UTF8.GetBytes("EFKE"));
				data.Add(BitConverter.GetBytes(Version));

				headerData = data.SelectMany(_ => _).ToArray();
			}

			var chunk = new Chunk();
			chunk.AddChunk("INFO", infoData);
			chunk.AddChunk("EDIT", Compress(editorData));
			chunk.AddChunk("BIN_", binaryDataLatest);

			// fallback
			if(Binary.ExporterVersion.Latest > Binary.ExporterVersion.Ver1500)
			{
				var binaryExporterFallback = new Binary.Exporter();
				var binaryDataFallback = binaryExporterFallback.Export(Core.Root, 1, Binary.ExporterVersion.Ver1500);
				chunk.AddChunk("BIN_", binaryDataFallback);
			}

			var chunkData = chunk.Save();

			return headerData.Concat(chunkData).ToArray();
		}

		public XmlDocument Load(string path)
		{
			byte[] allData = null;
			try
			{
				allData = System.IO.File.ReadAllBytes(path);
			}
			catch
			{
				return null;
			}

			return Load(allData);
		}

		public XmlDocument Load(byte[] allData)
		{
			if (allData.Length < 24) return null;

			if (allData[0] != 'E' ||
				allData[1] != 'F' ||
				allData[2] != 'K' ||
				allData[3] != 'E')
			{
				return null;
			}

			var version = BitConverter.ToInt32(allData, 4);

			var chunkData = allData.Skip(8).ToArray();

			var chunk = new Chunk();
			chunk.Load(chunkData);

			var editBlock = chunk.Blocks.FirstOrDefault(_ => _.Chunk == "EDIT");
			if (editBlock == null)
			{
				return null;
			}

			return Decompress(editBlock.Buffer);
		}
	}
}