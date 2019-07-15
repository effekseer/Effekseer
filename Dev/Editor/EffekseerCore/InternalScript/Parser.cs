using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.InternalScript
{
	class Expression
	{
		public int Line = 0;
	}

	class BinOpExpression : Expression
	{
		public string Operator = "";
		public Expression Lhs = null;
		public Expression Rhs = null;
	}

	class UnaryOpExpression : Expression
	{
		public string Operator = "";
		public Expression Expr = null;
	}
	class NumberExpression : Expression
	{
		public float Value;
		public NumberExpression(float value)
		{
			Value = value;
		}
	}

	class LabelExpression : Expression
	{
		public string Value;
		public LabelExpression(string value)
		{
			Value = value;
		}
	}

	class FunctionExpression : Expression
	{
		public string Value;
		public Expression[] Args = new Expression[0];
	}

	class AttributeExpression : Expression
	{
		public Expression Value = null;
		public string Attribute = null;
	}

	/// <summary>
	/// Unimplemented
	/// </summary>
	class SubstitutionExpression : Expression
	{
		public Expression Target = null;
		public Expression Value = null;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <remarks>
	/// Expr = Term {+ Term, - Term}
	/// Term = Group {* Group, / Group}
	/// Group = (Expr), Number, Value {.Value}
	/// Value = Label {Arg}
	/// Arg = (Expr, Expr...)
	/// Sentence = Expr {= Expr} { NL Sentence }
	/// </remarks>
	class Parser
	{
		List<Token> tokens = null;
		int index = 0;

		public Expression[] Parse(List<Token> tokens)
		{
			this.tokens = tokens;
			index = 0;
			return Sentence();
		}

		Expression[] Sentence()
		{
			List<Expression> sentences = new List<Expression>();

			while (true)
			{
				var empty = Peek();
				if (empty == null) break;
				if(empty.Type == TokenType.NewLine)
				{
					Next();
					continue;
				}

				var lhs = Expr();

				var token = Peek();

				if (token != null)
				{
					if(token.Type == TokenType.Equal)
					{
						Next();
						var rhs = Expr();
						var substitution = new SubstitutionExpression();
						substitution.Target = lhs;
						substitution.Value = rhs;
						sentences.Add(substitution);
					}
					else
					{
						sentences.Add(lhs);
					}
				}
				else
				{
					sentences.Add(lhs);
					break;
				}
			}

			return sentences.ToArray();
		}

		Expression Expr()
		{
			var lhs = Term();
			
			while (true)
			{
				var token = Peek();

				if (token != null)
				{
					if (token.Type == TokenType.Operator && (string)token.Value == "+")
					{
						Next();
						var rhs = Term();

						var ret = new BinOpExpression();
						ret.Line = token.Line;
						ret.Lhs = lhs;
						ret.Rhs = rhs;
						ret.Operator = (string)token.Value;
						lhs = ret;
					}
					else if (token.Type == TokenType.Operator && (string)token.Value == "-")
					{
						Next();
						var rhs = Term();

						var ret = new BinOpExpression();
						ret.Line = token.Line;
						ret.Lhs = lhs;
						ret.Rhs = rhs;
						ret.Operator = (string)token.Value;
						lhs = ret;
					}
					else if (token.Type == TokenType.Operator)
					{
						break;
					}
					else if (token.Type == TokenType.RightParentheses)
					{
						break;
					}
					else if (token.Type == TokenType.Equal || token.Type == TokenType.NewLine)
					{
						break;
					}
					else if (token.Type == TokenType.Comma)
					{
						break;
					}
					else
					{
						throw new InvalidTokenException(token.ToString(), token.Line);
					}
				}
				else
				{
					return lhs;
				}
			}

			return lhs;
		}

		Expression Term()
		{
			var lhs = Group();
			
			while (true)
			{
				var token = Peek();

				if (token != null)
				{
					if (token.Type == TokenType.Operator && (string)token.Value == "*")
					{
						Next();
						var rhs = Group();

						var ret = new BinOpExpression();
						ret.Line = token.Line;
						ret.Lhs = lhs;
						ret.Rhs = rhs;
						ret.Operator = (string)token.Value;
						lhs = ret;
					}
					else if (token.Type == TokenType.Operator && (string)token.Value == "/")
					{
						Next();
						var rhs = Group();

						var ret = new BinOpExpression();
						ret.Line = token.Line;
						ret.Lhs = lhs;
						ret.Rhs = rhs;
						ret.Operator = (string)token.Value;
						lhs = ret;
					}
					else if (token.Type == TokenType.Operator)
					{
						break;
					}
					else if (token.Type == TokenType.RightParentheses)
					{
						break;
					}
					else if (token.Type == TokenType.Equal || token.Type == TokenType.NewLine)
					{
						break;
					}
					else if (token.Type == TokenType.Comma)
					{
						break;
					}
					else
					{
						throw new InvalidTokenException(token.ToString(), token.Line);
					}
				}
				else
				{
					return lhs;
				}
			}

			return lhs;
		}

		Expression Group()
		{
			var token = Peek();
			Expression value = null;

			if(token != null)
			{
				if (token.Type == TokenType.LeftParentheses)
				{
					Next();
					var center = Expr();
					var right = Peek();

					if (right != null && right.Type == TokenType.RightParentheses)
					{
						Next();
						value = center;
					}
					else
					{
						if (token == null)
						{
							throw new InvalidEOFException(tokens.LastOrDefault().Line);
						}

						throw new CompileException(string.Format("Invalid token {0}", token), token.Line);
					}
				}
				else if (token.Type == TokenType.Operator && ((string)token.Value == "+" || (string)token.Value == "-"))
				{
					Next();
					var rhs = Group();

					var ret = new UnaryOpExpression();
					ret.Line = token.Line;
					ret.Expr = rhs;
					ret.Operator = (string)token.Value;
					value = ret;
				}
				else if (token.Type == TokenType.Label)
				{
					var ret = Value();
					ret.Line = token.Line;
					value = ret;
				}
				else if (token.Type == TokenType.Digit)
				{
					Next();
					var ret = new NumberExpression((float)token.Value);
					ret.Line = token.Line;
					value = ret;
				}
				else
				{
					throw new InvalidTokenException(token.ToString(), token.Line);
				}
			}
			else
			{
				throw new InvalidEOFException(tokens.LastOrDefault().Line);
			}

			while(true)
			{
				var dotToken = Peek();

				if (dotToken != null && dotToken.Type == TokenType.Dot)
				{
					var next = Next();
					if(next == null)
					{
						throw new InvalidEOFException(tokens.LastOrDefault().Line);
					}
					else if(next.Type != TokenType.Label)
					{
						throw new InvalidTokenException(token.ToString(), token.Line);
					}

					var v_ = new AttributeExpression();
					v_.Value = value;
					v_.Attribute = (string)next.Value;
					v_.Line = next.Line;
					value = v_;

					Next();
				}
				else
				{
					break;
				}
			}

			return value;
		}

		Expression Value()
		{
			var token = Peek();

			if (token != null)
			{
				var next = Next();

				if(next == null || next.Type != TokenType.LeftParentheses)
				{
					var ret = new LabelExpression((string)token.Value);
					ret.Line = token.Line;
					return ret;
				}
				else
				{
					var ret = new FunctionExpression();
					ret.Value = (string)token.Value;
					ret.Args = Arg();
					return ret;
				}
			}
			else
			{
				throw new Exception();
			}
		}

		Expression[] Arg()
		{
			List<Expression> exps = new List<Expression>();

			var token = Peek();

			if (token != null)
			{
				while(true)
				{
					var next = Next();
					if(next.Type == TokenType.RightParentheses)
					{
						// empty arguments
						Next();
						return exps.ToArray();
					}

					var expr = Expr();
					var next2 = Peek();

					if (next2 == null)
					{
						throw new InvalidEOFException(tokens.LastOrDefault().Line);
					}
					else if (next2.Type == TokenType.Comma)
					{
						exps.Add(expr);
					}
					else if (next2.Type == TokenType.RightParentheses)
					{
						exps.Add(expr);
						Next();
						return exps.ToArray();
					}
				}

			}
			else
			{
				throw new Exception();
			}
		}

		Token Peek()
		{
			if(tokens.Count() > index)
			{
				return tokens[index];
			}
			return null;
		}

		Token Next()
		{
			index++;
			return Peek();
		}
	}
}
