#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <filesystem>
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO::Table
{
	using Data = std::vector<std::vector<std::string>>;


	Core::Optional<Data> FromFile(const std::filesystem::path& path, char delimiter = '\t');
	Data                 FromString(const std::string& string, char delimiter = '\t');
}
