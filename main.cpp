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


// цвета для вывода текста в консоль
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define WHITE   "\033[37m"

// макросы для вывода сообщений
#define PRINT_SUCCESS(msg) std::cout << GREEN << msg << RESET << std::endl
#define PRINT_ERROR(msg)   std::cerr << RED << "Ошибка: " << msg << RESET << std::endl
#define PRINT_INFO(msg)    std::cout << WHITE << msg << RESET << std::endl

// функция для преобразования из UTF-16 (wchar_t) в UTF-8 (std::string) ????????????
// без нее не читает название файла из Проводника
std::string wstring_to_string(const std::wstring& wstr) 
{
    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
    return str;
}

// если бы у меня хватило терпения разбираться с проблемами,  
//которые возникали, когда у меня в шаблонных классах мог быть разный шаблонный параметр,
// то можно было бы использовать VARIANT:
//using MatrixVariant = std::variant<Matrix<int>, Matrix<float>, Matrix<double>>;
//using VectorVariant = std::variant<Vector<int>, Vector<float>, Vector<double>>;
//
//std::map<char, MatrixVariant> matrixes;
//std::map<char, VectorVariant> vectors;


std::map<char, Matrix<int>> matrixes;
std::map<char, Vector<int>> vectors;

int Str_to_Num_Parsing(const std::string& s)
{
    return std::stoi(s);
}

void Vec_Create(const std::string& command, char name, bool isColumn = false) 
{
    std::istringstream iss(command); // это поток ввода из строки
    std::string num;  // переменная для хранения каждого числа пока в виде строки
    std::vector<int> elements;

    while (getline(iss, num, ',')) // ">>" читает из потока iss строку до запятой
                                   // то есть разделяет строку по запятым
    {
        if (!num.empty()) {
            elements.push_back(Str_to_Num_Parsing(num));
        }
    }

    // вектор-столбец представлен как моя матрица Nx1
    if (isColumn) 
    {
        Matrix<int> mat(elements.size(), 1);
        for (size_t i = 0; i < elements.size(); ++i) 
        {
            mat(i, 0) = elements[i];
        }
        matrixes[name] = mat;
    }
    // вектор-строка как мой вектор
    else 
    {
        Vector<int> vec(elements.size());
        for (size_t i = 0; i < elements.size(); ++i)
        {
            vec[i] = elements[i];
        }
        vectors[name] = vec;
    }
}

void Mat_Create(const std::string& data, char name) 
{
    std::vector<std::string> rows;
    std::istringstream rowStream(data);
    std::string rowStr;

    while (getline(rowStream, rowStr, ';')) 
    {
        if (!rowStr.empty()) rows.push_back(rowStr);
    }

    if (rows.empty()) return;

    std::vector<std::string> firstRowElements;
    std::istringstream firstRow(rows[0]);
    std::string num;
    while (getline(firstRow, num, ','))
    {
        if (!num.empty()) firstRowElements.push_back(num);
    }

    Matrix<int> mat(rows.size(), firstRowElements.size());
    for (size_t i = 0; i < rows.size(); ++i) 
    {
        std::istringstream numStream(rows[i]);
        std::string numStr;
        size_t j = 0;

        while (getline(numStream, numStr, ',')) 
        {
            if (!numStr.empty())
            {
                mat(i, j) = Str_to_Num_Parsing(numStr);
                j++;
            }
        }
    }
    matrixes[name] = mat;
}

void Print_Element(char name)
{
    if (matrixes.count(name))
    {
        std::cout << matrixes[name] << "\n";
        return;
    }

    if (vectors.count(name)) 
    {
        std::cout << vectors[name] << "\n";
        return;
    }

    PRINT_ERROR("переменная '" << name << "' не найдена");
}

bool is_valid_letter(char c) 
{
    return std::isalpha(c);
}

bool is_valid_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

void Do_Operation(const std::string& operation)
{
    if (operation.length() != 5 || !is_valid_letter(operation[0]) || !is_valid_operator(operation[1]) ||
        operation[3] != '=' || !is_valid_letter(operation[2]) || !is_valid_letter(operation[4])) 
    {
        PRINT_ERROR("неверный формат операции. Используйте A+B=C");
        return;
    }

    char var1 = operation[0];
    char op = operation[1];
    char var2 = operation[2];
    char result = operation[4];

    try 
    {
        bool var1_is_matrix = matrixes.count(var1);
        bool var1_is_vector = vectors.count(var1);
        bool var2_is_matrix = matrixes.count(var2);
        bool var2_is_vector = vectors.count(var2);

        if (!var1_is_matrix && !var1_is_vector)
        {
            throw std::runtime_error("Переменная " + std::string(1, var1) + " не найдена");
        }
        if (!var2_is_matrix && !var2_is_vector)
        {
            throw std::runtime_error("Переменная " + std::string(1, var2) + " не найдена");
        }

        if (var1_is_matrix && var2_is_matrix) 
        {
            Matrix<int> res = (op == '+') ? (matrixes[var1] + matrixes[var2]) :
                              (op == '-') ? (matrixes[var1] - matrixes[var2]) :
                              (op == '*') ? (matrixes[var1] * matrixes[var2]) :
                              throw std::runtime_error("Операция не поддерживается для матриц");
            matrixes[result] = res;
        }

        else if (var1_is_vector && var2_is_vector)
        {
            Vector<int> res = (op == '+') ? vectors[var1] + vectors[var2] :
                              (op == '-') ? vectors[var1] - vectors[var2] :
                              (op == '*') ? vectors[var1] * vectors[var2] :
                              (op == '/') ? vectors[var1] / vectors[var2] :
                              throw std::runtime_error("Операция не поддерживается для векторов");
            vectors[result] = res;
        }

        else if (var1_is_matrix && var2_is_vector && op == '*')
        {
            Vector<int> res = matrixes[var1] * vectors[var2];
            vectors[result] = res;
        }
        else if (var1_is_vector && var2_is_matrix && op == '*') 
        {
            Vector<int> res = vectors[var1] * matrixes[var2];
            vectors[result] = res;
        }
        else
        {
            throw std::runtime_error("Несовместимые типы для операции");
        }

        PRINT_SUCCESS("Операция выполнена успешно. Результат в переменной " + std::string(1, result));
    }
    catch (const std::exception& e)
    {
        PRINT_ERROR(e.what());
    }
}

void parseData(const std::string& content, char name)
{
    if (content.find(';') != std::string::npos) 
    {
        if (!std::isupper(name))
        {
            PRINT_ERROR("Имя матрицы должно быть заглавной буквой");
            return;
        }
        Mat_Create(content, name);
    }
    else
    {
        if (!std::islower(name))
        {
            PRINT_ERROR("Имя вектора должно быть строчной буквой");
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

repeat_choice:                  // да-да, тут используется goto, но аккуратненько
    std::cout << "-> ";
    std::cin >> f_or_c;

    // Очистка ввода от симвла \n который остается под капотом в потоке после использования cin
    std::cin.ignore(10000, '\n');  

    switch (f_or_c) 
    {
    case '1': 
    {
        PRINT_INFO("--- Выбран ввод через консоль ---\n");

        while (true)
        {
            PRINT_INFO("Введите команду (пример: A = [1,2;3,4], x = [1,2,3], print(A), A+B=C, exit):");
            std::cout << "-> ";

            std::string command;
            std::getline(std::cin, command);
            command.erase(std::remove(command.begin(), command.end(), ' '), command.end());

            if (command == "exit") 
            {
                PRINT_INFO("Выход... Очищаю память и завершаю программу.");
                matrixes.clear();
                vectors.clear();
                return 0;
            }

            if (command.starts_with("print(") && command.ends_with(")"))
            {
                if (command.size() >= 8)
                {
                    char varName = command[6];
                    Print_Element(varName);
                }
                else
                {
                    PRINT_ERROR("Неверный формат команды print()");
                }
                continue;
            }

            if (command.size() == 5 && command[3] == '=' &&
                is_valid_letter(command[0]) && is_valid_operator(command[1]) &&
                is_valid_letter(command[2]) && is_valid_letter(command[4])) 
            {
                Do_Operation(command);
                continue;
            }

            size_t eq = command.find('=');
            size_t lb = command.find('[');
            size_t rb = command.find(']');

            if (eq == std::string::npos || lb == std::string::npos || rb == std::string::npos) 
            {
                PRINT_ERROR("Неверный формат команды");
                continue;
            }

            char name = command[0];

            if (!std::isalpha(name)) 
            {
                PRINT_ERROR("Имя переменной должно быть буквой");
                continue;
            }

            std::string content = command.substr(lb + 1, rb - lb - 1);
            parseData(content, name);
            PRINT_SUCCESS("Успешно создана переменная " + std::string(1, name));
        }
        break;
    }

    case '2':
    {
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

        if (!GetOpenFileNameW(&ofn))
        {
            PRINT_ERROR("Файл не выбран.");
            return 1;
        }

        // перевод wchar_t* -> std::string (с учётом кодировки UTF-8)
        std::wstring wfilename(szFile);
        std::string filename = wstring_to_string(wfilename);

        std::cout << "Путь к файлу: " << filename << std::endl; // для проверки правильности пути

        
        std::ifstream file(filename); // открывает файл
        if (!file)
        {
            PRINT_ERROR("Не удалось открыть файл: \"" << filename << "\"");
            return 1;
        }

        PRINT_SUCCESS("Открыт файл: " + filename);

        
        std::string line;
        while (std::getline(file, line))
        {
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            if (line.empty()) continue;

            
            std::cout << "Чтение строки: " << line << std::endl;  //  вывод содержимого из файла в консоль

            if (line == "exit") 
            {
                PRINT_INFO("Выход... Очищаю память и завершаю программу.");
                matrixes.clear();
                vectors.clear();
                return 0;
                break;
            }

            if (line.starts_with("print(") && line.ends_with(")")) 
            {
                if (line.size() >= 8) {
                    char varName = line[6];
                    Print_Element(varName);
                }
                else {
                    PRINT_ERROR("Неверный формат команды print()");
                }
                continue;
            }

            if (line.size() == 5 && line[3] == '=' &&
                is_valid_letter(line[0]) && is_valid_operator(line[1]) &&
                is_valid_letter(line[2]) && is_valid_letter(line[4])) {
                Do_Operation(line);
                continue;
            }

            size_t eq = line.find('=');
            size_t lb = line.find('[');
            size_t rb = line.find(']');

            if (eq == std::string::npos || lb == std::string::npos || rb == std::string::npos)
            {
                PRINT_ERROR("Неверный формат команды");
                continue;
            }

            char name = line[0];

            if (!std::isalpha(name)) 
            {
                PRINT_ERROR("Имя переменной должно быть буквой");
                continue;
            }

            std::string content = line.substr(lb + 1, rb - lb - 1);
            parseData(content, name);
            PRINT_SUCCESS("Успешно создана переменная " + std::string(1, name));
        }

        break;
    }
    default:
        PRINT_ERROR("Некорректный выбор. Попробуйте снова.");
        goto repeat_choice;  // возвращение к выбору формата ввода команд
    }

    return 0;
}

   

