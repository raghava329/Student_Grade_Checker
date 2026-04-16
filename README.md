# Student Grade Management System (SGMS)

A full-stack student grade management system built for the Data Structures and Algorithms course at IIT Jodhpur. The backend is written in C++ with a REST API, and the frontend is a single-page web application using vanilla HTML/CSS/JS.

> All core data structures (HashMap, Merge Sort, etc.) are implemented from scratch -- no STL containers like `map` or `unordered_map` were used.

---

## What does it do?

- **Admin Portal** -- Import student/subject/grade data from CSV files, add/edit/delete students & subjects, enroll students, assign grades, view statistics and leaderboards.
- **Student Portal** -- Students can log in with their roll number and auto-generated password to view their grades, SGPA, CGPA, and academic history.
- **Reports** -- Class statistics, department-wise filtering, top 20 leaderboard by CGPA.
- **Persistence** -- All data is stored in CSV files and loaded automatically when the server restarts.

---

## Screenshots

After running the server and opening `http://localhost:3000`, you'll see:

1. **Login Screen** -- Choose between Admin or Student portal
2. **Admin Dashboard** -- Sidebar navigation with import, student/subject management, grade assignment, and reports
3. **Student Detail** -- Click any student to see their full academic record with semester-wise breakdown
4. **Dark Mode** -- Toggle between light and dark themes (preference saved in browser)

---

## Tech Stack

| Component | Technology |
|-----------|------------|
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
| **Custom Pair** | Key-value storage in HashMap | -- |
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
├── main.cpp              # C++ backend -- server, API, data structures, everything
├── httplib.h             # HTTP library (third-party, single header)
├── server.exe            # Compiled executable (Windows)
├── .gitignore            # Files excluded from git
│
├── public/               # Frontend files (served by C++ server)
│   ├── index.html        # Single page HTML structure
│   ├── style.css         # Complete styling + dark theme + responsive
│   └── app.js            # Frontend SPA logic -- API calls, rendering, navigation
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
├── students.csv          # Sample data -- student records
├── subjects.csv          # Sample data -- subject list
├── student_subjects.csv  # Sample data -- enrollment mapping
├── studet_grades.csv     # Sample data -- grades
│
├── README.md             # This file
├── SGMS_Documentation_v3.docx  # Detailed project documentation
└── document.docx         # Additional documentation
```

---

## How to Run

This section covers **every step from scratch** for Windows, macOS, and Linux. Follow the instructions for your operating system.

---

### Step 1: Install the C++ Compiler

<details>
<summary><strong>Windows (MinGW)</strong></summary>

1. Download MinGW installer from: https://sourceforge.net/projects/mingw/
2. Run the installer. In the **MinGW Installation Manager**, check these packages:
   - `mingw32-gcc-g++`
   - `mingw32-gcc`
3. Click **Installation > Apply Changes**.
4. Add MinGW to your system PATH:
   - Open **Start Menu** > search **"Environment Variables"** > click **"Edit the system environment variables"**.
   - Click **Environment Variables** > under **System variables**, find `Path` > click **Edit**.
   - Click **New** and add: `C:\MinGW\bin`
   - Click **OK** on all dialogs.
5. Open a **new** Command Prompt or PowerShell and verify:
   ```bash
   g++ --version
   ```
   You should see something like `g++ (MinGW.org GCC Build-2) 9.2.0` or similar.

> **If you have an older MinGW** that does not support `-std=c++14`, you can replace it with `-std=c++11` in the compile command below.

</details>

<details>
<summary><strong>macOS</strong></summary>

1. Open **Terminal** (press `Cmd + Space`, type "Terminal", press Enter).
2. Install Xcode Command Line Tools (includes `clang++` which works as `g++`):
   ```bash
   xcode-select --install
   ```
3. A dialog will pop up asking to install. Click **Install** and wait for it to finish.
4. Verify:
   ```bash
   g++ --version
   ```
   You should see something like `Apple clang version 15.0.0` or similar. This is fine -- Apple's `g++` is actually `clang++` under the hood and it works perfectly.

> **Alternative (Homebrew):** If you prefer the real GCC instead of Apple Clang:
> ```bash
> /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
> brew install gcc
> ```
> Then use `g++-14` (or whatever version number Homebrew installed) instead of `g++`.

</details>

<details>
<summary><strong>Linux (Ubuntu / Debian)</strong></summary>

1. Open a terminal.
2. Update package lists and install build tools:
   ```bash
   sudo apt update
   sudo apt install build-essential git
   ```
3. Verify:
   ```bash
   g++ --version
   ```
   You should see something like `g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`.

</details>

<details>
<summary><strong>Linux (Fedora / RHEL)</strong></summary>

1. Open a terminal.
2. Install the development tools group:
   ```bash
   sudo dnf groupinstall "Development Tools"
   ```
3. Verify:
   ```bash
   g++ --version
   ```

</details>

<details>
<summary><strong>Linux (Arch)</strong></summary>

1. Open a terminal.
2. Install gcc:
   ```bash
   sudo pacman -S base-devel
   ```
3. Verify:
   ```bash
   g++ --version
   ```

</details>

---

### Step 2: Get the Project Files

**Option A -- Clone with Git:**
```bash
git clone https://github.com/your-username/sgms.git
cd sgms
```

**Option B -- Download ZIP:**
1. Download the project ZIP file.
2. Extract it to a folder (e.g., `Desktop/sgms`).
3. Open a terminal and navigate to that folder:
   ```bash
   # macOS / Linux
   cd ~/Desktop/sgms

   # Windows (PowerShell)
   cd $HOME\Desktop\sgms

   # Windows (CMD)
   cd %USERPROFILE%\Desktop\sgms
   ```

---

### Step 3: Compile the Backend

Open a terminal **inside the project folder** (where `main.cpp` is located) and run:

**Windows (MinGW):**
```bash
g++ -std=c++14 -O2 -o server.exe main.cpp -lws2_32
```
- `-lws2_32` links the Windows Sockets library (required for networking on Windows).
- If compilation fails with an error about `-std=c++14`, try `-std=c++11` instead.

**macOS:**
```bash
g++ -std=c++14 -pthread -O2 -o server main.cpp
```
- `-pthread` enables POSIX thread support (required by the HTTP library).
- You can also use `clang++ -std=c++14 -pthread -O2 -o server main.cpp`.

**Linux:**
```bash
g++ -std=c++14 -pthread -O2 -o server main.cpp
```
- Same flags as macOS.

If successful, you will see a new file: `server.exe` (Windows) or `server` (macOS/Linux).

---

### Step 4: Run the Server

**Windows:**
```bash
.\server.exe
```

**macOS / Linux:**
```bash
chmod +x server    # make it executable (only needed once)
./server
```

You should see this output:
```
  Data loaded: X students, Y subjects, Z enrollments, W grades

============================================
  SGMS Server running on http://localhost:3000
  Open this URL in your browser
============================================
```

**Keep this terminal window open!** The server runs until you press `Ctrl + C`.

---

### Step 5: Open the Website

1. Open any modern web browser (Chrome, Firefox, Safari, Edge).
2. Go to: **http://localhost:3000**

You will see the SGMS login screen.

---

### Step 6: Log In and Import Data

**Admin Login:**
- Password: `admin123`

**Import CSV data (first time only):**
1. Go to **Import** in the admin sidebar.
2. Import files **in this exact order**:
   1. `students.csv`
   2. `subjects.csv`
   3. `student_subjects.csv`
   4. `studet_grades.csv`

**Student Login:**
- Roll number + password
- Password = first 5 lowercase letters of name + last 4 characters of roll number
- Example: Student "Sriram Kumar" with roll "B24CI1009" -> password: `sriram1009`

---

### Step 7: Stopping the Server

Go back to the terminal where the server is running and press **Ctrl + C**.

---

### Troubleshooting

| Problem | Cause | Solution |
|---------|-------|----------|
| `g++: command not found` | Compiler not installed or not in PATH | Follow Step 1 for your OS |
| `fatal error: 'winsock2.h' not found` | Missing Windows SDK (rare on MinGW) | Reinstall MinGW with all packages selected |
| `Permission denied` when running `./server` | File not marked executable (macOS/Linux) | Run `chmod +x server` first |
| Server starts but browser shows blank page | `public/` folder is missing or misplaced | Make sure `public/` is in the same folder as the `server` binary |
| `Address already in use` error | Port 3000 is taken by another app | Close the other app, or change the port in `main.cpp` |
| Compilation fails with `-std=c++14` error | Old compiler version | Replace `-std=c++14` with `-std=c++11` |
| macOS: `'httplib.h' file not found` | Not running from the project directory | `cd` into the folder that contains `main.cpp` and `httplib.h` |

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

**SGPA** = Sum(grade_points x credits) / Sum(credits) for one semester

**CGPA** = Sum(grade_points x credits) / Sum(credits) across all semesters

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

- Admin password is hardcoded (`admin123`) -- not suitable for production use
- No session management -- authentication is per-request
- Student passwords are derived from name + roll number (predictable)
- Single-threaded server -- handles one request at a time
- No input validation for XSS on the backend (frontend sanitization added)

These are acceptable for a college project but would need to be addressed for any real deployment.

---

## License

This project was built as a course assignment for the Data Structures and Algorithms course (CS2020) at IIT Jodhpur, Semester 4, 2025-26.
