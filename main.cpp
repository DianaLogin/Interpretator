#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <variant>
#include <locale>
#include <map>
#include <sstream>
#include <algorithm>
#include "Vector.h"
#include "Matrix.h"

using MatrixVariant = std::variant<Matrix<int>, Matrix<float>, Matrix<double>>;
using VectorVariant = std::variant<Vector<int>, Vector<float>, Vector<double>>;

std::map<char, MatrixVariant> matrixes;
std::map<char, VectorVariant> vectors;

// Функция для определения типа числа и парсинга из строки в число
template<typename T>
T Str_to_Num(const std::string& s)
{
    if constexpr (std::is_same_v<T, int>) return std::stoi(s);
    else if constexpr (std::is_same_v<T, float>) return std::stof(s);
    else if constexpr (std::is_same_v<T, double>) return std::stod(s);
}

// Функция для создания вектора
template<typename T>
void Vec_Create(const std::string& comand, char name, bool isColumn = false)
{
    std::istringstream iss(comand); // это поток ввода из строки
    std::string num; // переменная для хранения каждого числа пока в виде строки
    std::vector<T> elements;

    while (getline(iss, num, ','))
    {
        if (!num.empty()) {
            elements.push_back(Str_to_Num<T>(num));
        }
    }

    if (isColumn)
    {
        Matrix<T> mat(elements.size(), 1);
        for (size_t i = 0; i < elements.size(); ++i)
        {
            mat(i, 0) = elements[i];
        }
        matrixes[name] = mat;
    }
    else
    {
        Vector<T> vec(elements.size());
        for (size_t i = 0; i < elements.size(); ++i)
        {
            vec[i] = elements[i];
        }
        vectors[name] = vec;
    }
}

// Функция для создания матрицы
template<typename T>
void createMatrix(const std::string& data, char name)
{
    std::vector<std::vector<T>> rows;
    std::istringstream rowStream(data);
    std::string rowStr;

    while (getline(rowStream, rowStr, ';'))
    {
        std::istringstream numStream(rowStr);
        std::string num;
        std::vector<T> currentRow;

        while (getline(numStream, num, ','))
        {
            if (!num.empty()) {
                currentRow.push_back(Str_to_Num<T>(num));
            }
        }

        if (!currentRow.empty()) {
            rows.push_back(currentRow);
        }
    }

    if (!rows.empty()) {
        Matrix<T> mat(rows.size(), rows[0].size());
        for (size_t i = 0; i < rows.size(); ++i) {
            for (size_t j = 0; j < rows[i].size(); ++j) {
                mat(i, j) = rows[i][j];
            }
        }
        matrixes[name] = mat;
    }
}

// Функция для вывода переменной
void printVariable(char name) {
    if (matrixes.count(name)) {
        std::visit([](auto&& mat) {
            std::cout << mat;
            }, matrixes[name]);
        return;
    }

    if (vectors.count(name)) {
        std::visit([](auto&& vec) {
            std::cout << vec;
            }, vectors[name]);
        return;
    }

    std::cerr << "Ошибка: переменная '" << name << "' не найдена\n";
}

// Функция для выполнения математических операций
template <typename T>
// Функция для выполнения математических операций (не шаблонная!)
void performOperation(const std::string& operation) {
    if (operation.length() != 5) {
        std::cerr << "Ошибка: неверный формат операции\n";
        return;
    }

    char var1 = operation[0];
    char op = operation[1];
    char var2 = operation[2];
    char result = operation[4]; // Формат: "A+B=C"

    try {
        // Проверяем существование переменных
        if (!matrixes.count(var1) && !vectors.count(var1)) {
            throw std::runtime_error("Переменная " + std::string(1, var1) + " не найдена");
        }
        if (!matrixes.count(var2) && !vectors.count(var2)) {
            throw std::runtime_error("Переменная " + std::string(1, var2) + " не найдена");
        }

        // Матричные операции
        if (matrixes.count(var1) && matrixes.count(var2)) {
            auto perform = [&](auto&& m1, auto&& m2) {
                auto res = m1; // Создаем копию для определения типа
                switch (op) {
                    case '+': res = m1 + m2; break;
                    case '-': res = m1 - m2; break;
                    case '*': res = m1 * m2; break;
                    default: throw std::runtime_error("Неподдерживаемая операция для матриц");
                }
                matrixes[result] = res;
            };
            std::visit(perform, matrixes[var1], matrixes[var2]);
        }
        // Векторные операции
        else if (vectors.count(var1) && vectors.count(var2)) {
            auto perform = [&](auto&& v1, auto&& v2) {
                auto res = v1; // Создаем копию для определения типа
                switch (op) {
                    case '+': res = v1 + v2; break;
                    case '-': res = v1 - v2; break;
                    case '*': res = v1 * v2; break;
                    case '/': res = v1 / v2; break;
                    default: throw std::runtime_error("Неподдерживаемая операция для векторов");
                }
                vectors[result] = res;
            };
            std::visit(perform, vectors[var1], vectors[var2]);
        }
        else {
            throw std::runtime_error("Несовместимые типы переменных для операции");
        }

        std::cout << "Операция выполнена успешно. Результат в переменной " << result << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

// Определяем тип чисел (int/float/double)
void parseData(const std::string& data, char name) {
    bool hasSemicolon = data.find(';') != std::string::npos;
    bool hasComma = data.find(',') != std::string::npos;
    bool hasDot = data.find('.') != std::string::npos;

    try {
        if (hasSemicolon && hasComma) {
            if (hasDot) createMatrix<double>(data, name);
            else createMatrix<int>(data, name);
        }
        else if (hasSemicolon) {
            if (hasDot) Vec_Create<double>(data, name, true);
            else Vec_Create<int>(data, name, true);
        }
        else {
            if (hasDot) Vec_Create<double>(data, name);
            else Vec_Create<int>(data, name);
        }
        std::cout << "Успешно создана переменная " << name << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при создании переменной " << name << ": " << e.what() << std::endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    char f_or_c;
    printf("---- Как вы хотите задать данные? ----\n1. Через консоль\n2. Через файл\n");

repeat_choice:
    printf("-> ");
    scanf_s(" %c", &f_or_c);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (f_or_c) {
    case '1': {
        printf("\n--- Выбран ввод через консоль ---\n\n");

        // Основной цикл ввода команд
        while (true) {
            printf("Введите команду (Примеры: A = [1,2;3,4], print(A), A+B=C, exit)\n-> ");

            std::string comand;
            std::getline(std::cin, comand);

            // Проверка на команду выхода
            if (comand == "exit") {
                return 0;
            }

            // Проверка на команду print
            if (comand.find("print(") != std::string::npos) {
                size_t start = comand.find('(');
                size_t end = comand.find(')');
                if (start != std::string::npos && end != std::string::npos) {
                    char varName = comand[start + 1];
                    printVariable(varName);
                }
                continue;
            }

            // Проверка на математическую операцию (формат: A+B=C)
            if (comand.size() == 5 && (comand[1] == '+' || comand[1] == '-' || comand[1] == '*' || comand[1] == '/') && comand[3] == '=') {
                performOperation<int>(comand);
                continue;
            }

            // Обработка создания переменной
            size_t start = comand.find('[');
            size_t end = comand.find(']');
            std::string filtered_comand;
            char varName = ' ';

            if (start != std::string::npos && end != std::string::npos) {
                // Извлекаем имя переменной (первую букву в строке)
                for (char c : comand) {
                    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                        varName = c;
                        break;
                    }
                }

                // Извлекаем данные между [ и ] и удаляем все пробелы
                filtered_comand = comand.substr(start + 1, end - start - 1);
                filtered_comand.erase(std::remove(filtered_comand.begin(), filtered_comand.end(), ' '), filtered_comand.end());
            }

            if (varName != ' ' && !filtered_comand.empty()) {
                printf("\nОтфильтрованные данные: %s\n", filtered_comand.c_str());
                parseData(filtered_comand, varName);
            }
            else {
                printf("\nОшибка: неверный формат команды\n");
            }
        }
        break;
    }
    case '2':
        printf("Выбран ввод через файл.\n");
        break;
    default:
        printf("Ошибка: введите 1 или 2.\n");
        goto repeat_choice;
    }

    return 0;
}