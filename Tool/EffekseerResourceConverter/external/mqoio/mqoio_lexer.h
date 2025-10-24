#pragma once

#include <ctype.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <string_view>
#include <optional>
#include <charconv>
#include <sstream>

namespace mqoio
{

/// <summary>
/// �g�[�N���̎��
/// </summary>
enum class TokenKind
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
	Binary,

	Unknown,
};

/// <summary>
/// �g�[�N��
/// </summary>
struct Token
{
	int Line;
	int Col;
	TokenKind Kind;
	std::string String;
	double Digit;
	std::string Letter;
	std::vector<uint8_t> Binary;
		
	Token(TokenKind kind, int line, int col)
	{
		Kind = kind;
		Line = line;
		Col = col;
	}

	Token(const std::string& s, int line, int col, bool isString)
	{
		if (isString)
		{
			Kind = TokenKind::String;
			String = s;
			Line = line;
			Col = col;
		}
		else
		{
			Kind = TokenKind::Letter;
			Letter = s;
			Line = line;
			Col = col;
		}
	}

	Token(double digit, int line, int col)
	{
		Kind = TokenKind::Digit;
		Digit = digit;
		Line = line;
		Col = col;
	}

	Token(const uint8_t* binary, size_t size, int line, int col)
	{
		Kind = TokenKind::Binary;
		Binary.assign(binary, binary + size);
		Line = line;
		Col = col;
	}
};

class Lexer
{
	/// <summary>
	/// �g�p�֎~����
	/// </summary>
	std::set<char> invalidChar;

	/// <summary>
	/// 1����
	/// </summary>
	std::map<char, TokenKind> ope1;

	/// <summary>
	/// 2����
	/// </summary>
	std::map<std::string_view, TokenKind> ope2;

	/// <summary>
	/// ��������
	/// </summary>
	std::map<std::string_view, TokenKind> opemulti;

public:
	Lexer()
	{
		ope1.emplace('(', TokenKind::Lparen);
		ope1.emplace(')', TokenKind::Rparen);
		ope1.emplace('{', TokenKind::LCurly);
		ope1.emplace('}', TokenKind::RCurly);
		ope1.emplace('+', TokenKind::Plus);
		ope1.emplace('-', TokenKind::Minus);
		ope1.emplace('*', TokenKind::Multi);
		ope1.emplace('/', TokenKind::Divi);
		ope1.emplace('<', TokenKind::Less);
		ope1.emplace('>', TokenKind::Great);
		ope1.emplace('=', TokenKind::Assign);
		ope1.emplace('.', TokenKind::Dot);
		ope1.emplace(',', TokenKind::Comma);
		ope1.emplace(':', TokenKind::Colon);
		ope1.emplace(';', TokenKind::Semicolon);

		ope2.emplace("==", TokenKind::Equal);
		ope2.emplace("!=", TokenKind::NotEq);
		ope2.emplace("<=", TokenKind::LessEq);
		ope2.emplace(">=", TokenKind::GreatEq);
	}

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="s">�R�[�h</param>
	/// <param name="err">�G���[</param>
	/// <returns>�g�[�N��</returns>
	std::vector<Token> Parse(const std::vector<char>& s, std::string& err)
	{
		std::vector<Token> tokens;
		int pos = 0;
		int ln = 0;
		int col = 0;

		while (true)
		{
			size_t index = tokens.size();
			auto tkn = GetNextToken(s, pos, ln, col, err);
			if (tkn.has_value())
			{
				tokens.emplace_back(tkn.value());
			}
			else
			{
				break;
			}
		}

		return tokens;
	}

private:
	std::optional<Token> GetNextToken(const std::vector<char>& s, int& pos, int& ln, int& col, std::string& err)
	{
		std::optional<Token> tkn;
		char ch1 = ' ';
		char ch2 = ' ';

		std::string sb;

		for (;;)
		{
			// ������O
			if (s.size() <= pos) return std::nullopt;

			ch1 = s[pos];

			if (IsIgnoredChar(ch1))
			{
				NextChar(s, pos, ln, col);
			}
			else
			{
				break;
			}
		}

		// �֑�����
		if (IsInvalidChar(ch1))
		{
			std::stringstream ss;
			ss << "Invalid char : " << ch1 << " line =" << ln << " col = " << col;
			err = ss.str();
			return std::nullopt;
		}

		if (IsLetterChar(ch1))
		{
			// ���ʕ�����
			auto _ln = ln;
			auto _col = col;
			while ((IsLetterChar(ch1) || IsDigitChar(ch1)) && pos < s.size())
			{
				sb.push_back(ch1);
				NextChar(s, pos, ln, col);
				if (pos < s.size())
				{
					ch1 = s[pos];
				}
			}
			tkn = Token(sb, _ln, _col, false);
		}
		else if (IsStartEndString(ch1))
		{
			// ������
			auto _ln = ln;
			auto _col = col;

			NextChar(s, pos, ln, col);
			ch1 = s[pos];

			while (!IsStartEndString(ch1) && pos < s.size())
			{
				if (ch1 == '\\')
				{
					NextChar(s, pos, ln, col);
					if (pos < s.size())
					{
						ch1 = s[pos];
					}
				}

				sb.push_back(ch1);

				NextChar(s, pos, ln, col);
				if (pos < s.size())
				{
					ch1 = s[pos];
				}
			}

			NextChar(s, pos, ln, col);

			tkn = Token(sb, _ln, _col, true);
		}
		else if (IsNumber(ch1))
		{
			// ���l
			int _ln = ln;
			int _col = col;
			while ((IsDigitChar(ch1) || ch1 == '.' || ch1 == 'e' || ch1 == '-') && pos < s.size())
			{
				sb.push_back(ch1);
				NextChar(s, pos, ln, col);
				ch1 = s[pos];
			}
			double f = 0.0;
			if (std::from_chars(&sb[0], &sb[0] + sb.size(), f).ec == std::errc{})
			{
				return Token(f, _ln, _col);
			}
			else
			{
				std::stringstream ss;
				ss << "Invalid number : " << sb << " line =" << ln << " col = " << col;
				err = ss.str();
				return std::nullopt;
			}
		}
		else if (IsBinarySizeStart(ch1))
		{
			// �o�C�i��
			int _ln = ln;
			int _col = col;
			NextChar(s, pos, ln, col);
			ch1 = s[pos];
			while (IsDigitChar(ch1) && pos < s.size())
			{
				sb.push_back(ch1);
				NextChar(s, pos, ln, col);
				ch1 = s[pos];
			}
			pos += 3;
			ln++;

			size_t binSize = 0;
			std::from_chars(sb.data(), sb.data() + sb.size(), binSize);

			const char* binPtr = s.data() + pos;
			pos += (int)binSize;

			pos += 1;
			ln++;

			tkn = Token(reinterpret_cast<const uint8_t*>(binPtr), binSize, _ln, _col);

			return tkn;
		}
		else
		{
			// ���̑�
			if (pos + 1 < s.size())
			{
				// 2�������Z�q
				std::string_view ch(&s[pos], 2);

				if (ope2.find(ch) != ope2.end())
				{
					tkn = Token(ope2[ch], ln, col);
					NextChar(s, pos, ln, col);
					NextChar(s, pos, ln, col);
					return tkn;
				}
			}

			// 1�������Z�q
			if (ope1.find(ch1) != ope1.end())
			{
				tkn = Token(ope1[ch1], ln, col);
				NextChar(s, pos, ln, col);
				return tkn;
			}
		}

		if (tkn.has_value())
		{
			auto& _tkn = tkn.value();
			if (_tkn.Kind == TokenKind::Letter)
			{
				// ���͎��ʕ�����͓o�^�ς�?
				if (auto it = opemulti.find(_tkn.Letter); it != opemulti.end())
				{
					tkn = Token(it->second, _tkn.Line, _tkn.Col);
				}
				return tkn;
			}
			else
			{
				return tkn;
			}
		}

		// �m��Ȃ�����
		{
			std::stringstream ss;
			ss << "Unknown char : " << ch1 << " line =" << ln << " col = " << col;
			err = ss.str();
		}

		return std::nullopt;
	}

	/// <summary>
	/// ���̕����Ɉړ�
	/// </summary>
	void NextChar(const std::vector<char>& s, int& pos, int& ln, int& col)
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
	/// �֎~������?
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsInvalidChar(char c)
	{
		if (invalidChar.find(c) != invalidChar.end()) return true;
		return false;
	}

	/// <summary>
	/// �����ł��镶����?
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsIgnoredChar(char c)
	{
		if (c == '\r') return true;
		if (c == '\n') return true;
		if (c == '\t') return true;
		if (c == ' ') return true;
		if (c == '�@') return true;
		return false;
	}

	/// <summary>
	/// ���l�Ƃ��Ĉ����镶����?
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsDigitChar(char c)
	{
		if (IsNumber(c)) return true;
		return false;
	}

	/// <summary>
	/// ���ʎq�Ƃ��Ĉ����镶����?
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsLetterChar(char c)
	{
		if (IsZenkaku(c)) return true;
		if (IsAlphabet(c)) return true;
		if (c == '_') return true;
		return false;
	}

	/// <summary>
	/// �������S�p���ǂ���?
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsZenkaku(char c)
	{
		return (uint8_t)c >= 0x80;
	}

	/// <summary>
	/// �������A���t�@�x�b�g���ǂ����H
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsAlphabet(char c)
	{
		return isalpha(c);
	}

	/// <summary>
	/// �������������ǂ����H
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsNumber(char c)
	{
		return isdigit(c);
	}

	/// <summary>
	/// ������̊J�n�I����
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	bool IsStartEndString(char c)
	{
		return c == '\"';
	}

	bool IsBinarySizeStart(char c)
	{
		return c == '[';
	}
};

}
