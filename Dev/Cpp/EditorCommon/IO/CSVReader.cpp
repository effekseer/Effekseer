#include "CSVReader.h"
#include <functional>

namespace Effekseer
{
namespace Editor
{
std::vector<std::vector<std::string>> ReadCSV(std::string csv)
{
	bool inDQ = false;
	bool isStarting = true;

	std::function<bool(int)> isCurrentDQ = [&](int i) -> bool {
		if (i >= csv.size())
			return false;
		return csv[i] == '"';
	};

	std::function<bool(int)> isNextDQ = [&](int i) -> bool {
		if (i >= csv.size() - 1)
			return false;
		return csv[i + 1] == '"';
	};

	// TODO : improve a performance
	auto sb = std::string();

	std::vector<std::vector<std::string>> columns;
	std::vector<std::string> rows;

	for (int i = 0; i < csv.size(); i++)
	{
		if (isCurrentDQ(i))
		{
			if (isStarting)
			{
				inDQ = true;
			}
			else if (isNextDQ(i))
			{
				if (inDQ)
				{
					sb += "\"";
				}
				else
				{
					sb += "\"\"";
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
				sb += csv[i];
				isStarting = false;
			}
			else
			{
				rows.emplace_back(sb);
				sb.clear();
				isStarting = true;
			}
		}
		else if (csv[i] == '\n')
		{
			if (inDQ)
			{
				sb += csv[i];
				isStarting = false;
			}
			else
			{
				rows.emplace_back(sb);
				sb.clear();
				columns.emplace_back(rows);
				rows = std::vector<std::string>{};
				isStarting = true;
			}
		}
		else if (csv[i] == '\r')
		{
			// Skip
		}
		else
		{
			sb += csv[i];
			isStarting = false;
		}
	}

	if (sb.size() > 0 || rows.size() > 0)
	{
		rows.emplace_back(sb);
		columns.emplace_back(rows);
	}

	return columns;
}

} // namespace Editor
} // namespace Effekseer