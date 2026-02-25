#pragma once


#include "Strawberry/Core/Math/Matrix.hpp"
#include <random>


namespace Strawberry::Core::Math::Noise::Adapter
{
	template <typename Base>
	class Rotate
	{
	public:
		Rotate(Base&& base, float orientation)
			: mBase(std::forward<Base>(base))
		{
			mOrientation = Core::Math::Mat2f{
				std::cosf(orientation), -std::sinf(orientation),
				std::sinf(orientation), std::cosf(orientation)};
		}


		Rotate(Base&& base)
			: mBase(std::forward<Base>(base))
		{
			std::uniform_real_distribution<float> distribution(0.0f, 2.0f * M_PI);

			std::random_device random;
			float orientation = distribution(random);
			mOrientation = Core::Math::Mat2f{
				std::cosf(orientation), -std::sinf(orientation),
				std::sinf(orientation), std::cosf(orientation)};
		}


		float Amplitude() const noexcept
		{
			return mBase.Amplitude();
		}


		float operator()(Vec2f position) const
		{
			return mBase(mOrientation * position);
		}


	private:
		Mat2f mOrientation;
		Base mBase;
	};


	template <typename Base>
	class Scale
	{
	public:
		Scale(float amplitude, Base&& base)
			: mAmplitude(amplitude)
			, mBase(std::forward<Base>(base))
		{}


		float Amplitude() const noexcept
		{
			return mAmplitude;
		}


		float operator()(Vec2f position) const
		{
			return mAmplitude * mBase(position);
		}


	private:
		float mAmplitude;
		Base mBase;
	};


	template <typename Base>
	class Layer
	{
	public:
		template <typename F> requires std::same_as<std::decay_t<std::invoke_result_t<F, unsigned int>>, Base>
		Layer(const unsigned int layerCount, F&& generator)
		{
			for (unsigned int i = 0; i < layerCount; i++)
			{
				mSignals.emplace_back(generator(i));
			}
		}

		float Amplitude() const noexcept
		{
			return std::ranges::fold_left(mSignals | std::views::transform([] (auto&& x) { return x.Amplitude(); }), 0.0f, std::plus());
		}


		auto begin(this auto& self)
		{
			return self.mSignals.begin();
		}

		auto end(this auto& self)
		{
			return self.mSignals.end();
		}


		float operator()(Vec2f position) const
		{
			float value = 0.0f;

			for (auto&& signal : mSignals)
			{
				value += signal(position);
			}

			return value;
		}
	private:
		std::vector<Base> mSignals;
	};


	template <typename T>
	Layer(unsigned int, T&&) -> Layer<std::decay_t<std::invoke_result_t<T, unsigned int>>>;


	template <typename Base>
	class TransformInput
	{
	public:
		template <typename F>
		TransformInput(F&& function, Base&& base)
			: mFunctor(std::forward<F>(function))
			, mBase(std::move(base))
		{}


		float Amplitude() const noexcept
		{
			return mBase.Amplitude();
		}


		float operator()(Vec2f position) const
		{
			position = mFunctor(position);
			return mBase(position);
		}


	private:
		std::function<Vec2f(Vec2f)>    mFunctor;
		Base mBase;
	};


	template <typename BaseA, typename BaseB>
	class Sum
	{
	public:
		Sum(BaseA&& a, BaseB&& b)
			: mA(std::move(a))
			, mB(std::move(b))
		{}


		float Amplitude() const noexcept
		{
			return mA.Amplitude() + mB.Amplitude();
		}


		float operator()(Vec2f position) const
		{
			return mA(position) + mB(position);
		}


	private:
		BaseA mA;
		BaseB mB;
	};
}
