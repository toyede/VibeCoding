#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cmath>

// A class to represent and operate on polynomials
class Polynomial {
public:
    // Map to store terms: key = exponent, value = coefficient
    std::map<int, int> terms;

    // Manually parse a string to initialize the polynomial object.
    // Returns true on success, false on failure.
    bool parse(std::string s) {
        terms.clear();
        if (s.empty()) return true;

        // Prepend a '+' to simplify parsing logic, so every term has a sign.
        if (s[0] != '+' && s[0] != '-') {
            s.insert(0, "+");
        }

        size_t pos = 0;
        while (pos < s.length()) {
            size_t next_pos = s.find_first_of("+-", pos + 1);
            if (next_pos == std::string::npos) {
                next_pos = s.length();
            }
            
            std::string term_str = s.substr(pos, next_pos - pos);
            pos = next_pos;

            if (term_str.empty()) continue;

            int sign = (term_str[0] == '-') ? -1 : 1;
            term_str = term_str.substr(1); // Remove sign

            int coeff = 1;
            int exp = 0;

            size_t x_pos = term_str.find('x');

            if (x_pos != std::string::npos) {
                // Part before 'x' is the coefficient
                std::string coeff_str = term_str.substr(0, x_pos);
                if (!coeff_str.empty() && coeff_str != "*") {
                    try {
                        coeff = std::stoi(coeff_str);
                    } catch (...) { return false; } // Invalid coefficient
                }

                // Part after 'x' is the exponent
                exp = 1; // Default exponent is 1 if 'x' exists
                size_t pow_pos = term_str.find('^');
                if (pow_pos == std::string::npos) {
                    pow_pos = term_str.find("**");
                }

                if (pow_pos != std::string::npos) {
                    std::string exp_str = term_str.substr(pow_pos + (term_str[pow_pos+1] == '*' ? 2 : 1));
                     if (!exp_str.empty()) {
                        try {
                           exp = std::stoi(exp_str);
                        } catch (...) { return false; } // Invalid exponent
                    }
                }
            } else { // Constant term (no 'x')
                exp = 0;
                if (!term_str.empty()) {
                    try {
                        coeff = std::stoi(term_str);
                    } catch (...) { return false; } // Invalid constant
                }
            }
            terms[exp] += sign * coeff;
        }

        // Remove terms with a zero coefficient
        for (std::map<int, int>::iterator it = terms.begin(); it != terms.end(); ) {
            if (it->second == 0) {
                it = terms.erase(it);
            } else {
                ++it;
            }
        }
        return true;
    }

    // Print the polynomial in a formatted way
    void print() const {
        if (terms.empty()) {
            std::cout << "0";
            return;
        }

        bool first_term = true;
        for (std::map<int, int>::const_reverse_iterator it = terms.rbegin(); it != terms.rend(); ++it) {
            int exp = it->first;
            int coeff = it->second;

            if (coeff == 0) continue;

            // Print sign
            if (!first_term) {
                std::cout << (coeff > 0 ? " + " : " - ");
            } else {
                if (coeff < 0) {
                    std::cout << "-";
                }
            }

            int abs_coeff = std::abs(coeff);

            // Print coefficient
            if (abs_coeff != 1 || exp == 0) {
                std::cout << abs_coeff;
            }
            
            // Print variable and exponent
            if (exp > 0) {
                std::cout << "x";
                if (exp > 1) {
                    std::cout << "^" << exp;
                }
            }
            first_term = false;
        }
        if (first_term) { // Case where all coefficients were zero
             std::cout << "0";
        }
    }

    // Static method for polynomial addition
    static Polynomial add(const Polynomial& a, const Polynomial& b) {
        Polynomial result = a;
        for (std::map<int, int>::const_iterator it = b.terms.begin(); it != b.terms.end(); ++it) {
            result.terms[it->first] += it->second;
        }
        // Clean up zero-coefficient terms
        for (std::map<int, int>::iterator it = result.terms.begin(); it != result.terms.end(); ) {
            if (it->second == 0) {
                it = result.terms.erase(it);
            } else {
                ++it;
            }
        }
        return result;
    }

    // Static method for polynomial multiplication
    static Polynomial multiply(const Polynomial& a, const Polynomial& b) {
        Polynomial result;
        if (a.terms.empty() || b.terms.empty()) return result;

        for (std::map<int, int>::const_iterator it_a = a.terms.begin(); it_a != a.terms.end(); ++it_a) {
            for (std::map<int, int>::const_iterator it_b = b.terms.begin(); it_b != b.terms.end(); ++it_b) {
                int new_exp = it_a->first + it_b->first;
                int new_coeff = it_a->second * it_b->second;
                result.terms[new_exp] += new_coeff;
            }
        }
        // Clean up zero-coefficient terms
        for (std::map<int, int>::iterator it = result.terms.begin(); it != result.terms.end(); ) {
            if (it->second == 0) {
                it = result.terms.erase(it);
            } else {
                ++it;
            }
        }
        return result;
    }
};

int main() {
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input.txt" << std::endl;
        return 1;
    }

    int n;
    inputFile >> n;
    std::string line;
    std::getline(inputFile, line); // Consume the rest of the first line

    for (int i = 1; i <= n; ++i) {
        std::cout << "== " << i << "번째 다항식 쌍 ==" << std::endl;

        std::string poly_str1, poly_str2;
        
        if (!std::getline(inputFile, poly_str1) || !std::getline(inputFile, poly_str2)) {
             std::cerr << "Error: Not enough polynomials in the input file." << std::endl;
             break;
        }

        // Remove carriage return characters for better cross-platform compatibility
        poly_str1.erase(std::remove(poly_str1.begin(), poly_str1.end(), '\r'), poly_str1.end());
        poly_str2.erase(std::remove(poly_str2.begin(), poly_str2.end(), '\r'), poly_str2.end());
        
        // Remove all whitespace
        poly_str1.erase(std::remove_if(poly_str1.begin(), poly_str1.end(), ::isspace), poly_str1.end());
        poly_str2.erase(std::remove_if(poly_str2.begin(), poly_str2.end(), ::isspace), poly_str2.end());


        Polynomial p1, p2;
        bool p1_valid = p1.parse(poly_str1);
        bool p2_valid = p2.parse(poly_str2);

        if (!p1_valid || !p2_valid) {
            std::cout << "연산이 불가능한 다항식 쌍입니다." << std::endl << std::endl;
            continue;
        }

        std::cout << "첫번째: ";
        p1.print();
        std::cout << std::endl;

        std::cout << "두번째: ";
        p2.print();
        std::cout << std::endl;

        Polynomial sum = Polynomial::add(p1, p2);
        std::cout << "덧셈결과: ";
        sum.print();
        std::cout << std::endl;

        Polynomial prod = Polynomial::multiply(p1, p2);
        std::cout << "곱셈결과: ";
        prod.print();
        std::cout << std::endl << std::endl;
    }

    inputFile.close();
    return 0;
}
