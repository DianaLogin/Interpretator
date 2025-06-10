#pragma once
#include <initializer_list>
#include <iostream>
#include <memory>
#include <ranges>
#include <iomanip>

template<typename T>
class Vector
{
private:
	size_t size;
	T* data;

public:
	Vector() = default;
	Vector(const std::initializer_list<T>& initList) : size(initList.size())
	{
		data = new T[size];

		for (auto [i, value] : initList | std::views::enumerate)
		{
			data[i] = value;
		}
	}

	Vector(size_t data_size) : size(data_size)
	{
		data = new T[size];
	}
	Vector(const Vector& other) : size(other.size)
	{
		data = nullptr;

		if (other.size > 0)
		{
			data = new T[other.size];
			//           откуда, куда и сколько байт копируетс€
			std::memcpy(data, other.data, other.size * sizeof(T));
		}
	}

	// доступ по индексу
	T& operator[](size_t idx)
	{
		if (idx >= size)
		{
			throw std::out_of_range("»ндекс выходит за гранцы массива!");
		}
		return data[idx];
	}
	const T& operator[] (size_t idx) const
	{
		if (idx >= size)
		{
			throw std::out_of_range("»ндекс выходит за гранцы массива!");
		}
		return data[idx];
	}



	const Vector& operator= (const Vector& rhs)
	{

		if (this == &rhs)
			return *this;

		delete[] data;

		size = rhs.size;
		data = new T[size];
		for (size_t i = 0; i < size; ++i)
		{
			data[i] = rhs.data[i];
		}

		return *this;
	}

	const Vector& operator=(Vector&& rhs) noexcept
	{
		if (this == &rhs)
			return *this;

		delete[] data;

		data = rhs.data;
		size = rhs.size;

		rhs.data = nullptr;
		rhs.size = 0;

		return *this;
	}

	size_t getSize() const
	{
		return size;
	}

	template <typename T>
	bool is_one_or_zero(T value)
	{
		return std::fabs(value - 1.0f) < 1e-6f || std::fabs(value - 0.0f) < 1e-6f;
	}

	template <typename T>
	bool is_one(T value)
	{
		return std::fabs(value - 1.0f) < 1e-6f;
	}

	bool all_of() const
	{
		for (const T& val : data)
		{
			if (!is_one_or_zero(val))
			{
				return false;
			}
		}
		return true;
	}

	bool any_of() const
	{
		for (const T& val : data)
		{
			if (is_one(val))
			{
				return true;
			}
		}
		return false;
	}

	~Vector()
	{
		delete[] data;
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& v)
{
	os << "{";
	for (size_t i = 0; i < v.getSize(); ++i)
	{
		os << std::fixed << std::setprecision(2) << v[i];
		if (i < v.getSize() - 1) os << ",";
	}
	os << "}";
	return os;
}

template <typename T>
Vector<T> operator+ (const Vector<T>& v_1, const Vector<T>& v_2)
{
	if (v_1.getSize() != v_2.getSize())
	{
		throw std::invalid_argument("–азмеры складываемых векторов не совпадают!");
	}

	Vector<T> sum(v_1.getSize());

	for (size_t i = 0; i < v_1.getSize(); ++i)
	{
		sum[i] = v_1[i] + v_2[i];
	}

	return sum;
}

template <typename T>
Vector<T> operator- (const Vector<T>& v_1, const Vector<T>& v_2)
{
	if (v_1.getSize() != v_2.getSize())
	{
		throw std::invalid_argument("–азмеры вычитаемых векторов не совпадают!");
	}

	Vector<T> sum(v_1.getSize());

	for (size_t i = 0; i < v_1.getSize(); ++i)
	{
		sum[i] = v_1[i] - v_2[i];
	}

	return sum;
}

template <typename T>
T operator* (const Vector<T>& v_1, const Vector<T>& v_2)
{
	if (v_1.getSize() != v_2.getSize())
	{
		throw std::invalid_argument("–азмеры умножаемых векторов не совпадают!");
	}

	T sum = 0;
	for (size_t i = 0; i < v_1.getSize(); ++i)
	{
		sum += v_1[i] * v_2[i];
	}

	return sum;
}

template <typename T>
Vector<T> operator* (const Vector<T>& v, const T& scalar)
{
	Vector<T> res(v.getSize());
	for (size_t i = 0; i < v.getSize(); ++i)
	{
		res[i] *= v[i] * scalar;
	}

	return res;
}

template <typename T>
Vector<T> operator/(const Vector<T>& v_1, const Vector<T>& v_2)
{
	if (v_1.getSize() != v_2.getSize())
	{
		throw std::invalid_argument("–азмеры делимых векторов не совпадают!");
	}

	Vector<T> result(v_1.getSize());
	for (size_t i = 0; i < v_1.getSize(); ++i)
	{
		result[i] = v_1[i] / v_2[i];
	}
	return result;
}

