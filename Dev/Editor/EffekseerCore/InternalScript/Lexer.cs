using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.InternalScript
{
	enum TokenType
	{
		Digit,
		Label,
		Operator,
		LeftParentheses,
		RightParentheses,
		Comma,
		Equal,
		Dot,
		NewLine,
	}
	class Token
	{
		public TokenType Type;
		public object Value;
		public int Line = 0;

		public override string ToString()
		{
			if (Type == TokenType.Digit) return Value.ToString();
			if (Type == TokenType.Label) return Value.ToString();
			if (Type == TokenType.Operator) return Value.ToString();
			if (Type == TokenType.LeftParentheses) return "(";
			if (Type == TokenType.RightParentheses) return ")";
			if (Type == TokenType.Comma) return ",";
			if (Type == TokenType.Equal) return "=";
			if (Type == TokenType.Dot) return ".";
			return "?";
		}
	}

	enum ErrorType
	{
		None,
		InvalidCharacter,
	}

	class LexerResult
	{
		public List<Token> Tokens = new List<Token>();

		public ErrorType Error = ErrorType.None;
		public Dictionary<string, object> ErrorInfo = new Dictionary<string, object>();
	}

	class Lexer
	{
		public Lexer()
		{
			operators.Add('+');
			operators.Add('-');
			operators.Add('*');
			operators.Add('/');
		}

		public LexerResult Analyze(string code)
		{
			List<Token> tokens = new List<Token>();

			// check special case
			// ..
			if(code.Contains(".."))
			{
				throw new InvalidTokenException(".", code.IndexOf(".."));
			}

			int index = 0;
		
			while (index < code.Length)
			{
				var c = code[index];
				var type = GetElemenetType(c);

				if(type == ElementType.Connect)
				{
					if(index + 1 < code.Length && GetElemenetType(code[index]) == ElementType.NewLine)
					{
						index += 2;
					}
					else
					{
						index += 1;
					}
				}
				else if(type == ElementType.NewLine)
				{
					var token = new Token();
					token.Type = TokenType.NewLine;
					token.Line = index;
					tokens.Add(token);
					index++;
				}
				else if (StartLabel(code, index))
				{
					var token = ParseLabel(code, ref index);
					tokens.Add(token);
				}
				else if (StartOperator(code, index))
				{
					var token = ParseOperator(code, ref index);
					tokens.Add(token);
				}
				else if (StartDigit(code, index))
				{
					var token = ParseDigit(code, ref index);
					tokens.Add(token);
				}
				else if(type == ElementType.LeftParentheses)
				{
					var token = new Token();
					token.Type = TokenType.LeftParentheses;
					token.Line = index;
					tokens.Add(token);
					index++;
				}
				else if (type == ElementType.RightParentheses)
				{
					var token = new Token();
					token.Type = TokenType.RightParentheses;
					token.Line = index;
					tokens.Add(token);
					index++;
				}
				else if (type == ElementType.Comma)
				{
					var token = new Token();
					token.Type = TokenType.Comma;
					token.Line = index;
					tokens.Add(token);
					index++;
				}
				else if (type == ElementType.Equal)
				{
					var token = new Token();
					token.Type = TokenType.Equal;
					token.Line = index;
					tokens.Add(token);
					index++;
				}
				else if (type == ElementType.Dot)
				{
					var token = new Token();
					token.Type = TokenType.Dot;
					token.Line = index;
					tokens.Add(token);
					index++;
				}
				else
				{
					if(type == ElementType.Space)
					{
						index++;
					}
					else
					{
						throw new InvalidTokenException(c.ToString(), index);
					}
				}
			}

			LexerResult result = new LexerResult();
			result.Tokens = tokens;
			return result;
		}

		bool StartLabel(string code, int index)
		{
			var c = code[index];
			var type = GetElemenetType(c);
			return type == ElementType.Alphabet || type == ElementType.SpecialLetter;
		}

		bool StartOperator(string code, int index)
		{
			var c = code[index];
			var type = GetElemenetType(c);
			return type == ElementType.Operator;
		}

		bool StartDigit(string code, int index)
		{
			var c = code[index];
			var type = GetElemenetType(c);

			if (type == ElementType.Digit) return true;

			if (type == ElementType.Dot && index < code.Count() - 1 && GetElemenetType(code[index + 1]) == ElementType.Digit) return true;

			return false;
		}

		Token ParseLabel(string code, ref int index)
		{
			var token = new Token();
			token.Type = TokenType.Label;
			token.Line = index;

			string str = "";
			
			while (index < code.Length)
			{
				var c = code[index];
				var type = GetElemenetType(c);

				// if (type != ElementType.Alphabet && type != ElementType.Digit && type != ElementType.SpecialLetter && type != ElementType.Dot) break;
				if (type != ElementType.Alphabet && type != ElementType.Digit && type != ElementType.SpecialLetter) break;

				str += c;
				index++;
			}

			token.Value = str;
			return token;
		}

		Token ParseOperator(string code, ref int index)
		{
			var token = new Token();
			token.Type = TokenType.Operator;
			token.Line = index;

			string str = "";

			while (index < code.Length)
			{
				var c = code[index];
				var type = GetElemenetType(c);

				if (type != ElementType.Operator) break;
				str += c;
				index++;
				break;
			}

			token.Value = str;
			return token;
		}

		Token ParseDigit(string code, ref int index)
		{
			var token = new Token();
			token.Type = TokenType.Digit;
			token.Line = index;

			string str = "";

			bool hasDot = false;

			while (index < code.Length)
			{
				var c = code[index];
				var type = GetElemenetType(c);

				if (type == ElementType.Space) break;
				if (type == ElementType.Other) break;
				if (type == ElementType.Operator) break;
				if (type == ElementType.SpecialLetter) break;
				if (type == ElementType.LeftParentheses) break;
				if (type == ElementType.RightParentheses) break;
				if (type == ElementType.Comma) break;
				if (type == ElementType.Equal) break;
				if (type == ElementType.NewLine) break;
				if (type == ElementType.Connect) break;

				if (type == ElementType.Dot)
				{
					if(hasDot)
					{
						throw new InvalidTokenException(".", index);
					}
					hasDot = true;
				}

				str += c;
				index++;
			}
			
			try
			{
				token.Value = float.Parse(str);
			}
			catch
			{
				throw new InvalidTokenException(".", index);
			}

			return token;
		}

		ElementType GetElemenetType(Char c)
		{
			if (c == '\n')
				return ElementType.NewLine;
			if (c == '\\') return ElementType.Connect;
			if (Char.IsLetter(c)) return ElementType.Alphabet;
			if (Char.IsDigit(c)) return ElementType.Digit;
			if (Char.IsWhiteSpace(c)) return ElementType.Space;
			if (operators.Contains(c)) return ElementType.Operator;
			if (c == '(') return ElementType.LeftParentheses;
			if (c == ')') return ElementType.RightParentheses;
			if (c == '@') return ElementType.SpecialLetter;
			if (c == '.') return ElementType.Dot;
			if (c == ',') return ElementType.Comma;
			if (c == '=') return ElementType.Equal;

			return ElementType.Other;
		}

		HashSet<Char> operators = new HashSet<char>();
		enum ElementType
		{
			Alphabet,
			SpecialLetter,
			Digit,
			Space,
			Operator,
			Other,
			LeftParentheses,
			RightParentheses,
			Dot,
			Comma,
			Equal,
			NewLine,
			Connect,
		}
	}
}
