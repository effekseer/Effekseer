#pragma once

#include <string.h>
#include <optional>
#include <fstream>
#include "mqoio_data.h"
#include "mqoio_lexer.h"

namespace mqoio
{

class Loader
{
public:
	std::optional<Data> Load(std::string path)
	{
		std::ifstream fs(path, std::ios::binary);
		if (!fs.is_open())
		{
			return std::nullopt;
		}

		std::vector<char> bytes;
		fs.seekg(0, std::ios::end);
		bytes.resize(fs.tellg());
		fs.seekg(0, std::ios::beg);
		fs.read(bytes.data(), bytes.size());

		Lexer lexer;
		std::string err;
		std::vector<Token> tokens = lexer.Parse(bytes, err);
		if (tokens.empty())
		{
			return std::nullopt;
		}

		// マイナス符号の処理
		{
			std::vector<Token> tknList;
			for (int t = 0; t < tokens.size(); t++)
			{
				if (t < tokens.size() - 1)
				{
					if (tokens[t].Kind == TokenKind::Minus && tokens[t + 1].Kind == TokenKind::Digit)
					{
						tknList.emplace_back(Token(-tokens[t + 1].Digit, tokens[t].Line, tokens[t].Col));
						t++;
						continue;
					}
				}
				tknList.emplace_back(std::move(tokens[t]));
			}
			tokens = std::move(tknList);
		}

		Data data;

		int n = 0;

		while (n < tokens.size())
		{
			if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "Scene")
			{
				data.SceneInfo = LoadScene(tokens, n);
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "Material")
			{
				data.Materials = LoadMaterials(tokens, n);
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "Object")
			{
				data.Objects.emplace_back(LoadObject(tokens, n, data.Materials));
			}
			else
			{
				n++;
			}
		}

		return data;
	}

private:
	Scene LoadScene(const std::vector<Token>& tokens, int& n)
	{
		Scene scene{};

		while (true)
		{
			if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "pos")
			{
				scene.Position.X = tokens[n + 1].Digit;
				scene.Position.Y = tokens[n + 2].Digit;
				scene.Position.Z = tokens[n + 3].Digit;
				n += 4;
			}
			if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "dirlights")
			{
				while (true)
				{
					if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "light")
					{
						if (tokens[n].Kind == TokenKind::RCurly)
						{
							n++;
							break;
						}
						else
						{
							n++;
						}
					}
					else if (tokens[n].Kind == TokenKind::RCurly)
					{
						n++;
						break;
					}
					else
					{
						n++;
					}
				}
			}
			else if (tokens[n].Kind == TokenKind::RCurly)
			{
				n++;
				break;
			}
			else
			{
				n++;
			}

		}

		return scene;
	}

	std::vector<Material> LoadMaterials(const std::vector<Token>& tokens, int& n)
	{
		std::vector<Material> materials;
		materials.resize((int)tokens[n + 1].Digit);

		n += 3;

		for (int index = 0; index < materials.size(); index++)
		{
			if (tokens[n].Kind == TokenKind::String)
			{
				materials[index].Name = tokens[n].String;
				n++;
			}

			while (true)
			{
				if (tokens[n].Kind == TokenKind::String)
				{
					break;
				}
				else if (tokens[n].Kind == TokenKind::Letter)
				{
					auto valueName = tokens[n].Letter;
					n++;

					// (
					n++;

					int valueCount = 0;

					while (tokens[n + valueCount].Kind != TokenKind::Rparen)
					{
						valueCount++;
					}

					if (valueCount == 1 && tokens[n].Kind == TokenKind::String)
					{
						materials[index].Paramaters.emplace(valueName, tokens[n + 0].String);
					}
					else if (valueCount == 1)
					{
						materials[index].Paramaters.emplace(valueName, tokens[n + 0].Digit);
					}
					else if (valueCount == 2)
					{
						materials[index].Paramaters.emplace(valueName, Vector2D{ tokens[n + 0].Digit, tokens[n + 1].Digit });
					}
					else if (valueCount == 3)
					{
						materials[index].Paramaters.emplace(valueName, Vector3D{ tokens[n + 0].Digit, tokens[n + 1].Digit, tokens[n + 2].Digit });
					}
					else if (valueCount == 4)
					{
						materials[index].Paramaters.emplace(valueName, Vector4D{ tokens[n + 0].Digit, tokens[n + 1].Digit, tokens[n + 2].Digit, tokens[n + 3].Digit });
					}

					n += valueCount;

					// )
					n++;
				}
				else if (tokens[n].Kind == TokenKind::RCurly)
				{
					break;
				}
				else
				{
					n++;
				}
			}
		}

		n++;

		return materials;
	}

	Object LoadObject(const std::vector<Token>& tokens, int& n, const std::vector<Material>& materials)
	{
		Object obj;

		obj.Name = tokens[n + 1].String;

		n += 3;

		while (true)
		{
			if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "facet")
			{
				obj.Facet = tokens[n + 1].Digit;
				n += 2;
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "visible")
			{
				obj.Visible = tokens[n + 1].Digit > 0;
				n += 2;
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "vertex")
			{
				obj.Vertexes.resize((int)tokens[n + 1].Digit);
				n += 3;

				for (int vind = 0; vind < obj.Vertexes.size(); vind++)
				{
					obj.Vertexes[vind].X = tokens[n + 0].Digit;
					obj.Vertexes[vind].Y = tokens[n + 1].Digit;
					obj.Vertexes[vind].Z = tokens[n + 2].Digit;
					n += 3;
				}

				n += 1;
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "BVertex")
			{
				n += 3;

				while (tokens[n].Kind != TokenKind::RCurly)
				{
					if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "Vector")
					{
						auto& binary = tokens[n + 2].Binary;
						obj.Vertexes.resize((int)binary.size() / 12);

						for (int vind = 0; vind < obj.Vertexes.size(); vind++)
						{
							float x, y, z;
							memcpy(&x, &binary[vind * 12 + 0], sizeof(float));
							memcpy(&y, &binary[vind * 12 + 4], sizeof(float));
							memcpy(&z, &binary[vind * 12 + 8], sizeof(float));
							obj.Vertexes[vind].X = x;
							obj.Vertexes[vind].Y = y;
							obj.Vertexes[vind].Z = z;
						}
						n += 3;
					}
					else
					{
						n++;
					}
				}
				n += 1;
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "face")
			{
				obj.Faces.resize((int)tokens[n + 1].Digit);
				n += 3;

				for (int find = 0; find < obj.Faces.size(); find++)
				{
					int count = (int)tokens[n + 0].Digit;
					obj.Faces[find].Indexes.resize(count);

					n += 1;

					while (true)
					{
						if (tokens[n].Kind == TokenKind::Digit)
						{
							break;
						}
						else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "V")
						{
							n += 2;

							for (int c = 0; c < count; c++)
							{
								obj.Faces[find].Indexes[c] = (int)tokens[n + c].Digit;
							}
							n += count;

							n += 1;
						}
						else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "M")
						{
							n += 2;
							obj.Faces[find].MaterialIndex = (int)tokens[n + 0].Digit;
							n += 1;
							n += 1;
						}
						else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "UV")
						{
							n += 2;

							obj.Faces[find].UV.resize(count);
							for (int c = 0; c < count; c++)
							{
								auto u = tokens[n + c * 2 + 0].Digit;
								auto v = tokens[n + c * 2 + 1].Digit;
								obj.Faces[find].UV[c] = Vector2D{ u, v };
							}

							n += count * 2;
							n += 1;
						}
						else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "COL")
						{
							n += 2;

							obj.Faces[find].Colors.resize(count);
							for (int c = 0; c < count; c++)
							{
								auto color = (uint64_t)tokens[n + c].Digit;
								auto r = (color & 0x000000FF) >> 0;
								auto g = (color & 0x0000FF00) >> 8;
								auto b = (color & 0x00FF0000) >> 16;
								auto a = (color & 0xFF000000) >> 24;

								obj.Faces[find].Colors[c] = Color{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
							}
							n += count;

							n += 1;
						}
						else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "CRS")
						{
							n += 2;
							n += count;
							n += 1;
						}
						else if (tokens[n].Kind == TokenKind::RCurly)
						{
							break;
						}
						else
						{
							n++;
						}
					}
				}

				n += 1;
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "mirror")
			{
				obj.Mirror = (int)tokens[n + 1].Digit;
				n += 2;
			}
			else if (tokens[n].Kind == TokenKind::Letter && tokens[n].Letter == "mirror_axis")
			{
				obj.MirrorAxis = (int)tokens[n + 1].Digit;
				n += 2;
			}
			else if (tokens[n].Kind == TokenKind::RCurly)
			{
				n++;
				break;
			}
			else
			{
				n++;
			}

		}

		return obj;
	}
};

}
