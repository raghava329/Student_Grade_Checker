# Student Grade Management System (SGMS)

A full-stack student grade management system built for the Data Structures and Algorithms course at IIT Jodhpur. The backend is written in C++ with a REST API, and the frontend is a single-page web application using vanilla HTML/CSS/JS.

> All core data structures (HashMap, Merge Sort, etc.) are implemented from scratch — no STL containers like `map` or `unordered_map` were used.

---

## What does it do?

- **Admin Portal** — Import student/subject/grade data from CSV files, add/edit/delete students & subjects, enroll students, assign grades, view statistics and leaderboards.
- **Student Portal** — Students can log in with their roll number and auto-generated password to view their grades, SGPA, CGPA, and academic history.
- **Reports** — Class statistics, department-wise filtering, top 20 leaderboard by CGPA.
- **Persistence** — All data is stored in CSV files and loaded automatically when the server restarts.

---

## Screenshots

After running the server and opening `http://localhost:3000`, you'll see:

1. **Login Screen** — Choose between Admin or Student portal
2. **Admin Dashboard** — Sidebar navigation with import, student/subject management, grade assignment, and reports
3. **Student Detail** — Click any student to see their full academic record with semester-wise breakdown
4. **Dark Mode** — Toggle between light and dark themes (preference saved in browser)

---

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Backend | C++ (compiled with g++) |
| HTTP Server | [cpp-httplib](https://github.com/yhirose/cpp-httplib) (single header, included) |
| Frontend | Vanilla HTML + CSS + JavaScript |
| Data Storage | CSV files (no database needed) |
| Font | [Inter](https://fonts.google.com/specimen/Inter) via Google Fonts |

---

## Custom Data Structures & Algorithms

Since this is a DSA course project, we implemented the following from scratch instead of using STL:

| Data Structure / Algorithm | Where it's used | Time Complexity |
|---------------------------|-----------------|-----------------|
| **HashMap** (chaining + djb2 hash) | Student and Subject storage | O(1) avg lookup/insert |
| **Custom Pair** | Key-value storage in HashMap | — |
| **Merge Sort** | Sorting students by name/CGPA/roll | O(n log n) |
| **Binary Search** | Searching sorted arrays | O(log n) |
| **Manual JSON Builder** | API response serialization | O(n) |
| **Manual JSON Parser** | Parsing request bodies | O(n) |
| **Manual CSV Parser** | Reading/writing data files | O(n) |

String utility functions (trim, toLower, stoi, etc.) use STL since string manipulation isn't the focus of the DSA component.

---

## Project Structure

```
SGMS/
├── main.cpp              # C++ backend — server, API, data structures, everything
├── httplib.h             # HTTP library (third-party, single header)
├── server.exe            # Compiled executable (Windows)
├── .gitignore            # Files excluded from git
│
├── public/               # Frontend files (served by C++ server)
│   ├── index.html        # Single page HTML structure
│   ├── style.css         # Complete styling + dark theme + responsive
│   └── app.js            # Frontend SPA logic — API calls, rendering, navigation
│
├── contributions/        # Team member work division
│   ├── member1_backend_core.md
│   ├── member2_api_server.md
│   ├── member3_frontend_ui.md
│   └── member4_frontend_logic.md
│
├── explanations/         # Detailed code explanations
│   ├── main_cpp_explained.md
│   ├── app_js_explained.md
│   ├── index_html_explained.md
│   └── style_css_explained.md
│
├── students.csv          # Sample data — student records
├── subjects.csv          # Sample data — subject list
├── student_subjects.csv  # Sample data — enrollment mapping
├── studet_grades.csv     # Sample data — grades
│
├── README.md             # This file
├── SGMS_Documentation_v3.docx  # Detailed project documentation
└── document.docx         # Additional documentation
```

---

## How to Run

### Prerequisites

- **g++ compiler** (MinGW on Windows, or GCC on Linux/Mac)
- A modern web browser (Chrome, Firefox, Edge)
- No other dependencies — everything is self-contained

### Compile

**Windows:**
```bash
g++ -std=c++14 -O2 -o server.exe main.cpp -lws2_32
```

**Linux/Mac:**
```bash
g++ -std=c++14 -pthread -O2 -o server main.cpp
```

The `-lws2_32` flag on Windows links the Winsock library (needed for networking). On Linux, `-pthread` is needed instead.

### Run

```bash
./server.exe        # Windows
./server            # Linux/Mac
```

You should see:
```
  Data loaded: X students, Y subjects, Z enrollments, W grades

============================================
  SGMS Server running on http://localhost:3000
  Open this URL in your browser
============================================
```

### Use

1. Open `http://localhost:3000` in your browser
2. **Admin login:** Password is `admin123`
3. Import CSV data in order: Students → Subjects → Student-Subjects → Grades
4. **Student login:** Roll number + password (first 5 lowercase letters of name + last 4 chars of roll number)
   - Example: Student "Sriram Kumar" with roll "B24CI1009" → password: `sriram1009`

---

## CSV File Formats

### students.csv
```
RollNo,Name,Dept,Semester,Year
B24CI1001,Aarav Sharma,Computer Science,2,2025
```

### subjects.csv
```
Code,Name,Credits
CSL101,Introduction to Programming,4.0
```

### student_subjects.csv
```
RollNo,SubCode,Semester,Year
B24CI1001,CSL101,2,2025
```

### studet_grades.csv
```
RollNo,SubCode,Grade,Semester,Year
B24CI1001,CSL101,A,2,2025
```

Valid grades: `A`, `A-`, `B`, `B-`, `C`, `C-`, `D`, `F`, `W`

---

## Grading System

| Grade | Points | Counts for GPA? |
|-------|--------|-----------------|
| A | 10.0 | Yes |
| A- | 9.0 | Yes |
| B | 8.0 | Yes |
| B- | 7.0 | Yes |
| C | 6.0 | Yes |
| C- | 5.0 | Yes |
| D | 4.0 | Yes |
| F | 0.0 | No |
| W | 0.0 | No (Withdrawn) |

**SGPA** = Σ(grade_points × credits) / Σ(credits) for one semester

**CGPA** = Σ(grade_points × credits) / Σ(credits) across all semesters

---

## API Endpoints

<details>
<summary>Click to expand full API reference</summary>

| Method | Endpoint | Description |
|--------|---------|-------------|
| GET | `/api/stats` | System statistics |
| POST | `/api/login/admin` | Admin authentication |
| POST | `/api/login/student` | Student authentication |
| GET | `/api/students?sort=roll&filter=` | List students (sortable, filterable) |
| GET | `/api/students/:roll` | Student detail with grades |
| POST | `/api/students` | Add student |
| PUT | `/api/students/:roll` | Update student |
| DELETE | `/api/students/:roll` | Delete student + cascade |
| GET | `/api/subjects` | List subjects |
| POST | `/api/subjects` | Add subject |
| PUT | `/api/subjects/:code` | Update subject |
| DELETE | `/api/subjects/:code` | Delete subject |
| POST | `/api/enroll` | Enroll student in subject |
| GET | `/api/grades?roll=` | Get grades |
| POST | `/api/grades` | Assign/update grade |
| POST | `/api/import/students` | Upload students CSV |
| POST | `/api/import/subjects` | Upload subjects CSV |
| POST | `/api/import/enrollments` | Upload enrollments CSV |
| POST | `/api/import/grades` | Upload grades CSV |
| GET | `/api/export/students` | Download students CSV |
| GET | `/api/reports/leaderboard` | Top 20 by CGPA |
| GET | `/api/reports/stats` | Detailed statistics |
| GET | `/api/reports/department?dept=` | Filter by department |
| POST | `/api/reset` | Reset all data |

</details>

---

## Team Contributions

| Member | Area | Details |
|--------|------|---------|
| Member 1 | Backend Core | Data structures (HashMap, Merge Sort), grade logic, CSV persistence |
| Member 2 | API Server | REST endpoints, CSV import/export, JSON parsing |
| Member 3 | Frontend UI | HTML structure, CSS design system, dark theme, responsive layout |
| Member 4 | Frontend Logic | JavaScript SPA, API integration, dynamic rendering, form handling |

See the `contributions/` folder for detailed breakdowns of each member's work.

---

## Known Limitations

- Admin password is hardcoded (`admin123`) — not suitable for production use
- No session management — authentication is per-request
- Student passwords are derived from name + roll number (predictable)
- Single-threaded server — handles one request at a time
- No input validation for XSS on the backend (frontend sanitization added)

These are acceptable for a college project but would need to be addressed for any real deployment.

---

## License

This project was built as a course assignment for the Data Structures and Algorithms course (CS2020) at IIT Jodhpur, Semester 4, 2025-26.
