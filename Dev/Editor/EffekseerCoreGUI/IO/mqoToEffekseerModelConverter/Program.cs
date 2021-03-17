using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace mqoToEffekseerModelConverter
{
	class Program
	{
		const int Version = 3;

		static void PrintUsage()
		{
			Console.WriteLine("Effekseer Model Conveter");
			Console.WriteLine("Usage: mqoToEffekseerModelConverter InputFile(*.mqo) {OutputFile(*.efkmodel)} {options}");
			Console.WriteLine("");
			Console.WriteLine("Options: -modelcount : Max Render Count (1 - n)");
			Console.WriteLine("         -scale      : Scaling (ex. 0.5, 1.2)");
			Console.WriteLine("");
			Console.WriteLine("Examples: mqoToEffekseerModelConverter foo.mqo -scale 0.1");
		}

		static List<string> ParseArgs(string[] args, string optionMark, out Dictionary<string, string> optionItems)
		{
			var items = new List<string>();
			optionItems = new Dictionary<string, string>();
			for (int i = 0; i < args.Length; i++)
			{
				if (i + 1 < args.Length && args[i].IndexOf(optionMark) == 0)
				{
					optionItems.Add(args[i], args[i + 1]);
					i += 1;
				}
				else
				{
					items.Add(args[i]);
				}
			}
			return items;
		}

		public static void Call(string[] args)
		{
			if (args.Length == 0)
			{
				PrintUsage();
				return;
			}

			Dictionary<string, string> optionItems = null;
			List<string> items = ParseArgs(args, "-", out optionItems);
			
			string src = null;
			string dst = null;
			int modelCount = 1;
			float modelScale = 1.0f;

			if (items.Count >= 1)
			{
				var ext = Path.GetExtension(items[0]);
                ext = ext.ToLower();
				if (ext != ".mqo")
				{
					Console.WriteLine("InputFile is not mqo");
					return;
				}
				src = items[0];
			}

			if (items.Count >= 2)
			{
				var ext = Path.GetExtension(items[1]);
                ext = ext.ToLower();
                if (ext != ".efkmodel")
				{
					Console.WriteLine("OutputFile is not efkmodel");
					return;
				}
				dst = items[1];
			}
			else
			{
				dst = Path.ChangeExtension(src, "efkmodel");
			}

			if (optionItems.ContainsKey("-modelcount"))
			{
				if (!int.TryParse(optionItems["-modelcount"], out modelCount))
				{
					Console.WriteLine("modelcount is not number");
					return;
				}
			}
			
			if (optionItems.ContainsKey("-scale"))
			{
				if (!float.TryParse(optionItems["-scale"], out modelScale))
				{
					Console.WriteLine("scale is not number");
					return;
				}
			}

			var mqoModel = mqoIO.Loader.Load(src);

			if (mqoModel.Objects.Length == 0)
			{
				Console.WriteLine("Object is not found.");
				return;
			}

			var model = mqoIO.Realtime.Model.Create(mqoModel);
			
			List<byte[]> data = new List<byte[]>();

			data.Add(BitConverter.GetBytes(Version));

            //data.Add(BitConverter.GetBytes(modelScale));

            data.Add(BitConverter.GetBytes(modelCount));

            var vcount = model.Objects.Sum(_ => _.Vertexes.Length);
            var fcount = model.Objects.Sum(_ => _.Faces.Length);

            data.Add(BitConverter.GetBytes((int)vcount));

            foreach(var obj in model.Objects)
            {
                foreach (var v in obj.Vertexes)
                {
                    v.Position *= modelScale;

                    data.Add(BitConverter.GetBytes(v.Position.X));
                    data.Add(BitConverter.GetBytes(v.Position.Y));
                    data.Add(BitConverter.GetBytes(v.Position.Z));

                    data.Add(BitConverter.GetBytes(v.Normal.X));
                    data.Add(BitConverter.GetBytes(v.Normal.Y));
                    data.Add(BitConverter.GetBytes(v.Normal.Z));

                    data.Add(BitConverter.GetBytes(v.Binormal.X));
                    data.Add(BitConverter.GetBytes(v.Binormal.Y));
                    data.Add(BitConverter.GetBytes(v.Binormal.Z));

                    data.Add(BitConverter.GetBytes(v.Tangent.X));
                    data.Add(BitConverter.GetBytes(v.Tangent.Y));
                    data.Add(BitConverter.GetBytes(v.Tangent.Z));

                    data.Add(BitConverter.GetBytes(v.UV.X));
                    data.Add(BitConverter.GetBytes(v.UV.Y));

                    data.Add(
                        new byte[]
                        {
                    (byte)(v.Color.R * 255.0),
                    (byte)(v.Color.G * 255.0),
                    (byte)(v.Color.B * 255.0),
                    (byte)(v.Color.A * 255.0)});
                }
            }


            data.Add(BitConverter.GetBytes((int)fcount));

            int foffset = 0;

            foreach (var obj in model.Objects)
            {
                foreach (var f in obj.Faces)
                {
                    data.Add(BitConverter.GetBytes(foffset + f.Indexes[0]));
                    data.Add(BitConverter.GetBytes(foffset + f.Indexes[1]));
                    data.Add(BitConverter.GetBytes(foffset + f.Indexes[2]));
                }

                foffset += obj.Faces.Length;
            }

			data.Add(BitConverter.GetBytes(modelScale));

			System.IO.File.WriteAllBytes(dst, data.SelectMany(_ => _).ToArray());
		}
	}
}
