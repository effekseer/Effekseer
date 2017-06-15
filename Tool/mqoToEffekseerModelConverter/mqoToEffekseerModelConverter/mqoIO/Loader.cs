using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO
{
	public class Loader
	{
		public static Data Load(string path)
		{
			var line = System.IO.File.ReadAllText(path);

			var err = string.Empty;
			var tokens = Lexer.Parse(line, ref err);

			// マイナス符号の処理
			{
				List<Token> tknList = new List<Token>();
				for (int t = 0; t < tokens.Length; t++)
				{
					if (t < tokens.Length - 1)
					{
						if (tokens[t].Kind == TokenKind.Minus && tokens[t + 1].Kind == TokenKind.Digit)
						{
							tknList.Add(new Token(-(float)tokens[t + 1].Digit, tokens[t].Line, tokens[t].Col));
							t++;
							continue;
						}
					}
					tknList.Add(tokens[t]);
				}
				tokens = tknList.ToArray();
			}

			Data data = new Data();

			List<Object> obj = new List<Object>();

			int n = 0;

			while (n < tokens.Length)
			{
				if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "Scene")
				{
					data.Scene = LoadScene(tokens, ref n);
				}
				else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "Material")
				{
					data.Materials = LoadMaterials(tokens, ref n);
				}
				else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "Object")
				{
					obj.Add(LoadObject(tokens, ref n, data.Materials));
				}
				else
				{
					n++;
				}
			}

			data.Objects = obj.ToArray();

			return data;
		}

		static Scene LoadScene(Token[] tokens, ref int n)
		{
			var scene = new Scene();

			while (true)
			{
				if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "pos")
				{
					scene.Position.X = (float)tokens[n + 1].Digit;
					scene.Position.Y = (float)tokens[n + 2].Digit;
					scene.Position.Z = (float)tokens[n + 3].Digit;
					n += 4;
				}
				if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "dirlights")
				{
					while (true)
					{
						if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "light")
						{
							if (tokens[n].Kind == TokenKind.RCurly)
							{
								n++;
								break;
							}
							else
							{
								n++;
							}
						}
						else if (tokens[n].Kind == TokenKind.RCurly)
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
				else if (tokens[n].Kind == TokenKind.RCurly)
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

		static Material[] LoadMaterials(Token[] tokens, ref int n)
		{
			var materials = new Material[(int)tokens[n + 1].Digit];
			for (int i = 0; i < materials.Length; i++)
			{
				materials[i] = new Material();
			}

			n += 3;

			for (int index = 0; index < materials.Length; index++)
			{
				if (tokens[n].Kind == TokenKind.String)
				{
					materials[index].Name = tokens[n].String;
					n++;
				}

				while (true)
				{
					if (tokens[n].Kind == TokenKind.String)
					{
						break;
					}
					else if (tokens[n].Kind == TokenKind.Letter)
					{
						var valueName = tokens[n].Letter;
						n++;

						// (
						n++;

						int valueCount = 0;

						while (tokens[n + valueCount].Kind != TokenKind.Rparen)
						{
							valueCount++;
						}

						if (valueCount == 1 && tokens[n].Kind == TokenKind.String)
						{
							materials[index].Paramaters.Add(valueName, tokens[n + 0].String);
						}
						else if (valueCount == 1)
						{
							materials[index].Paramaters.Add(valueName, new float[] { (float)tokens[n + 0].Digit });
						}
						else if (valueCount == 2)
						{
							materials[index].Paramaters.Add(valueName, new float[] { (float)tokens[n + 0].Digit, (float)tokens[n + 1].Digit });
						}
						else if (valueCount == 3)
						{
							materials[index].Paramaters.Add(valueName, new float[] { (float)tokens[n + 0].Digit, (float)tokens[n + 1].Digit, (float)tokens[n + 2].Digit });
						}
						else if (valueCount == 4)
						{
							materials[index].Paramaters.Add(valueName, new float[] { (float)tokens[n + 0].Digit, (float)tokens[n + 1].Digit, (float)tokens[n + 2].Digit, (float)tokens[n + 3].Digit });
						}

						n += valueCount;

						// )
						n++;
					}
					else if (tokens[n].Kind == TokenKind.RCurly)
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

			return materials.ToArray();
		}

		static Object LoadObject(Token[] tokens, ref int n, Material[] materials)
		{
			Object obj = new Object();

			obj.Name = tokens[n + 1].String;

			n += 3;

			while (true)
			{
                if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "facet")
                {
                    obj.Facet = (float)tokens[n + 1].Digit;
                    n += 2;
                }
                else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "visible")
                {
                    obj.Visible = tokens[n + 1].Digit > 0;
                    n += 2;
                }
                else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "vertex")
                {
                    obj.Vertexes = new Vector3D[(int)tokens[n + 1].Digit];
                    n += 3;

                    for (int vind = 0; vind < obj.Vertexes.Length; vind++)
                    {
                        obj.Vertexes[vind].X = (float)tokens[n + 0].Digit;
                        obj.Vertexes[vind].Y = (float)tokens[n + 1].Digit;
                        obj.Vertexes[vind].Z = (float)tokens[n + 2].Digit;
                        n += 3;
                    }

                    n += 1;
                }
                else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "face")
                {
                    obj.Faces = new Face[(int)tokens[n + 1].Digit];
                    for (int i = 0; i < obj.Faces.Length; i++)
                    {
                        obj.Faces[i] = new Face();
                    }

                    n += 3;

                    for (int find = 0; find < obj.Faces.Length; find++)
                    {
                        int count = (int)tokens[n + 0].Digit;
                        obj.Faces[find].Indexes = new int[count];
                        obj.Faces[find].UV = new float[count * 2];
                        obj.Faces[find].Colors = new Color[count];

                        for (int fvind = 0; fvind < count; fvind++)
                        {
                            obj.Faces[find].Colors[fvind] = new Color(1.0f, 1.0f, 1.0f, 1.0f);
                        }

                        n += 1;

                        while (true)
                        {
                            if (tokens[n].Kind == TokenKind.Digit)
                            {
                                break;
                            }
                            else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "V")
                            {
                                n += 2;

                                for (int c = 0; c < count; c++)
                                {
                                    obj.Faces[find].Indexes[c] = (int)tokens[n + c].Digit;
                                }
                                n += count;

                                n += 1;
                            }
                            else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "M")
                            {
                                n += 2;
                                obj.Faces[find].MaterialIndex = (int)tokens[n + 0].Digit;
                                n += 1;
                                n += 1;
                            }
                            else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "UV")
                            {
                                n += 2;

                                for (int c = 0; c < count; c++)
                                {
                                    obj.Faces[find].UV[c * 2 + 0] = (float)tokens[n + c * 2 + 0].Digit;
                                    obj.Faces[find].UV[c * 2 + 1] = (float)tokens[n + c * 2 + 1].Digit;
                                }

                                n += count * 2;
                                n += 1;
                            }
                            else if (tokens[n].Kind == TokenKind.Letter && tokens[n].Letter == "COL")
                            {
                                n += 2;

                                for (int c = 0; c < count; c++)
                                {
                                    var color = (UInt64)tokens[n + c].Digit;
                                    var r = (color & 0x000000FF) >> 0;
                                    var g = (color & 0x0000FF00) >> 8;
                                    var b = (color & 0x00FF0000) >> 16;
                                    var a = (color & 0xFF000000) >> 24;

                                    obj.Faces[find].Colors[c] = new Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
                                }
                                n += count;

                                n += 1;
                            }
                            else if (tokens[n].Kind == TokenKind.RCurly)
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
                else if (tokens[n].Kind == TokenKind.RCurly)
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
	}
}
