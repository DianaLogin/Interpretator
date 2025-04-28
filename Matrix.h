#pragma once
#include <array>
#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <string>
#include "Vector.h"

template <typename T, size_t Rows, size_t Cols>
class Matrix
{
private:
    std::array<T, Rows* Cols> data;

    constexpr size_t index(size_t row, size_t col) const
    {
        return row * Cols + col;
    }

public:
    Matrix() = default;
    // Даже если конструктор с двумерным листом, можно его элементы скопировать в одномерный массив
    // с помощью циклов, которые надо умудриться додуматься праильно написать еще 
    Matrix(const std::initializer_list<std::initializer_list<T>>& list)
    {
        if (list.size() != Rows)
        {
            throw std::invalid_argument("Неверное количество строк!");
        }

        size_t i = 0;
        for (const auto& row : list)
        {
            if (row.size() != Cols)
            {
                throw std::invalid_argument("Неверное количество столбцов!");
            }
            for (const auto& val : row)
            {
                data[i++] = val;
            }
        }
    }

    // Доступ к элементам через (), потому что массив в поле класса одномерный,
    // а не двумерный. Не получится через [][] 
    T& operator()(size_t row, size_t col)
    {
        return data[index(row, col)];
    }
    const T& operator()(size_t row, size_t col) const
    {
        return data[index(row, col)];
    }


    Matrix<T, Rows, Cols> operator+(const Matrix<T, Rows, Cols>& other) const
    {
        Matrix<T, Rows, Cols> result;
        for (size_t i = 0; i < Rows; ++i)
        {
            for (size_t j = 0; j < Cols; ++j)
            {
                result(i, j) = (*this)(i, j) + other(i, j);
            }
        }
        return result;
    }

    Matrix<T, Rows, Cols> operator-(const Matrix<T, Rows, Cols>& other) const
    {
        Matrix<T, Rows, Cols> result;
        for (size_t i = 0; i < Rows; ++i)
        {
            for (size_t j = 0; j < Cols; ++j)
            {
                result(i, j) = (*this)(i, j) - other(i, j);
            }
        }
        return result;
    }
    // Умножение на скаляр
    Matrix<T, Rows, Cols> operator*(T scalar) const
    {
        Matrix<T, Rows, Cols> result;
        for (size_t i = 0; i < Rows; ++i)
        {
            for (size_t j = 0; j < Cols; ++j)
            {
                result(i, j) = (*this)(i, j) * scalar;
            }
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& out, const Matrix<T, Rows, Cols>& m)
    {
        out << "{\n";
        for (size_t i = 0; i < Rows; ++i)
        {
            out << "  {";
            for (size_t j = 0; j < Cols; ++j)
            {
                out << m(i, j);
                if (j < Cols - 1)
                {
                    out << ", ";
                }
            }
            out << "}";
            if (i < Rows - 1)
            {
                out << ",";
            }
            out << "\n";
        }
        out << "}";
        return out;
    }


};

// Нужен шаблон аж с 4 параметрами(!!!!!!!), потому что при запуске этой функции
// умственно отсталый компилятор не знает ничего
// кроме своих обычных Rows и Cols, которые в шаблоне класса, во Мудила он!!!!!

template<typename T, size_t Rows_1, size_t Cols_1, size_t Rows_2, size_t Cols_2>
// Тут явно показывыаю 2 аргумента умножения, чтоб удобнее было работать с колс ровс
// А еще только этой функции нужно особое приглашение, чтоб она работала
// Видишь ли, если она находится внутри класса, то тогда : "бинарный "operator *" имеет слишком много параметров"
// Ну пришлось выгнать этот оператор из тусовки внутренностей класса
// Ну и правильно... Туда его... На нашей тусовке могут быть простые операторы,
// а не этот треш матричного умножения
Matrix<T, Rows_1, Cols_2> operator*(const Matrix<T, Rows_1, Cols_1>& m_1, const Matrix<T, Rows_2, Cols_2>& m_2)
{
    if (Cols_1 != Rows_2)
    {
        throw std::invalid_argument(
            "Количество столбцов 1 матрицы должно быть равно количеству строк 2 матрицы: " +
            std::to_string(Cols_1) + " columns != " +
            std::to_string(Rows_2) + " rows"
        );
    }
    Matrix<T, Rows_1, Cols_2> result;
    for (size_t i = 0; i < Rows_1; ++i)
    {
        for (size_t j = 0; j < Cols_2; ++j)
        {
            result(i, j) = 0;
            for (size_t k = 0; k < Cols_1; ++k)
            {
                result(i, j) += m_1(i, k) * m_2(k, j);
            }
        }
    }
    return result;
}



// Эти смешанные умножения тоже стрёмные, поэтому их тоже выкидываю вне основного класса матрицы :)

// Умножение матрицы на Вектор-столбец
template<typename T, size_t Rows, size_t Cols>
Vector<T> operator*(const Matrix<T, Rows, Cols>& matrix, const Vector<T>& vector)
{
    if (vector.getSize() != Cols)
    {
        throw std::invalid_argument(
            "Размер вектора должен совпадать с количеством столбцов матрицы: " +
            std::to_string(vector.getSize()) + " != " + std::to_string(Cols)
        );
    }

    Vector<T> result(Rows); // Вектор-столбец 
    for (size_t i = 0; i < Rows; ++i)
    {
        T sum = 0;
        for (size_t j = 0; j < Cols; ++j)
        {
            sum += matrix(i, j) * vector[j];
        }
        result[i] = sum;
    }
    return result;
}


// Умножение вектора-строки на матрицу
template<typename T, size_t Rows, size_t Cols>
Vector<T> operator*(const Vector<T>& vector, const Matrix<T, Rows, Cols>& matrix)
{
    if (vector.getSize() != Rows)
    {
        throw std::invalid_argument(
            "Размер вектора должен совпадать с количеством строк матрицы: " +
            std::to_string(vector.getSize()) + " != " + std::to_string(Rows)
        );
    }

    Vector<T> result(Cols); // Вектор-строка размером Cols
    for (size_t j = 0; j < Cols; ++j)
    {
        T sum = 0;
        for (size_t i = 0; i < Rows; ++i)
        {
            sum += vector[i] * matrix(i, j);
        }
        result[j] = sum;
    }
    return result;
}