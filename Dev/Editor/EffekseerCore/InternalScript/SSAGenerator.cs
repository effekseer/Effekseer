using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.InternalScript
{
	/// <summary>
	/// generate Static single assignment form
	/// </summary>
	/// <remarks>
	/// https://en.wikipedia.org/wiki/Static_single_assignment_form
	/// </remarks>
	class SSAGenerator
	{
		public class Node
		{
			public Expression Expression;
			public List<Value> Inputs = new List<Value>();
			public List<Value> Outputs = new List<Value>();
		}

		public class NodeConstantNumber : Node
		{
			public float Value = 0.0f;
		}

		public class NodeOperator : Node
		{
			public OperatorType Type;
		}

		public class NodePredefined :Node
		{
			public string Value = string.Empty;

			public NodePredefined(string value)
			{
				Value = value;
			}
		}

		public class Value
		{
			public int Index = 0;
			public Node Generator = null;
			public int RegisterIndex = -1;
		}

		public class Attribute
		{
			public object Value = null;

			public Attribute(object value = null)
			{
				Value = value;
			}
		}

		public class SymbolTable
		{
			public Dictionary<string, Attribute> Tables = new Dictionary<string, Attribute>();

			public SymbolTable Clone()
			{
				var ret = new SymbolTable();
				foreach (var kv in Tables)
				{
					var value = kv.Value.Value;

					if (value is Value)
					{
						ret.Tables.Add(kv.Key, new Attribute(value));
					}

					if (value is SymbolTable)
					{
						var table = value as SymbolTable;
						ret.Tables.Add(kv.Key, new Attribute(table));
					}
				}

				return ret;
			}
		}

		class EvalOption
		{
			/// <summary>
			/// Get value as writing target
			/// </summary>
			public bool ForWriting = false;
		}

		public SymbolTable RootTable = new SymbolTable();


		object EvalAttribute(AttributeExpression expr, EvalOption option)
		{
			var value = Eval(expr.Value, option);
			if (value is Attribute)
			{
				value = (value as Attribute).Value;
			}

			var table = value as SymbolTable;

			if (table == null)
			{
				throw new UnknownValueException(expr.Attribute, expr.Line);
			}

			if (!table.Tables.ContainsKey(expr.Attribute))
			{
				throw new UnknownValueException(expr.Attribute, expr.Line);
			}

			return table.Tables[expr.Attribute];
		}

		void EvalSubstitution(SubstitutionExpression expr, EvalOption option)
		{
			option = new EvalOption();
			option.ForWriting = true;

			var target = Eval(expr.Target, option);
			var value = Eval(expr.Value, null);
			if (value is Attribute)
			{
				value = (value as Attribute).Value;
			}

			var targetAttribute = target as Attribute;
			if (targetAttribute == null)
			{
				throw new InvalidSubstitution(expr.Line);
			}

			if (value is Value)
			{
				targetAttribute.Value = value;
			}
		}

		object EvalLabel(LabelExpression expr, EvalOption option)
		{
			if (!RootTable.Tables.ContainsKey(expr.Value))
			{
				if (option != null && option.ForWriting)
				{
					var attribute = new Attribute();
					RootTable.Tables[expr.Value] = attribute;
				}
				else
				{
					throw new UnknownValueException(expr.Value, expr.Line);
				}
			}

			return RootTable.Tables[expr.Value];
		}

		object EvalFunction(FunctionExpression expr, EvalOption option)
		{
			var node = new NodeOperator();
			node.Expression = expr;
			if (expr.Value == "sin")
			{
				node.Type = OperatorType.Sine;
				if (expr.Args.Count() != 1) throw new ArgSizeException(expr.Args.Count(), 1, expr.Line);
			}

			if (expr.Value == "cos")
			{
				node.Type = OperatorType.Cos;
				if (expr.Args.Count() != 1) throw new ArgSizeException(expr.Args.Count(), 1, expr.Line);
			}

			if (expr.Value == "rand")
			{
				node.Type = OperatorType.Rand;
				
				if (expr.Args.Count() == 0)
				{
					node.Type = OperatorType.Rand;
				}
				else if (expr.Args.Count() == 1)
				{
					node.Type = OperatorType.Rand_WithSeed;
				}
				else
				{
					throw new ArgSizeException(expr.Args.Count(), new[] { 0, 1 }, expr.Line);
				}
			}

			// input
			for(int i = 0; i < expr.Args.Length; i++)
			{
				var input = Eval(expr.Args[i], null);
				if (input is Attribute)
				{
					input = (input as Attribute).Value;
				}

				if (input is SymbolTable)
				{
					var retTable = new SymbolTable();
					int ind = 0;
					foreach (var table in (input as SymbolTable).Tables)
					{
						if (!(table.Value.Value is Value))
						{
							throw new InvalidOperationException(expr.Line);
						}

						node.Inputs.Add(table.Value.Value as Value);
					}
					return retTable;
				}
				else if (input is Value)
				{
					node.Inputs.Add(input as Value);
				}
				else
				{
					throw new Exception();
				}
			}

			// output
			if (expr.Args.Count() > 0)
			{
				var input = Eval(expr.Args[0], null);
				if (input is Attribute)
				{
					input = (input as Attribute).Value;
				}

				if (input is SymbolTable)
				{
					var retTable = new SymbolTable();
					int ind = 0;
					foreach (var table in (input as SymbolTable).Tables)
					{
						var ret = new Value();
						ret.Index = ind;
						ret.Generator = node;
						node.Outputs.Add(ret);
						retTable.Tables.Add(table.Key, new Attribute(ret));
						ind++;
					}
					return retTable;
				}
				else if (input is Value)
				{
					var ret = new Value();
					ret.Index = 0;
					ret.Generator = node;
					node.Outputs.Add(ret);
					return ret;
				}
				else
				{
					throw new Exception();
				}

			}
			else
			{
				var ret = new Value();
				ret.Index = 0;
				ret.Generator = node;
				node.Outputs.Add(ret);
				return ret;
			}
		}

		object Eval(Expression expression, EvalOption option)
		{
			if (expression is AttributeExpression)
			{
				return EvalAttribute(expression as AttributeExpression, option);
			}
			else if (expression is SubstitutionExpression)
			{
				EvalSubstitution(expression as SubstitutionExpression, option);
				return null;
			}
			else if (expression is LabelExpression)
			{
				return EvalLabel(expression as LabelExpression, option);
			}
			else if (expression is NumberExpression)
			{
				var expr = expression as NumberExpression;
				var value = new Value();
				var node = new NodeConstantNumber();
				node.Expression = expr;
				node.Value = expr.Value;
				node.Outputs.Add(value);
				value.Generator = node;
				value.Index = 0;
				return value;
			}
			else if (expression is FunctionExpression)
			{
				return EvalFunction(expression as FunctionExpression, option);
			}
			else if (expression is BinOpExpression)
			{
				var expr = expression as BinOpExpression;
				var node = new NodeOperator();
				node.Expression = expr;
				if (expr.Operator == "+") node.Type = OperatorType.Add;
				if (expr.Operator == "-") node.Type = OperatorType.Sub;
				if (expr.Operator == "*") node.Type = OperatorType.Mul;
				if (expr.Operator == "/") node.Type = OperatorType.Div;


				var lhs = Eval(expr.Lhs, null);
				if (lhs is Attribute)
				{
					lhs = (lhs as Attribute).Value;
				}

				var rhs = Eval(expr.Rhs, null);
				if (rhs is Attribute)
				{
					rhs = (rhs as Attribute).Value;
				}

				if (lhs is SymbolTable)
				{
					var retTable = new SymbolTable();
					int ind = 0;
					foreach (var table in (lhs as SymbolTable).Tables)
					{
						var ret = new Value();
						ret.Index = ind;
						ret.Generator = node;
						node.Inputs.Add(table.Value.Value as Value);

						if (!(table.Value.Value is Value))
						{
							throw new InvalidOperationException(expression.Line);
						}

						if (rhs is Value)
						{
							node.Inputs.Add(rhs as Value);
						}
						else if (rhs is SymbolTable)
						{
							var rhs_ = rhs as SymbolTable;
							if (!rhs_.Tables.ContainsKey(table.Key))
							{
								throw new InvalidOperationException(expression.Line);
							}

							var value = rhs_.Tables[table.Key].Value as Value;
							if (value == null)
							{
								throw new InvalidOperationException(expression.Line);
							}

							node.Inputs.Add(value);
						}

						node.Outputs.Add(ret);
						retTable.Tables.Add(table.Key, new Attribute(ret));
						ind++;
					}
					return retTable;
				}
				else if (lhs is Value)
				{
					if (!(rhs is Value))
					{
						throw new InvalidOperationException(expression.Line);
					}

					var ret = new Value();
					ret.Index = 0;
					ret.Generator = node;
					node.Inputs.Add(lhs as Value);
					node.Inputs.Add(rhs as Value);
					node.Outputs.Add(ret);
					return ret;
				}
			}
			else if (expression is UnaryOpExpression)
			{
				var expr = expression as UnaryOpExpression;
				var node = new NodeOperator();
				if (expr.Operator == "+") node.Type = OperatorType.UnaryAdd;
				if (expr.Operator == "-") node.Type = OperatorType.UnarySub;

				var input = Eval(expr.Expr, null);
				if (input is Attribute)
				{
					input = (input as Attribute).Value;
				}

				if (input is SymbolTable)
				{
					var retTable = new SymbolTable();
					int ind = 0;
					foreach (var table in (input as SymbolTable).Tables)
					{
						var ret = new Value();
						ret.Index = ind;
						ret.Generator = node;

						if (!(table.Value.Value is Value))
						{
							throw new InvalidOperationException(expression.Line);
						}

						node.Inputs.Add(table.Value.Value as Value);
						node.Outputs.Add(ret);
						retTable.Tables.Add(table.Key, new Attribute(ret));
						ind++;
					}
					return retTable;
				}
				else if (input is Value)
				{
					var ret = new Value();
					ret.Index = 0;
					ret.Generator = node;
					node.Inputs.Add(input as Value);
					node.Outputs.Add(ret);
					return ret;
				}
			}

			throw new Exception();
		}

		public void Eval(Expression[] expressions)
		{
			RootTable = new SymbolTable();

			Func<string, Value> generatePredefined = (source) =>
			{
				var v = new Value();
				var generator = new NodePredefined(source);
				generator.Outputs.Add(v);
				v.Generator = generator;
				return v;
			};

			RootTable.Tables.Add("@1", new Attribute(generatePredefined("@1")));
			RootTable.Tables.Add("@2", new Attribute(generatePredefined("@2")));
			RootTable.Tables.Add("@3", new Attribute(generatePredefined("@3")));
			RootTable.Tables.Add("@4", new Attribute(generatePredefined("@4")));
			RootTable.Tables.Add("@GTime", new Attribute(generatePredefined("@GTime")));
			RootTable.Tables.Add("@PTime", new Attribute(generatePredefined("@PTime")));

			var localParam = new SymbolTable();
			localParam.Tables.Add("x", new Attribute(generatePredefined("@P.x")));
			localParam.Tables.Add("y", new Attribute(generatePredefined("@P.y")));
			localParam.Tables.Add("z", new Attribute(generatePredefined("@P.z")));
			localParam.Tables.Add("w", new Attribute(generatePredefined("@P.w")));

			RootTable.Tables.Add("@P", new Attribute(localParam));

			var outputParam = new SymbolTable();
			outputParam.Tables.Add("x", new Attribute(generatePredefined("@P.x")));
			outputParam.Tables.Add("y", new Attribute(generatePredefined("@P.y")));
			outputParam.Tables.Add("z", new Attribute(generatePredefined("@P.z")));
			outputParam.Tables.Add("w", new Attribute(generatePredefined("@P.w")));

			RootTable.Tables.Add("@O", new Attribute(outputParam));

			RootTable.Tables.Add("@In0", new Attribute(generatePredefined("@In0")));
			RootTable.Tables.Add("@In1", new Attribute(generatePredefined("@In1")));
			RootTable.Tables.Add("@In2", new Attribute(generatePredefined("@In2")));
			RootTable.Tables.Add("@In3", new Attribute(generatePredefined("@In3")));

			foreach (var expression in expressions)
			{
				Eval(expression, null);
			}
		}

		public Node[] GatherNodes(Value[] values)
		{
			List<Value> enabledValues = new List<Value>();
			List<Node> enabledNodes = new List<Node>();

			// correct using values
			Action<Value> collectValues = null;
			collectValues = (Value value) =>
			{
				if (enabledValues.Contains(value))
					return;

				enabledValues.Add(value);
				if (value.Generator != null)
				{
					enabledNodes.Add(value.Generator);

					foreach (var input in value.Generator.Inputs)
					{
						collectValues(input);
					}
				}
			};

			foreach(var value in values)
			{
				collectValues(value);
			}

			// sort
			List<Node> nodes = new List<Node>();

			while (enabledNodes.Count > 0)
			{
				var usedForInput = enabledNodes.SelectMany(_ => _.Inputs).ToArray();
				var restValues = enabledValues.Where(_ => !usedForInput.Contains(_)).ToArray();

				foreach(var value in restValues)
				{
					if(value.Generator != null)
					{
						nodes.Add(value.Generator);
					}
					enabledNodes.Remove(value.Generator);
					enabledValues.Remove(value);
				}
			}

			nodes.Reverse();

			return nodes.ToArray();
		}
	}
}
