#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "Matrix.h"
#include "Vector.h"
#include <locale>
#include <cstdio>


int main()

{
    setlocale(LC_ALL, "Russian");


    char f_or_c;
    printf("---- Как вы хотите задать данные? ----\n1. Через консоль\n2. Через файл\n");

repeat__f_or_c:
    printf("-> ");
    std::cin >> f_or_c; // не scanf, потому что возникает бесконечный цикл

    switch (f_or_c)
    {
    case '1':
        printf("Выбран ввод через консоль.\n");
        break;
    case '2':
        printf("Выбран ввод через файл.\n");
        break;
    default:
        printf("Ошибка: введите 1 или 2.\n");
        goto repeat__f_or_c;
    }

    std::string equation;
    std::getline(std::cin,equation);

    size_t pos_1 = equation.find("[");
    size_t pos_2 = equation.find("]");


    char word = 0;
    char matrix_char = 0, vector_col_char = 0, vector_row_char = 0;


    if (word >= 65 && word <= 90)
    {
        word = matrix_char;
    }

   /* if (equation.starts_with())
    {
        
    }*/
}









































//void handleInputMatrixOrVector() {
//    std::string name, type;
//    std::cout << "Введите имя переменной (например, A или v): ";
//    std::cin >> name;
//
//    std::cout << "Введите тип (matrix/vector): ";
//    std::cin >> type;
//
//    std::cin.ignore(); // Очистка буфера
//
//    if (type == "matrix") {
//        std::cout << "Введите матрицу в формате [a1, a2, a3; b1, b2, b3; c1, c2, c3]: ";
//        std::string input;
//        std::getline(std::cin, input);
//
//        // Преобразование ввода в матрицу (в примере для 3x3)
//        std::stringstream ss(input);
//        Matrix<double, 3, 3> matrix;
//        char ch;
//        double val;
//        for (size_t i = 0; i < 3; ++i) {
//            for (size_t j = 0; j < 3; ++j) {
//                ss >> val;
//                matrix(i, j) = val;
//                ss >> ch;  // Пропуск запятой или точки с запятой
//            }
//        }
//        matrices[name] = matrix;  // Сохраняем матрицу
//    }
//    else if (type == "vector") {
//        std::cout << "Введите вектор в формате [1; 2; 3]: ";
//        std::string input;
//        std::getline(std::cin, input);
//
//        // Преобразование ввода в вектор
//        std::stringstream ss(input);
//        Vector<double> vector;
//        double val;
//        char ch;
//        while (ss >> val) {
//            vector.push_back(val);
//            ss >> ch;  // Пропуск точки с запятой
//        }
//        vectors[name] = vector;  // Сохраняем вектор
//    }
//}
//
//void performOperations() {
//    std::string expr;
//    std::cout << "Введите выражение для выполнения (например, A + B, A * v, и т.д.): ";
//    std::cin.ignore();  // Очистка буфера
//    std::getline(std::cin, expr);
//
//    std::stringstream ss(expr);
//    std::string leftOperand, op, rightOperand;
//    ss >> leftOperand >> op >> rightOperand;
//
//    try {
//        if (op == "+") {
//            if (matrices.find(leftOperand) != matrices.end() && matrices.find(rightOperand) != matrices.end()) {
//                std::cout << matrices[leftOperand] + matrices[rightOperand] << std::endl;
//            }
//            else if (vectors.find(leftOperand) != vectors.end() && vectors.find(rightOperand) != vectors.end()) {
//                std::cout << vectors[leftOperand] + vectors[rightOperand] << std::endl;
//            }
//            else {
//                throw std::invalid_argument("Ошибка: несоответствующие типы данных для сложения!");
//            }
//        }
//        else if (op == "-") {
//            if (matrices.find(leftOperand) != matrices.end() && matrices.find(rightOperand) != matrices.end()) {
//                std::cout << matrices[leftOperand] - matrices[rightOperand] << std::endl;
//            }
//            else if (vectors.find(leftOperand) != vectors.end() && vectors.find(rightOperand) != vectors.end()) {
//                std::cout << vectors[leftOperand] - vectors[rightOperand] << std::endl;
//            }
//            else {
//                throw std::invalid_argument("Ошибка: несоответствующие типы данных для вычитания!");
//            }
//        }
//        else if (op == "*") {
//            if (matrices.find(leftOperand) != matrices.end() && vectors.find(rightOperand) != vectors.end()) {
//                std::cout << matrices[leftOperand] * vectors[rightOperand] << std::endl;
//            }
//            else if (vectors.find(leftOperand) != vectors.end() && matrices.find(rightOperand) != matrices.end()) {
//                std::cout << vectors[leftOperand] * matrices[rightOperand] << std::endl;
//            }
//            else {
//                throw std::invalid_argument("Ошибка: несоответствующие типы данных для умножения!");
//            }
//        }
//        else {
//            std::cout << "Неверная операция!" << std::endl;
//        }
//    }
//    catch (const std::exception& e) {
//        std::cout << "Ошибка: " << e.what() << std::endl;
//    }
//}
//
//void printVariable() {
//    std::string name;
//    std::cout << "Введите имя переменной для печати: ";
//    std::cin >> name;
//
//    if (matrices.find(name) != matrices.end()) {
//        std::cout << matrices[name] << std::endl;
//    }
//    else if (vectors.find(name) != vectors.end()) {
//        std::cout << vectors[name] << std::endl;
//    }
//    else {
//        std::cout << "Переменная не найдена!" << std::endl;
//    }
//}
//
//int mainn() {
//    bool running = true;
//    while (running) {
//        printMenu();
//        int choice;
//        std::cout << "Выберите опцию: ";
//        std::cin >> choice;
//
//        switch (choice) {
//        case 1:
//            handleInputMatrixOrVector();
//            break;
//        case 2:
//            performOperations();
//            break;
//        case 3:
//            printVariable();
//            break;
//        case 4:
//            running = false;
//            break;
//        default:
//            std::cout << "Неверный выбор, попробуйте снова!" << std::endl;
//        }
//    }
//    return 0;
//}
