#include <bits/stdc++.h>
#include <httplib.h>
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