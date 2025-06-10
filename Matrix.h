#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <string>
#include "Vector.h"
#include<iomanip>

template <typename T>
class Matrix
{
private:
    size_t rows, cols;
    std::vector<T> data;

    size_t index(size_t row, size_t col) const
    {
        return row * cols + col;
    }

public:
    Matrix() = default;

    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows* cols) { }

    Matrix(const std::initializer_list<std::initializer_list<T>>& list)
    {
        rows = list.size();
        if (rows == 0)
        {
            throw std::invalid_argument("Пустая матрица!");
        }

        getCols = list.begin()->size();
        for (const auto& row : list)
        {
            if (row.size() != getCols)
            {
                throw std::invalid_argument("Строки матрицы разной длины!");
            }
        }

        data.reserve(rows * getCols);
        for (const auto& row : list)
        {
            for (const auto& val : row)
            {
                data.push_back(val);
            }
        }
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    T& operator()(size_t row, size_t col)
    {
        return data[index(row, col)];
    }

    const T& operator()(size_t row, size_t col) const
    {
        return data[index(row, col)];
    }

    Matrix<T> operator+(const Matrix<T>& other) const
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw std::invalid_argument("Размеры матриц не совпадают!");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] + other.data[i];
        }

        return result;
    }

    Matrix<T> operator-(const Matrix<T>& other) const
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw std::invalid_argument("Размеры матриц не совпадают!");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] - other.data[i];
        }

        return result;
    }


    Matrix<T> operator*(T scalar) const
    {
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] * scalar;
        }

        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& m)
    {
        os << "[";
        for (size_t i = 0; i < m.getRows(); ++i)
        {
            os << "{";
            for (size_t j = 0; j < m.getCols(); ++j)
            {
                os << std::fixed << std::setprecision(2) << m(i, j);
                if (j < m.getCols() - 1) os << ",";
            }
            os << "}";
            if (i < m.getRows() - 1) os << ";";
        }
        os << "]";
        return os;
    }

    Matrix<T> operator*(const Matrix<T>& other) const
    {
        if (cols != other.rows)
        {
            throw std::invalid_argument(
                "Невозможно перемножить: " + std::to_string(cols) +
                " != " + std::to_string(other.rows));
        }

        Matrix<T> result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j)
            {
                T sum = 0;
                for (size_t k = 0; k < cols; ++k)
                {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
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
        for (const auto& row : data)
        {
            for (const T& val : row)
            {
                if (!is_one_or_zero(val))
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool any_of() const
    {
        for (const auto& row : data)
        {
            for (const T& val : row)
            {
                if (is_one(val))
                {
                    return true;
                }
            }
        }
        return false;
    }
};

// Умножение матрицы на вектор-столбец
template <typename T>
Vector<T> operator*(const Matrix<T>& matrix, const Vector<T>& vector)
{
    if (matrix.getCols() != vector.getSize())
    {
        throw std::invalid_argument(
            "Размер вектора должен совпадать с количеством столбцов матрицы");
    }

    Vector<T> result(matrix.getRows());
    for (size_t i = 0; i < matrix.getRows(); ++i)
    {
        T sum = 0;
        for (size_t j = 0; j < matrix.getCols(); ++j)
        {
            sum += matrix(i, j) * vector[j];
        }
        result[i] = sum;
    }
    return result;
}

// Умножение вектора-строки на матрицу
template <typename T>
Vector<T> operator*(const Vector<T>& vector, const Matrix<T>& matrix)
{
    if (vector.getSize() != matrix.getRows())
    {
        throw std::invalid_argument(
            "Размер вектора должен совпадать с количеством строк матрицы");
    }

    Vector<T> result(matrix.getCols());
    for (size_t j = 0; j < matrix.getCols(); ++j)
    {
        T sum = 0;
        for (size_t i = 0; i < matrix.getRows(); ++i)
        {
            sum += vector[i] * matrix(i, j);
        }
        result[j] = sum;
    }
    return result;
}
