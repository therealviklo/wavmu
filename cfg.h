#pragma once
#include <cctype>
#include <string>
#include "utils.h"
#include "envelope.h"

class CfgParser
{
private:
	const char* cur;
	const char* end;
public:
	CfgParser(const char* cur, const char* end);

	bool atEnd() noexcept { return cur == end; }
	void skipWhitespace();
	char get();
	std::string getToken();
	bool tryParse(const char* str);
};

void readCfg(const char* name, Envelope& envelope);