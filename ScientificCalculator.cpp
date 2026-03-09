/*
 * ═══════════════════════════════════════════════════════════════════
 *  STONE SCIENTIFIC CALCULATOR
 *  Compile with MSYS2/MinGW:
 *    g++ -o ScientificCalculator.exe ScientificCalculator.cpp \
 *        -lgdi32 -luser32 -lcomctl32 -mwindows -std=c++17 -O2
 * ═══════════════════════════════════════════════════════════════════
 */

#define UNICODE
#define _UNICODE
#define _USE_MATH_DEFINES

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <math.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <functional>
#include <stack>
#include <stdexcept>
#include <algorithm>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")

// ─── Color Palette ───────────────────────────────────────────────────────────
#define COL_BG          RGB(13,  14,  22)
#define COL_PANEL       RGB(18,  20,  34)
#define COL_DISPLAY_BG  RGB(8,   9,   16)
#define COL_GRID        RGB(24,  27,  46)
#define COL_ACCENT      RGB(0,   210, 180)
#define COL_ACCENT2     RGB(130, 90,  255)
#define COL_BTN_NUM     RGB(28,  32,  52)
#define COL_BTN_OP      RGB(38,  42,  72)
#define COL_BTN_SCI     RGB(20,  24,  42)
#define COL_BTN_SPECIAL RGB(0,   160, 130)
#define COL_BTN_EQUAL   RGB(0,   190, 160)
#define COL_BTN_CLEAR   RGB(190, 45,  65)
#define COL_TEXT_MAIN   RGB(235, 238, 255)
#define COL_TEXT_DIM    RGB(110, 120, 160)
#define COL_TEXT_ACCENT RGB(0,   225, 195)
#define COL_TEXT_DARK   RGB(8,   10,  18)
#define COL_BTN_HOVER   RGB(50,  55,  90)
#define COL_BORDER      RGB(40,  46,  78)


#define WIN_W    420
#define WIN_H    690
#define DISP_H   130
#define BTN_ROWS 9
#define BTN_COLS 6
#define BTN_M    4
#define BTN_X0   6
#define BTN_Y0   (DISP_H + 6)
#define BTN_W    ((WIN_W - BTN_X0*2 - BTN_M*(BTN_COLS-1)) / BTN_COLS)
#define BTN_H    ((WIN_H - BTN_Y0 - 6 - BTN_M*(BTN_ROWS-1)) / BTN_ROWS)

// ─── IDs ─────────────────────────────────────────────────────────────────────
#define ID_BTN_BASE 1000


struct CalcState {
    std::wstring input;
    std::wstring expression;
    double memory = 0.0;
    double lastResult = 0.0;
    bool justEvaluated = false;
    bool isError = false;
    bool isDeg = true; // degrees vs radians
    bool isInverse = false;
    bool is2nd = false;
};

CalcState g_calc;
HWND g_hwnd = NULL;
HFONT g_fontDisplay = NULL;
HFONT g_fontExpr = NULL;
HFONT g_fontBtn = NULL;
HFONT g_fontBtnSm = NULL;
HFONT g_fontTitle = NULL;


struct Button {
    int col, row;
    int colSpan;
    std::wstring label;
    std::wstring label2nd;
    COLORREF color;
    COLORREF textColor;
    std::wstring action;
    std::wstring action2nd;
    bool isSmallText;
};

std::vector<Button> g_buttons = {
    
    {0,0,1, L"DEG",  L"RAD",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"DEG",   L"RAD",   false},
    {1,0,1, L"2nd",  L"2nd",   COL_BTN_SCI,   RGB(180,130,255), L"2nd",   L"2nd",   false},
    {2,0,1, L"MC",   L"MC",    COL_BTN_SCI,   COL_TEXT_DIM,     L"MC",    L"MC",    false},
    {3,0,1, L"MR",   L"MR",    COL_BTN_SCI,   COL_TEXT_DIM,     L"MR",    L"MR",    false},
    {4,0,1, L"M+",   L"M-",    COL_BTN_SCI,   COL_TEXT_DIM,     L"M+",    L"M-",    false},
    {5,0,1, L"MS",   L"MS",    COL_BTN_SCI,   COL_TEXT_DIM,     L"MS",    L"MS",    false},

    
    {0,1,1, L"sin",  L"asin",  COL_BTN_SCI,   COL_TEXT_ACCENT,  L"sin",   L"asin",  false},
    {1,1,1, L"cos",  L"acos",  COL_BTN_SCI,   COL_TEXT_ACCENT,  L"cos",   L"acos",  false},
    {2,1,1, L"tan",  L"atan",  COL_BTN_SCI,   COL_TEXT_ACCENT,  L"tan",   L"atan",  false},
    {3,1,1, L"sinh", L"asinh", COL_BTN_SCI,   COL_TEXT_ACCENT,  L"sinh",  L"asinh", false},
    {4,1,1, L"cosh", L"acosh", COL_BTN_SCI,   COL_TEXT_ACCENT,  L"cosh",  L"acosh", false},
    {5,1,1, L"tanh", L"atanh", COL_BTN_SCI,   COL_TEXT_ACCENT,  L"tanh",  L"atanh", false},


    {0,2,1, L"x²",   L"√x",    COL_BTN_SCI,   COL_TEXT_ACCENT,  L"x2",    L"sqrt",  false},
    {1,2,1, L"x³",   L"∛x",    COL_BTN_SCI,   COL_TEXT_ACCENT,  L"x3",    L"cbrt",  false},
    {2,2,1, L"√x",   L"x²",    COL_BTN_SCI,   COL_TEXT_ACCENT,  L"sqrt",  L"x2",    false},
    {3,2,1, L"∛x",   L"x³",    COL_BTN_SCI,   COL_TEXT_ACCENT,  L"cbrt",  L"x3",    false},
    {4,2,1, L"xʸ",   L"ʸ√x",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"pow",   L"ypow",  false},
    {5,2,1, L"1/x",  L"|x|",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"inv",   L"abs",   false},

    
    {0,3,1, L"log",  L"10^x",  COL_BTN_SCI,   COL_TEXT_ACCENT,  L"log",   L"10^x",  false},
    {1,3,1, L"ln",   L"e^x",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"ln",    L"e^x",   false},
    {2,3,1, L"log₂", L"2^x",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"log2",  L"2^x",   false},
    {3,3,1, L"n!",   L"nPr",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"fact",  L"nPr",   false},
    {4,3,1, L"nCr",  L"nPr",   COL_BTN_SCI,   COL_TEXT_ACCENT,  L"nCr",   L"nPr",   false},
    {5,3,1, L"π",    L"e",     COL_BTN_SCI,   RGB(200,180,255), L"pi",    L"euler", false},

    
    {0,4,1, L"floor",L"ceil",  COL_BTN_SCI,   COL_TEXT_DIM,     L"floor", L"ceil",  true},
    {1,4,1, L"abs",  L"sign",  COL_BTN_SCI,   COL_TEXT_DIM,     L"abs",   L"sign",  false},
    {2,4,1, L"(",    L")",     COL_BTN_OP,    COL_TEXT_MAIN,    L"(",     L")",     false},
    {3,4,1, L")",    L"(",     COL_BTN_OP,    COL_TEXT_MAIN,    L")",     L"(",     false},
    {4,4,1, L"⌫",    L"⌫",     COL_BTN_OP,    RGB(255,160,80),  L"back",  L"back",  false},
    {5,4,1, L"AC",   L"AC",    COL_BTN_CLEAR, RGB(255,220,220), L"clear", L"clear", false},

    
    {0,5,1, L"7",    L"7",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"7",     L"7",     false},
    {1,5,1, L"8",    L"8",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"8",     L"8",     false},
    {2,5,1, L"9",    L"9",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"9",     L"9",     false},
    {3,5,1, L"÷",    L"÷",     COL_BTN_OP,    COL_TEXT_ACCENT,  L"/",     L"/",     false},
    {4,5,1, L"%",    L"mod",   COL_BTN_OP,    COL_TEXT_ACCENT,  L"%",     L"mod",   false},
    {5,5,1, L"EE",   L"EE",    COL_BTN_OP,    COL_TEXT_DIM,     L"EE",    L"EE",    false},

    
    {0,6,1, L"4",    L"4",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"4",     L"4",     false},
    {1,6,1, L"5",    L"5",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"5",     L"5",     false},
    {2,6,1, L"6",    L"6",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"6",     L"6",     false},
    {3,6,1, L"×",    L"×",     COL_BTN_OP,    COL_TEXT_ACCENT,  L"*",     L"*",     false},
    {4,6,1, L"^",    L"^",     COL_BTN_OP,    COL_TEXT_ACCENT,  L"^",     L"^",     false},
    {5,6,1, L"Rnd",  L"Rnd",   COL_BTN_SCI,   COL_TEXT_DIM,     L"rand",  L"rand",  false},

    
    {0,7,1, L"1",    L"1",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"1",     L"1",     false},
    {1,7,1, L"2",    L"2",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"2",     L"2",     false},
    {2,7,1, L"3",    L"3",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"3",     L"3",     false},
    {3,7,1, L"−",    L"−",     COL_BTN_OP,    COL_TEXT_ACCENT,  L"-",     L"-",     false},
    {4,7,1, L"+",    L"+",     COL_BTN_OP,    COL_TEXT_ACCENT,  L"+",     L"+",     false},
    {5,7,2, L"=",    L"=",     COL_BTN_EQUAL, COL_TEXT_DARK,    L"=",     L"=",     false},

    
    {0,8,1, L"+/−",  L"+/−",   COL_BTN_NUM,   COL_TEXT_DIM,     L"neg",   L"neg",   false},
    {1,8,1, L"0",    L"0",     COL_BTN_NUM,   COL_TEXT_MAIN,    L"0",     L"0",     false},
    {2,8,1, L".",    L".",     COL_BTN_NUM,   COL_TEXT_MAIN,     L".",     L".",     false},
    {3,8,1, L"GCD",  L"LCM",   COL_BTN_SCI,   COL_TEXT_DIM,     L"GCD",   L"LCM",   false},
    {4,8,1, L"ans",  L"ans",   COL_BTN_SCI,   COL_TEXT_DIM,     L"ans",   L"ans",   false},
    
};


int g_hoverBtn = -1;


std::wstring formatNumber(double v) {
    if (std::isinf(v)) return v > 0 ? L"∞" : L"-∞";
    if (std::isnan(v)) return L"Error";

    
    std::wostringstream oss;
    double absV = std::abs(v);
    if (absV != 0 && (absV >= 1e12 || absV < 1e-9)) {
        oss << std::scientific << std::setprecision(8) << v;
    } else {
        oss << std::setprecision(12) << v;
        std::wstring s = oss.str();
        // Remove trailing zeros after decimal
        if (s.find(L'.') != std::wstring::npos) {
            size_t last = s.find_last_not_of(L'0');
            if (last != std::wstring::npos && s[last] == L'.')
                s = s.substr(0, last);
            else if (last != std::wstring::npos)
                s = s.substr(0, last + 1);
        }
        return s;
    }
    return oss.str();
}


double toRad(double deg) { return deg * M_PI / 180.0; }
double toDeg(double rad) { return rad * 180.0 / M_PI; }

double factorial(double n) {
    if (n < 0 || n != std::floor(n)) throw std::runtime_error("Domain");
    if (n > 170) throw std::runtime_error("Overflow");
    double r = 1;
    for (int i = 2; i <= (int)n; i++) r *= i;
    return r;
}


struct Token { int type; double val; wchar_t op; };


std::vector<Token> tokenize(const std::wstring& expr) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < expr.size()) {
        wchar_t c = expr[i];
        if (iswspace(c)) { i++; continue; }
        if (c == L'-' && (tokens.empty() || tokens.back().type == 1 || tokens.back().type == 2)) {
            
            i++;
            std::wstring num;
            while (i < expr.size() && (iswdigit(expr[i]) || expr[i] == L'.' || expr[i] == L'e' || expr[i] == L'E'
                || ((expr[i] == L'+' || expr[i] == L'-') && i > 0 && (expr[i-1] == L'e' || expr[i-1] == L'E')))) {
                num += expr[i++];
            }
            if (num.empty()) num = L"0";
            tokens.push_back({0, -std::stod(num), 0});
        } else if (iswdigit(c) || c == L'.') {
            std::wstring num;
            while (i < expr.size() && (iswdigit(expr[i]) || expr[i] == L'.' || expr[i] == L'e' || expr[i] == L'E'
                || ((expr[i] == L'+' || expr[i] == L'-') && i > 0 && (expr[i-1] == L'e' || expr[i-1] == L'E')))) {
                num += expr[i++];
            }
            tokens.push_back({0, std::stod(num), 0});
        } else if (c == L'(' ) { tokens.push_back({2, 0, c}); i++; }
        else if (c == L')' ) { tokens.push_back({3, 0, c}); i++; }
        else if (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'^' || c == L'%') {
            tokens.push_back({1, 0, c}); i++;
        } else { i++; } 
    }
    return tokens;
}

int precedence(wchar_t op) {
    if (op == L'+' || op == L'-') return 1;
    if (op == L'*' || op == L'/' || op == L'%') return 2;
    if (op == L'^') return 3;
    return 0;
}

double applyOp(double a, double b, wchar_t op) {
    switch(op) {
        case L'+': return a + b;
        case L'-': return a - b;
        case L'*': return a * b;
        case L'/': if (b == 0) throw std::runtime_error("Div by zero"); return a / b;
        case L'%': if (b == 0) throw std::runtime_error("Div by zero"); return std::fmod(a, b);
        case L'^': return std::pow(a, b);
    }
    return 0;
}

double evaluate(const std::wstring& expr) {
    auto tokens = tokenize(expr);
    std::stack<double> vals;
    std::stack<wchar_t> ops;

    auto process = [&]() {
        if (vals.size() < 2 || ops.empty()) throw std::runtime_error("Syntax");
        double b = vals.top(); vals.pop();
        double a = vals.top(); vals.pop();
        wchar_t op = ops.top(); ops.pop();
        vals.push(applyOp(a, b, op));
    };

    for (auto& t : tokens) {
        if (t.type == 0) {
            vals.push(t.val);
        } else if (t.type == 2) {
            ops.push(L'(');
        } else if (t.type == 3) {
            while (!ops.empty() && ops.top() != L'(') process();
            if (!ops.empty()) ops.pop();
        } else if (t.type == 1) {
            while (!ops.empty() && ops.top() != L'(' && precedence(ops.top()) >= precedence(t.op)
                   && t.op != L'^') process();
            ops.push(t.op);
        }
    }
    while (!ops.empty()) process();
    if (vals.empty()) throw std::runtime_error("Empty");
    return vals.top();
}


void processAction(const std::wstring& action) {
    CalcState& c = g_calc;
    bool deg = c.isDeg;

    
    if (action == L"DEG") { c.isDeg = true; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"RAD") { c.isDeg = false; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"2nd") { c.is2nd = !c.is2nd; InvalidateRect(g_hwnd, NULL, FALSE); return; }


    bool startsNew = c.justEvaluated && (
        action == L"0"||action==L"1"||action==L"2"||action==L"3"||action==L"4"||
        action == L"5"||action==L"6"||action==L"7"||action==L"8"||action==L"9"||
        action == L"."
    );
    if (startsNew) { c.input = L""; c.expression = L""; }
    c.justEvaluated = false;
    c.isError = false;

    
    if (action == L"MC") { c.memory = 0; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"MR") {
        c.input = formatNumber(c.memory);
        c.justEvaluated = true;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L"M+") {
        double v = 0;
        try { v = c.input.empty() ? c.lastResult : evaluate(c.input); } catch(...) {}
        c.memory += v; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L"M-") {
        double v = 0;
        try { v = c.input.empty() ? c.lastResult : evaluate(c.input); } catch(...) {}
        c.memory -= v; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action == L"clear") {
        c.input = L""; c.expression = L""; c.isError = false;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L"back") {
        if (!c.input.empty()) c.input.pop_back();
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    
    if (action==L"0"||action==L"1"||action==L"2"||action==L"3"||action==L"4"||
        action==L"5"||action==L"6"||action==L"7"||action==L"8"||action==L"9") {
        c.input += action; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L".") {
        
        size_t lastOp = c.input.find_last_of(L"+-*/%^(");
        std::wstring lastNum = (lastOp == std::wstring::npos) ? c.input : c.input.substr(lastOp+1);
        if (lastNum.find(L'.') == std::wstring::npos) c.input += L".";
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action==L"+"||action==L"-"||action==L"*"||action==L"/"||action==L"%"||action==L"^") {
        if (c.input.empty() && action != L"-") { InvalidateRect(g_hwnd, NULL, FALSE); return; }
        c.input += action; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L"(") { c.input += L"("; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L")") { c.input += L")"; InvalidateRect(g_hwnd, NULL, FALSE); return; }

    if (action == L"EE") { c.input += L"e"; InvalidateRect(g_hwnd, NULL, FALSE); return; }

    if (action == L"neg") {
        if (!c.input.empty() && c.input[0] == L'-') c.input = c.input.substr(1);
        else c.input = L"-" + c.input;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action == L"pi")    { c.input += std::to_wstring(M_PI); InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"euler") { c.input += std::to_wstring(M_E);  InvalidateRect(g_hwnd, NULL, FALSE); return; }

    
    auto applyFn = [&](std::function<double(double)> fn, const std::wstring& label) {
        double v = 0;
        bool ok = true;
        try {
            if (c.input.empty()) v = c.lastResult;
            else v = evaluate(c.input);
        } catch(...) { ok = false; }
        if (!ok) { c.input = L"Error"; c.isError = true; InvalidateRect(g_hwnd, NULL, FALSE); return; }
        try {
            double res = fn(v);
            c.expression = label + L"(" + formatNumber(v) + L")";
            c.input = formatNumber(res);
            c.lastResult = res;
            c.justEvaluated = true;
        } catch(std::exception& e) {
            c.input = L"Error"; c.isError = true;
        }
        InvalidateRect(g_hwnd, NULL, FALSE);
    };

    if (action == L"sin")  { applyFn([&](double x){ return std::sin(deg ? toRad(x) : x); },  L"sin"); return; }
    if (action == L"cos")  { applyFn([&](double x){ return std::cos(deg ? toRad(x) : x); },  L"cos"); return; }
    if (action == L"tan")  { applyFn([&](double x){
        double r = deg ? toRad(x) : x;
        if (std::abs(std::cos(r)) < 1e-15) throw std::runtime_error("Undef");
        return std::tan(r); }, L"tan"); return; }
    if (action == L"asin") { applyFn([&](double x){
        if(x<-1||x>1) throw std::runtime_error("Domain");
        double r = std::asin(x); return deg ? toDeg(r) : r; }, L"asin"); return; }
    if (action == L"acos") { applyFn([&](double x){
        if(x<-1||x>1) throw std::runtime_error("Domain");
        double r = std::acos(x); return deg ? toDeg(r) : r; }, L"acos"); return; }
    if (action == L"atan") { applyFn([&](double x){
        double r = std::atan(x); return deg ? toDeg(r) : r; }, L"atan"); return; }

    
    if (action == L"sinh")  { applyFn([](double x){ return std::sinh(x); },  L"sinh");  return; }
    if (action == L"cosh")  { applyFn([](double x){ return std::cosh(x); },  L"cosh");  return; }
    if (action == L"tanh")  { applyFn([](double x){ return std::tanh(x); },  L"tanh");  return; }
    if (action == L"asinh") { applyFn([](double x){ return std::asinh(x); }, L"asinh"); return; }
    if (action == L"acosh") { applyFn([](double x){
        if(x<1) throw std::runtime_error("Domain"); return std::acosh(x); }, L"acosh"); return; }
    if (action == L"atanh") { applyFn([](double x){
        if(x<=-1||x>=1) throw std::runtime_error("Domain"); return std::atanh(x); }, L"atanh"); return; }

    if (action == L"log")   { applyFn([](double x){ if(x<=0) throw std::runtime_error("Domain"); return std::log10(x); }, L"log");  return; }
    if (action == L"ln")    { applyFn([](double x){ if(x<=0) throw std::runtime_error("Domain"); return std::log(x); },   L"ln");   return; }
    if (action == L"log2")  { applyFn([](double x){ if(x<=0) throw std::runtime_error("Domain"); return std::log2(x); },  L"log₂"); return; }
    if (action == L"10^x")  { applyFn([](double x){ return std::pow(10.0, x); }, L"10^"); return; }
    if (action == L"e^x")   { applyFn([](double x){ return std::exp(x); },        L"e^");  return; }
    if (action == L"2^x")   { applyFn([](double x){ return std::pow(2.0, x); },   L"2^");  return; }

    if (action == L"x2")    { applyFn([](double x){ return x*x; },   L"x²"); return; }
    if (action == L"x3")    { applyFn([](double x){ return x*x*x; }, L"x³"); return; }
    if (action == L"sqrt")  { applyFn([](double x){ if(x<0) throw std::runtime_error("Domain"); return std::sqrt(x); }, L"√");  return; }
    if (action == L"cbrt")  { applyFn([](double x){ return std::cbrt(x); }, L"∛"); return; }
    if (action == L"inv")   { applyFn([](double x){ if(x==0) throw std::runtime_error("Div"); return 1.0/x; }, L"1/"); return; }
    if (action == L"abs")   { applyFn([](double x){ return std::abs(x); },   L"|x|");  return; }
    if (action == L"sign")  { applyFn([](double x){ return (x>0)-(x<0) ? (double)((x>0)-(x<0)) : 0.0; }, L"sign"); return; }
    if (action == L"floor") { applyFn([](double x){ return std::floor(x); }, L"floor"); return; }
    if (action == L"ceil")  { applyFn([](double x){ return std::ceil(x); },  L"ceil");  return; }
    if (action == L"fact")  { applyFn([](double x){ return factorial(x); },  L"n!");    return; }

    if (action == L"nCr") {
    
        c.input += L"C"; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L"pow")  { c.input += L"^";    InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"ypow") { c.input += L"^(1/"; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"nPr")  { c.input += L"P";    InvalidateRect(g_hwnd, NULL, FALSE); return; }
    if (action == L"mod")  { c.input += L"%";    InvalidateRect(g_hwnd, NULL, FALSE); return; }

    if (action == L"GCD") {
        
        c.input += L"G"; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (action == L"LCM") {
        c.input += L"L"; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action == L"rand") {
        double r = (double)rand() / RAND_MAX;
        c.expression = L"rand()";
        c.input = formatNumber(r);
        c.lastResult = r;
        c.justEvaluated = true;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action == L"ans") {
        c.input += formatNumber(c.lastResult);
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action == L"MS") {
        double v = 0;
        try { v = c.input.empty() ? c.lastResult : evaluate(c.input); } catch(...) {}
        c.memory = v; InvalidateRect(g_hwnd, NULL, FALSE); return;
    }

    if (action == L"=") {
        if (c.input.empty()) { InvalidateRect(g_hwnd, NULL, FALSE); return; }
        c.expression = c.input + L" =";
        try {
            double res = evaluate(c.input);
            c.lastResult = res;
            c.input = formatNumber(res);
            c.justEvaluated = true;
        } catch(std::exception& e) {
            c.input = L"Error";
            c.isError = true;
        }
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
}


RECT getBtnRect(int idx) {
    const Button& b = g_buttons[idx];
    int col = b.col;
    int row = b.row;

    int x = BTN_X0 + col * (BTN_W + BTN_M);
    int y = BTN_Y0 + row * (BTN_H + BTN_M);
    int w = BTN_W;
    int h = BTN_H;


    if (b.action == L"=" && b.col == 5) h = BTN_H * 2 + BTN_M;

    RECT r = {x, y, x + w, y + h};
    return r;
}


void RoundRect2(HDC hdc, RECT r, int radius) {
    RoundRect(hdc, r.left, r.top, r.right, r.bottom, radius, radius);
}


void drawButton(HDC hdc, int idx, bool hover) {
    const Button& b = g_buttons[idx];
    bool use2nd = g_calc.is2nd && !b.label2nd.empty() && b.label2nd != b.label;

    RECT r = getBtnRect(idx);

    COLORREF baseBg = b.color;
    COLORREF bg = baseBg;

    if (hover) {
        int rr = std::min((int)GetRValue(bg) + 40, 255);
        int gg = std::min((int)GetGValue(bg) + 40, 255);
        int bb2 = std::min((int)GetBValue(bg) + 45, 255);
        bg = RGB(rr, gg, bb2);
    }

    int radius = 12;

    
    if (b.color == COL_BTN_EQUAL || b.color == COL_BTN_CLEAR) {
        COLORREF glowCol = (b.color == COL_BTN_EQUAL) ? RGB(0,160,140) : RGB(160,30,50);
        for (int g2 = 3; g2 >= 1; g2--) {
            HPEN glowPen = CreatePen(PS_SOLID, g2*2, glowCol);
            HBRUSH nullBr = (HBRUSH)GetStockObject(NULL_BRUSH);
            SelectObject(hdc, glowPen);
            SelectObject(hdc, nullBr);
            RoundRect(hdc, r.left - g2, r.top - g2, r.right + g2, r.bottom + g2, radius+g2, radius+g2);
            DeleteObject(glowPen);
        }
    }

    
    {
        HBRUSH shadowBr = CreateSolidBrush(RGB(0,0,0));
        HPEN nullP = CreatePen(PS_NULL,0,0);
        SelectObject(hdc, nullP);
        SelectObject(hdc, shadowBr);
        RoundRect(hdc, r.left+3, r.top+3, r.right+3, r.bottom+3, radius, radius);
        DeleteObject(shadowBr);
        DeleteObject(nullP);
    }

    
    {
        HBRUSH btnBr = CreateSolidBrush(bg);
        HPEN borderP;
        if (b.color == COL_BTN_EQUAL)
            borderP = CreatePen(PS_SOLID, 1, RGB(0,230,200));
        else if (b.color == COL_BTN_CLEAR)
            borderP = CreatePen(PS_SOLID, 1, RGB(230,80,100));
        else if (hover)
            borderP = CreatePen(PS_SOLID, 1, RGB(60,70,110));
        else
            borderP = CreatePen(PS_SOLID, 1, COL_BORDER);

        SelectObject(hdc, btnBr);
        SelectObject(hdc, borderP);
        RoundRect(hdc, r.left, r.top, r.right, r.bottom, radius, radius);
        DeleteObject(btnBr);
        DeleteObject(borderP);
    }

    
    {
        HBRUSH hilightBr = CreateSolidBrush(RGB(
            std::min((int)GetRValue(bg)+22, 255),
            std::min((int)GetGValue(bg)+22, 255),
            std::min((int)GetBValue(bg)+22, 255)));
        HPEN nullP = CreatePen(PS_NULL,0,0);
        SelectObject(hdc, nullP);
        SelectObject(hdc, hilightBr);
        RECT topR = {r.left+2, r.top+2, r.right-2, r.top + (r.bottom-r.top)/3};
        RoundRect(hdc, topR.left, topR.top, topR.right, topR.bottom, 10, 10);
        DeleteObject(hilightBr);
        DeleteObject(nullP);
    }

    
    if (b.color == COL_BTN_OP || b.color == COL_BTN_SCI) {
        COLORREF barCol = (b.color == COL_BTN_OP) ? RGB(0,200,170) : RGB(100,80,200);
        HPEN barPen = CreatePen(PS_SOLID, 2, barCol);
        HBRUSH nullBr = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, barPen);
        SelectObject(hdc, nullBr);
        MoveToEx(hdc, r.left+2, r.top+8, NULL);
        LineTo(hdc, r.left+2, r.bottom-8);
        DeleteObject(barPen);
    }

    
    std::wstring label = use2nd ? b.label2nd : b.label;
    COLORREF tc = b.textColor;
    if (use2nd && b.color == COL_BTN_SCI) tc = RGB(190, 150, 255);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, tc);


    bool useSmall = (label.size() > 3) || b.isSmallText;
    SelectObject(hdc, useSmall ? g_fontBtnSm : g_fontBtn);

    
    RECT textR = r;
    textR.top += 1;
    DrawTextW(hdc, label.c_str(), -1, &textR, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    
    if (!g_calc.is2nd && b.label2nd != b.label && !b.label2nd.empty()) {
        HFONT tinyFont = CreateFontW(-9, 0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
        SelectObject(hdc, tinyFont);
        SetTextColor(hdc, RGB(60,70,110));
        RECT hintR = {r.left, r.top+3, r.right-3, r.top+14};
        DrawTextW(hdc, b.label2nd.c_str(), -1, &hintR, DT_RIGHT | DT_TOP | DT_SINGLELINE);
        DeleteObject(tinyFont);
    }
}


void drawDisplay(HDC hdc, int w) {
    
    HPEN nullPen = CreatePen(PS_NULL, 0, 0);
    SelectObject(hdc, nullPen);

    HBRUSH dispBrush = CreateSolidBrush(COL_DISPLAY_BG);
    SelectObject(hdc, dispBrush);
    Rectangle(hdc, 0, 0, w, DISP_H);
    DeleteObject(dispBrush);

    
    HBRUSH topHighlight = CreateSolidBrush(RGB(18, 20, 32));
    SelectObject(hdc, topHighlight);
    Rectangle(hdc, 0, 0, w, 4);
    DeleteObject(topHighlight);

    DeleteObject(nullPen);


    HPEN thinAccent = CreatePen(PS_SOLID, 1, RGB(30,35,60));
    SelectObject(hdc, thinAccent);
    MoveToEx(hdc, 0, DISP_H - 3, NULL);
    LineTo(hdc, w, DISP_H - 3);
    DeleteObject(thinAccent);

    HPEN accentPen = CreatePen(PS_SOLID, 2, COL_ACCENT);
    SelectObject(hdc, accentPen);
    MoveToEx(hdc, 0, DISP_H - 1, NULL);
    LineTo(hdc, w, DISP_H - 1);
    DeleteObject(accentPen);

    
    HPEN cp2 = CreatePen(PS_SOLID, 2, COL_ACCENT2);
    SelectObject(hdc, cp2);
    MoveToEx(hdc, 0, DISP_H - 1, NULL);
    LineTo(hdc, 80, DISP_H - 1);
    DeleteObject(cp2);

    SetBkMode(hdc, TRANSPARENT);


    {
        HFONT brandFont = CreateFontW(-10, 0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
        SelectObject(hdc, brandFont);
        SetTextColor(hdc, RGB(40,50,80));
        RECT br2 = {10, 6, 200, 20};
        DrawTextW(hdc, L"STONE  SCIENTIFIC", -1, &br2, DT_LEFT|DT_TOP);
        DeleteObject(brandFont);
    }

    
    {
        std::wstring mode = g_calc.isDeg ? L" DEG " : L" RAD ";
        COLORREF modeCol = g_calc.isDeg ? RGB(0,180,150) : RGB(140,100,255);
        HBRUSH pillBr = CreateSolidBrush(RGB(
            GetRValue(modeCol)/5, GetGValue(modeCol)/5, GetBValue(modeCol)/5));
        HPEN pillPen = CreatePen(PS_SOLID, 1, modeCol);
        SelectObject(hdc, pillBr);
        SelectObject(hdc, pillPen);
        RoundRect(hdc, w-60, 5, w-8, 22, 8, 8);
        DeleteObject(pillBr);
        DeleteObject(pillPen);
        SelectObject(hdc, g_fontBtnSm);
        SetTextColor(hdc, modeCol);
        RECT modeR = {w-60, 5, w-8, 22};
        DrawTextW(hdc, mode.c_str(), -1, &modeR, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    
    if (g_calc.memory != 0) {
        HBRUSH memBr = CreateSolidBrush(RGB(50, 40, 0));
        HPEN memPen = CreatePen(PS_SOLID, 1, RGB(200,160,0));
        SelectObject(hdc, memBr);
        SelectObject(hdc, memPen);
        RoundRect(hdc, w-110, 5, w-68, 22, 8, 8);
        DeleteObject(memBr);
        DeleteObject(memPen);
        SelectObject(hdc, g_fontBtnSm);
        SetTextColor(hdc, RGB(255, 210, 60));
        RECT memR = {w-110, 5, w-68, 22};
        DrawTextW(hdc, L" M ", -1, &memR, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    
    if (g_calc.is2nd) {
        HBRUSH ndBr = CreateSolidBrush(RGB(30,20,60));
        HPEN ndPen = CreatePen(PS_SOLID, 1, RGB(180,130,255));
        SelectObject(hdc, ndBr);
        SelectObject(hdc, ndPen);
        int ox = (g_calc.memory != 0) ? w-160 : w-110;
        RoundRect(hdc, ox, 5, ox+48, 22, 8, 8);
        DeleteObject(ndBr);
        DeleteObject(ndPen);
        SelectObject(hdc, g_fontBtnSm);
        SetTextColor(hdc, RGB(200, 160, 255));
        RECT r2 = {ox, 5, ox+48, 22};
        DrawTextW(hdc, L" 2nd ", -1, &r2, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    
    if (!g_calc.expression.empty()) {
        SelectObject(hdc, g_fontExpr);
        SetTextColor(hdc, COL_TEXT_DIM);
        RECT exprR = {12, 24, w-12, 48};
        DrawTextW(hdc, g_calc.expression.c_str(), -1, &exprR, DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    }

    
    std::wstring disp = g_calc.input.empty() ? L"0" : g_calc.input;
    COLORREF textCol = g_calc.isError   ? RGB(255, 80, 100) :
                       g_calc.justEvaluated ? COL_TEXT_ACCENT : COL_TEXT_MAIN;

    
    int fsize = 36;
    if ((int)disp.size() > 12) fsize = 28;
    if ((int)disp.size() > 16) fsize = 22;
    if ((int)disp.size() > 20) fsize = 18;

    HFONT dynFont = CreateFontW(-fsize, 0,0,0, FW_LIGHT, FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI Light");
    SelectObject(hdc, dynFont);
    SetTextColor(hdc, textCol);

    RECT inputR = {12, 48, w-14, DISP_H-10};
    DrawTextW(hdc, disp.c_str(), -1, &inputR, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    DeleteObject(dynFont);

    
    static bool cursorOn = true;
    cursorOn = !cursorOn;
    if (cursorOn && !g_calc.justEvaluated && !g_calc.isError) {
        HPEN cursorPen = CreatePen(PS_SOLID, 2, COL_ACCENT);
        SelectObject(hdc, cursorPen);
        MoveToEx(hdc, w - 14, DISP_H - 16, NULL);
        LineTo(hdc, w - 14, DISP_H - 30);
        DeleteObject(cursorPen);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HDC memDC = NULL;
    static HBITMAP memBmp = NULL;
    static int lastW = 0, lastH = 0;

    switch (msg) {
    case WM_CREATE: {
        
        g_fontDisplay = CreateFontW(-44, 0,0,0, FW_THIN, FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI Light");
        g_fontExpr = CreateFontW(-14, 0,0,0, FW_NORMAL, FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
        g_fontBtn = CreateFontW(-15, 0,0,0, FW_SEMIBOLD, FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
        g_fontBtnSm = CreateFontW(-11, 0,0,0, FW_NORMAL, FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
        g_fontTitle = CreateFontW(-11, 0,0,0, FW_NORMAL, FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");

        SetTimer(hwnd, 1, 600, NULL);
        return 0;
    }

    case WM_TIMER:
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT cr; GetClientRect(hwnd, &cr);
        int cw = cr.right, ch = cr.bottom;

        
        if (!memDC || cw != lastW || ch != lastH) {
            if (memDC) { DeleteDC(memDC); DeleteObject(memBmp); }
            memDC = CreateCompatibleDC(hdc);
            memBmp = CreateCompatibleBitmap(hdc, cw, ch);
            SelectObject(memDC, memBmp);
            lastW = cw; lastH = ch;
        }

        HBRUSH bgBrush = CreateSolidBrush(COL_BG);
        HPEN nullP = CreatePen(PS_NULL,0,0);
        SelectObject(memDC, nullP);
        SelectObject(memDC, bgBrush);
        Rectangle(memDC, 0, 0, cw, ch);
        DeleteObject(bgBrush); DeleteObject(nullP);

        
        {
            HPEN dotPen = CreatePen(PS_SOLID, 1, RGB(20,24,40));
            SelectObject(memDC, dotPen);
            for (int gx = BTN_X0; gx < cw - BTN_X0; gx += 16)
                for (int gy = BTN_Y0; gy < ch; gy += 16)
                    SetPixel(memDC, gx, gy, RGB(22,26,44));
            DeleteObject(dotPen);
        }

    
        drawDisplay(memDC, cw);

        
        for (int i = 0; i < (int)g_buttons.size(); i++) {
        
            if (g_buttons[i].row == 8 && g_buttons[i].col == 5) continue;
            drawButton(memDC, i, g_hoverBtn == i);
        }

        
        {
            SetBkMode(memDC, TRANSPARENT);
            SelectObject(memDC, g_fontBtnSm);
            SetTextColor(memDC, RGB(35, 40, 65));
            RECT titleR = {0, ch - 18, cw, ch};
            DrawTextW(memDC, L"STONE SCIENTIFIC CALCULATOR", -1, &titleR, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        BitBlt(hdc, 0, 0, cw, ch, memDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
        for (int i = 0; i < (int)g_buttons.size(); i++) {
            if (g_buttons[i].row == 8 && g_buttons[i].col == 5) continue;
            RECT r = getBtnRect(i);
            if (mx >= r.left && mx < r.right && my >= r.top && my < r.bottom) {
                bool use2nd = g_calc.is2nd;
                std::wstring act = use2nd ? g_buttons[i].action2nd : g_buttons[i].action;
                if (g_calc.is2nd && act != L"2nd") g_calc.is2nd = false;
                processAction(act);
                break;
            }
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
        int hover = -1;
        for (int i = 0; i < (int)g_buttons.size(); i++) {
            if (g_buttons[i].row == 8 && g_buttons[i].col == 5) continue;
            RECT r = getBtnRect(i);
            if (mx >= r.left && mx < r.right && my >= r.top && my < r.bottom) {
                hover = i; break;
            }
        }
        if (hover != g_hoverBtn) {
            g_hoverBtn = hover;
            InvalidateRect(hwnd, NULL, FALSE);
            
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        g_hoverBtn = -1;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_KEYDOWN: {
        wchar_t key = (wchar_t)wp;
        switch (wp) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                wchar_t d[2] = {(wchar_t)wp, 0};
                processAction(std::wstring(d)); break;
            }
            case VK_DECIMAL: case VK_OEM_PERIOD: processAction(L"."); break;
            case VK_ADD:    case VK_OEM_PLUS:   processAction(g_calc.is2nd ? L"-" : L"+"); break;
            case VK_SUBTRACT: case VK_OEM_MINUS: processAction(L"-"); break;
            case VK_MULTIPLY: processAction(L"*"); break;
            case VK_DIVIDE:   processAction(L"/"); break;
            case VK_RETURN:   processAction(L"="); break;
            case VK_BACK:     processAction(L"back"); break;
            case VK_ESCAPE:   processAction(L"clear"); break;
        }
        return 0;
    }

    case WM_CHAR: {
        switch(wp) {
            case L'+': processAction(L"+"); break;
            case L'-': processAction(L"-"); break;
            case L'*': processAction(L"*"); break;
            case L'/': processAction(L"/"); break;
            case L'(': processAction(L"("); break;
            case L')': processAction(L")"); break;
            case L'.': processAction(L"."); break;
            case L'%': processAction(L"%"); break;
            case L'^': processAction(L"^"); break;
            case L'=': case L'\r': processAction(L"="); break;
        }
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(g_fontDisplay);
        DeleteObject(g_fontExpr);
        DeleteObject(g_fontBtn);
        DeleteObject(g_fontBtnSm);
        DeleteObject(g_fontTitle);
        if (memDC) DeleteDC(memDC);
        if (memBmp) DeleteObject(memBmp);
        PostQuitMessage(0);
        return 0;

    case WM_ERASEBKGND:
        return 1; 
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    INITCOMMONCONTROLSEX icex = {sizeof(icex), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icex);

    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"StoneCalc";
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExW(&wc);

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    
    RECT wr = {0, 0, WIN_W, WIN_H};
    AdjustWindowRectEx(&wr, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, FALSE, WS_EX_APPWINDOW);
    int totalW = wr.right - wr.left;
    int totalH = wr.bottom - wr.top;

    int wx = (sw - totalW) / 2;
    int wy = (sh - totalH) / 2;

    g_hwnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        L"StoneCalc",
        L"Stone Scientific Calculator",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        wx, wy, totalW, totalH,
        NULL, NULL, hInst, NULL
    );

    if (!g_hwnd) return 1;

    ShowWindow(g_hwnd, nShow);
    UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}
