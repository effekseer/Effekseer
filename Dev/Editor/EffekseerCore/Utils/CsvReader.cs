using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	class CsvReaader
	{
		public static List<List<string>> Read(string csv)
		{
			bool inDQ = false;
			bool isStarting = true;

			Func<int, bool> isCurrentDQ = (int i) =>
			{
				if (i >= csv.Length) return false;
				return csv[i] == '"';
			};

			Func<int, bool> isNextDQ = (int i) => 
			{
				if (i >= csv.Length - 1) return false;
				return csv[i + 1] == '"';
			};

			var sb = new StringBuilder();

			List<List<string>> columns = new List<List<string>>();
			List<string> rows = new List<string>();

			for (int i = 0; i < csv.Length; i++)
			{
				if (isCurrentDQ(i))
				{
					if(isStarting)
					{
						inDQ = true;
					}
					else if (isNextDQ(i))
					{
						if (inDQ)
						{
							sb.Append("\"");
						}
						else
						{
							sb.Append("\"\"");
						}
						i++;
					}
					else
					{
						inDQ = !inDQ;
					}
					isStarting = false;
				}
				else if (csv[i] == ',')
				{
					if (inDQ)
					{
						sb.Append(csv[i]);
						isStarting = false;
					}
					else
					{
						rows.Add(sb.ToString());
						sb.Clear();
						isStarting = true;
					}
				}
				else if (csv[i] == '\n')
				{
					if (inDQ)
					{
						sb.Append(csv[i]);
						isStarting = false;
					}
					else
					{
						rows.Add(sb.ToString());
						sb.Clear();
						columns.Add(rows);
						rows = new List<string>();
						isStarting = true;
					}
				}
				else if (csv[i] == '\r')
				{
					// Skip
				}
				else
				{
					sb.Append(csv[i]);
					isStarting = false;
				}
			}

			if(sb.Length > 0 || rows.Count > 0)
			{
				rows.Add(sb.ToString());
				columns.Add(rows);
			}

			return columns;
		}
	}
}
