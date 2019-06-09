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
	}
	class Token
	{
		public TokenType Type;
		public object Value;
		public int Line = 0;
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

			int index = 0;
		
			while (index < code.Length)
			{
				var c = code[index];
				var type = GetElemenetType(c);

				if (StartLabel(code, index))
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
				else
				{
					if(type == ElementType.Space)
					{
						index++;
					}
					else
					{
						LexerResult erroresult = new LexerResult();
						erroresult.Error = ErrorType.InvalidCharacter;
						erroresult.ErrorInfo.Add("Line", index);
						erroresult.ErrorInfo.Add("Character", c.ToString());
						return erroresult;
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
			return type == ElementType.Digit || type == ElementType.Dot;
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

				if (type != ElementType.Alphabet && type != ElementType.Digit && type != ElementType.SpecialLetter && type != ElementType.Dot) break;

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

				if(type == ElementType.Dot)
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
			
			token.Value = float.Parse(str);
			return token;
		}

		ElementType GetElemenetType(Char c)
		{
			if (Char.IsLetter(c)) return ElementType.Alphabet;
			if (Char.IsDigit(c)) return ElementType.Digit;
			if (Char.IsWhiteSpace(c)) return ElementType.Space;
			if (operators.Contains(c)) return ElementType.Operator;
			if (c == '(') return ElementType.LeftParentheses;
			if (c == ')') return ElementType.RightParentheses;
			if (c == '@') return ElementType.SpecialLetter;
			if (c == '.') return ElementType.Dot;
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
		}
	}
}
