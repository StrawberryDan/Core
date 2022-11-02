#include "Standard/Net/HTTP/Header.hpp"



#include "Standard/Assert.hpp"
#include "Standard/Utilities.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	void Header::Add(const Header::Key& key, const Header::Value& value)
	{
		auto lc = ToLowercase(key);
		if (mEntries.contains(lc))
		{
			mEntries.at(lc).push_back(value);
		} else
		{
			mEntries.insert({lc, {value}});
		}
	}



	void Header::Set(const Header::Key& key, const Header::Value& value)
	{
		auto lc = ToLowercase(key);
		mEntries.insert_or_assign(lc, std::vector<Value>{value});
	}



	Header::Value Header::Get(const Header::Key& key) const
	{
		auto lc = ToLowercase(key);
		Assert(mEntries.contains(lc));
		return mEntries.at(lc)[0];
	}



	std::vector<Header::Value> Header::GetAll(const Header::Key& key) const
	{
		auto lc = ToLowercase(key);
		Assert(mEntries.contains(lc));
		return mEntries.at(lc);
	}



	bool Header::Contains(const Header::Key& key) const
	{
		auto lc = ToLowercase(key);
		return mEntries.contains(lc);
	}
}