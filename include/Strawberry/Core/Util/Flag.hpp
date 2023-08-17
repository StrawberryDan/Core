#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	class Flag
	{
	public:
		Flag() = default;
		Flag(bool value); // NOLINT(*-explicit-constructor)

		void Set();
		void Unset();

		Flag& operator=(bool value);

		operator bool() const { return mIsSet; }

	private:
		bool mIsSet = false;
	};
} // namespace Strawberry::Core