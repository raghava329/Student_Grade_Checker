#include <bits/stdc++.h>
using namespace std;
// ========================= String Utilities ========================= //


string str_trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

string str_lower(const string& s) {
    string r = s;
    for (int i = 0; i < (int)r.size(); i++)
        if (r[i] >= 'A' && r[i] <= 'Z') r[i] += 32;
    return r;
}

bool str_eq_ci(const string& a, const string& b) {
    return str_lower(a) == str_lower(b);
}

bool str_contains_ci(const string& hay, const string& needle) {
    return str_lower(hay).find(str_lower(needle)) != string::npos;
}

int safe_stoi(const string& s) {
    string t = str_trim(s);
    if (t.empty()) return -1;
    try { return stoi(t); }
    catch (...) { return -1; }
}

float safe_stof(const string& s) {
    string t = str_trim(s);
    if (t.empty()) return 0.0f;
    try { return stof(t); }
    catch (...) { return 0.0f; }
}

string float_to_str(float v, int dec = 2) {
    ostringstream oss;
    oss << fixed << setprecision(dec) << v;
    return oss.str();
}

vector<string> split_csv(const string& line) {
    vector<string> parts;
    string cur;
    bool in_quotes = false;
    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] == '"') {
            in_quotes = !in_quotes;
        } else if (line[i] == ',' && !in_quotes) {
            parts.push_back(str_trim(cur));
            cur.clear();
        } else {
            cur += line[i];
        }
    }
    parts.push_back(str_trim(cur));
    return parts;
}





// ========================= Basic Structs ========================= //

struct Subject {
    string code;
    string grade;
    int semester;

    Subject(string c, int sem) {
        code = c;
        grade = "N";
        semester = sem;
    }

    Subject(string c, string g, int sem) {
        code = c;
        grade = g;
        semester = sem;
    }
};

struct Student {
    string id;
    string name;
    string department;
    int joining_year;
    int current_semester;
    vector<Subject> subjects;
    double cgpa;
    vector<double> sgpa;
    vector<int> dept_rank;

    Student() {
        id = "";
        name = "";
        department = "";
        joining_year = 0;
        current_semester = 1;
        cgpa = 0.0;
    }

    Student(string i, string n, string dept, int year) {
        id = i;
        name = n;
        department = dept;
        joining_year = year;
        current_semester = 1;
        cgpa = 0.0;
    }
};

struct Node {
    Student data;
    Node* next;

    Node() {
        next = nullptr;
    }

    Node(Student s) {
        data = s;
        next = nullptr;
    }
};

// ========================= Grade-Points ========================= //

double getGradePoints(string grade) {
    if (grade == "A")  return 10.0;
    if (grade == "A-") return 9.0;
    if (grade == "B")  return 8.0;
    if (grade == "B-") return 7.0;
    if (grade == "C")  return 6.0;
    if (grade == "C-") return 5.0;
    if (grade == "D")  return 4.0;
    if (grade == "F")  return -1.0;
    if (grade == "W")  return -1.0;
    return -1.0;
}

bool isExcluded(string grade) {
    return grade == "F" || grade == "W";
}

// ========================= Hash-Table ========================= //

const int TABLE_SIZE = 97;

struct HashTable {
    Node** table;
    int count;
    int size;

    HashTable() {
        size = TABLE_SIZE;
        count = 0;
        table = new Node*[size];
        for (int i = 0; i < size; i++) {
            table[i] = nullptr;
        }
    }

    int hashFunction(string id) {
        int sum = 0;
        for (int i = 0; i < id.length(); i++) {
            sum += (int)id[i];
        }
        return sum % size;
    }

    bool needsRehash() {
        return (double)count / size > 0.7;
    }

    int nextPrime(int n) {
        n++;
        while (true) {
            bool prime = true;
            for (int i = 2; i * i <= n; i++) {
                if (n % i == 0) {
                    prime = false;
                    break;
                }
            }
            if (prime) return n;
            n++;
        }
    }

    void rehash() {
        int newSize = nextPrime(size * 2);
        Node** newTable = new Node*[newSize];
        for (int i = 0; i < newSize; i++) {
            newTable[i] = nullptr;
        }
        for (int i = 0; i < size; i++) {
            Node* curr = table[i];
            while (curr != nullptr) {
                int newIndex = 0;
                for (int j = 0; j < curr->data.id.length(); j++) {
                    newIndex += (int)curr->data.id[j];
                }
                newIndex = newIndex % newSize;
                Node* next = curr->next;
                curr->next = newTable[newIndex];
                newTable[newIndex] = curr;
                curr = next;
            }
        }
        delete[] table;
        table = newTable;
        size = newSize;
    }

    void insert(Student s) {
        if (needsRehash()) rehash();
        int index = hashFunction(s.id);
        Node* curr = table[index];
        while (curr != nullptr) {
            if (curr->data.id == s.id) {
                curr->data = s;
                return;
            }
            curr = curr->next;
        }
        Node* newNode = new Node(s);
        newNode->next = table[index];
        table[index] = newNode;
        count++;
    }

    Node* search(string id) {
        int index = hashFunction(id);
        Node* curr = table[index];
        while (curr != nullptr) {
            if (curr->data.id == id) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    bool remove(string id) {
        int index = hashFunction(id);
        Node* curr = table[index];
        Node* prev = nullptr;
        while (curr != nullptr) {
            if (curr->data.id == id) {
                if (prev == nullptr) {
                    table[index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    ~HashTable() {
        for (int i = 0; i < size; i++) {
            Node* curr = table[i];
            while (curr != nullptr) {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
        }
        delete[] table;
    }
};

