using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO.Realtime
{
	public class Model
	{
		public Material[] Materials = new Material[0];

		public Object[] Objects = new Object[0];


		public static unsafe Model Create(mqoIO.Data mqodata)
		{
			var model = new Model();

			// マテリアル変換

			// マテリアルなしのポリゴンが存在するか
			var hasNoMaterial = mqodata.Objects.Where(_ => _.Faces.Where(__ => __.MaterialIndex == -1).Count() > 0).Count() > 0;

			if (hasNoMaterial)
			{
				model.Materials = new Material[mqodata.Materials.Length + 1];
				model.Materials[mqodata.Materials.Length] = new Material();
			}
			else
			{
				model.Materials = new Material[mqodata.Materials.Length];
			}

			foreach (var item in mqodata.Materials.Select((v, i) => new { v, i }))
			{
				model.Materials[item.i] = new Material();

				if (item.v.Paramaters.ContainsKey("tex"))
				{
					model.Materials[item.i].ColorTexture = (string)item.v.Paramaters["tex"];
				}

				if (item.v.Paramaters.ContainsKey("aplane"))
				{
					model.Materials[item.i].AlphaTexture = (string)item.v.Paramaters["aplane"];
				}

				if (item.v.Paramaters.ContainsKey("bump"))
				{
					model.Materials[item.i].NormalTexture = (string)item.v.Paramaters["bump"];
				}

				var col = (float[])item.v.Paramaters["col"];
				var dif = (float[])item.v.Paramaters["dif"];
				var amb = (float[])item.v.Paramaters["amb"];

				model.Materials[item.i].Diffuse = new Color(col[0] * dif[0], col[1] * dif[0], col[2] * dif[0], col[3]);
				model.Materials[item.i].Ambient = new Color(dif[0] * 0.2f, dif[0] * 0.2f, dif[0] * 0.2f, 0.0f);
			}

            var enabledObjects = mqodata.Objects.Where(_ => _.Visible).ToArray();

			model.Objects = new Object[enabledObjects.Length];

			// オブジェクト変換
			foreach (var item in enabledObjects.Select((v, i) => new { v, i }))
			{
				model.Objects[item.i] = CreateObject(item.v, model.Materials);
			}

			return model;
		}

		static unsafe Object CreateObject(mqoIO.Object original, Material[] materials)
		{
			var vertexes = new List<Vertex>();
			var faces = new List<Face>();
			var obj = new Object();

			obj.Name = original.Name;

			// 頂点一時コピー
			foreach (var vertex in original.Vertexes)
			{
				var v = new Vertex();
				v.Position.X = vertex.X;
				v.Position.Y = vertex.Y;
				v.Position.Z = vertex.Z;
				vertexes.Add(v);
			}

			// 面一時コピー
			foreach (var face in original.Faces)
			{
				if (face.Indexes.Count() == 2)
				{

				}
				else if (face.Indexes.Count() == 3)
				{
					var f = new Face();
					f.Indexes[0] = face.Indexes[0];
					f.Indexes[1] = face.Indexes[1];
					f.Indexes[2] = face.Indexes[2];

					f.Colors[0] = face.Colors[0];
					f.Colors[1] = face.Colors[1];
					f.Colors[2] = face.Colors[2];

					f.UV[0].X = face.UV[0 * 2 + 0];
					f.UV[0].Y = face.UV[0 * 2 + 1];
					f.UV[1].X = face.UV[1 * 2 + 0];
					f.UV[1].Y = face.UV[1 * 2 + 1];
					f.UV[2].X = face.UV[2 * 2 + 0];
					f.UV[2].Y = face.UV[2 * 2 + 1];

					f.Material = face.MaterialIndex;
					if (f.Material < 0) f.Material = materials.Length - 1;
					faces.Add(f);
				}
				if (face.Indexes.Count() == 4)
				{
					var f1 = new Face();
					f1.Indexes[0] = face.Indexes[0];
					f1.Indexes[1] = face.Indexes[1];
					f1.Indexes[2] = face.Indexes[2];

					f1.Colors[0] = face.Colors[0];
					f1.Colors[1] = face.Colors[1];
					f1.Colors[2] = face.Colors[2];

					f1.UV[0].X = face.UV[0 * 2 + 0];
					f1.UV[0].Y = face.UV[0 * 2 + 1];
					f1.UV[1].X = face.UV[1 * 2 + 0];
					f1.UV[1].Y = face.UV[1 * 2 + 1];
					f1.UV[2].X = face.UV[2 * 2 + 0];
					f1.UV[2].Y = face.UV[2 * 2 + 1];

					f1.Material = face.MaterialIndex;
					if (f1.Material < 0) f1.Material = materials.Length - 1;
					faces.Add(f1);

					var f2 = new Face();
					f2.Indexes[0] = face.Indexes[0];
					f2.Indexes[1] = face.Indexes[2];
					f2.Indexes[2] = face.Indexes[3];

					f2.Colors[0] = face.Colors[0];
					f2.Colors[1] = face.Colors[2];
					f2.Colors[2] = face.Colors[3];

					f2.UV[0].X = face.UV[0 * 2 + 0];
					f2.UV[0].Y = face.UV[0 * 2 + 1];
					f2.UV[1].X = face.UV[2 * 2 + 0];
					f2.UV[1].Y = face.UV[2 * 2 + 1];
					f2.UV[2].X = face.UV[3 * 2 + 0];
					f2.UV[2].Y = face.UV[3 * 2 + 1];

					f2.Material = face.MaterialIndex;
					if (f2.Material < 0) f2.Material = materials.Length - 1;
					faces.Add(f2);
				}
			}

			// 法線による分割
			{
				var normals = new Vector3D[faces.Count];

				var vertextoface = new Dictionary<int, List<int>>();
				for (int i = 0; i < vertexes.Count; i++)
				{
					vertextoface.Add(i, new List<int>());
				}


				// 法線を求める+所属頂点を設定
				foreach (var fiv in faces.Select((v, i) => new { v, i }))
				{
					var fv = fiv.v;
					var v1 = vertexes[fv.Indexes[0]].Position - vertexes[fv.Indexes[1]].Position;
					var v2 = vertexes[fv.Indexes[2]].Position - vertexes[fv.Indexes[1]].Position;

					Vector3D n;
					Vector3D.Cross(out n, ref v1, ref v2);
					Vector3D.Normal(out n, ref n);
					normals[fiv.i] = n;

					vertextoface[fv.Indexes[0]].Add(fiv.i);
					vertextoface[fv.Indexes[1]].Add(fiv.i);
					vertextoface[fv.Indexes[2]].Add(fiv.i);
				}

				// 面の法線から頂点分離
				foreach (var vf in vertextoface)
				{
					var fgroup = new List<List<int>>();

					// 分別
					foreach (var fi in vf.Value)
					{
						var setted = false;
						foreach (var fg in fgroup)
						{
							Vector3D cross;
							Vector3D.Cross(out cross, ref normals[fg[0]], ref normals[fi]);
							var s = cross.GetLength();
							var c = Vector3D.Dot(ref normals[fg[0]], ref normals[fi]);

							if (Math.Atan2(s, c) < original.Facet / 180.0 * 3.14)
							{
								fg.Add(fi);
								setted = true;
								break;
							}
						}

						if (!setted)
						{
							fgroup.Add(new List<int>(new int[] { fi }));
						}
					}

					// 分離
					if (fgroup.Count > 1)
					{
						for (int i = 1; i < fgroup.Count; i++)
						{
							// 新規頂点
							vertexes.Add(new Vertex());
							var oldV = vf.Key;
							var newV = vertexes.Count - 1;
							vertexes[newV] = vertexes[oldV].Clone();

							foreach (var fi in fgroup[i])
							{
								var f = faces[fi];
								if (f.Indexes[0] == oldV) f.Indexes[0] = newV;
								if (f.Indexes[1] == oldV) f.Indexes[1] = newV;
								if (f.Indexes[2] == oldV) f.Indexes[2] = newV;
								faces[fi] = f;
							}
						}
					}
				}

				// クリア
				foreach (var bt in vertextoface)
				{
					bt.Value.Clear();
				}

				// 所属頂点を設定
				foreach (var fiv in faces.Select((v, i) => new { v, i }))
				{
					for (int loop = 0; loop < 3; loop++)
					{
						var fv = fiv.v;
						if (!vertextoface.ContainsKey(fv.Indexes[loop]))
						{
							vertextoface.Add(fv.Indexes[loop], new List<int>());
						}
						vertextoface[fv.Indexes[loop]].Add(fiv.i);
					}
				}

				// 法線を設定
				foreach (var vf in vertextoface)
				{
					var v = vertexes[vf.Key];
					Vector3D normal = new Vector3D();

					foreach (var f in vf.Value)
					{
						normal.X += normals[f].X;
						normal.Y += normals[f].Y;
						normal.Z += normals[f].Z;
					}

					normal.X /= (float)vf.Value.Count();
					normal.Y /= (float)vf.Value.Count();
					normal.Z /= (float)vf.Value.Count();
					v.Normal = normal;
					vertexes[vf.Key] = v;
				}

				{

				}

				// UV,Color設定
				{
					// 元の頂点とそこから派生した頂点インデックスへのDictoinary
					var originalToVertex = new Dictionary<int, List<int>>();
					foreach (var v in vertexes.Select((v, i) => new { v, i }))
					{
						originalToVertex.Add(v.i, new List<int>());
					}

					// 面の値を元に頂点を差し替え
					foreach (var f in faces.Select((v, i) => new { v, i }))
					{
						var face = faces[f.i];

						for (int v = 0; v < 3; v++)
						{
							var vert = vertexes[face.Indexes[v]];
							var o2v = originalToVertex[face.Indexes[v]];

							if (o2v.Count == 0)
							{
								// 新規
								vert.UV = face.UV[v];
								vert.Color = face.Colors[v];
								o2v.Add(face.Indexes[v]);
							}
							else
							{
								// 探索

								bool found = false;
								foreach (var vind in o2v)
								{
									if (vertexes[vind].UV == face.UV[v] && vertexes[vind].Color == face.Colors[v])
									{
										face.Indexes[v] = vind;
										found = true;
										break;
									}
								}

								if (found) continue;

								// 追加
								var newVert = new Vertex();
								newVert.Position = vert.Position;
								newVert.Normal = vert.Normal;
								newVert.UV = face.UV[v];
								newVert.Color = face.Colors[v];
								o2v.Add(vertexes.Count);
								face.Indexes[v] = vertexes.Count;
								vertexes.Add(newVert);
							}
						}
					}
				}
			}

			// 面材質によるソート
			{
				faces = faces.OrderBy(f => f.Material).ToList();
			}

			obj.Vertexes = vertexes.ToArray();
			obj.Faces = faces.ToArray();

			AppendTangentAndBinormal(obj);

			return obj;
		}

		/// <summary>
		/// TangentとBinormalを設定する。
		/// </summary>
		/// <param name="obj"></param>
		static void AppendTangentAndBinormal(Object obj)
		{
			var fToTB = new Dictionary<Face, Tuple<Vector3D, Vector3D>>();

			foreach (var face in obj.Faces)
			{
				var v0 = obj.Vertexes[face.Indexes[0]];
				var v1 = obj.Vertexes[face.Indexes[1]];
				var v2 = obj.Vertexes[face.Indexes[2]];

				Vector3D tangent;
				Vector3D binormal;

				CalculateTangentAndBinormal(
					v0.Position,
					new Vector2D(v0.UV.X, 1.0f-v0.UV.Y),
					v1.Position,
					new Vector2D(v1.UV.X, 1.0f - v1.UV.Y),
					v2.Position,
					new Vector2D(v2.UV.X, 1.0f - v2.UV.Y),
					out tangent,
					out binormal);

				fToTB.Add(face, Tuple.Create(tangent, binormal));
			}

			var vToF = obj.Vertexes.Select(
				(v, i) => Tuple.Create(
					v,
					obj.Faces.Where(_ => _.Indexes.Contains(i)).ToArray()));

			foreach (var vf in vToF)
			{
				Vector3D tangent = new Vector3D();
				Vector3D binormal = new Vector3D();

				foreach (var f in vf.Item2)
				{
					tangent = tangent + fToTB[f].Item1;
					binormal = binormal + fToTB[f].Item2;
				}

				vf.Item1.Tangent = tangent / (float)vf.Item2.Count();
				vf.Item1.Binormal = binormal / (float)vf.Item2.Count();
			}
		}

        /// <summary>
        /// Calculate tangent and binormal
        /// </summary>
        /// <param name="p0"></param>
        /// <param name="uv0"></param>
        /// <param name="p1"></param>
        /// <param name="uv1"></param>
        /// <param name="p2"></param>
        /// <param name="uv2"></param>
        /// <param name="tangent"></param>
        /// <param name="binormal"></param>
        static void CalculateTangentAndBinormal(
			Vector3D p0, Vector2D uv0,
			Vector3D p1, Vector2D uv1,
			Vector3D p2, Vector2D uv2,
			out Vector3D tangent, out Vector3D binormal)
		{
			var cp0 = new Vector3D[] { 
				new Vector3D(p0.X, uv0.X, uv0.Y),
				new Vector3D(p0.Y, uv0.X, uv0.Y),
				new Vector3D(p0.Z, uv0.X, uv0.Y),
				};

			var cp1 = new Vector3D[] { 
				new Vector3D(p1.X, uv1.X, uv1.Y),
				new Vector3D(p1.Y, uv1.X, uv1.Y),
				new Vector3D(p1.Z, uv1.X, uv1.Y),
				};

			var cp2 = new Vector3D[] { 
				new Vector3D(p2.X, uv2.X, uv2.Y),
				new Vector3D(p2.Y, uv2.X, uv2.Y),
				new Vector3D(p2.Z, uv2.X, uv2.Y),
				};

			var u = new float[3];
			var v = new float[3];


			for (int i = 0; i < 3; ++i)
			{
				Vector3D v1 = cp1[i] - cp0[i];
				Vector3D v2 = cp2[i] - cp1[i];
				Vector3D abc;
				Vector3D.Cross(out abc, ref v1, ref v2);

				if (abc.X == 0.0f)
				{
					Console.WriteLine("Warning : Failed to calculate tangent");
					tangent.X = 0.0f;
					tangent.Y = 0.0f;
					tangent.Z = 0.0f;

					binormal.X = 0.0f;
					binormal.Y = 0.0f;
					binormal.Z = 0.0f;
					return;
				}
				u[i] = -abc.Y / abc.X;
				v[i] = -abc.Z / abc.X;
			}

			tangent.X = u[0];
			tangent.Y = u[1];
			tangent.Z = u[2];

			binormal.X = v[0];
			binormal.Y = v[1];
			binormal.Z = v[2];

			Vector3D.Normal(out tangent, ref tangent);
			Vector3D.Normal(out binormal, ref binormal);
		}
	}
}
