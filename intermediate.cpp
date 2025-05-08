#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cctype>
#include <map>

using namespace std;

struct Quadruple {
    string op, arg1, arg2, result;
};

struct Triple {
    string op, arg1, arg2;
};

// Operator precedence
int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

// Check if character is operator
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Infix to postfix conversion
vector<string> infixToPostfix(string expr) {
    stack<char> ops;
    vector<string> output;
    string operand;

    for (char ch : expr) {
        if (isspace(ch)) continue;
        if (isalnum(ch)) {
            operand += ch;
        } else {
            if (!operand.empty()) {
                output.push_back(operand);
                operand = "";
            }

            if (ch == '(') {
                ops.push(ch);
            } else if (ch == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    output.push_back(string(1, ops.top()));
                    ops.pop();
                }
                ops.pop(); // pop '('
            } else if (isOperator(ch)) {
                while (!ops.empty() && precedence(ops.top()) >= precedence(ch)) {
                    output.push_back(string(1, ops.top()));
                    ops.pop();
                }
                ops.push(ch);
            }
        }
    }

    if (!operand.empty()) output.push_back(operand);
    while (!ops.empty()) {
        output.push_back(string(1, ops.top()));
        ops.pop();
    }

    return output;
}

// Generate intermediate code from postfix
vector<Quadruple> generateCodeFromPostfix(vector<string> postfix, string resultVar) {
    stack<string> st;
    vector<Quadruple> quads;
    int tempCount = 1;

    for (string token : postfix) {
        if (isOperator(token[0]) && token.length() == 1) {
            string arg2 = st.top(); st.pop();
            string arg1 = st.top(); st.pop();
            string temp = "t" + to_string(tempCount++);
            quads.push_back({token, arg1, arg2, temp});
            st.push(temp);
        } else {
            st.push(token);
        }
    }

    quads.push_back({"=", st.top(), "", resultVar});
    return quads;
}

// Convert to Triples
vector<Triple> convertToTriples(const vector<Quadruple>& quads) {
    vector<Triple> triples;
    map<string, int> tempMap;

    for (size_t i = 0; i < quads.size(); ++i) {
        string a1 = quads[i].arg1;
        string a2 = quads[i].arg2;

        if (a1[0] == 't') a1 = to_string(tempMap[a1]);
        if (!a2.empty() && a2[0] == 't') a2 = to_string(tempMap[a2]);

        triples.push_back({quads[i].op, a1, a2});

        if (quads[i].result[0] == 't')
            tempMap[quads[i].result] = i;
    }

    return triples;
}

int main() {
    string input;
    cout << "Enter expression (e.g., a = b + c * d): ";
    getline(cin, input);

    size_t eqPos = input.find('=');
    if (eqPos == string::npos) {
        cout << "Invalid expression. Must contain '='.\n";
        return 1;
    }

    string lhs = input.substr(0, eqPos);
    string rhs = input.substr(eqPos + 1);

    vector<string> postfix = infixToPostfix(rhs);
    vector<Quadruple> quads = generateCodeFromPostfix(postfix, lhs);

    cout << "\nQuadruples:\n";
    for (const auto& q : quads)
        cout << "(" << q.op << ", " << q.arg1 << ", " << q.arg2 << ", " << q.result << ")\n";

    vector<Triple> triples = convertToTriples(quads);
    cout << "\nTriples:\n";
    for (size_t i = 0; i < triples.size(); ++i)
        cout << "(" << i << ") (" << triples[i].op << ", " << triples[i].arg1 << ", " << triples[i].arg2 << ")\n";

    cout << "\nIndirect Triples:\n";
    for (size_t i = 0; i < triples.size(); ++i)
        cout << "Pointer[" << i << "] -> Triple " << i << "\n";

    return 0;
}
