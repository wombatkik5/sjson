#ifndef __PARSEJSON_CPP__
#define __PARSEJSON_CPP__

#include "ParseJson.h"

/*
 * Based on the implementation of JSON parsing in RapidJSON, traverse the whole JSON document and construct the
 * representation.
 */

void Sjson::parseJson(std::ifstream &fs, std::unordered_map<std::string, uint64_t> &nt)
{
	tree.set(tree.getCounter());

	skipWhitespaceAndComments(fs);

	parseHasName = false;
	parseNameId = 0;

	if(fs.peek() == '\0')
	{
		fs.close();
		std::cout << "parseJson finished." << std::endl;
		tree.shrink();
		return;
	}
	else
	{
		parseValue(fs, nt);
	}

	std::cout << "parseJson finished." << std::endl;
}

/*
 * Traversing the document.
 */

void Sjson::parseNull(std::ifstream &fs) {
	assert(fs.peek() == 'n');
	fs.get();

	if (fs.get() == 'u' && fs.get() == 'l' && fs.get() == 'l') {
		if(parseHasName) {
			values.append<NamedJval>(NamedJval(Jval::TYPE_NULL, parseNameId));
		} else {
			values.append<Jval>(Jval(Jval::TYPE_NULL));
		}
		parseHasName = false;
		tree.incCounter(1);
	}
	else {
		std::cout << "Wrong at null." << std::endl;
		return;
	}
}

void Sjson::parseTrue(std::ifstream &fs) {
	assert(fs.peek() == 't');
	fs.get();

	if (fs.get() == 'r' && fs.get() == 'u' && fs.get() == 'e') {
		if(parseHasName) {
			values.append<NamedJval>(NamedJval(Jval::TYPE_TRUE, parseNameId));
		} else {
			values.append<Jval>(Jval(Jval::TYPE_TRUE));
		}
		parseHasName = false;
		tree.incCounter(1);
	}
	else
	{
		std::cout << "Wrong at true." << std::endl;
		return;
	}
}

void Sjson::parseFalse(std::ifstream &fs) {
	assert(fs.peek() == 'f');
	fs.get();

	if (fs.get() == 'a' && fs.get() == 'l' && fs.get() == 's' && fs.get() == 'e') {
		if(parseHasName) {
			values.append<NamedJval>(NamedJval(Jval::TYPE_FALSE, parseNameId));
		} else {
			values.append<Jval>(Jval(Jval::TYPE_FALSE));
		}
		parseHasName = false;
		tree.incCounter(1);
	}
	else {
		std::cout << "Wrong at false." << std::endl;
		return;
	}
}

unsigned Sjson::parseHex4(std::ifstream &fs) {
	unsigned codepoint = 0;
	for (int i = 0; i < 4; i++) {
		char c = fs.get();
		codepoint <<= 4;
		codepoint += static_cast<unsigned>(c);
		if (c >= '0' && c <= '9')
			codepoint -= '0';
		else if (c >= 'A' && c <= 'F')
			codepoint -= 'A' - 10;
		else if (c >= 'a' && c <= 'f')
			codepoint -= 'a' - 10;
		else
			return 0;
	}
	return codepoint;
}

void Sjson::parseStringToStream(std::ifstream &fs, std::deque<char> &ss) {
#define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	static const char escape[256] = {
			Z16, Z16, 0, 0,'\"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,'/',
			Z16, Z16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,'\\', 0, 0, 0,
			0, 0,'\b', 0, 0, 0,'\f', 0, 0, 0, 0, 0, 0, 0,'\n', 0,
			0, 0,'\r', 0,'\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16
	};
#undef Z16

	assert(fs.peek() == '\"');//RAPIDJSON_ASSERT(is.Peek() == '\"');
	fs.get();  // Skip '\"'

	for (;;) {
		char c = fs.peek();
		if (c == '\\') {	// Escape
			fs.get();
			char e = fs.get();
			if ((sizeof(c) == 1 || unsigned(e) < 256) && escape[(unsigned char)e])
				ss.push_back(escape[(unsigned char)e]);
			else if (e == 'u') // Unicode, skip for now.
			{
				continue;
			}
			else
				return;
		}
		else if (c == '"') {	// Closing double quote
			fs.get();
			ss.push_back('\0');
			return;
		}
		else if (c == '\0')
			return;
		else if ((unsigned)c < 0x20)
			return;
		else
			ss.push_back(fs.get());
	}
}

void Sjson::parseString(std::ifstream &fs, std::unordered_map<std::string, uint64_t> &nt, bool isKey = false)
{
	std::deque<char> stackStream;
	parseStringToStream(fs, stackStream);

	char *str;

	uint64_t length = static_cast<uint64_t>(stackStream.size());
	if(length > 0)
	{
		str = new char[length];
		for(uint64_t i = 0; i < length - 1; i++)
		{
			str[i] = stackStream.front();
			stackStream.pop_front();
		}
		str[length - 1] = '\0';
	}
	else
	{
		str = new char[1];
		str[0] = '\0';
	}

	if(isKey)
	{ //Key.
		assert(!parseHasName);
		parseNameId = resolveNameId(str, nt);
		parseHasName = true;
	}
	else
	{ //Value.
		uint64_t strLen;
		if(isStringCompressed)
		{
			strLen = stringValues.size();
			str[length - 1] = '$';
			stringValues.append(str);
		}
		else
		{
			strLen = stringOValues.size();
			stringOValues.push_back(string_to_cstr(str));
		}
		if(parseHasName)
		{
			values.append<NamedStringJval>(NamedStringJval(parseNameId, strLen/*Id*/));
		}
		else
		{
			values.append<StringJval>(StringJval(strLen/*Id*/));
		}
		parseHasName = false;
		tree.incCounter(1);
	}

	delete[] str;
}

void Sjson::parseObject(std::ifstream &fs, std::unordered_map<std::string, uint64_t> &nt) {
	assert(fs.peek() == '{');
	fs.get();  // Skip '{'

	if(parseHasName)
		values.append<NamedJval>(NamedJval(Jval::TYPE_OBJECT, parseNameId));
	else
		values.append<Jval>(Jval(Jval::TYPE_OBJECT));
	parseHasName = false;

	uint64_t headerCounter = tree.getCounter();
	skipWhitespaceAndComments(fs);

	if (fs.peek() == '}') {
		fs.get();
		tree.incCounter(1);
		return;
	}

	for (uint64_t memberCount = 0;;) {
		/* Name part. */
		if (fs.peek() != '"') {
			std::cout << "Wrong at name." << std::endl;
			return;
		}

		parseString(fs, nt, true);

		skipWhitespaceAndComments(fs);

		if (fs.get() != ':') {
			std::cout << "Wrong at whitespace." << std::endl;
			return;
		}

		/* Value part. */
		skipWhitespaceAndComments(fs);

		parseValue(fs, nt);

		skipWhitespaceAndComments(fs);

		++memberCount;

		switch (fs.get()) {
			case ',':
				skipWhitespaceAndComments(fs);
				break;
			case '}':
				//std::cout << "O" << headerCounter << '\t' << memberCount << std::endl;
				tree.setPush(headerCounter, memberCount);
				/*
				for (uint64_t j = 0; j < memberCount; j++) {
					tree.setPush(headerCounter);
					headerCounter++;
				}
				*/
				headerCounter += memberCount;
				tree.unsetPush(headerCounter, 1);
				return;
			default:
				break;
		}
	}
}

void Sjson::parseArray(std::ifstream &fs, std::unordered_map<std::string, uint64_t> &nt) {
	assert(fs.peek() == '[');
	fs.get();  // Skip '['

	if(parseHasName)
		values.append<NamedJval>(NamedJval(Jval::TYPE_ARRAY, parseNameId));
	else
		values.append<Jval>(Jval(Jval::TYPE_ARRAY));
	parseHasName = false;

	uint64_t headerCounter = tree.getCounter();

	skipWhitespaceAndComments(fs);

	if (fs.peek() == ']') {
		fs.get();
		tree.incCounter(1);
		return;
	}

	for (uint64_t elementCount = 0;;) {
		parseValue(fs, nt);

		++elementCount;
		skipWhitespaceAndComments(fs);

		switch (fs.get()) {
			case ',':
				skipWhitespaceAndComments(fs);
				break;
			case ']':
				//std::cout << "Array closed." << std::endl;
				//std::cout << "A" << headerCounter << '\t' << elementCount << std::endl;
				tree.setPush(headerCounter, elementCount);
				/*
				for (uint64_t j = 0; j < elementCount; j++) {
					std::cout << elementCount << std::endl;
					tree.setPush(headerCounter);
					headerCounter++;
				}
				*/
				headerCounter += elementCount;
				tree.unsetPush(headerCounter, 1);
				return;
			default:
				break;
		}
	}
}

void Sjson::parseNumber(std::ifstream &fs)
{
	// Parse minus
	bool minus = false;
	if(fs.peek() == '-')
	{
		minus = true;
		fs.get();
	}

	// Parse int: zero / ( digit1-9 *DIGIT )
	char c;
	int64_t i = 0;
	uint64_t significandDigit = 0;
	uint64_t length = 0;
	bool useDouble = false;
	double d = 0.0;
	if(fs.peek() == '0')
	{
		i = 0;
		c = fs.get();
		length++;
	}
	else if(fs.peek() >= '1' && fs.peek() <= '9')
	{
		c = fs.get();
		length++;
		i = static_cast<unsigned>(c - '0');

		if(minus)
		{
			while(fs.peek() >= '0' && fs.peek() <= '9')
			{
				if(i >= ((static_cast<uint64_t>(0x0CCCCCCC) << 32) | static_cast<uint64_t>(0xCCCCCCCC))) // 2^63 = 9223372036854775808
				{
					if(i != ((static_cast<uint64_t>(0x0CCCCCCC) << 32) | static_cast<uint64_t>(0xCCCCCCCC)) || fs.peek() > '8')
					{
						d = i;
						useDouble = true;
						break;
					}
				}
				c = fs.get();
				length++;
				i = i * 10 + static_cast<unsigned>(c - '0');
				significandDigit++;
			}
		}
		else
		{
			while(fs.peek() >= '0' && fs.peek() <= '9')
			{
				if(i >= ((static_cast<uint64_t>(0x19999999) << 32) | static_cast<uint64_t>(0x99999999))) // 2^64 - 1 = 18446744073709551615
				{
					if(i != ((static_cast<uint64_t>(0x19999999) << 32) | static_cast<uint64_t>(0x99999999)) || fs.peek() > '5')
					{
						d = i;
						useDouble = true;
						break;
					}
				}
				c = fs.get();
				length++;
				i = i * 10 + static_cast<unsigned>(c - '0');
				significandDigit++;
			}
		}
	}
	else
	{
		std::cout << "Wrong at number " << fs.peek() << " " << tree.getCounter() << "." << std::endl;
		std::string buffffff;
		std::getline(fs, buffffff);
		std::cout << buffffff << std::endl;
		return;
	}

	// Force double for big integer
	if(useDouble)
	{
		while(fs.peek() >= '0' && fs.peek() <= '9')
		{
			if(d >= 1.7976931348623157e307) // DBL_MAX / 10.0
			{
				return;
			}
			c = fs.get();
			length++;
			d = d * 10 + (c - '0');
		}
	}

	// Parse frac = decimal-point 1*DIGIT
	int64_t expFrac = 0;
	uint64_t decimalPosition;
	if(fs.peek() == '.')
	{
		fs.get();
		decimalPosition = length;

		if(!(fs.peek() >= '0' && fs.peek() <= '9'))
		{
			std::cout << "Wrong at double." << std::endl;
			return;
		}

		if(!useDouble)
		{
			while(fs.peek() >= '0' && fs.peek() <= '9')
			{
				if(i > ((static_cast<uint64_t>(0x1FFFFF) << 32) | static_cast<uint64_t>(0xFFFFFFFF))) // 2^53 - 1 for fast path
				{
					break;
				}
				else
				{
					i = i * 10 + static_cast<unsigned>(fs.get() - '0');
					--expFrac;
					if(i != 0)
					{
						significandDigit++;
					}
				}
			}
			d = (double)i;
			useDouble = true;
		}

		while(fs.peek() >= '0' && fs.peek() <= '9')
		{
			if(significandDigit < 17)
			{
				c = fs.get();
				length++;
				d = d * 10.0 + (c - '0');
				--expFrac;
				if(d > 0.0)
				{
					significandDigit++;
				}
			}
			else
			{
				c = fs.get();
				length++;
			}
		}
	}
	else
	{
		decimalPosition = length;
	}

	// Parse exp = e [ minus / plus ] 1*DIGIT
	int64_t exp = 0;
	if(fs.peek() == 'e' || fs.peek() == 'E')
	{
		if(!useDouble)
		{
			d = i;
			useDouble = true;
		}
		fs.get();

		bool expMinus = false;
		if(fs.peek() == '+')
		{
			fs.get();
		}
		else if(fs.peek() == '-')
		{
			fs.get();
			expMinus = true;
		}

		if(fs.peek() >= '0' && fs.peek() <= '9')
		{
			exp = fs.get() - '0';
			if(expMinus)
			{
				while(fs.peek() >= '0' && fs.peek() <= '9')
				{
					exp = exp * 10 + (fs.get() - '0');
					if(exp >= 214748364)
					{							// Issue #313: prevent overflow exponent
						while(fs.peek() >= '0' && fs.peek() <= '9')  // Consume the rest of exponent
						{
							fs.get();
						}
					}
				}
			}
			else	// positive exp
			{
				uint64_t maxExp = 308 - expFrac;
				while(fs.peek() >= '0' && fs.peek() <= '9')
				{
					exp = exp * 10 + (fs.get() - '0');
					if(exp > maxExp)
					{
						return;
					}
				}
			}
		}
		else
		{
			return;
		}

		if(expMinus)
		{
			exp = -exp;
		}
	}

	// Finish parsing, call event according to the type of number.
	bool cont = true;

	if(useDouble)
	{
		int64_t p = exp + expFrac;
		if(p < -308)
		{
			d = d / std::pow(10, 308);
			d = d / std::pow(10, -308 - p);
		}
		else if(p >= 0)
		{
			d = d * std::pow(10, p);
		}
		else
		{
			d = d / std::pow(10, p);
		}
		if(minus)
		{
			if(parseHasName)
			{
				if(significandDigit > 7 || std::abs(p) > 37)
				{
					values.append<NamedDoubleJval>(NamedDoubleJval(parseNameId, -d));
				}
				else if(significandDigit > 3 || std::abs(p) > 4)
				{
					values.append<NamedFloatJval>(NamedFloatJval(parseNameId, static_cast<float>(-d)));
				}
				else
				{
					values.append<NamedHalfJval>(NamedHalfJval(parseNameId, static_cast<half_float::half>(-d)));
				}
			}
			else
			{
				if(significandDigit > 7 || std::abs(p) > 37)
				{
					values.append<DoubleJval>(DoubleJval(-d));
				}
				else if(significandDigit > 3 || std::abs(p) > 4)
				{
					values.append<FloatJval>(FloatJval(static_cast<float>(-d)));
				}
				else
				{
					values.append<HalfJval>(HalfJval(static_cast<half_float::half>(-d)));
				}
			}
			parseHasName = false;
		}
		else
		{
			if(parseHasName)
			{
				if(significandDigit > 7 || std::abs(p) > 37)
				{
					values.append<NamedDoubleJval>(NamedDoubleJval(parseNameId, d));
				}
				else if(significandDigit > 3 || std::abs(p) > 4)
				{
					values.append<NamedFloatJval>(NamedFloatJval(parseNameId, static_cast<float>(d)));
				}
				else
				{
					values.append<NamedHalfJval>(NamedHalfJval(parseNameId, static_cast<half_float::half>(d)));
				}
			}
			else
			{
				if(significandDigit > 7 || std::abs(p) > 37)
				{
					values.append<DoubleJval>(DoubleJval(d));
				}
				else if(significandDigit > 3 || std::abs(p) > 4)
				{
					values.append<FloatJval>(FloatJval(static_cast<float>(d)));
				}
				else
				{
					values.append<HalfJval>(HalfJval(static_cast<half_float::half>(d)));
				}
			}
			parseHasName = false;
		}
		tree.incCounter(1);
	}
	else { /* This part not implemented yet!!! */
		if (minus) {
			if(parseHasName) {
				if(static_cast<int64_t>(~i + 1) <= -1 * std::pow(2, 31))
				{
					values.append<NamedLongJval>(NamedLongJval(parseNameId, static_cast<int64_t>(~i + 1)));
				}
				else if(static_cast<int64_t>(~i + 1) <= -1 * std::pow(2, 15))
				{
					values.append<NamedIntJval>(NamedIntJval(parseNameId, static_cast<int32_t>(~i + 1)));
				}
				else
				{
					values.append<NamedShortJval>(NamedShortJval(parseNameId, static_cast<int16_t>(~i + 1)));
				}
			} else {
				if(static_cast<int64_t>(~i + 1) <= -1 * std::pow(2, 31))
				{
					values.append<LongJval>(LongJval(static_cast<int64_t>(~i + 1)));
				}
				else if(static_cast<int64_t>(~i + 1) <= -1 * std::pow(2, 15))
				{
					values.append<IntJval>(IntJval(static_cast<int32_t>(~i + 1)));
				}
				else
				{
					values.append<ShortJval>(ShortJval(static_cast<int16_t>(~i + 1)));
				}
			}
			parseHasName = false;

			tree.incCounter(1);
		}
		else {
			if(parseHasName) {
				if(i >= std::pow(2, 31) - 1)
				{
					values.append<NamedLongJval>(NamedLongJval(parseNameId, i));
				}
				else if(i >= std::pow(2, 15) - 1)
				{
					values.append<NamedIntJval>(NamedIntJval(parseNameId, i));
				}
				else
				{
					values.append<NamedShortJval>(NamedShortJval(parseNameId, i));
				}
			} else {
				if(i >= std::pow(2, 31) - 1)
				{
					values.append<LongJval>(LongJval(i));
				}
				else if(i >= std::pow(2, 15) - 1)
				{
					values.append<IntJval>(IntJval(i));
				}
				else
				{
					values.append<ShortJval>(ShortJval(i));
				}
			}
			parseHasName = false;

			tree.incCounter(1);
		}
	}
	if(!cont)
	{
		std::cout << "Wrong at cont." << std::endl;
		return;
	}
		//return;
}

void Sjson::parseValue(std::ifstream &fs, std::unordered_map<std::string, uint64_t> &nt) {
	//std::cout << fs.tellg() << std::endl;
	switch (fs.peek()) {
		case 'n':
			//std::cout << "Null" << std::endl;
			parseNull(fs);
			break;
		case 't':
			//std::cout << "True" << std::endl;
			parseTrue(fs);
			break;
		case 'f':
			//std::cout << "False" << std::endl;
			parseFalse(fs);
			break;
		case '"':
			//std::cout << "String" << std::endl;
			parseString(fs, nt);
			break;
		case '{':
			//std::cout << "Object" << std::endl;
			parseObject(fs, nt);
			break;
		case '[':
			//std::cout << "Array" << std::endl;
			parseArray(fs, nt);
			break;
		default :
			//std::cout << (char)fs.peek() << std::endl;
			//std::cout << "Number" << std::endl;
			parseNumber(fs);
			break;

	}

}

/*
 * Exclude whitespaces and comments while processing the document.
 */

void Sjson::skipWhitespace(std::ifstream &fs) {
	while (fs.peek() == ' ' || fs.peek() == '\n' || fs.peek() == '\r' || fs.peek() == '\t')
		fs.get();
}

void Sjson::skipWhitespaceAndComments(std::ifstream &fs) {
	skipWhitespace(fs);

	while (fs.peek() == '/') {
		fs.get();

		if (fs.peek() == '*') {
			fs.get();
			while (true) {
				if (fs.peek() == '\0')
					return;

				if (fs.get() == '*') {
					if (fs.peek() == '\0')
						return;

					if (fs.get() == '/')
						break;
				}
			}
		} else if (fs.peek() == '/') {
			fs.get();
			while (fs.peek() != '\0' && fs.get() != '\n') { }
		} else {
			return;
		}

		skipWhitespace(fs);
	}
}

#endif
