#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

// ========================= basic structs ========================= //

struct Subject {
    string name;
    string grade;

    Subject(string n) {
        name = n;
        grade = "N";
    }

    Subject(string n, string g) {
        name = n;
        grade = g;
    }
};

struct Student {
    string id;
    string name;
    vector<Subject> subjects;
    double gpa;

    Student() {
        id = "";
        name = "";
        gpa = 0.0;
    }

    Student(string i, string n) {
        id = i;
        name = n;
        gpa = 0.0;
    }

    Student(string i, string n, vector<Subject> s) {
        id = i;
        name = n;
        subjects = s;
        gpa = 0.0;
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

// ========================= HASH TABLE ========================= //

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