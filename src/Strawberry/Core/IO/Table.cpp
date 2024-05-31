//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Table.hpp"
#include "DynamicByteBuffer.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO::Table
{
    Core::Optional<Data> FromFile(const std::filesystem::path& path, char delimiter)
    {
        auto bytes = DynamicByteBuffer::FromFile(path);
        if (!bytes) return Core::NullOpt;

        return FromString(std::string(reinterpret_cast<const char*>(bytes->Data()), bytes->Size()), delimiter);
    }


    Data FromString(const std::string& string, char delimiter)
    {
        auto SplitIntoLines = [&]()
        {
            std::vector<std::string> lines;
            std::string              currentLine = "";

            for (auto c: string)
            {
                if (c == '\n')
                {
                    lines.push_back(currentLine);
                    currentLine = "";
                }
                else
                {
                    currentLine += c;
                }
            }
            if (!currentLine.empty()) lines.push_back(currentLine);

            return lines;
        };


        auto SplitDelimiter = [delimiter](const std::string& line)
        {
            std::vector<std::string> data;
            std::string              currentData = "";

            for (auto c: line)
            {
                if (c == delimiter)
                {
                    data.push_back(currentData);
                    currentData = "";
                }
                else
                {
                    currentData += c;
                }
            }
            if (!currentData.empty()) data.emplace_back(std::move(currentData));

            return data;
        };


        Data data;
        for (auto line: SplitIntoLines())
        {
            data.emplace_back(SplitDelimiter(line));
        }

        return data;
    }
}
