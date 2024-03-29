/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

// Header string
static const char s_strCppHeader[] =
u8R"(/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

// Note: This file is auto-generated using the CaseFolding.txt data file

#include "JxInternal.h"

namespace Jinx::Impl
{

	struct FoldTable
	{
		static inline const CaseFoldingData caseFoldingTable[] =
		{
)";

// Tail string
static const char s_strCppTail[] =
u8R"(
		};
	};

	inline_t bool FindCaseFoldingData(char32_t sourceCodePoint, char32_t * destCodePoint1, char32_t * destCodePoint2)
	{
		auto retVal = std::lower_bound(std::begin(Impl::FoldTable::caseFoldingTable), std::end(Impl::FoldTable::caseFoldingTable), sourceCodePoint,
			[] (const CaseFoldingData & e, char32_t v)
		{
			return e.sourceCodePoint < v;
		}
		);
		if (retVal == std::end(Impl::FoldTable::caseFoldingTable) || (sourceCodePoint < retVal->sourceCodePoint))
			return false;
		if (destCodePoint1 && destCodePoint2)
		{
			*destCodePoint1 = retVal->destCodePoint1;
			*destCodePoint2 = retVal->destCodePoint2;
		}
		return true;
	}

} // namespace Jinx::Impl

)";


// Storage format
struct FoldingData
{
    string source;
    string type;
    string target1;
    string target2;
};

int main()
{
    // Open and read file into memory, then close
    FILE * file;
    auto err = fopen_s(&file, "..//Utils//CaseFoldGen//CaseFolding.txt", "r");
    if (err != 0)
        return -1;
    fseek(file, 0, SEEK_END);
    auto size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char * buffer = (char *)malloc(size); 
    auto readSize = fread_s(buffer, size, 1, size, file);
    fclose(file);

    // Parse lines into a vector of strings, ignoring comments or empty lines
    char * curr = buffer;
    char * end = buffer + size;
    vector<string> lines;
    lines.reserve(1500);
    string tmpstr;
    tmpstr.reserve(256);
    lines.push_back(tmpstr);
    bool comment = false;
    while (curr < end)
    {
        if (*curr == '#')
            comment = true;
        else if (*curr == '\n')
        {
            if (lines.back().length() > 1)
                lines.push_back(tmpstr);
            comment = false;
        }
        else if (!comment)
        {
            lines.back() += *curr;
        }
        ++curr;
    }
    free(buffer);
    
    // Extract case folding values for each parsed line
    auto regxstr = R"(([0-9a-fA-F]+);[\s]+(C|F|S|T);[\s]+([0-9a-fA-F]+)[\s|;]+?([0-9a-fA-F]+)?)";
    regex r(regxstr);
    vector<FoldingData> foldingData;
    foldingData.reserve(1500);
    for (const auto & s : lines)
    {
        FoldingData fd;
        auto m = smatch {};
        if (regex_search(s, m, r))
        {
            fd.source = m[1].str();
            fd.type = m[2].str();
            fd.target1 = m[3].str();
            if (m.size() > 4)
                fd.target2 = m[4].str();
            else
                fd.target2.clear();
			if (fd.type != "T" && fd.type != "S")
				foldingData.push_back(fd);
        }
    }
    
    // Open converted source file for writing
    err = fopen_s(&file, "..//Source//JxUnicodeCaseFolding.cpp", "w");
    if (err != 0)
        return -1;

    // Write source header
    fwrite(s_strCppHeader, 1, strlen(s_strCppHeader), file);

    string s;
    s.reserve(100);
    // Write all extracted folding data in a table
    for (const auto & fd : foldingData)
    {
        s += "\t\t\t{ ";
        s += "0x";
        s += fd.source;
        s += ", 0x";
        s += fd.target1;
        s += ", 0x";
        if (fd.target2.empty())
            s += "0000";
        else
            s += fd.target2;
        s += "},\n";
        fwrite(s.c_str(), 1, s.length(), file);
        s.clear();
    }

    // Write source tail and close
    fwrite(s_strCppTail, 1, strlen(s_strCppTail), file);
    fclose(file);

    return 0;
}

