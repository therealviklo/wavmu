#include "cfg.h"

CfgParser::CfgParser(const char* cur, const char* end) :
	cur(cur),
	end(end) {}

void CfgParser::skipWhitespace()
{
	while (!atEnd() && std::isspace(*cur))
		cur++;
}

char CfgParser::get()
{
	if (atEnd())
		throw WRE(L"Unexpected end of file");
	return *cur++;
}

std::string CfgParser::getToken()
{
	if (atEnd())
		throw WRE(L"Unexpected end of file");
	
	auto isLoneCharacter = [](char c) -> bool {
		return c == '[' || c == ']';
	};

	if (isLoneCharacter(*cur))
		return std::string(1, *cur++);
	std::string ret;
	while (!atEnd() && !std::isspace(*cur) && !isLoneCharacter(*cur))
		ret += *cur++;
	return ret;
}

bool CfgParser::tryParse(const char* str)
{
	const char* tmpcur = cur;
	while (*str != '\0')
	{
		if (atEnd())
			return false;
		if (*str++ != *tmpcur++)
			return false;
	}
	cur = tmpcur;
	return true;
}

void readCfg(const char* name, Envelope& envelope)
{
	const std::vector<unsigned char> data = readfile(name);
	CfgParser p((const char*)data.data(), (const char*)data.data() + data.size());
	while (p.skipWhitespace(), !p.atEnd())
	{
		if (!p.tryParse("["))
			throw WRE(L'\"' + stringToWstring(p.getToken()) + L"\" unexpected, '[' expected");
		p.skipWhitespace();
		auto tryParseLabel = [&](const char* label) -> bool {
			if (!p.tryParse(label) && !p.tryParse(stringtolower(label).c_str()))
				return false;
			p.skipWhitespace();
			if (!p.tryParse("]"))
				throw WRE(L'\"' + stringToWstring(p.getToken()) + L"\" unexpected, ']' expected");
			return true;
		};
		if (tryParseLabel("Envelope"))
		{
			auto convStringToDouble = [](const std::string& str) -> double {
				try
				{
					return std::stod(str);
				}
				catch (...)
				{
					throw WRE(L"Failed to convert \"" + stringToWstring(str) + L"\" to double");
				}
			};
			Envelope newEnvelope;
			const std::string a = (p.skipWhitespace(), p.getToken());
			newEnvelope.attack = convStringToDouble(a);
			const std::string d = (p.skipWhitespace(), p.getToken());
			newEnvelope.decay = convStringToDouble(d);
			const std::string s = (p.skipWhitespace(), p.getToken());
			newEnvelope.sustain = convStringToDouble(s);
			const std::string r = (p.skipWhitespace(), p.getToken());
			newEnvelope.release = convStringToDouble(r);
			envelope = newEnvelope;
		}
		else throw WRE(L"Unknown label \"" + stringToWstring(p.getToken()) + L'\"');
	}
}