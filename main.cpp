#include <bits/stdc++.h>
using namespace std;


//sumedha made changes
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

