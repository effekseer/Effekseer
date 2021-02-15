using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO
{
	class Lexer
	{
		static System.Globalization.NumberFormatInfo NFI;

		/// <summary>
		/// 使用禁止文字
		/// </summary>
		static SortedSet<char> invalidChar = new SortedSet<char>();

		/// <summary>
		/// 1文字
		/// </summary>
		static Dictionary<char, TokenKind> ope1 = new Dictionary<char, TokenKind>();

		/// <summary>
		/// 2文字
		/// </summary>
		static Dictionary<string, TokenKind> ope2 = new Dictionary<string, TokenKind>();

		/// <summary>
		/// 複数文字
		/// </summary>
		static Dictionary<string, TokenKind> opemulti = new Dictionary<string, TokenKind>();

		static Lexer()
		{
			var culture = new System.Globalization.CultureInfo("ja-JP");
			NFI = culture.NumberFormat;

			ope1.Add('(', TokenKind.Lparen);
			ope1.Add(')', TokenKind.Rparen);
			ope1.Add('{', TokenKind.LCurly);
			ope1.Add('}', TokenKind.RCurly);
			ope1.Add('+', TokenKind.Plus);
			ope1.Add('-', TokenKind.Minus);
			ope1.Add('*', TokenKind.Multi);
			ope1.Add('/', TokenKind.Divi);
			ope1.Add('<', TokenKind.Less);
			ope1.Add('>', TokenKind.Great);
			ope1.Add('=', TokenKind.Assign);
			ope1.Add('.', TokenKind.Dot);
			ope1.Add(',', TokenKind.Comma);
			ope1.Add(':', TokenKind.Colon);
			ope1.Add(';', TokenKind.Semicolon);

			ope2.Add("==", TokenKind.Equal);
			ope2.Add("!=", TokenKind.NotEq);
			ope2.Add("<=", TokenKind.LessEq);
			ope2.Add(">=", TokenKind.GreatEq);
		}

		/// <summary>
		/// 字句解析
		/// </summary>
		/// <param name="s">コード</param>
		/// <param name="err">エラー</param>
		/// <returns>トークン</returns>
		public static Token[] Parse(string s, ref string err)
		{
			List<Token> tokens = new List<Token>();
			int pos = 0;
			int ln = 0;
			int col = 0;

			Token tkn = GetNextToken(ref s, ref pos, ref ln, ref col, ref err);

			while (tkn != null)
			{
				tokens.Add(tkn);
				tkn = GetNextToken(ref s, ref pos, ref ln, ref col, ref err);
			}

			return tokens.ToArray();
		}

		static Token GetNextToken(ref string s, ref int pos, ref int ln, ref int col, ref string err)
		{
			Token tkn = null;
			char ch1 = ' ';
			char ch2 = ' ';

			StringBuilder sb = new StringBuilder();

			for (; ; )
			{
				// 文字列外
				if (s.Length <= pos) return null;

				ch1 = s[pos];

				if (IsIgnoredChar(ch1))
				{
					NextChar(ref s, ref pos, ref ln, ref col);
				}
				else
				{
					break;
				}
			}

			// 禁則文字
			if (IsInvalidChar(ch1))
			{
				err = "Invalid char : " + ch1 + " line =" + ln.ToString() + " col = " + col.ToString();
				return null;
			}

			if (IsLetterChar(ch1))
			{
				// 識別文字列
				var _ln = ln;
				var _col = col;
				while ((IsLetterChar(ch1) || IsDigitChar(ch1)) && pos < s.Length)
				{
					sb.Append(ch1);
					NextChar(ref s, ref pos, ref ln, ref col);
					if (pos < s.Length)
					{
						ch1 = s[pos];
					}
				}
				tkn = new Token(sb.ToString(), _ln, _col, false);
			}
			else if (IsStartEndString(ch1))
			{
				// 文字列
				var _ln = ln;
				var _col = col;

				NextChar(ref s, ref pos, ref ln, ref col);
				ch1 = s[pos];

				while (!IsStartEndString(ch1) && pos < s.Length)
				{
					if (ch1 == '\\')
					{
						NextChar(ref s, ref pos, ref ln, ref col);
						if (pos < s.Length)
						{
							ch1 = s[pos];
						}
					}

					sb.Append(ch1);

					NextChar(ref s, ref pos, ref ln, ref col);
					if (pos < s.Length)
					{
						ch1 = s[pos];
					}
				}

				NextChar(ref s, ref pos, ref ln, ref col);

				tkn = new Token(sb.ToString(), _ln, _col, true);
			}
			else if (IsNumber(ch1))
			{
				// 数値
				var _ln = ln;
				var _col = col;
				while ((IsDigitChar(ch1) || ch1 == '.') && pos < s.Length)
				{
					sb.Append(ch1);
					NextChar(ref s, ref pos, ref ln, ref col);
					ch1 = s[pos];
				}
				var f = 0.0;
				if (double.TryParse(sb.ToString(), System.Globalization.NumberStyles.Float, NFI, out f))
				{
					return new Token(f, _ln, _col);
				}
				else
				{
					err = "Invalid number : " + sb.ToString() + " line =" + ln.ToString() + " col = " + col.ToString();
					return null;
				}
			}
			else
			{
				// その他
				if (pos + 1 < s.Length)
				{
					// 2文字演算子
					ch2 = s[pos + 1];
					string ch = ch1.ToString() + ch2.ToString();

					if (ope2.ContainsKey(ch))
					{
						tkn = new Token(ope2[ch], ln, col);
						NextChar(ref s, ref pos, ref ln, ref col);
						NextChar(ref s, ref pos, ref ln, ref col);
						return tkn;
					}
				}

				// 1文字演算子
				if (ope1.ContainsKey(ch1))
				{
					tkn = new Token(ope1[ch1], ln, col);
					NextChar(ref s, ref pos, ref ln, ref col);
					return tkn;
				}
			}


			if (tkn != null && tkn.Kind == TokenKind.Letter)
			{
				// 実は識別文字列は登録済み?
				if (opemulti.ContainsKey(tkn.Letter))
				{
					tkn = new Token(opemulti[tkn.Letter], tkn.Line, tkn.Col);
				}
				return tkn;
			}
			else if (tkn != null)
			{
				return tkn;
			}

			// 知らない文字
			err = "Unknown char : " + ch1 + " line =" + ln.ToString() + " col = " + col.ToString();
			return null;
		}

		/// <summary>
		/// 次の文字に移動
		/// </summary>
		static void NextChar(ref string s, ref int pos, ref int ln, ref int col)
		{
			if (s[pos] == '\n')
			{
				ln++;
				col = 0;
			}
			else
			{
				col++;
			}
			pos++;
		}

		/// <summary>
		/// 禁止文字か?
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsInvalidChar(char c)
		{
			if (invalidChar.Contains(c)) return true;
			return false;
		}

		/// <summary>
		/// 無視できる文字か?
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsIgnoredChar(char c)
		{
			if (c == '\r') return true;
			if (c == '\n') return true;
			if (c == '\t') return true;
			if (c == ' ') return true;
			if (c == '　') return true;
			return false;
		}

		/// <summary>
		/// 数値として扱える文字か?
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsDigitChar(char c)
		{
			if (IsNumber(c)) return true;
			return false;
		}

		/// <summary>
		/// 識別子として扱える文字か?
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsLetterChar(char c)
		{
			if (IsZenkaku(c)) return true;
			if (IsAlphabet(c)) return true;
			if (c == '_') return true;
			return false;
		}

		/// <summary>
		/// 文字が全角かどうか?
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsZenkaku(char c)
		{
			int num = Encoding.UTF8.GetByteCount(new char[] { c });
			return num != 1;
		}

		/// <summary>
		/// 文字がアルファベットかどうか？
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsAlphabet(char c)
		{
			return char.IsLetter(c);
		}

		/// <summary>
		/// 文字が数字かどうか？
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsNumber(char c)
		{
			return char.IsDigit(c);
		}

		/// <summary>
		/// 文字列の開始終了か
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsStartEndString(char c)
		{
			return c == '\"';
		}
	}

	/// <summary>
	/// トークンの種類
	/// </summary>
	enum TokenKind
	{
		Lparen,
		Rparen,
		LCurly,
		RCurly,
		Plus,
		Minus,
		Multi,
		Divi,
		Equal,
		NotEq,
		Less,
		LessEq,
		Great,
		GreatEq,
		Assign,
		Dot,
		Comma,
		Colon,
		Semicolon,

		Letter,
		Digit,
		String,


		Unknown,
	}

	/// <summary>
	/// トークン
	/// </summary>
	class Token
	{
		public int Line
		{
			get;
			private set;
		}

		public int Col
		{
			get;
			private set;
		}

		public TokenKind Kind
		{
			get;
			private set;
		}

		public string Letter
		{
			get;
			private set;
		}

		public string String
		{
			get;
			private set;
		}

		public double Digit
		{
			get;
			private set;
		}

		public Token(TokenKind kind, int line, int col)
		{
			Kind = kind;
			Line = line;
			Col = col;
		}

		public Token(string s, int line, int col, bool isString)
		{
			if (isString)
			{
				Kind = TokenKind.String;
				String = s;
				Line = line;
				Col = col;
			}
			else
			{
				Kind = TokenKind.Letter;
				Letter = s;
				Line = line;
				Col = col;
			}
		}

		public Token(double digit, int line, int col)
		{
			Kind = TokenKind.Digit;
			Digit = digit;
			Line = line;
			Col = col;
		}

		public override string ToString()
		{
			if (Kind == TokenKind.Letter)
			{
				return Kind.ToString() + " : letter = " + Letter.ToString() + " : line = " + Line.ToString() + " : col = " + Col.ToString();
			}
			else if (Kind == TokenKind.String)
			{
				return Kind.ToString() + " : string = " + String.ToString() + " : line = " + Line.ToString() + " : col = " + Col.ToString();
			}
			else if (Kind == TokenKind.Digit)
			{
				return Kind.ToString() + " : digit = " + Digit.ToString() + " : line = " + Line.ToString() + " : col = " + Col.ToString();
			}

			return Kind.ToString() + " : line = " + Line.ToString() + " : col = " + Col.ToString();
		}
	}
}
