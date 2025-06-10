#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>
#include <locale>
#include <cctype>
#include <limits>
#include <functional>
#include <algorithm>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define WHITE   "\033[37m"

#define PRINT_SUCCESS(msg) std::cout << GREEN << msg << RESET << std::endl
#define PRINT_ERROR(msg)    std::cerr << RED << "Ошибка: " << msg << RESET << std::endl
#define PRINT_INFO(msg)    std::cout << WHITE << msg << RESET << std::endl

using Matrix = std::vector<std::vector<double>>;
std::map<std::string, Matrix> vars;


std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

Matrix ParseMat(const std::string& s) 
{
    std::string content = trim(s);
    if (content.length() < 2 || content.front() != '[' || content.back() != ']') 
    {
        PRINT_ERROR("Матрица: неверный формат скобок: " + s);
    }
    content = content.substr(1, content.length() - 2);
    content = trim(content);
    if (content.empty()) return Matrix();

    Matrix matrix;
    std::stringstream ss(content);
    std::string row_item;
    while (std::getline(ss, row_item, ';')) 
    {
        matrix.push_back({});
        std::string r_str = trim(row_item);
        if (r_str.empty()) continue;

        std::stringstream val_ss(r_str);
        std::string val_item;
        while (std::getline(val_ss, val_item, ','))
        {
            std::string v_str = trim(val_item);
            if (v_str.empty()) PRINT_ERROR("Матрица: пустое значение в строке.");
            try
            {
                matrix.back().push_back(std::stod(v_str));
            }
            catch (const std::exception&) 
            {
                PRINT_ERROR("Матрица: не могу разобрать число: " + v_str);
            }
        }
    }

    // Проверка на одинаковую длину строк
    if (matrix.size() > 1)
    {
        size_t first_row_size = matrix[0].size();
        for (size_t i = 1; i < matrix.size(); ++i) 
        {
            if (matrix[i].size() != first_row_size) 
            {
                PRINT_ERROR("Матрица: строки разной длины.");
            }
        }
    }

    return matrix;
}

void printMat(const Matrix& m)
{
    if (m.empty())
    {
        std::cout << "[]" << std::endl;
        return;
    }
    std::cout << "["; for (size_t i = 0; i < m.size(); ++i)
    {
        if (i > 0) std::cout << "; ";
        if (m[i].empty()) { }
        else 
        {
            for (size_t j = 0; j < m[i].size(); ++j) 
            {
                std::cout << m[i][j] << (j == m[i].size() - 1 ? "" : ", ");
            }
        }
    }
    std::cout << "]" << std::endl;
}

bool isScalar(const Matrix& m) 
{
    return m.size() == 1 && m[0].size() == 1;
}

void Check_Dims_For_Elementwise(const Matrix& a, const Matrix& b, const std::string& op)
{
    if (a.empty() && b.empty()) return;
    if (a.empty() || b.empty())
    {
        if ((a.empty() && !b.empty()) || (!a.empty() && b.empty()))
        {
            PRINT_ERROR("Операция '" + op + "' на пустой и непустой матрице.");
        }
        return;
    }
    if ((a[0].empty() && !b[0].empty()) || (!a[0].empty() && b[0].empty()))
    {
        PRINT_ERROR("Операция '" + op + "': одна матрица Nx0, другая MxK (K>0).");
    }
    if (a[0].empty() && b[0].empty())
    {
        if (a.size() != b.size())
        {
            PRINT_ERROR("Матрицы Nx0: размеры (строки) не совпадают для '" + op + "'.");
        }
        return;
    }
    if (a.size() != b.size() || a[0].size() != b[0].size())
    {
        PRINT_ERROR("Матрицы: размеры не совпадают для '" + op + "'.");
    }
}


Matrix Add(const Matrix& a, const Matrix& b) 
{
    if (isScalar(a) && !isScalar(b)) 
    {
        if (b.empty() || b[0].empty())
        {
            PRINT_ERROR("Сложение: скаляр с пустой (Mx0) матрицей.");
        }
        double scalar = a[0][0];
        Matrix res = b;

        for (size_t i = 0; i < res.size(); ++i)
            for (size_t j = 0; j < res[0].size(); ++j)
                res[i][j] += scalar;

        return res;
    }
    else if (!isScalar(a) && isScalar(b)) 
    {
        return Add(b, a);
    }
    else
    {
        Check_Dims_For_Elementwise(a, b, "+");
        if (a.empty()) return Matrix();
        if (a[0].empty()) return Matrix(a.size());
        Matrix res = a;

        for (size_t i = 0; i < a.size(); ++i)
            for (size_t j = 0; j < a[0].size(); ++j)
                res[i][j] += b[i][j];

        return res;
    }
}

Matrix Minus(const Matrix& a, const Matrix& b)
{
    if (isScalar(a) && !isScalar(b)) 
    {
        if (b.empty() || b[0].empty()) PRINT_ERROR("Вычитание: скаляр минус пустая (Mx0) матрица.");
        double scalar_a = a[0][0];
        Matrix res(b.size(), std::vector<double>(b[0].size()));
       
        for (size_t i = 0; i < res.size(); ++i)
            for (size_t j = 0; j < res[0].size(); ++j)
                res[i][j] = scalar_a - b[i][j];

        return res;
    }
    else if (!isScalar(a) && isScalar(b)) 
    {
        if (a.empty() || a[0].empty()) PRINT_ERROR("Вычитание: пустая (Mx0) матрица минус скаляр.");
        double scalar_b = b[0][0];
        Matrix res = a;
        
        for (size_t i = 0; i < res.size(); ++i)
            for (size_t j = 0; j < res[0].size(); ++j)
                res[i][j] -= scalar_b;

        return res;
    }
    else 
    {
        Check_Dims_For_Elementwise(a, b, "-");
        if (a.empty()) return Matrix();
        if (a[0].empty()) return Matrix(a.size());
        Matrix res = a;

        for (size_t i = 0; i < a.size(); ++i)
            for (size_t j = 0; j < a[0].size(); ++j)
                res[i][j] -= b[i][j];

        return res;
    }
}

Matrix Multiply(const Matrix& a, const Matrix& b)
{
    if (isScalar(a) && !isScalar(b))
    {
        double scalar = a[0][0]; Matrix res = b;
        for (auto& r : res) for (auto& v : r) v *= scalar;
        return res;
    }
    if (isScalar(b) && !isScalar(a))
    {
        return Multiply(b, a);
    }
    if (a.empty() || b.empty()) 
    {
        if (a.empty() && b.empty()) return Matrix();
        if ((a.size() > 0 && a[0].empty() && b.empty()) || (b.size() > 0 && b[0].empty() && a.empty()))
        {
            return Matrix(a.size(), std::vector<double>());
        }
        PRINT_ERROR("Умножение: одна из матриц пуста некорректно.");
    }

    if (a[0].size() != b.size()) 
    {
        PRINT_ERROR("Умножение: несоответствие размеров матриц (" + std::to_string(a[0].size()) + " != " + std::to_string(b.size()) + ").");
    }

    if (b[0].empty())
    {
        return Matrix(a.size(), std::vector<double>());
    }

    Matrix res(a.size(), std::vector<double>(b[0].size(), 0.0));
   
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = 0; j < b[0].size(); ++j)
            for (size_t k = 0; k < b.size(); ++k)
                res[i][j] += a[i][k] * b[k][j];

    return res;
}

Matrix Compare(const Matrix& a, const Matrix& b, const std::string& op_str) 
{
    auto do_cmp = [&](double v1, double v2)
        {
        if (op_str == "==") return v1 == v2;
        if (op_str == "!=") return v1 != v2;
        if (op_str == "<") return v1 < v2;
        if (op_str == ">") return v1 > v2;
        if (op_str == "<=") return v1 <= v2;
        if (op_str == ">=") return v1 >= v2;
        return false;
        };

    if (isScalar(a) && !isScalar(b))
    {
        if (b.empty() || b[0].empty())
        {
            PRINT_ERROR("Сравнение: скаляр с пустой (Mx0) матрицей.");
        }
        double scalar_a = a[0][0]; Matrix res = b;
        for (size_t i = 0; i < res.size(); ++i) for (size_t j = 0; j < res[0].size(); ++j)
        {
            res[i][j] = do_cmp(scalar_a, b[i][j]) ? 1.0 : 0.0;
        }
        return res;
    }
    else if (!isScalar(a) && isScalar(b))
    {
        std::string inv_op = op_str;
        if (op_str == "<") inv_op = ">";
        else if (op_str == ">") inv_op = "<";
        else if (op_str == "<=") inv_op = ">=";
        else if (op_str == ">=") inv_op = "<=";
        return Compare(b, a, inv_op);
    }

    Check_Dims_For_Elementwise(a, b, op_str);
    if (a.empty()) return Matrix();
    if (a[0].empty()) return Matrix(a.size());

    Matrix res(a.size(), std::vector<double>(a[0].size()));
    for (size_t i = 0; i < a.size(); ++i) 
    {
        for (size_t j = 0; j < a[0].size(); ++j) 
        {
            res[i][j] = do_cmp(a[i][j], b[i][j]) ? 1.0 : 0.0;
        }
    }
    return res;
}

Matrix allOf(const Matrix& m)
{
    if (m.empty() || (!m.empty() && m[0].empty())) return { {1.0} };
    for (const auto& row : m)
    {
        for (double val : row)
        {
            if (val == 0.0) return { {0.0} };
        }
    }
    return { {1.0} };
}

Matrix anyOf(const Matrix& m) 
{
    if (m.empty() || (!m.empty() && m[0].empty())) return { {0.0} };
    for (const auto& row : m)
    {
        for (double val : row)
        {
            if (val != 0.0) return { {1.0} };
        }
    }
    return { {0.0} };
}

enum class TokType { VAR, MAT, OP, FN, LPAR, RPAR, END, IF, ELSE, LBRACE, RBRACE };
struct Tok { TokType type; std::string val; Tok(TokType t, std::string v = "") : type(t), val(std::move(v)) {} };

class Lexer
{
public:
    Lexer(const std::string& txt) : text(txt), pos(0) {}
    std::vector<Tok> tokenize()
    {
        std::vector<Tok> tokens;
        while (pos < text.length())
        {
            char c = text[pos];
            if (std::isspace(c)) { pos++; continue; }
            if (c == '[') { tokens.push_back(scanMatLit()); continue; }
            if (std::isalpha(c))
            {
                std::string id = scanId();
                if (id == "all_of" || id == "any_of") tokens.push_back(Tok(TokType::FN, id));
                else if (id == "if") tokens.push_back(Tok(TokType::IF, id));
                else if (id == "else") tokens.push_back(Tok(TokType::ELSE, id));
                else if (id == "print") tokens.push_back(Tok(TokType::VAR, id));
                else tokens.push_back(Tok(TokType::VAR, id));
                continue;
            }
            if (c == '(') { tokens.push_back(Tok(TokType::LPAR, "(")); pos++; continue; }
            if (c == ')') { tokens.push_back(Tok(TokType::RPAR, ")")); pos++; continue; }
            if (c == '{') { tokens.push_back(Tok(TokType::LBRACE, "{")); pos++; continue; }
            if (c == '}') { tokens.push_back(Tok(TokType::RBRACE, "}")); pos++; continue; }

            std::string op2 = (pos + 1 < text.length()) ? text.substr(pos, 2) : "";
            if (op2 == "==" || op2 == "!=" || op2 == "<=" || op2 == ">=")
            {
                tokens.push_back(Tok(TokType::OP, op2)); pos += 2; continue;
            }
            std::string op1(1, c);
            if (op1 == "*" || op1 == "<" || op1 == ">" || op1 == "+" || op1 == "-") 
            {
                tokens.push_back(Tok(TokType::OP, op1)); pos++; continue;
            }
            if (!(std::isspace(c) || c == '[' || std::isalpha(c) || c == '(' || c == ')' || c == '{' || c == '}' 
                                  || (op2 == "==" || op2 == "!=" || op2 == "<=" || op2 == ">=") || (op1 == "*" 
                                  || op1 == "<" || op1 == ">" || op1 == "+" || op1 == "-")))
            {
                PRINT_ERROR("Неизвестный символ: " + std::string(1, c)); pos++; continue;
            }

        }
        return tokens;
    }
private:
    std::string text; size_t pos;
    Tok scanMatLit()
    {
        size_t start = pos; int lvl = 0;
        do
        {
            if (pos >= text.length()) break;
            if (text[pos] == '[') lvl++;
            else if (text[pos] == ']') lvl--;
            pos++;
        } while (lvl > 0);
        if (lvl != 0) PRINT_ERROR("Незакрытая скобка в матричном литерале.");
        return Tok(TokType::MAT, text.substr(start, pos - start));
    }
    std::string scanId()
    {
        size_t start = pos;
        while (pos < text.length() && (std::isalnum(text[pos]) || text[pos] == '_')) pos++;
        return text.substr(start, pos - start);
    }
};

class Parser
{
public:
    Parser(const std::vector<Tok>& toks) : tokens(toks), curIdx(0)
    {
        if (tokens.empty() || tokens.back().type != TokType::END) 
        {
            this->tokens.push_back(Tok(TokType::END));
        }
    }
    Matrix parseExpr() { Matrix res = parseComp(); return res; }
private:
    std::vector<Tok> tokens; size_t curIdx;
    const Tok& peek() const 
    {
        if (curIdx >= tokens.size()) PRINT_ERROR("Неожиданный конец выражения.");
        return tokens[curIdx];
    }
    Tok consume(TokType expect = TokType::END) 
    {
        const Tok& t = peek();
        if (expect != TokType::END && t.type != expect)
        {
            PRINT_ERROR("Ошибка парсера: Ожидался другой тип токена (получен '" + t.val + "').");
        }
        curIdx++; return t;
    }
    Matrix parseFactor()
    {
        Tok t = peek();
        if (t.type == TokType::MAT) { consume(); return ParseMat(t.val); }
        if (t.type == TokType::VAR) 
        {
            if (t.val == "print")
            {
                PRINT_ERROR("Неожиданное использование 'print' в выражении.");
            }
            consume();
            if (vars.find(t.val) == vars.end())
            {
                PRINT_ERROR("Неизвестная переменная: " + t.val);
            }
            return vars[t.val];
        }
        if (t.type == TokType::FN)
        {
            consume(); std::string fname = t.val; consume(TokType::LPAR); Matrix arg = parseComp(); consume(TokType::RPAR);
            if (fname == "all_of") 
            {
                PRINT_SUCCESS("Функция 'all_of' успешно выполнена.");
                return allOf(arg);
            }
            if (fname == "any_of") 
            {
                PRINT_SUCCESS("Функция 'any_of' успешно выполнена.");
                return anyOf(arg);
            }
            PRINT_ERROR("Неизвестная функция: " + fname);
        }
        if (t.type == TokType::LPAR) { consume(); Matrix res = parseComp(); consume(TokType::RPAR); return res; }
        PRINT_ERROR("Неожиданный токен в: " + t.val);
    }
    Matrix parseMul()
    {
        Matrix L = parseFactor();
        while (peek().type == TokType::OP && peek().val == "*") { consume(); Matrix R = parseFactor(); L = Multiply(L, R); }
        return L;
    }
    Matrix parseAdd() 
    {
        Matrix L = parseMul();
        while (peek().type == TokType::OP && (peek().val == "+" || peek().val == "-")) 
        {
            Tok op = consume(); Matrix R = parseMul();
            if (op.val == "+") L = Add(L, R); else L = Minus(L, R);
        }
        return L;
    }
    Matrix parseComp() 
    {
        Matrix L = parseAdd();
        while (peek().type == TokType::OP && (peek().val == "==" || peek().val == "!=" || 
               peek().val == "<" || peek().val == ">" || peek().val == "<=" || peek().val == ">="))
        {
            Tok op = consume(); Matrix R = parseAdd(); L = Compare(L, R, op.val);
        }
        return L;
    }
};


// ======================================================================
//       НОВЫЙ ИНТЕРПРЕТАТОР 
// ======================================================================

struct ScriptState
{
    const std::string& script;
    size_t pos = 0;
    bool isInteractive;

    ScriptState(const std::string& s, bool interactive) : script(s), isInteractive(interactive) {}

    void Skip_White_Space() 
    {
        pos = script.find_first_not_of(" \t\n\r", pos);
        if (pos == std::string::npos) pos = script.length();
    }

    bool eof()
    {
        Skip_White_Space();
        return pos >= script.length();
    }

    std::string peekWord() 
    {
        Skip_White_Space();
        if (eof()) return "";
        size_t end_pos = script.find_first_of(" \t\n\r(){}=;", pos);
        if (end_pos == std::string::npos) return script.substr(pos);
        if (end_pos == pos) 
        {
            return script.substr(pos, 1);
        }
        return script.substr(pos, end_pos - pos);
    }
};

void Execute_Statement(ScriptState& state);

std::string Get_block_content(ScriptState& state)
{
    state.Skip_White_Space();
    if (state.pos >= state.script.length() || state.script[state.pos] != '{')
    {
        PRINT_ERROR("Ожидался символ '{' для начала блока.");
    }
    state.pos++;

    int brace_level = 1;
    size_t block_start = state.pos;

    while (state.pos < state.script.length() && brace_level > 0)
    {
        if (state.script[state.pos] == '{') brace_level++;
        if (state.script[state.pos] == '}') brace_level--;
        state.pos++;
    }

    if (brace_level > 0)
    {
        PRINT_ERROR("Незакрытая скобка '{' в блоке.");
    }

    return state.script.substr(block_start, state.pos - block_start - 1);
}

void Execute_Script(const std::string& script_text, bool isInteractive)
{
    ScriptState state(script_text, isInteractive);
    while (!state.eof()) {
        Execute_Statement(state);
    }
}

void Execute_Statement(ScriptState& state)
{
    state.Skip_White_Space();
    if (state.eof()) return;

    std::string word = state.peekWord();

    if (word == "if")
    {
        state.pos += 2;
        state.Skip_White_Space();
        if (state.script[state.pos] != '(') PRINT_ERROR("Ожидалась '(' после if.");
        state.pos++;

        int paren_level = 1; size_t cond_start = state.pos;
        while (state.pos < state.script.length() && paren_level > 0)
        {
            if (state.script[state.pos] == '(') paren_level++;
            if (state.script[state.pos] == ')') paren_level--;
            state.pos++;
        }
        if (paren_level > 0) PRINT_ERROR("Незакрытая скобка '(' в условии if.");
        std::string cond_str = state.script.substr(cond_start, state.pos - cond_start - 1);

        Lexer lex(cond_str); Parser p(lex.tokenize()); Matrix res = p.parseExpr();
        bool is_true = (!res.empty() && !res[0].empty() && res[0][0] != 0.0);

        std::string if_block = Get_block_content(state);

        state.Skip_White_Space();
        std::string else_block;
        bool has_else = false;
        if (!state.eof() && state.peekWord() == "else")
        {
            state.pos += 4;
            has_else = true;
            else_block = Get_block_content(state);
        }

        if (is_true) 
        {
            PRINT_SUCCESS("Условие 'if' истинно. Выполнение блока 'if'.");
            Execute_Script(if_block, state.isInteractive);
        }
        else if (has_else)
        {
            PRINT_SUCCESS("Условие 'if' ложно. Выполнение блока 'else'.");
            Execute_Script(else_block, state.isInteractive);
        }
        else 
        {
            PRINT_INFO("Условие 'if' ложно. Блок 'else' отсутствует.");
        }
        return;
    }

    size_t end_of_line = state.script.find_first_of("\n", state.pos);
    if (end_of_line == std::string::npos) end_of_line = state.script.length();

    std::string line = state.script.substr(state.pos, end_of_line - state.pos);
    state.pos = end_of_line;

    std::string cmd = trim(line);
    if (cmd.empty()) return;

    if (cmd.rfind("print", 0) == 0 && (cmd.length() == 5 || std::isspace(cmd[5])))
    {
        std::string arg = trim(cmd.substr(5));
        if (vars.count(arg))
        {
            printMat(vars.at(arg));
            PRINT_SUCCESS("Переменная '" + arg + "' успешно выведена.");
            return;
        }
        try
        {
            double num_val = std::stod(arg);
            std::cout << arg << std::endl;
            PRINT_SUCCESS("Число '" + arg + "' успешно выведено.");
            return;
        }
        catch (const std::invalid_argument&)
        {
        }
        catch (const std::out_of_range&)
        {
        }
        try
        {
            Lexer lex(arg); Parser p(lex.tokenize());
            printMat(p.parseExpr());
            PRINT_SUCCESS("Выражение успешно вычислено и выведено.");
        }
        catch (const std::exception& e)
        {
            PRINT_ERROR("Ошибка print: неверное выражение или неизвестная переменная '" + arg + "'");
        }
    }
    else
    {
        size_t eq_pos = cmd.find("=");
        size_t cmp_pos = cmd.find("==");
        if (eq_pos != std::string::npos && (cmp_pos == std::string::npos || eq_pos != cmp_pos))
        {
            std::string dest = trim(cmd.substr(0, eq_pos));
            std::string expr = trim(cmd.substr(eq_pos + 1));
            if (dest.empty() || expr.empty()) PRINT_ERROR("Ошибка присваивания.");
            if (std::isdigit(dest[0]) || dest.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos)
            {
                PRINT_ERROR("Недопустимое имя переменной: " + dest);
            }
            Lexer lex(expr); Parser p(lex.tokenize()); vars[dest] = p.parseExpr();
            PRINT_SUCCESS("Переменная '" + dest + "' успешно создана/обновлена.");
        }
        else
        {
            if (state.isInteractive)
            {
                Lexer lex(cmd); Parser p(lex.tokenize());
                Matrix result = p.parseExpr();
                printMat(result);
                PRINT_SUCCESS("Выражение успешно вычислено.");
            }
            else 
            {
                // В файловом режиме простое выражение без присваивания - ошибка
                PRINT_ERROR("Неизвестная команда или выражение без присваивания: " + cmd);
            }
        }
    }
}


int main(int argc, char* argv[]) 
{
    std::setlocale(LC_ALL, "Russian");

    std::cout << "Введите команды:" << std::endl;

    if (argc > 1)
    {
        std::string filename = argv[1];
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Ошибка: Не удалось открыть файл " << filename << std::endl; return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::cout << "\n--- Выполнение из файла: " << filename << " ---" << std::endl;
        try 
        {
            Execute_Script(buffer.str(), false);
        }
        catch (const std::runtime_error& e) 
        {
            std::cerr << "Ошибка выполнения: " << e.what() << std::endl; return 1;
        }
        std::cout << "--- Выполнение из файла завершено ---" << std::endl;
    }
    else
    {
        

        std::string command_buffer;

        while (true)
        {
            // Если буфер пуст, это начало нового ввода
            //if (command_buffer.empty()) {
            std::cout << ">> ";
            // }
            // else {
                 // std::cout << ".. ";
            // }

            std::string line_input;
            if (!std::getline(std::cin, line_input)) break;

            // Выход по команде, только если буфер пуст
            std::string trimmed_input = trim(line_input);
            if (command_buffer.empty() && (trimmed_input == "exit" || trimmed_input == "quit")) 
            {
                break;
            }

            // Если введена пустая строка и буфер НЕ пуст - выполняем
            if (trimmed_input.empty() && !command_buffer.empty())
            {
                try
                {
                    Execute_Script(command_buffer, true);
                }
                catch (const std::runtime_error& e) 
                {
                    std::cerr << "Ошибка: " << e.what() << std::endl;
                }
                // Очищаем буфер для следующего блока
                command_buffer.clear();
            }
            else
            {
                // Иначе просто добавляем строку в буфер
                command_buffer += line_input + "\n";
            }
        }
        std::cout << "--- Завершение работы ---" << std::endl;
    }
    return 0;
}