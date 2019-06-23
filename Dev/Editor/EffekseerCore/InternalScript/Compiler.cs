using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.InternalScript
{
	enum OperatorType
	{
		Constant = 0,
		Add = 1,
		Sub = 2,
		Mul = 3,
		Div = 4,

		UnaryAdd = 11,
		UnarySub = 12,

		Sine = 21,
		Cos = 22,

	}

	public enum RunningPhaseType : int
	{
		Global,
		Local,
	}

	class Operator
	{
		public OperatorType Type;
		public List<string> Inputs = new List<string>();
		public List<string> Outputs = new List<string>();
		public Dictionary<string, object> Attributes = new Dictionary<string, object>();
	}

	public class CompileResult
	{
		public byte[] Bytecode = null;
		public RunningPhaseType RunningPhase;
		public CompileException Error;
	}
	public class Compiler
	{
		RunningPhaseType runningPhase;

		public CompileResult Compile(string code)
		{
			CompileResult compileResult = new CompileResult();
			runningPhase = RunningPhaseType.Global;
			
			if(string.IsNullOrEmpty(code))
			{
				return compileResult;
			}

			// export
			List<byte[]> data = new List<byte[]>();

			try
			{
				var lexer = new Lexer();
				var tokens = lexer.Analyze(code);

				var parser = new Parser();
				var sentense = parser.Parse(tokens.Tokens);

				var ssaGenerator = new SSAGenerator();
				ssaGenerator.Eval(sentense);
				var outputAttribute =ssaGenerator.RootTable.Tables["@O"].Value;
				var outputValues = new SSAGenerator.Value[4];
				if(outputAttribute is SSAGenerator.Value)
				{
					outputValues[0] = outputAttribute as SSAGenerator.Value;
					outputValues[1] = outputAttribute as SSAGenerator.Value;
					outputValues[2] = outputAttribute as SSAGenerator.Value;
					outputValues[3] = outputAttribute as SSAGenerator.Value;
				}
				else if (outputAttribute is SSAGenerator.SymbolTable)
				{
					outputValues[0] = (outputAttribute as SSAGenerator.SymbolTable).Tables["x"].Value as SSAGenerator.Value;
					outputValues[1] = (outputAttribute as SSAGenerator.SymbolTable).Tables["y"].Value as SSAGenerator.Value;
					outputValues[2] = (outputAttribute as SSAGenerator.SymbolTable).Tables["z"].Value as SSAGenerator.Value;
					outputValues[3] = (outputAttribute as SSAGenerator.SymbolTable).Tables["w"].Value as SSAGenerator.Value;
				}

				var nodes = ssaGenerator.GatherNodes(outputValues);

				var registerOffset = 0;
				var validNodeCount = 0;

				foreach(var node in nodes)
				{
					if(node is SSAGenerator.NodePredefined)
					{
						var node_ = node as SSAGenerator.NodePredefined;
						foreach (var v in node.Outputs)
						{
							v.RegisterIndex = GetInputIndex(node_.Value);
						}
					}
					else
					{
						foreach(var v in node.Outputs)
						{
							v.RegisterIndex = registerOffset;
							registerOffset += 1;
						}
						validNodeCount++;
					}
				}

				// Operators
				List<byte[]> dataOp = new List<byte[]>();

				foreach (var node in nodes)
				{
					if(node is SSAGenerator.NodeConstantNumber)
					{
						var node_ = node as SSAGenerator.NodeConstantNumber;

						dataOp.Add(BitConverter.GetBytes((int)OperatorType.Constant));
						dataOp.Add(BitConverter.GetBytes((int)node.Inputs.Count));
						dataOp.Add(BitConverter.GetBytes((int)node.Outputs.Count));
						dataOp.Add(BitConverter.GetBytes(1));

						foreach (var o in node.Inputs)
						{
							dataOp.Add(BitConverter.GetBytes((int)o.RegisterIndex));
						}

						foreach (var o in node.Outputs)
						{
							dataOp.Add(BitConverter.GetBytes((int)o.RegisterIndex));
						}

						dataOp.Add(BitConverter.GetBytes(node_.Value));
					}
					else if (node is SSAGenerator.NodePredefined)
					{
						continue;
					}
					else if (node is SSAGenerator.NodeOperator)
					{
						var node_ = node as SSAGenerator.NodeOperator;

						dataOp.Add(BitConverter.GetBytes((int)node_.Type));
						dataOp.Add(BitConverter.GetBytes((int)node.Inputs.Count));
						dataOp.Add(BitConverter.GetBytes((int)node.Outputs.Count));
						dataOp.Add(BitConverter.GetBytes(0));

						foreach (var o in node.Inputs)
						{
							dataOp.Add(BitConverter.GetBytes((int)o.RegisterIndex));
						}

						foreach (var o in node.Outputs)
						{
							dataOp.Add(BitConverter.GetBytes((int)o.RegisterIndex));
						}
					}
				}

				int version = 0;
				data.Add(BitConverter.GetBytes(version));
				data.Add(BitConverter.GetBytes((int)runningPhase));
				data.Add(BitConverter.GetBytes(registerOffset));
				data.Add(BitConverter.GetBytes(validNodeCount));
				data.Add(BitConverter.GetBytes(outputValues[0].RegisterIndex));
				data.Add(BitConverter.GetBytes(outputValues[1].RegisterIndex));
				data.Add(BitConverter.GetBytes(outputValues[2].RegisterIndex));
				data.Add(BitConverter.GetBytes(outputValues[3].RegisterIndex));
				data.Add(dataOp.SelectMany(_ => _).ToArray());

				/*
				Compile(sentense[0]);

				Dictionary<string, int> variableList = new Dictionary<string, int>();
				foreach (var opt in operators)
				{
					foreach (var o in opt.Inputs.Concat(opt.Outputs))
					{
						if (IsValidLabel(o)) continue;

						if (!variableList.ContainsKey(o))
						{
							variableList.Add(o, variableList.Count);
						}
					}
				}

				int outputIndex = -1;

				// Output register
				var outputName = GetOutputName(sentense[0]);
				if (variableList.ContainsKey(outputName))
				{
					outputIndex = variableList[outputName];
				}
				else
				{
					outputIndex = GetInputIndex(outputName);
				}

				// Operators
				List<byte[]> dataOp = new List<byte[]>();

				foreach (var op in operators)
				{
					dataOp.Add(BitConverter.GetBytes((int)op.Type));
					dataOp.Add(BitConverter.GetBytes((int)op.Inputs.Count));
					dataOp.Add(BitConverter.GetBytes((int)op.Outputs.Count));

					// Attribute
					if (op.Type == OperatorType.Constant)
					{
						dataOp.Add(BitConverter.GetBytes(1));
					}
					else
					{
						dataOp.Add(BitConverter.GetBytes(0));
					}

					foreach (var o in op.Inputs)
					{
						if (variableList.ContainsKey(o))
						{
							var index = variableList[o];
							dataOp.Add(BitConverter.GetBytes(index));
						}
						else
						{
							var index = GetInputIndex(o);
							dataOp.Add(BitConverter.GetBytes(index));
						}
					}

					foreach (var o in op.Outputs)
					{
						if (variableList.ContainsKey(o))
						{
							var index = variableList[o];
							dataOp.Add(BitConverter.GetBytes(index));
						}
						else
						{
							var index = GetInputIndex(o);
							dataOp.Add(BitConverter.GetBytes(index));
						}
					}

					// Attribute
					if (op.Type == OperatorType.Constant)
					{
						var value = (float)op.Attributes["Constant"];
						dataOp.Add(BitConverter.GetBytes(value));
					}
				}

				int version = 0;
				data.Add(BitConverter.GetBytes(version));
				data.Add(BitConverter.GetBytes((int)runningPhase));
				data.Add(BitConverter.GetBytes(variableList.Count));
				data.Add(BitConverter.GetBytes(operators.Count));
				data.Add(BitConverter.GetBytes(outputIndex));
				data.Add(dataOp.SelectMany(_=>_).ToArray());
				*/
			}
			catch (CompileException e)
			{
				compileResult.Error = e;
				return compileResult;
			}

			compileResult.Bytecode = data.SelectMany(_ => _).ToArray();
			compileResult.RunningPhase = runningPhase;
			return compileResult;
		}

		/*
		void Compile(Expression expr)
		{
			if (expr is BinOpExpression)
			{
				var e = expr as BinOpExpression;
				var o = new Operator();
				if (e.Operator == "+") o.Type = OperatorType.Add;
				if (e.Operator == "-") o.Type = OperatorType.Sub;
				if (e.Operator == "*") o.Type = OperatorType.Mul;
				if (e.Operator == "/") o.Type = OperatorType.Div;

				Compile(e.Lhs);
				Compile(e.Rhs);

				o.Inputs.Add(GetOutputName(e.Lhs));
				o.Inputs.Add(GetOutputName(e.Rhs));
				o.Outputs.Add(GetOutputName(e));
				operators.Add(o);
			}
			else if (expr is UnaryOpExpression)
			{
				var e = expr as UnaryOpExpression;
				var o = new Operator();
				if (e.Operator == "+") o.Type = OperatorType.UnaryAdd;
				if (e.Operator == "-") o.Type = OperatorType.UnarySub;

				Compile(e.Expr);

				o.Inputs.Add(GetOutputName(e.Expr));
				o.Outputs.Add(GetOutputName(e));
				operators.Add(o);
			}
			else if (expr is FunctionExpression)
			{
				var e = expr as FunctionExpression;

				if(e.Value == "sin")
				{
					if (e.Args.Count() != 1) throw new ArgSizeException(e.Args.Count(), 1, e.Line);

					Compile(e.Args[0]);

					var o = new Operator();
					o.Type = OperatorType.Sine;
					o.Inputs.Add(GetOutputName(e.Args[0]));
					o.Outputs.Add(GetOutputName(e));
					operators.Add(o);
				}
				else if (e.Value == "cos")
				{
					if (e.Args.Count() != 1) throw new ArgSizeException(e.Args.Count(), 1, e.Line);

					Compile(e.Args[0]);

					var o = new Operator();
					o.Type = OperatorType.Cos;
					o.Inputs.Add(GetOutputName(e.Args[0]));
					o.Outputs.Add(GetOutputName(e));
					operators.Add(o);
				}
				else
				{
					throw new UnknownFunctionException(e.Value, e.Line);
				}
			}
			else if (expr is LabelExpression)
			{
				var e = expr as LabelExpression;
				if (!IsValidLabel(e.Value))
				{
					throw new CompileException(string.Format("Invalid label {0}", e.Value), e.Line);
				}

			}
			else if (expr is NumberExpression)
			{
				var e = expr as NumberExpression;
				var o = new Operator();
				o.Type = OperatorType.Constant;
				o.Outputs.Add(GetOutputName(e));
				o.Attributes.Add("Constant", e.Value);
				operators.Add(o);
			}
		}
		*/
		string GetOutputName(Expression expr)
		{
			if(expr is LabelExpression)
			{
				var e = expr as LabelExpression;
				return e.Value;
			}
			else
			{
				return expr.Line.ToString();
			}
		}

		bool IsValidLabel(string label)
		{
			HashSet<string> valid = new HashSet<string>();
			valid.Add("@1");
			valid.Add("@2");
			valid.Add("@3");
			valid.Add("@4");

			valid.Add("@GTime");

			valid.Add("@P.x");
			valid.Add("@P.y");
			valid.Add("@P.z");
			valid.Add("@P.w");
			valid.Add("@PTime");

			return valid.Contains(label);
		}

		int GetInputIndex(string label)
		{
			if (label == "@1") return 0 + 0x1000;
			if (label == "@2") return 1 + 0x1000;
			if (label == "@3") return 2 + 0x1000;
			if (label == "@4") return 3 + 0x1000;

			if (label == "@GTime") return 0 + 0x1000 + 0x100;

			runningPhase = RunningPhaseType.Local;

			if (label == "@P.x") return 0 + 0x1000 + 0x200;
			if (label == "@P.y") return 1 + 0x1000 + 0x200;
			if (label == "@P.z") return 2 + 0x1000 + 0x200;
			if (label == "@P.w") return 3 + 0x1000 + 0x200;
			if (label == "@PTime") return 4 + 0x1000 + 0x200;

			throw new Exception();
		}
	}
}
