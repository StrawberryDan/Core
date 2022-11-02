#pragma once



#include <cstdint>
#include <type_traits>
#include <vector>
#include "Standard/Option.hpp"



#include "nlohmann/json.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	class SimplePayload
	{
	public:
	    SimplePayload() = default;
	    explicit SimplePayload(std::vector<uint8_t>  bytes);

	    void Read(uint8_t* data, size_t len);

	    template<typename T>
	    T Read() requires(std::is_fundamental_v<T>);

	    void Write(const uint8_t* data, size_t len);

	    template<typename T>
	    void Write(const T& data) requires(std::is_fundamental_v<T>);

	    std::vector<uint8_t>&       operator*()       { return mData; }
	    const std::vector<uint8_t>& operator*() const { return mData; }

	    [[nodiscard]] const uint8_t* Data() const { return mData.data(); }
	    [[nodiscard]] size_t Size() const { return mData.size(); }



	    Option<nlohmann::json> AsJSON();


	private:
	    std::vector<uint8_t> mData;
	};



	template<typename T>
	T SimplePayload::Read() requires(std::is_fundamental_v<T>)
	{
	    T data;
	    Read(reinterpret_cast<uint8_t*>(&data), sizeof(T));
	    return data;
	}



	template<typename T>
	void SimplePayload::Write(const T& data) requires (std::is_fundamental_v<T>)
	{
	    Write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
	}
}
