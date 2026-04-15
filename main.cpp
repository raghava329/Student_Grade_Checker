// ================================================================
// Student Grade Management System - IIT Jodhpur
// C++ REST API Backend | Web UI Frontend
// All algorithms and data structures self-implemented
// HTTP via cpp-httplib (single header)
// Compile (Windows): g++ -std=c++14 -o server main.cpp -lws2_32
// Compile (Linux):   g++ -std=c++14 -pthread -o server main.cpp
// Run: ./server   then open http://localhost:3000
// ================================================================

#include <bits/stdc++.h>
#include "httplib.h"
using namespace std;

// ================================================================
// SECTION 1: CUSTOM PAIR
// ================================================================

template<typename A, typename B>
struct Pair {
    A first;
    B second;
    Pair() {}
    Pair(const A& a, const B& b) : first(a), second(b) {}
};

// ================================================================
// SECTION 2: STRING UTILITIES
// ================================================================

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

// ================================================================
// SECTION 3: JSON HELPERS (manual — no JSON library)
// ================================================================

string json_escape(const string& s) {
    string r;
    for (int i = 0; i < (int)s.size(); i++) {
        char c = s[i];
        if (c == '"')       r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\r') r += "\\r";
        else if (c == '\t') r += "\\t";
        else r += c;
    }
    return r;
}

string json_str(const string& key, const string& val) {
    return "\"" + key + "\":\"" + json_escape(val) + "\"";
}

string json_int(const string& key, int val) {
    return "\"" + key + "\":" + to_string(val);
}

string json_float(const string& key, float val) {
    return "\"" + key + "\":" + float_to_str(val, 2);
}

string json_bool(const string& key, bool val) {
    return "\"" + key + "\":" + (val ? "true" : "false");
}

// ================================================================
// SECTION 4: HASH MAP (manual chaining with djb2)
// ================================================================

template<typename K, typename V>
struct HashMap {
    static const int BUCKETS = 512;
    vector<Pair<K,V>> table[BUCKETS];
    int count;

    HashMap() : count(0) {}

    int hash_key(const string& k) const {
        unsigned int h = 5381;
        for (int i = 0; i < (int)k.size(); i++)
            h = ((h << 5) + h) ^ (unsigned char)k[i];
        return (int)(h % BUCKETS);
    }

    void insert(const K& key, const V& val) {
        int b = hash_key(key);
        for (int i = 0; i < (int)table[b].size(); i++) {
            if (table[b][i].first == key) {
                table[b][i].second = val;
                return;
            }
        }
        table[b].push_back(Pair<K,V>(key, val));
        count++;
    }

    V* find(const K& key) {
        int b = hash_key(key);
        for (int i = 0; i < (int)table[b].size(); i++)
            if (table[b][i].first == key) return &table[b][i].second;
        return nullptr;
    }

    const V* find(const K& key) const {
        int b = hash_key(key);
        for (int i = 0; i < (int)table[b].size(); i++)
            if (table[b][i].first == key) return &table[b][i].second;
        return nullptr;
    }

    bool erase(const K& key) {
        int b = hash_key(key);
        for (int i = 0; i < (int)table[b].size(); i++) {
            if (table[b][i].first == key) {
                table[b][i] = table[b][table[b].size() - 1];
                table[b].pop_back();
                count--;
                return true;
            }
        }
        return false;
    }

    vector<V> all_values() const {
        vector<V> out;
        for (int i = 0; i < BUCKETS; i++)
            for (int j = 0; j < (int)table[i].size(); j++)
                out.push_back(table[i][j].second);
        return out;
    }

    void clear() {
        for (int i = 0; i < BUCKETS; i++) table[i].clear();
        count = 0;
    }

    int size() const { return count; }
};

// ================================================================
// SECTION 5: SORTING (merge sort) & SEARCHING (binary search)
// ================================================================

template<typename T, typename Cmp>
void merge_sort(vector<T>& arr, int l, int r, Cmp cmp) {
    if (r <= l) return;
    int mid = (l + r) / 2;
    merge_sort(arr, l, mid, cmp);
    merge_sort(arr, mid + 1, r, cmp);
    vector<T> tmp;
    int i = l, j = mid + 1;
    while (i <= mid && j <= r)
        tmp.push_back(cmp(arr[i], arr[j]) ? arr[i++] : arr[j++]);
    while (i <= mid) tmp.push_back(arr[i++]);
    while (j <= r)   tmp.push_back(arr[j++]);
    for (int k = 0; k < (int)tmp.size(); k++) arr[l + k] = tmp[k];
}

template<typename T, typename Cmp>
void sort_vec(vector<T>& arr, Cmp cmp) {
    if ((int)arr.size() > 1)
        merge_sort(arr, 0, (int)arr.size() - 1, cmp);
}

int bin_search_str(const vector<string>& arr, const string& key) {
    int lo = 0, hi = (int)arr.size() - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (arr[mid] == key)  return mid;
        if (arr[mid] < key)   lo = mid + 1;
        else                  hi = mid - 1;
    }
    return -1;
}

// ================================================================
// SECTION 6: DATA MODELS
// ================================================================

struct Student {
    string rollNo, name, dept;
    int    semester, year;
};

struct Subject {
    string code, name;
    float  credits;
};

struct Enrollment {
    string rollNo, subCode;
    int    semester, year;
};

struct GradeEntry {
    string rollNo, subCode, grade;
    int    semester, year;
};

// ================================================================
// SECTION 7: GLOBAL STATE
// ================================================================

HashMap<string, Student> studentMap;
HashMap<string, Subject> subjectMap;
vector<Enrollment>       enrollments;
vector<GradeEntry>       gradeEntries;

// ================================================================
// SECTION 8: GRADE LOGIC
// ================================================================

bool is_valid_grade(const string& g) {
    return g == "A" || g == "A-" || g == "B" || g == "B-" ||
           g == "C" || g == "C-" || g == "D" || g == "F" || g == "W";
}

float grade_to_pts(const string& g) {
    if (g == "A")  return 10.0f;
    if (g == "A-") return 9.0f;
    if (g == "B")  return 8.0f;
    if (g == "B-") return 7.0f;
    if (g == "C")  return 6.0f;
    if (g == "C-") return 5.0f;
    if (g == "D")  return 4.0f;
    return 0.0f;
}

bool counts_for_gpa(const string& g) {
    return g != "F" && g != "W";
}

GradeEntry* find_grade(const string& roll, const string& sub, int sem, int yr) {
    for (int i = 0; i < (int)gradeEntries.size(); i++) {
        GradeEntry& ge = gradeEntries[i];
        if (ge.rollNo == roll && str_eq_ci(ge.subCode, sub) &&
            ge.semester == sem && ge.year == yr)
            return &ge;
    }
    return nullptr;
}

Enrollment* find_enrollment(const string& roll, const string& sub, int sem, int yr) {
    for (int i = 0; i < (int)enrollments.size(); i++) {
        Enrollment& e = enrollments[i];
        if (e.rollNo == roll && str_eq_ci(e.subCode, sub) &&
            e.semester == sem && e.year == yr)
            return &e;
    }
    return nullptr;
}

float calc_sgpa(const string& roll, int sem, int yr) {
    float pts = 0, cred = 0;
    for (int i = 0; i < (int)gradeEntries.size(); i++) {
        const GradeEntry& ge = gradeEntries[i];
        if (ge.rollNo != roll || ge.semester != sem || ge.year != yr) continue;
        if (!counts_for_gpa(ge.grade)) continue;
        Subject* sub = subjectMap.find(ge.subCode);
        if (!sub) continue;
        pts  += grade_to_pts(ge.grade) * sub->credits;
        cred += sub->credits;
    }
    return cred > 0 ? pts / cred : 0.0f;
}

float calc_cgpa(const string& roll) {
    float pts = 0, cred = 0;
    for (int i = 0; i < (int)gradeEntries.size(); i++) {
        const GradeEntry& ge = gradeEntries[i];
        if (ge.rollNo != roll || !counts_for_gpa(ge.grade)) continue;
        Subject* sub = subjectMap.find(ge.subCode);
        if (!sub) continue;
        pts  += grade_to_pts(ge.grade) * sub->credits;
        cred += sub->credits;
    }
    return cred > 0 ? pts / cred : 0.0f;
}

string make_password(const string& name, const string& roll) {
    string nm = str_lower(name);
    string pfx;
    for (int i = 0; i < (int)nm.size() && (int)pfx.size() < 5; i++)
        if (nm[i] != ' ') pfx += nm[i];
    string sfx = ((int)roll.size() >= 4) ? roll.substr(roll.size() - 4) : roll;
    return pfx + sfx;
}

// ================================================================
// SECTION 9: PERSISTENCE (CSV file I/O)
// ================================================================

const string F_STUDENTS    = "data_students.csv";
const string F_SUBJECTS    = "data_subjects.csv";
const string F_ENROLLMENTS = "data_enrollments.csv";
const string F_GRADES      = "data_grades.csv";

void save_students() {
    ofstream f(F_STUDENTS);
    if (!f) return;
    f << "RollNo,Name,Dept,Semester,Year\n";
    vector<Student> all = studentMap.all_values();
    for (int i = 0; i < (int)all.size(); i++)
        f << all[i].rollNo << "," << all[i].name << "," << all[i].dept << ","
          << all[i].semester << "," << all[i].year << "\n";
}

void save_subjects() {
    ofstream f(F_SUBJECTS);
    if (!f) return;
    f << "Code,Name,Credits\n";
    vector<Subject> all = subjectMap.all_values();
    for (int i = 0; i < (int)all.size(); i++)
        f << all[i].code << "," << all[i].name << ","
          << float_to_str(all[i].credits, 1) << "\n";
}

void save_enrollments() {
    ofstream f(F_ENROLLMENTS);
    if (!f) return;
    f << "RollNo,SubCode,Semester,Year\n";
    for (int i = 0; i < (int)enrollments.size(); i++)
        f << enrollments[i].rollNo << "," << enrollments[i].subCode << ","
          << enrollments[i].semester << "," << enrollments[i].year << "\n";
}

void save_grades() {
    ofstream f(F_GRADES);
    if (!f) return;
    f << "RollNo,SubCode,Grade,Semester,Year\n";
    for (int i = 0; i < (int)gradeEntries.size(); i++)
        f << gradeEntries[i].rollNo << "," << gradeEntries[i].subCode << ","
          << gradeEntries[i].grade << "," << gradeEntries[i].semester << ","
          << gradeEntries[i].year << "\n";
}

void save_all() {
    save_students();
    save_subjects();
    save_enrollments();
    save_grades();
}

void load_all() {
    {
        ifstream f(F_STUDENTS);
        if (f) {
            string line;
            getline(f, line);
            while (getline(f, line)) {
                line = str_trim(line);
                if (line.empty()) continue;
                vector<string> p = split_csv(line);
                if ((int)p.size() < 5) continue;
                Student s;
                s.rollNo = p[0]; s.name = p[1]; s.dept = p[2];
                s.semester = safe_stoi(p[3]); s.year = safe_stoi(p[4]);
                if (!s.rollNo.empty()) studentMap.insert(s.rollNo, s);
            }
        }
    }
    {
        ifstream f(F_SUBJECTS);
        if (f) {
            string line;
            getline(f, line);
            while (getline(f, line)) {
                line = str_trim(line);
                if (line.empty()) continue;
                vector<string> p = split_csv(line);
                if ((int)p.size() < 3) continue;
                Subject s;
                s.code = p[0]; s.name = p[1]; s.credits = safe_stof(p[2]);
                if (!s.code.empty()) subjectMap.insert(s.code, s);
            }
        }
    }
    {
        ifstream f(F_ENROLLMENTS);
        if (f) {
            string line;
            getline(f, line);
            while (getline(f, line)) {
                line = str_trim(line);
                if (line.empty()) continue;
                vector<string> p = split_csv(line);
                if ((int)p.size() < 4) continue;
                Enrollment e;
                e.rollNo = p[0]; e.subCode = p[1];
                e.semester = safe_stoi(p[2]); e.year = safe_stoi(p[3]);
                if (!e.rollNo.empty()) enrollments.push_back(e);
            }
        }
    }
    {
        ifstream f(F_GRADES);
        if (f) {
            string line;
            getline(f, line);
            while (getline(f, line)) {
                line = str_trim(line);
                if (line.empty()) continue;
                vector<string> p = split_csv(line);
                if ((int)p.size() < 5) continue;
                GradeEntry ge;
                ge.rollNo = p[0]; ge.subCode = p[1]; ge.grade = p[2];
                ge.semester = safe_stoi(p[3]); ge.year = safe_stoi(p[4]);
                if (!ge.rollNo.empty()) gradeEntries.push_back(ge);
            }
        }
    }
}

// ================================================================
// SECTION 10: JSON SERIALIZERS
// ================================================================

string student_to_json(const Student& s) {
    float cgpa = calc_cgpa(s.rollNo);
    return "{" + json_str("rollNo", s.rollNo) + ","
         + json_str("name", s.name) + ","
         + json_str("dept", s.dept) + ","
         + json_int("semester", s.semester) + ","
         + json_int("year", s.year) + ","
         + json_float("cgpa", cgpa) + "}";
}

string subject_to_json(const Subject& s) {
    return "{" + json_str("code", s.code) + ","
         + json_str("name", s.name) + ","
         + json_float("credits", s.credits) + "}";
}

string grade_to_json(const GradeEntry& ge) {
    Subject* sub = subjectMap.find(ge.subCode);
    return "{" + json_str("rollNo", ge.rollNo) + ","
         + json_str("subCode", ge.subCode) + ","
         + json_str("subName", sub ? sub->name : "(unknown)") + ","
         + json_float("credits", sub ? sub->credits : 0) + ","
         + json_str("grade", ge.grade) + ","
         + json_float("points", grade_to_pts(ge.grade)) + ","
         + json_bool("countsForGpa", counts_for_gpa(ge.grade)) + ","
         + json_int("semester", ge.semester) + ","
         + json_int("year", ge.year) + "}";
}

string enrollment_to_json(const Enrollment& e) {
    Subject* sub = subjectMap.find(e.subCode);
    return "{" + json_str("rollNo", e.rollNo) + ","
         + json_str("subCode", e.subCode) + ","
         + json_str("subName", sub ? sub->name : "(unknown)") + ","
         + json_float("credits", sub ? sub->credits : 0) + ","
         + json_int("semester", e.semester) + ","
         + json_int("year", e.year) + "}";
}

// Full student detail with current subjects and semester history
string student_detail_json(const Student& s) {
    string json = "{";
    json += json_str("rollNo", s.rollNo) + ",";
    json += json_str("name", s.name) + ",";
    json += json_str("dept", s.dept) + ",";
    json += json_int("semester", s.semester) + ",";
    json += json_int("year", s.year) + ",";
    json += json_float("cgpa", calc_cgpa(s.rollNo)) + ",";

    // Current subjects (enrolled but no grade)
    json += "\"currentSubjects\":[";
    bool first = true;
    for (int i = 0; i < (int)enrollments.size(); i++) {
        const Enrollment& e = enrollments[i];
        if (e.rollNo != s.rollNo) continue;
        if (find_grade(s.rollNo, e.subCode, e.semester, e.year)) continue;
        if (!first) json += ",";
        json += enrollment_to_json(e);
        first = false;
    }
    json += "],";

    // Collect unique (sem, year) combos from grades
    vector<int> sem_v, yr_v;
    for (int i = 0; i < (int)gradeEntries.size(); i++) {
        if (gradeEntries[i].rollNo != s.rollNo) continue;
        int gs = gradeEntries[i].semester, gy = gradeEntries[i].year;
        bool found = false;
        for (int j = 0; j < (int)sem_v.size(); j++)
            if (sem_v[j] == gs && yr_v[j] == gy) { found = true; break; }
        if (!found) { sem_v.push_back(gs); yr_v.push_back(gy); }
    }
    // Bubble sort by (year, sem)
    for (int i = 0; i < (int)sem_v.size() - 1; i++)
        for (int j = 0; j < (int)sem_v.size() - 1 - i; j++)
            if (yr_v[j] > yr_v[j+1] || (yr_v[j] == yr_v[j+1] && sem_v[j] > sem_v[j+1])) {
                int ts = sem_v[j]; sem_v[j] = sem_v[j+1]; sem_v[j+1] = ts;
                int ty = yr_v[j];  yr_v[j]  = yr_v[j+1];  yr_v[j+1]  = ty;
            }

    // Semesters array
    json += "\"semesters\":[";
    for (int si = 0; si < (int)sem_v.size(); si++) {
        int sem = sem_v[si], yr = yr_v[si];
        if (si > 0) json += ",";
        json += "{";
        json += json_int("semester", sem) + ",";
        json += json_int("year", yr) + ",";
        json += json_float("sgpa", calc_sgpa(s.rollNo, sem, yr)) + ",";
        json += "\"grades\":[";
        bool gfirst = true;
        for (int i = 0; i < (int)gradeEntries.size(); i++) {
            const GradeEntry& ge = gradeEntries[i];
            if (ge.rollNo != s.rollNo || ge.semester != sem || ge.year != yr) continue;
            if (!gfirst) json += ",";
            json += grade_to_json(ge);
            gfirst = false;
        }
        json += "]}";
    }
    json += "]}";
    return json;
}

// ================================================================
// SECTION 11: SIMPLE JSON PARSER (for request bodies)
// ================================================================

// Parse a flat JSON object like {"key":"value","key2":123}
// Returns key-value pairs as strings
HashMap<string, string> parse_json_body(const string& body) {
    HashMap<string, string> result;
    int i = 0, n = (int)body.size();
    // Skip to first {
    while (i < n && body[i] != '{') i++;
    i++;
    while (i < n) {
        // Skip whitespace
        while (i < n && (body[i] == ' ' || body[i] == '\t' || body[i] == '\n' || body[i] == '\r')) i++;
        if (i >= n || body[i] == '}') break;
        if (body[i] == ',') { i++; continue; }
        // Read key
        if (body[i] != '"') { i++; continue; }
        i++;
        string key;
        while (i < n && body[i] != '"') { key += body[i]; i++; }
        i++; // skip closing quote
        // Skip to colon
        while (i < n && body[i] != ':') i++;
        i++;
        // Skip whitespace
        while (i < n && (body[i] == ' ' || body[i] == '\t')) i++;
        // Read value
        string val;
        if (i < n && body[i] == '"') {
            // String value
            i++;
            while (i < n && body[i] != '"') {
                if (body[i] == '\\' && i + 1 < n) { val += body[i+1]; i += 2; }
                else { val += body[i]; i++; }
            }
            i++; // skip closing quote
        } else {
            // Number/bool value
            while (i < n && body[i] != ',' && body[i] != '}' && body[i] != ' ') {
                val += body[i]; i++;
            }
        }
        result.insert(key, val);
    }
    return result;
}

// ================================================================
// SECTION 12: CSV IMPORT LOGIC (reusable for API)
// ================================================================

struct ImportResult {
    int imported, skipped;
    string errors;
};

ImportResult do_import_students(const string& csv_content) {
    ImportResult res = {0, 0, ""};
    istringstream stream(csv_content);
    string line;
    getline(stream, line); // header
    if (!str_contains_ci(line, "rollno") && !str_contains_ci(line, "roll_no")) {
        res.errors = "Header must contain RollNo";
        return res;
    }
    int lineNum = 1;
    while (getline(stream, line)) {
        lineNum++;
        line = str_trim(line);
        if (line.empty()) continue;
        vector<string> p = split_csv(line);
        if ((int)p.size() < 5) { res.skipped++; continue; }
        if (p[0].empty() || p[1].empty() || p[2].empty()) { res.skipped++; continue; }
        int sem = safe_stoi(p[3]), yr = safe_stoi(p[4]);
        if (sem < 1 || yr < 2000) { res.skipped++; continue; }
        Student s;
        s.rollNo = p[0]; s.name = p[1]; s.dept = p[2];
        s.semester = sem; s.year = yr;
        studentMap.insert(p[0], s);
        res.imported++;
    }
    save_students();
    return res;
}

ImportResult do_import_subjects(const string& csv_content) {
    ImportResult res = {0, 0, ""};
    istringstream stream(csv_content);
    string line;
    getline(stream, line); // header
    subjectMap.clear();
    int lineNum = 1;
    while (getline(stream, line)) {
        lineNum++;
        line = str_trim(line);
        if (line.empty()) continue;
        vector<string> p = split_csv(line);
        if ((int)p.size() < 3) { res.skipped++; continue; }
        float cred = safe_stof(p[2]);
        if (p[0].empty() || p[1].empty() || cred <= 0) { res.skipped++; continue; }
        Subject s;
        s.code = p[0]; s.name = p[1]; s.credits = cred;
        subjectMap.insert(p[0], s);
        res.imported++;
    }
    save_subjects();
    return res;
}

ImportResult do_import_enrollments(const string& csv_content) {
    ImportResult res = {0, 0, ""};
    if (studentMap.size() == 0) { res.errors = "No students loaded"; return res; }
    if (subjectMap.size() == 0) { res.errors = "No subjects loaded"; return res; }
    istringstream stream(csv_content);
    string line;
    getline(stream, line);
    int lineNum = 1;
    while (getline(stream, line)) {
        lineNum++;
        line = str_trim(line);
        if (line.empty()) continue;
        vector<string> p = split_csv(line);
        if ((int)p.size() < 4) { res.skipped++; continue; }
        if (!studentMap.find(p[0])) { res.skipped++; continue; }
        if (!subjectMap.find(p[1])) { res.skipped++; continue; }
        int sem = safe_stoi(p[2]), yr = safe_stoi(p[3]);
        if (find_enrollment(p[0], p[1], sem, yr)) { res.skipped++; continue; }
        Enrollment e;
        e.rollNo = p[0]; e.subCode = p[1]; e.semester = sem; e.year = yr;
        enrollments.push_back(e);
        res.imported++;
    }
    save_enrollments();
    return res;
}

ImportResult do_import_grades(const string& csv_content) {
    ImportResult res = {0, 0, ""};
    if (enrollments.empty()) { res.errors = "No enrollments loaded"; return res; }
    istringstream stream(csv_content);
    string line;
    getline(stream, line);

    vector<GradeEntry> incoming;
    int lineNum = 1;
    while (getline(stream, line)) {
        lineNum++;
        line = str_trim(line);
        if (line.empty()) continue;
        vector<string> p = split_csv(line);
        if ((int)p.size() < 5) { res.skipped++; continue; }
        string roll = p[0], sub = p[1], grade = p[2];
        int sem = safe_stoi(p[3]), yr = safe_stoi(p[4]);
        if (!studentMap.find(roll)) { res.skipped++; continue; }
        if (!is_valid_grade(grade)) { res.skipped++; continue; }
        if (!find_enrollment(roll, sub, sem, yr)) { res.skipped++; continue; }
        GradeEntry ge;
        ge.rollNo = roll; ge.subCode = sub; ge.grade = grade;
        ge.semester = sem; ge.year = yr;
        incoming.push_back(ge);
    }

    // Collect batch (sem,yr) combos
    vector<int> isems, iyrs;
    for (int i = 0; i < (int)incoming.size(); i++) {
        int s = incoming[i].semester, y = incoming[i].year;
        bool found = false;
        for (int j = 0; j < (int)isems.size(); j++)
            if (isems[j] == s && iyrs[j] == y) { found = true; break; }
        if (!found) { isems.push_back(s); iyrs.push_back(y); }
    }

    int auto_w = 0;
    for (int i = 0; i < (int)incoming.size(); i++) {
        GradeEntry& ge = incoming[i];
        GradeEntry* ex = find_grade(ge.rollNo, ge.subCode, ge.semester, ge.year);
        if (ex) ex->grade = ge.grade;
        else    gradeEntries.push_back(ge);
        res.imported++;
    }
    // Auto-W
    for (int i = 0; i < (int)enrollments.size(); i++) {
        const Enrollment& e = enrollments[i];
        bool in_batch = false;
        for (int j = 0; j < (int)isems.size(); j++)
            if (isems[j] == e.semester && iyrs[j] == e.year) { in_batch = true; break; }
        if (!in_batch) continue;
        if (!find_grade(e.rollNo, e.subCode, e.semester, e.year)) {
            GradeEntry ge;
            ge.rollNo = e.rollNo; ge.subCode = e.subCode;
            ge.grade = "W"; ge.semester = e.semester; ge.year = e.year;
            gradeEntries.push_back(ge);
            auto_w++;
        }
    }
    save_grades();
    res.errors = "auto_w:" + to_string(auto_w);
    return res;
}

// ================================================================
// SECTION 13: HTTP SERVER & API ROUTES
// ================================================================

int main() {
    load_all();
    cout << "  Data loaded: " << studentMap.size() << " students, "
         << subjectMap.size() << " subjects, "
         << enrollments.size() << " enrollments, "
         << gradeEntries.size() << " grades\n";

    httplib::Server svr;

    // Serve static files from public/
    svr.set_mount_point("/", "./public");

    // CORS headers for all responses
    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    svr.Options("/(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204;
    });

    // ---- STATS ----
    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        string json = "{" + json_int("students", studentMap.size()) + ","
                    + json_int("subjects", subjectMap.size()) + ","
                    + json_int("enrollments", (int)enrollments.size()) + ","
                    + json_int("grades", (int)gradeEntries.size()) + "}";
        res.set_content(json, "application/json");
    });

    // ---- LOGIN ----
    svr.Post("/api/login/admin", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* pwd = body.find("password");
        if (pwd && *pwd == "admin123") {
            res.set_content("{" + json_bool("success", true) + "}", "application/json");
        } else {
            res.status = 401;
            res.set_content("{" + json_bool("success", false) + "," + json_str("error", "Wrong password") + "}", "application/json");
        }
    });

    svr.Post("/api/login/student", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* roll = body.find("rollNo");
        string* pwd  = body.find("password");
        if (!roll || !pwd) {
            res.status = 400;
            res.set_content("{" + json_str("error", "Missing fields") + "}", "application/json");
            return;
        }
        Student* s = studentMap.find(*roll);
        if (!s) {
            res.status = 401;
            res.set_content("{" + json_str("error", "Roll number not found") + "}", "application/json");
            return;
        }
        if (*pwd != make_password(s->name, s->rollNo)) {
            res.status = 401;
            res.set_content("{" + json_str("error", "Incorrect password") + "}", "application/json");
            return;
        }
        res.set_content("{" + json_bool("success", true) + "," + json_str("rollNo", s->rollNo) + "," + json_str("name", s->name) + "}", "application/json");
    });

    // ---- STUDENTS ----
    svr.Get("/api/students", [](const httplib::Request& req, httplib::Response& res) {
        vector<Student> all = studentMap.all_values();
        string sort_by = req.has_param("sort") ? req.get_param_value("sort") : "";
        string filter  = req.has_param("filter") ? req.get_param_value("filter") : "";

        // Filter
        if (!filter.empty()) {
            vector<Student> filtered;
            for (int i = 0; i < (int)all.size(); i++) {
                if (str_contains_ci(all[i].name, filter) ||
                    str_contains_ci(all[i].rollNo, filter) ||
                    str_contains_ci(all[i].dept, filter))
                    filtered.push_back(all[i]);
            }
            all = filtered;
        }

        // Sort
        if (sort_by == "roll")
            sort_vec(all, [](const Student& a, const Student& b) { return a.rollNo < b.rollNo; });
        else if (sort_by == "name")
            sort_vec(all, [](const Student& a, const Student& b) { return a.name < b.name; });
        else if (sort_by == "cgpa")
            sort_vec(all, [](const Student& a, const Student& b) { return calc_cgpa(a.rollNo) > calc_cgpa(b.rollNo); });
        else if (sort_by == "dept")
            sort_vec(all, [](const Student& a, const Student& b) { return a.dept < b.dept; });

        string json = "[";
        for (int i = 0; i < (int)all.size(); i++) {
            if (i > 0) json += ",";
            json += student_to_json(all[i]);
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Get("/api/students/:roll", [](const httplib::Request& req, httplib::Response& res) {
        string roll = req.path_params.at("roll");
        Student* s = studentMap.find(roll);
        if (!s) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Student not found") + "}", "application/json");
            return;
        }
        res.set_content(student_detail_json(*s), "application/json");
    });

    svr.Post("/api/students", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* roll = body.find("rollNo");
        string* name = body.find("name");
        string* dept = body.find("dept");
        string* sem  = body.find("semester");
        string* yr   = body.find("year");
        if (!roll || !name || !dept || !sem || !yr ||
            roll->empty() || name->empty() || dept->empty()) {
            res.status = 400;
            res.set_content("{" + json_str("error", "All fields required") + "}", "application/json");
            return;
        }
        int semester = safe_stoi(*sem), year = safe_stoi(*yr);
        if (semester < 1 || year < 2000) {
            res.status = 400;
            res.set_content("{" + json_str("error", "Semester>=1, Year>=2000") + "}", "application/json");
            return;
        }
        if (studentMap.find(*roll)) {
            res.status = 409;
            res.set_content("{" + json_str("error", "Roll No already exists") + "}", "application/json");
            return;
        }
        Student s;
        s.rollNo = *roll; s.name = *name; s.dept = *dept;
        s.semester = semester; s.year = year;
        studentMap.insert(s.rollNo, s);
        save_students();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    svr.Put("/api/students/:roll", [](const httplib::Request& req, httplib::Response& res) {
        string roll = req.path_params.at("roll");
        Student* s = studentMap.find(roll);
        if (!s) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Not found") + "}", "application/json");
            return;
        }
        auto body = parse_json_body(req.body);
        string* name = body.find("name");
        string* dept = body.find("dept");
        string* sem  = body.find("semester");
        string* yr   = body.find("year");
        if (name && !name->empty()) s->name = *name;
        if (dept && !dept->empty()) s->dept = *dept;
        if (sem  && safe_stoi(*sem) >= 1)    s->semester = safe_stoi(*sem);
        if (yr   && safe_stoi(*yr) >= 2000)  s->year     = safe_stoi(*yr);
        save_students();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    svr.Delete("/api/students/:roll", [](const httplib::Request& req, httplib::Response& res) {
        string roll = req.path_params.at("roll");
        if (!studentMap.find(roll)) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Not found") + "}", "application/json");
            return;
        }
        studentMap.erase(roll);
        vector<Enrollment> ne;
        for (int i = 0; i < (int)enrollments.size(); i++)
            if (enrollments[i].rollNo != roll) ne.push_back(enrollments[i]);
        enrollments = ne;
        vector<GradeEntry> ng;
        for (int i = 0; i < (int)gradeEntries.size(); i++)
            if (gradeEntries[i].rollNo != roll) ng.push_back(gradeEntries[i]);
        gradeEntries = ng;
        save_all();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    // ---- SUBJECTS ----
    svr.Get("/api/subjects", [](const httplib::Request&, httplib::Response& res) {
        vector<Subject> all = subjectMap.all_values();
        sort_vec(all, [](const Subject& a, const Subject& b) { return a.code < b.code; });
        string json = "[";
        for (int i = 0; i < (int)all.size(); i++) {
            if (i > 0) json += ",";
            json += subject_to_json(all[i]);
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/subjects", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* code = body.find("code");
        string* name = body.find("name");
        string* cred = body.find("credits");
        if (!code || !name || !cred || code->empty() || name->empty()) {
            res.status = 400;
            res.set_content("{" + json_str("error", "All fields required") + "}", "application/json");
            return;
        }
        float credits = safe_stof(*cred);
        if (credits <= 0) {
            res.status = 400;
            res.set_content("{" + json_str("error", "Credits must be > 0") + "}", "application/json");
            return;
        }
        if (subjectMap.find(*code)) {
            res.status = 409;
            res.set_content("{" + json_str("error", "Code already exists") + "}", "application/json");
            return;
        }
        Subject s;
        s.code = *code; s.name = *name; s.credits = credits;
        subjectMap.insert(s.code, s);
        save_subjects();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    svr.Put("/api/subjects/:code", [](const httplib::Request& req, httplib::Response& res) {
        string code = req.path_params.at("code");
        Subject* s = subjectMap.find(code);
        if (!s) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Not found") + "}", "application/json");
            return;
        }
        auto body = parse_json_body(req.body);
        string* name = body.find("name");
        string* cred = body.find("credits");
        if (name && !name->empty()) s->name = *name;
        if (cred && safe_stof(*cred) > 0) s->credits = safe_stof(*cred);
        save_subjects();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    svr.Delete("/api/subjects/:code", [](const httplib::Request& req, httplib::Response& res) {
        string code = req.path_params.at("code");
        if (!subjectMap.find(code)) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Not found") + "}", "application/json");
            return;
        }
        subjectMap.erase(code);
        save_subjects();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    // ---- ENROLLMENT ----
    svr.Post("/api/enroll", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* roll = body.find("rollNo");
        string* sub  = body.find("subCode");
        string* sem  = body.find("semester");
        string* yr   = body.find("year");
        if (!roll || !sub || !sem || !yr) {
            res.status = 400;
            res.set_content("{" + json_str("error", "All fields required") + "}", "application/json");
            return;
        }
        if (!studentMap.find(*roll)) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Student not found") + "}", "application/json");
            return;
        }
        if (!subjectMap.find(*sub)) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Subject not found") + "}", "application/json");
            return;
        }
        int semester = safe_stoi(*sem), year = safe_stoi(*yr);
        if (find_enrollment(*roll, *sub, semester, year)) {
            res.status = 409;
            res.set_content("{" + json_str("error", "Already enrolled") + "}", "application/json");
            return;
        }
        Enrollment e;
        e.rollNo = *roll; e.subCode = *sub; e.semester = semester; e.year = year;
        enrollments.push_back(e);
        save_enrollments();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    // ---- GRADES ----
    svr.Get("/api/grades", [](const httplib::Request& req, httplib::Response& res) {
        string roll = req.has_param("roll") ? req.get_param_value("roll") : "";
        string json = "[";
        bool first = true;
        for (int i = 0; i < (int)gradeEntries.size(); i++) {
            if (!roll.empty() && gradeEntries[i].rollNo != roll) continue;
            if (!first) json += ",";
            json += grade_to_json(gradeEntries[i]);
            first = false;
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/grades", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* roll  = body.find("rollNo");
        string* sub   = body.find("subCode");
        string* grade = body.find("grade");
        string* sem   = body.find("semester");
        string* yr    = body.find("year");
        if (!roll || !sub || !grade || !sem || !yr) {
            res.status = 400;
            res.set_content("{" + json_str("error", "All fields required") + "}", "application/json");
            return;
        }
        if (!studentMap.find(*roll)) {
            res.status = 404;
            res.set_content("{" + json_str("error", "Student not found") + "}", "application/json");
            return;
        }
        if (!is_valid_grade(*grade)) {
            res.status = 400;
            res.set_content("{" + json_str("error", "Invalid grade") + "}", "application/json");
            return;
        }
        int semester = safe_stoi(*sem), year = safe_stoi(*yr);
        if (!find_enrollment(*roll, *sub, semester, year)) {
            res.status = 400;
            res.set_content("{" + json_str("error", "Not enrolled in this subject") + "}", "application/json");
            return;
        }
        GradeEntry* ge = find_grade(*roll, *sub, semester, year);
        if (ge) {
            ge->grade = *grade;
        } else {
            GradeEntry ng;
            ng.rollNo = *roll; ng.subCode = *sub; ng.grade = *grade;
            ng.semester = semester; ng.year = year;
            gradeEntries.push_back(ng);
        }
        save_grades();
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    // ---- CSV IMPORTS (file upload) ----
    svr.Post("/api/import/students", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("{" + json_str("error", "No file uploaded") + "}", "application/json");
            return;
        }
        auto file = req.get_file_value("file");
        ImportResult r = do_import_students(file.content);
        string json = "{" + json_int("imported", r.imported) + "," + json_int("skipped", r.skipped);
        if (!r.errors.empty()) json += "," + json_str("errors", r.errors);
        json += "}";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/import/subjects", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("{" + json_str("error", "No file uploaded") + "}", "application/json");
            return;
        }
        auto file = req.get_file_value("file");
        ImportResult r = do_import_subjects(file.content);
        string json = "{" + json_int("imported", r.imported) + "," + json_int("skipped", r.skipped);
        if (!r.errors.empty()) json += "," + json_str("errors", r.errors);
        json += "}";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/import/enrollments", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("{" + json_str("error", "No file uploaded") + "}", "application/json");
            return;
        }
        auto file = req.get_file_value("file");
        ImportResult r = do_import_enrollments(file.content);
        string json = "{" + json_int("imported", r.imported) + "," + json_int("skipped", r.skipped);
        if (!r.errors.empty()) json += "," + json_str("errors", r.errors);
        json += "}";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/import/grades", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("{" + json_str("error", "No file uploaded") + "}", "application/json");
            return;
        }
        auto file = req.get_file_value("file");
        ImportResult r = do_import_grades(file.content);
        string json = "{" + json_int("imported", r.imported) + "," + json_int("skipped", r.skipped);
        if (!r.errors.empty()) json += "," + json_str("errors", r.errors);
        json += "}";
        res.set_content(json, "application/json");
    });

    // ---- EXPORT ----
    svr.Get("/api/export/students", [](const httplib::Request& req, httplib::Response& res) {
        vector<Student> all = studentMap.all_values();
        string sort_by = req.has_param("sort") ? req.get_param_value("sort") : "roll";
        if (sort_by == "name")
            sort_vec(all, [](const Student& a, const Student& b) { return a.name < b.name; });
        else if (sort_by == "cgpa")
            sort_vec(all, [](const Student& a, const Student& b) { return calc_cgpa(a.rollNo) > calc_cgpa(b.rollNo); });
        else
            sort_vec(all, [](const Student& a, const Student& b) { return a.rollNo < b.rollNo; });

        string csv = "RollNo,Name,Dept,Semester,Year,CGPA\n";
        for (int i = 0; i < (int)all.size(); i++) {
            csv += all[i].rollNo + "," + all[i].name + "," + all[i].dept + ","
                 + to_string(all[i].semester) + "," + to_string(all[i].year) + ","
                 + float_to_str(calc_cgpa(all[i].rollNo), 2) + "\n";
        }
        res.set_header("Content-Disposition", "attachment; filename=students_export.csv");
        res.set_content(csv, "text/csv");
    });

    // ---- REPORTS ----
    svr.Get("/api/reports/leaderboard", [](const httplib::Request&, httplib::Response& res) {
        vector<Student> all = studentMap.all_values();
        sort_vec(all, [](const Student& a, const Student& b) {
            return calc_cgpa(a.rollNo) > calc_cgpa(b.rollNo);
        });
        int show = (int)all.size() < 20 ? (int)all.size() : 20;
        string json = "[";
        for (int i = 0; i < show; i++) {
            if (i > 0) json += ",";
            json += "{" + json_int("rank", i + 1) + ","
                  + json_str("rollNo", all[i].rollNo) + ","
                  + json_str("name", all[i].name) + ","
                  + json_str("dept", all[i].dept) + ","
                  + json_float("cgpa", calc_cgpa(all[i].rollNo)) + "}";
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Get("/api/reports/stats", [](const httplib::Request&, httplib::Response& res) {
        vector<Student> all = studentMap.all_values();
        float sum = 0, best = -1, worst = 11;
        string best_name, worst_name;
        int graded = 0;
        for (int i = 0; i < (int)all.size(); i++) {
            float c = calc_cgpa(all[i].rollNo);
            if (c <= 0.0f) continue;
            graded++; sum += c;
            if (c > best)  { best = c;  best_name  = all[i].name; }
            if (c < worst) { worst = c; worst_name = all[i].name; }
        }
        string json = "{";
        json += json_int("totalStudents", studentMap.size()) + ",";
        json += json_int("totalSubjects", subjectMap.size()) + ",";
        json += json_int("totalEnrollments", (int)enrollments.size()) + ",";
        json += json_int("totalGrades", (int)gradeEntries.size()) + ",";
        json += json_int("gradedStudents", graded) + ",";
        json += json_float("avgCgpa", graded > 0 ? sum / graded : 0) + ",";
        json += json_float("highestCgpa", best > 0 ? best : 0) + ",";
        json += json_str("highestName", best_name) + ",";
        json += json_float("lowestCgpa", worst < 11 ? worst : 0) + ",";
        json += json_str("lowestName", worst_name);
        json += "}";
        res.set_content(json, "application/json");
    });

    svr.Get("/api/reports/department", [](const httplib::Request& req, httplib::Response& res) {
        string dept = req.has_param("dept") ? req.get_param_value("dept") : "";
        vector<Student> all = studentMap.all_values();
        vector<Student> filtered;
        for (int i = 0; i < (int)all.size(); i++)
            if (dept.empty() || str_contains_ci(all[i].dept, dept))
                filtered.push_back(all[i]);
        sort_vec(filtered, [](const Student& a, const Student& b) {
            return calc_cgpa(a.rollNo) > calc_cgpa(b.rollNo);
        });
        string json = "[";
        for (int i = 0; i < (int)filtered.size(); i++) {
            if (i > 0) json += ",";
            json += student_to_json(filtered[i]);
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    // ---- RESET ----
    svr.Post("/api/reset", [](const httplib::Request& req, httplib::Response& res) {
        auto body = parse_json_body(req.body);
        string* conf = body.find("confirm");
        if (!conf || *conf != "CONFIRM") {
            res.status = 400;
            res.set_content("{" + json_str("error", "Type CONFIRM to proceed") + "}", "application/json");
            return;
        }
        studentMap.clear(); subjectMap.clear();
        enrollments.clear(); gradeEntries.clear();
        { ofstream f(F_STUDENTS,    ios::trunc); }
        { ofstream f(F_SUBJECTS,    ios::trunc); }
        { ofstream f(F_ENROLLMENTS, ios::trunc); }
        { ofstream f(F_GRADES,      ios::trunc); }
        res.set_content("{" + json_bool("success", true) + "}", "application/json");
    });

    // Start server
    cout << "\n============================================\n";
    cout << "  SGMS Server running on http://localhost:3000\n";
    cout << "  Open this URL in your browser\n";
    cout << "============================================\n\n";

    svr.listen("0.0.0.0", 3000);
    return 0;
}
