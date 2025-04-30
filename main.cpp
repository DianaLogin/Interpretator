#include <iostream> 
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <ShlObj.h>
#include <locale>
#include <codecvt>
#include <limits>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include "Vector.h"
#include "Matrix.h"


// Цвета для вывода
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define WHITE   "\033[37m"

// Макросы для вывода сообщений
#define PRINT_SUCCESS(msg) std::cout << GREEN << msg << RESET << std::endl
#define PRINT_ERROR(msg)   std::cerr << RED << "Ошибка: " << msg << RESET << std::endl
#define PRINT_INFO(msg)    std::cout << WHITE << msg << RESET << std::endl

// Функция для преобразования из UTF-16 (wchar_t) в UTF-8 (std::string)
std::string wstring_to_string(const std::wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
    return str;
}


std::map<char, Matrix<int>> matrixes;
std::map<char, Vector<int>> vectors;

int Str_to_Num(const std::string& s) {
    return std::stoi(s);
}

void Vec_Create(const std::string& command, char name, bool isColumn = false) {
    std::istringstream iss(command);
    std::string num;
    std::vector<int> elements;

    while (getline(iss, num, ',')) {
        if (!num.empty()) {
            elements.push_back(Str_to_Num(num));
        }
    }

    if (isColumn) {
        Matrix<int> mat(elements.size(), 1);
        for (size_t i = 0; i < elements.size(); ++i) {
            mat(i, 0) = elements[i];
        }
        matrixes[name] = mat;
    }
    else {
        Vector<int> vec(elements.size());
        for (size_t i = 0; i < elements.size(); ++i) {
            vec[i] = elements[i];
        }
        vectors[name] = vec;
    }
}

void createMatrix(const std::string& data, char name) {
    std::vector<std::string> rows;
    std::istringstream rowStream(data);
    std::string rowStr;

    while (getline(rowStream, rowStr, ';')) {
        if (!rowStr.empty()) rows.push_back(rowStr);
    }

    if (rows.empty()) return;

    std::vector<std::string> firstRowElements;
    std::istringstream firstRow(rows[0]);
    std::string num;
    while (getline(firstRow, num, ',')) {
        if (!num.empty()) firstRowElements.push_back(num);
    }

    Matrix<int> mat(rows.size(), firstRowElements.size());
    for (size_t i = 0; i < rows.size(); ++i) {
        std::istringstream numStream(rows[i]);
        std::string numStr;
        size_t j = 0;

        while (getline(numStream, numStr, ',')) {
            if (!numStr.empty()) {
                mat(i, j) = Str_to_Num(numStr);
                j++;
            }
        }
    }
    matrixes[name] = mat;
}

void printVariable(char name) {
    if (matrixes.count(name)) {
        std::cout << matrixes[name] << "\n";
        return;
    }

    if (vectors.count(name)) {
        std::cout << vectors[name] << "\n";
        return;
    }

    PRINT_ERROR("переменная '" << name << "' не найдена");
}

bool is_valid_letter(char c) {
    return std::isalpha(c);
}

bool is_valid_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

void performOperation(const std::string& operation) {
    if (operation.length() != 5 ||
        !is_valid_letter(operation[0]) ||
        !is_valid_operator(operation[1]) ||
        operation[3] != '=' ||
        !is_valid_letter(operation[2]) ||
        !is_valid_letter(operation[4])) {

        PRINT_ERROR("неверный формат операции. Используйте A+B=C");
        return;
    }

    char var1 = operation[0];
    char op = operation[1];
    char var2 = operation[2];
    char result = operation[4];

    try {
        bool var1_is_matrix = matrixes.count(var1);
        bool var1_is_vector = vectors.count(var1);
        bool var2_is_matrix = matrixes.count(var2);
        bool var2_is_vector = vectors.count(var2);

        if (!var1_is_matrix && !var1_is_vector)
            throw std::runtime_error("Переменная " + std::string(1, var1) + " не найдена");
        if (!var2_is_matrix && !var2_is_vector)
            throw std::runtime_error("Переменная " + std::string(1, var2) + " не найдена");

        if (var1_is_matrix && var2_is_matrix) {
            Matrix<int> res = (op == '+') ? (matrixes[var1] + matrixes[var2]) :
                (op == '-') ? (matrixes[var1] - matrixes[var2]) :
                (op == '*') ? (matrixes[var1] * matrixes[var2]) :
                throw std::runtime_error("Операция не поддерживается для матриц");
            matrixes[result] = res;
        }

        else if (var1_is_vector && var2_is_vector) {
            Vector<int> res = (op == '+') ? vectors[var1] + vectors[var2] :
                (op == '-') ? vectors[var1] - vectors[var2] :
                (op == '*') ? vectors[var1] * vectors[var2] :
                (op == '/') ? vectors[var1] / vectors[var2] :
                throw std::runtime_error("Операция не поддерживается для векторов");
            vectors[result] = res;
        }

        else if (var1_is_matrix && var2_is_vector && op == '*') {
            Vector<int> res = matrixes[var1] * vectors[var2];
            vectors[result] = res;
        }
        else if (var1_is_vector && var2_is_matrix && op == '*') {
            Vector<int> res = vectors[var1] * matrixes[var2];
            vectors[result] = res;
        }
        else {
            throw std::runtime_error("Несовместимые типы для операции");
        }

        PRINT_SUCCESS("Операция выполнена успешно. Результат в переменной " + std::string(1, result));
    }
    catch (const std::exception& e) {
        PRINT_ERROR(e.what());
    }
}

void parseData(const std::string& content, char name) {
    if (content.find(';') != std::string::npos) {
        if (!std::isupper(name)) {
            PRINT_ERROR("имя матрицы должно быть заглавной буквой");
            return;
        }
        createMatrix(content, name);
    }
    else {
        if (!std::islower(name)) {
            PRINT_ERROR("имя вектора должно быть строчной буквой");
            return;
        }
        Vec_Create(content, name);
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

    char f_or_c;
    PRINT_INFO("---- Как вы хотите задать данные? ----");
    PRINT_INFO("1. Через консоль\n2. Через файл");

repeat_choice:
    std::cout << "-> ";
    std::cin >> f_or_c;
    std::cin.ignore(10000, '\n');  // Очистка ввода

    switch (f_or_c) {
    case '1': {
        PRINT_INFO("--- Выбран ввод через консоль ---\n");

        while (true) {
            PRINT_INFO("Введите команду (пример: A = [1,2;3,4], x = [1,2,3], print(A), A+B=C, exit):");
            std::cout << "-> ";

            std::string command;
            std::getline(std::cin, command);
            command.erase(std::remove(command.begin(), command.end(), ' '), command.end());

            if (command == "exit") {
                PRINT_INFO("Выход... Очищаю память и завершаю программу.");
                matrixes.clear();
                vectors.clear();
                return 0;
            }

            // Заменяем starts_with и ends_with на find
            if (command.find("print(") == 0 && command.rfind(")") == command.size() - 1) {
                if (command.size() >= 8) {
                    char varName = command[6];
                    printVariable(varName);
                }
                else {
                    PRINT_ERROR("неверный формат команды print()");
                }
                continue;
            }

            if (command.size() == 5 && command[3] == '=' &&
                is_valid_letter(command[0]) && is_valid_operator(command[1]) &&
                is_valid_letter(command[2]) && is_valid_letter(command[4])) {
                performOperation(command);
                continue;
            }

            size_t eq = command.find('=');
            size_t lb = command.find('[');
            size_t rb = command.find(']');

            if (eq == std::string::npos || lb == std::string::npos || rb == std::string::npos) {
                PRINT_ERROR("неверный формат команды");
                continue;
            }

            char name = command[0];

            if (!std::isalpha(name)) {
                PRINT_ERROR("имя переменной должно быть буквой");
                continue;
            }

            std::string content = command.substr(lb + 1, rb - lb - 1);
            parseData(content, name);
            PRINT_SUCCESS("Успешно создана переменная " + std::string(1, name));
        }
        break;
    }

    case '2': {
        PRINT_INFO("--- Выбран ввод через файл ---");

        // Настройка структуры для выбора файла
        OPENFILENAMEW ofn;
        wchar_t szFile[MAX_PATH] = L"";  // Буфер для пути файла
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (!GetOpenFileNameW(&ofn)) {
            PRINT_ERROR("Файл не выбран.");
            return 1;
        }

        // Переводим wchar_t* -> std::string (с учётом кодировки UTF-8)
        std::wstring wfilename(szFile);
        std::string filename = wstring_to_string(wfilename);

        std::cout << "Путь к файлу: " << filename << std::endl; // Для отладки

        // Открытие файла
        std::ifstream file(filename);
        if (!file) {
            PRINT_ERROR("Не удалось открыть файл: \"" << filename << "\"");
            return 1;
        }

        PRINT_SUCCESS("Открыт файл: " + filename);

        // Чтение строк из файла
        std::string line;
        while (std::getline(file, line)) {
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            if (line.empty()) continue;

            if (line == "exit") {
                PRINT_INFO("Обнаружена команда exit. Завершение обработки файла.");
                break;
            }

            // Ваши дальнейшие обработки строк...
            std::cout << "Чтение строки: " << line << std::endl;  // Пример вывода содержимого
        }

        break;
    }
    default:
        PRINT_ERROR("Некорректный выбор. Попробуйте снова.");
        goto repeat_choice;  // Возвращаемся к выбору ввода
    }

    return 0;
}

   

