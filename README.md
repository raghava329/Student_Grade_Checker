# Student Grade Management System (SGMS)

A robust, full-stack student grade management system. Built with a C++ REST API backend and a vanilla HTML/CSS/JS single-page web application frontend.

## Features

- **Admin Portal**: Import CSV data, manage students/subjects, process enrollments, and assign grades.
- **Student Portal**: Secure login to track personal grades, SGPA, CGPA, and academic trajectory.
- **Persistent Storage**: Lightweight CSV-based data persistence.

## Tech Stack

- **Backend**: Standard C++ (g++ 17)
- **Networking**: `cpp-httplib`
- **Frontend**: Vanilla HTML / CSS / JavaScript

## Prerequisites

A standard C++ compiler is required. 

> **Windows Note:** MSYS2 MinGW does not natively include the Windows Socket libraries required by the HTTP server. You must link `ws2_32` during compilation (`-lws2_32`).

---

## Installation & Setup

### Windows

1. **Compiler**: Install MSYS2 MinGW. In the MSYS2 UCRT64 terminal, run `pacman -S mingw-w64-ucrt-x86_64-gcc`. Add `C:\msys64\ucrt64\bin` to your System PATH.
2. **Clone**: `git clone https://github.com/raghava329/Student_Grade_Checker.git`
3. **Compile**: `g++ -std=c++17 -O2 -o server.exe main.cpp -lws2_32`
4. **Run**: `.\server.exe`

### macOS

1. **Compiler**: Install Apple Command Line Tools via `xcode-select --install`.
2. **Clone**: `git clone https://github.com/raghava329/Student_Grade_Checker.git`
3. **Compile**: `g++ -std=c++17 -pthread -O2 -o server main.cpp`
4. **Run**: `chmod +x server && ./server`

### Linux

1. **Compiler**: Install build tools (e.g., `sudo apt update && sudo apt install build-essential`).
2. **Clone**: `git clone https://github.com/raghava329/Student_Grade_Checker.git`
3. **Compile**: `g++ -std=c++17 -pthread -O2 -o server main.cpp`
4. **Run**: `chmod +x server && ./server`

---

## Usage

After starting the server, open your web browser and navigate to `http://localhost:3000`.

### Authentication

- **Admin Login**: `admin123` (Initial setup: Import `students.csv`, `subjects.csv`, `student_subjects.csv`, and `studet_grades.csv` sequentially).
- **Student Login**: Password format is the first 5 lowercase letters of the student's name + the last 4 characters of their roll number (e.g., `sriram1009`).

---

## API Endpoints

| Method | Endpoint | Description |
|--------|---------|-------------|
| GET | `/api/stats` | System statistics |
| POST | `/api/login/admin` | Admin authentication |
| POST | `/api/login/student` | Student authentication |
| GET | `/api/students?sort=roll&filter=` | List students |
| GET | `/api/students/:roll` | Get student details and grades |
| POST | `/api/students` | Add a new student |
| PUT | `/api/students/:roll` | Update student information |
| DELETE | `/api/students/:roll` | Delete a student |
| GET | `/api/subjects` | List subjects |
| POST | `/api/subjects` | Add a new subject |
| PUT | `/api/subjects/:code` | Update subject |
| DELETE | `/api/subjects/:code` | Delete a subject |
| POST | `/api/enroll` | Enroll a student into a subject |
| GET | `/api/grades?roll=` | Retrieve grades |
| POST | `/api/grades` | Assign or update a grade |
| POST | `/api/import/students` | Batch import students (CSV) |
| POST | `/api/import/subjects` | Batch import subjects (CSV) |
| POST | `/api/import/enrollments` | Batch import enrollments (CSV) |
| POST | `/api/import/grades` | Batch import grades (CSV) |
| GET | `/api/export/students` | Export students (CSV) |
| POST | `/api/reset` | Factory reset database |

---

## License

Developed for the Data Structures and Algorithms course (CS2020) at IIT Jodhpur (2025-26).
