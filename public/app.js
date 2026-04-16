// ================================================================
// SGMS Frontend — Single Page Application
// ================================================================

const API = ""; // same origin

// ============ STATE ============
let currentPage = "import";
let currentSort = "roll";
let searchTimer = null;
let loggedInStudent = null;

// ============ DOM HELPERS ============
const $ = (id) => document.getElementById(id);
const show = (id) => {
  $(id).classList.add("active");
};
const hide = (id) => {
  $(id).classList.remove("active");
};

// ============ THEME ============
function initTheme() {
  const saved = localStorage.getItem("sgms-theme") || "light";
  document.documentElement.setAttribute("data-theme", saved);
  updateThemeButton(saved);
}

function toggleTheme() {
  const current = document.documentElement.getAttribute("data-theme");
  const next = current === "dark" ? "light" : "dark";
  document.documentElement.setAttribute("data-theme", next);
  localStorage.setItem("sgms-theme", next);
  updateThemeButton(next);
}

function updateThemeButton(theme) {
  document.querySelectorAll(".theme-toggle").forEach((btn) => {
    btn.textContent = theme === "dark" ? "☀️" : "🌙";
  });
}

// ============ TOAST ============
function showToast(message, type = "info") {
  const container = $("toast-container");
  const toast = document.createElement("div");
  toast.className = `toast ${type}`;
  toast.textContent = message;
  container.appendChild(toast);
  setTimeout(() => toast.remove(), 3000);
}

// ============ API HELPERS ============
async function api(method, path, body = null) {
  const opts = { method, headers: {} };
  if (body && !(body instanceof FormData)) {
    opts.headers["Content-Type"] = "application/json";
    opts.body = JSON.stringify(body);
  } else if (body instanceof FormData) {
    opts.body = body;
  }
  const res = await fetch(API + path, opts);
  const data = await res.json();
  if (!res.ok) throw data;
  return data;
}

// ============ GRADE HELPERS ============
function gradeClass(grade) {
  if (grade === "A" || grade === "A-") return "grade-green";
  if (grade === "B" || grade === "B-" || grade === "C" || grade === "C-")
    return "grade-yellow";
  return "grade-red";
}

function cgpaClass(cgpa) {
  if (cgpa >= 8) return "grade-green";
  if (cgpa >= 5) return "grade-yellow";
  return "grade-red";
}

function cgpaColor(cgpa) {
  if (cgpa >= 8) return "#0d904f";
  if (cgpa >= 5) return "#b45309";
  return "#c62828";
}

function getInitials(name) {
  return name
    .split(" ")
    .map((w) => w[0])
    .join("")
    .toUpperCase()
    .substring(0, 2);
}

// Escape strings for safe use inside HTML attributes (onclick handlers etc.)
function escapeAttr(str) {
  return String(str)
    .replace(/\\/g, "\\\\")
    .replace(/'/g, "\\'")
    .replace(/"/g, "&quot;");
}

function escapeHtml(str) {
  return String(str)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;");
}

// ============ SCREEN MANAGEMENT ============
function showScreen(screen) {
  document
    .querySelectorAll(".screen")
    .forEach((s) => s.classList.remove("active"));
  $(screen).classList.add("active");
}

// ============ LOGIN ============
async function loadLoginStats() {
  try {
    const stats = await api("GET", "/api/stats");
    $(
      "login-stats"
    ).textContent = `${stats.students} Students • ${stats.subjects} Subjects • ${stats.enrollments} Enrollments • ${stats.grades} Grades`;
  } catch (e) {
    $("login-stats").textContent = "Server not connected";
  }
}

async function loginAdmin() {
  const pwd = $("admin-password").value;
  $("admin-error").textContent = "";
  try {
    await api("POST", "/api/login/admin", { password: pwd });
    showScreen("admin-screen");
    navigateTo("import");
  } catch (e) {
    $("admin-error").textContent = e.error || "Login failed";
  }
}

async function loginStudent() {
  const roll = $("student-roll").value.trim();
  const pwd = $("student-password").value;
  $("student-error").textContent = "";
  try {
    const res = await api("POST", "/api/login/student", {
      rollNo: roll,
      password: pwd,
    });
    loggedInStudent = roll;
    showScreen("student-screen");
    loadStudentPortal(roll);
  } catch (e) {
    $("student-error").textContent = e.error || "Login failed";
  }
}

function logout() {
  loggedInStudent = null;
  $("admin-password").value = "";
  $("student-roll").value = "";
  $("student-password").value = "";
  $("admin-error").textContent = "";
  $("student-error").textContent = "";
  showScreen("login-screen");
  loadLoginStats();
}

// ============ ENTER KEY SUPPORT ============
document.addEventListener("keydown", (e) => {
  if (e.key !== "Enter") return;
  if (document.activeElement === $("admin-password")) loginAdmin();
  if (
    document.activeElement === $("student-password") ||
    document.activeElement === $("student-roll")
  )
    loginStudent();
});

// ============ SIDEBAR NAVIGATION ============
function toggleSidebar() {
  $("sidebar").classList.toggle("open");
}

function navigateTo(page) {
  currentPage = page;
  $("sidebar").classList.remove("open");

  // Update active link
  document.querySelectorAll(".sidebar-link").forEach((link) => {
    link.classList.toggle("active", link.dataset.page === page);
  });

  const titles = {
    import: "Import Data",
    students: "All Students",
    "student-add": "Add Student",
    subjects: "All Subjects",
    "subject-add": "Add Subject",
    enroll: "Enroll Student",
    "assign-grade": "Assign Grade",
  };
  $("page-title").textContent = titles[page] || page;

  // Load content
  const loaders = {
    import: loadImportPage,
    students: loadStudentsPage,
    "student-add": loadAddStudentPage,
    subjects: loadSubjectsPage,
    "subject-add": loadAddSubjectPage,
    enroll: loadEnrollPage,
    "assign-grade": loadAssignGradePage,
  };
  if (loaders[page]) loaders[page]();
}

// ============ MODAL ============
function openModal(html) {
  $("modal-content").innerHTML = html;
  $("modal-overlay").classList.add("active");
}

function closeModal() {
  $("modal-overlay").classList.remove("active");
}

// ============ IMPORT PAGE ============
async function loadImportPage() {
  const stats = await api("GET", "/api/stats");
  const area = $("content-area");

  area.innerHTML = `
        <div class="import-card" id="import-students">
            <div class="import-card-header">
                <div class="import-card-title">📋 1. Import Students</div>
                <span class="import-badge ${
                  stats.students > 0 ? "loaded" : "empty"
                }">
                    ${stats.students > 0 ? stats.students + " loaded" : "empty"}
                </span>
            </div>
            <div class="import-hint">Format: RollNo, Name, Dept, Semester, Year</div>
            <input type="file" id="file-students" accept=".csv" style="display:none" onchange="uploadCSV('students')">
            <button class="file-upload-btn" onclick="$('file-students').click()">📁 Choose CSV File</button>
            <div id="result-students"></div>
        </div>

        <div class="import-card" id="import-subjects">
            <div class="import-card-header">
                <div class="import-card-title">📚 2. Import Subjects</div>
                <span class="import-badge ${
                  stats.subjects > 0 ? "loaded" : "empty"
                }">
                    ${stats.subjects > 0 ? stats.subjects + " loaded" : "empty"}
                </span>
            </div>
            <div class="import-hint">Format: Code, Name, Credits  ⚠️ Replaces existing</div>
            <input type="file" id="file-subjects" accept=".csv" style="display:none" onchange="uploadCSV('subjects')">
            <button class="file-upload-btn" onclick="$('file-subjects').click()">📁 Choose CSV File</button>
            <div id="result-subjects"></div>
        </div>

        <div class="import-card ${
          stats.students === 0 || stats.subjects === 0 ? "disabled" : ""
        }" id="import-enrollments">
            <div class="import-card-header">
                <div class="import-card-title">📝 3. Import Student-Subjects</div>
                <span class="import-badge ${
                  stats.enrollments > 0 ? "loaded" : "empty"
                }">
                    ${
                      stats.enrollments > 0
                        ? stats.enrollments + " loaded"
                        : "empty"
                    }
                </span>
            </div>
            <div class="import-hint">Format: RollNo, SubjectCode, Semester, Year</div>
            <input type="file" id="file-enrollments" accept=".csv" style="display:none" onchange="uploadCSV('enrollments')">
            <button class="file-upload-btn" ${
              stats.students === 0 || stats.subjects === 0 ? "disabled" : ""
            } onclick="$('file-enrollments').click()">📁 Choose CSV File</button>
            <div id="result-enrollments"></div>
        </div>

        <div class="import-card ${
          stats.enrollments === 0 ? "disabled" : ""
        }" id="import-grades">
            <div class="import-card-header">
                <div class="import-card-title">✏️ 4. Import Grades</div>
                <span class="import-badge ${
                  stats.grades > 0 ? "loaded" : "empty"
                }">
                    ${stats.grades > 0 ? stats.grades + " loaded" : "empty"}
                </span>
            </div>
            <div class="import-hint">Format: RollNo, SubjectCode, Grade, Semester, Year  |  Grades: A A- B B- C C- D F W</div>
            <input type="file" id="file-grades" accept=".csv" style="display:none" onchange="uploadCSV('grades')">
            <button class="file-upload-btn" ${
              stats.enrollments === 0 ? "disabled" : ""
            } onclick="$('file-grades').click()">📁 Choose CSV File</button>
            <div id="result-grades"></div>
        </div>
    `;
}

async function uploadCSV(type) {
  const fileInput = $("file-" + type);
  const file = fileInput.files[0];
  if (!file) return;

  const resultDiv = $("result-" + type);
  resultDiv.innerHTML =
    '<div class="import-result" style="background:var(--bg)">Uploading...</div>';

  const formData = new FormData();
  formData.append("file", file);

  try {
    const res = await api("POST", "/api/import/" + type, formData);
    resultDiv.innerHTML = `<div class="import-result success">
            ✅ Imported: ${res.imported} | Skipped: ${res.skipped}
            ${res.errors ? " | " + res.errors : ""}
        </div>`;
    showToast(`Imported ${res.imported} ${type}`, "success");
    // Refresh the import page to update badges
    setTimeout(() => loadImportPage(), 1000);
  } catch (e) {
    resultDiv.innerHTML = `<div class="import-result error">❌ ${
      e.error || e.errors || "Upload failed"
    }</div>`;
    showToast(e.error || "Import failed", "error");
  }
  fileInput.value = "";
}

// ============ STUDENTS PAGE ============
async function loadStudentsPage() {
  const area = $("content-area");
  area.innerHTML = `
        <div class="toolbar">
            <input type="text" class="search-input" id="student-search" placeholder="🔍  Search by name, roll no, or department..." oninput="debounceStudentSearch()">
            <div class="btn-group">
                <button class="sort-btn ${
                  currentSort === "roll" ? "active" : ""
                }" onclick="sortStudents('roll')">Roll No</button>
                <button class="sort-btn ${
                  currentSort === "name" ? "active" : ""
                }" onclick="sortStudents('name')">Name</button>
                <button class="sort-btn ${
                  currentSort === "cgpa" ? "active" : ""
                }" onclick="sortStudents('cgpa')">CGPA</button>
                <button class="sort-btn ${
                  currentSort === "dept" ? "active" : ""
                }" onclick="sortStudents('dept')">Dept</button>
            </div>
            <button class="btn btn-secondary btn-sm" onclick="exportStudents()">📥 Export CSV</button>
        </div>
        <div id="students-table-container"></div>
    `;
  fetchAndRenderStudents();
}

function debounceStudentSearch() {
  clearTimeout(searchTimer);
  searchTimer = setTimeout(fetchAndRenderStudents, 300);
}

function sortStudents(sort) {
  currentSort = sort;
  document
    .querySelectorAll(".sort-btn")
    .forEach((b) => b.classList.remove("active"));
  event.target.classList.add("active");
  fetchAndRenderStudents();
}

async function fetchAndRenderStudents() {
  const search = document.getElementById("student-search");
  const filter = search ? search.value.trim() : "";
  const students = await api(
    "GET",
    `/api/students?sort=${currentSort}&filter=${encodeURIComponent(filter)}`
  );
  renderStudentsTable(students);
}

function renderStudentsTable(students) {
  const container = document.getElementById("students-table-container");
  if (!students.length) {
    container.innerHTML = `<div class="empty-state"><div class="empty-state-icon">👥</div><p>No students found</p></div>`;
    return;
  }
  let html = `<div class="table-container"><table>
        <thead><tr>
            <th>Roll No</th><th>Name</th><th>Department</th><th>Sem</th><th>Year</th><th>CGPA</th><th>Actions</th>
        </tr></thead><tbody>`;

  for (const s of students) {
    html += `<tr class="clickable" onclick="viewStudentDetail('${escapeAttr(
      s.rollNo
    )}')">
            <td><strong>${escapeHtml(s.rollNo)}</strong></td>
            <td>${escapeHtml(s.name)}</td>
            <td>${escapeHtml(s.dept)}</td>
            <td>${s.semester}</td>
            <td>${s.year}</td>
            <td><span class="cgpa-badge ${cgpaClass(s.cgpa)}">${s.cgpa.toFixed(
      2
    )}</span></td>
            <td>
                <button class="btn btn-secondary btn-sm" onclick="event.stopPropagation(); editStudent('${escapeAttr(
                  s.rollNo
                )}')">Edit</button>
                <button class="btn btn-danger btn-sm" onclick="event.stopPropagation(); deleteStudent('${escapeAttr(
                  s.rollNo
                )}', '${escapeAttr(s.name)}')">Delete</button>
            </td>
        </tr>`;
  }
  html += `</tbody></table></div>
        <p class="text-center mt-16" style="color:var(--text-muted); font-size:13px">${students.length} student(s)</p>`;
  container.innerHTML = html;
}

async function viewStudentDetail(roll) {
  try {
    const s = await api("GET", `/api/students/${roll}`);
    let html = `
            <div class="profile-card" style="margin:-32px -32px 24px -32px; border-radius: var(--radius) var(--radius) 0 0;">
                <div class="profile-avatar">${getInitials(s.name)}</div>
                <div class="profile-info">
                    <h2>${s.name}</h2>
                    <p>${s.rollNo} • ${s.dept} • Semester ${
      s.semester
    } • Year ${s.year}</p>
                </div>
                <div class="profile-cgpa">
                    <div class="cgpa-big" style="color:${cgpaColor(
                      s.cgpa
                    )}">${s.cgpa.toFixed(2)}</div>
                    <div class="profile-cgpa-label">CGPA / 10</div>
                </div>
            </div>`;

    // Current Subjects
    html += `<div class="card-title">📝 Current Subjects</div>`;
    if (s.currentSubjects.length === 0) {
      html += `<p style="color:var(--text-muted); font-size:13px; margin-bottom:20px">No current enrollments</p>`;
    } else {
      html += `<div class="table-container mb-16"><table>
                <thead><tr><th>Code</th><th>Subject</th><th>Credits</th></tr></thead><tbody>`;
      for (const cs of s.currentSubjects) {
        html += `<tr><td>${cs.subCode}</td><td>${
          cs.subName
        }</td><td>${cs.credits.toFixed(1)}</td></tr>`;
      }
      html += `</tbody></table></div>`;
    }

    // Semesters
    if (s.semesters.length > 0) {
      html += `<div class="card-title" style="display:flex; justify-content:space-between; align-items:center; margin-top:20px;">
                  <span>📊 Previous Subjects & Grades</span>
                  <button class="btn btn-secondary btn-sm" onclick="document.getElementById('prev-subjects-admin').style.display = document.getElementById('prev-subjects-admin').style.display === 'none' ? 'block' : 'none'">Toggle Previous Subjects</button>
               </div>
               <div id="prev-subjects-admin" style="display:none;">`;
      for (const sem of s.semesters) {
        html += `
                <div class="semester-card">
                    <div class="semester-header" onclick="this.classList.toggle('open'); this.nextElementSibling.classList.toggle('open')">
                        <span class="semester-title">Semester ${
                          sem.semester
                        } (${sem.year})</span>
                        <span>
                            <span class="semester-sgpa ${cgpaClass(
                              sem.sgpa
                            )}">SGPA: ${sem.sgpa.toFixed(2)}</span>
                            <span class="semester-chevron"> ▼</span>
                        </span>
                    </div>
                    <div class="semester-body">
                        <table style="margin-top:12px">
                            <thead><tr><th>Code</th><th>Subject</th><th>Credits</th><th>Grade</th><th>Points</th></tr></thead>
                            <tbody>`;
        for (const g of sem.grades) {
          html += `<tr>
                        <td>${g.subCode}</td>
                        <td>${g.subName}</td>
                        <td>${g.credits.toFixed(1)}</td>
                        <td><span class="grade-badge ${gradeClass(g.grade)}">${
            g.grade
          }</span>${
            !g.countsForGpa
              ? ' <span style="color:var(--text-muted);font-size:11px">(not in GPA)</span>'
              : ""
          }</td>
                        <td>${g.points.toFixed(1)}</td>
                    </tr>`;
        }
        html += `</tbody></table></div></div>`;
      }
      html += `</div>`;
    }
    openModal(html);
  } catch (e) {
    showToast("Failed to load student details", "error");
  }
}

async function editStudent(roll) {
  try {
    const s = await api("GET", `/api/students/${roll}`);
    openModal(`
            <h2 style="margin-bottom:20px">Edit Student: ${escapeHtml(
              s.rollNo
            )}</h2>
            <div class="form-group"><label>Name</label><input class="form-input" id="edit-name" value="${escapeHtml(
              s.name
            )}"></div>
            <div class="form-group"><label>Department</label><input class="form-input" id="edit-dept" value="${escapeHtml(
              s.dept
            )}"></div>
            <div class="form-row">
                <div class="form-group"><label>Semester</label><input class="form-input" id="edit-sem" type="number" value="${
                  s.semester
                }"></div>
                <div class="form-group"><label>Year</label><input class="form-input" id="edit-year" type="number" value="${
                  s.year
                }"></div>
            </div>
            <button class="btn btn-primary mt-16" onclick="submitEditStudent('${escapeAttr(
              roll
            )}')">Save Changes</button>
        `);
  } catch (e) {
    showToast("Failed to load student", "error");
  }
}

async function submitEditStudent(roll) {
  try {
    await api("PUT", `/api/students/${roll}`, {
      name: $("edit-name").value,
      dept: $("edit-dept").value,
      semester: $("edit-sem").value,
      year: $("edit-year").value,
    });
    closeModal();
    showToast("Student updated", "success");
    fetchAndRenderStudents();
  } catch (e) {
    showToast(e.error || "Update failed", "error");
  }
}

async function deleteStudent(roll, name) {
  if (!confirm(`Delete student ${name} (${roll}) and all their records?`))
    return;
  try {
    await api("DELETE", `/api/students/${roll}`);
    showToast("Student removed", "success");
    fetchAndRenderStudents();
  } catch (e) {
    showToast(e.error || "Delete failed", "error");
  }
}

function exportStudents() {
  window.open(`/api/export/students?sort=${currentSort}`, "_blank");
}

// ============ ADD STUDENT ============
function loadAddStudentPage() {
  $("content-area").innerHTML = `
        <div class="card" style="max-width:600px">
            <div class="card-title">➕ Add New Student</div>
            <div class="form-group"><label>Roll Number</label><input class="form-input" id="add-roll" placeholder="e.g. B24CI1009"></div>
            <div class="form-group"><label>Name</label><input class="form-input" id="add-name" placeholder="Full Name"></div>
            <div class="form-group"><label>Department</label><input class="form-input" id="add-dept" placeholder="e.g. Computer Science"></div>
            <div class="form-row">
                <div class="form-group"><label>Semester</label><input class="form-input" id="add-sem" type="number" min="1" placeholder="1"></div>
                <div class="form-group"><label>Year</label><input class="form-input" id="add-year" type="number" min="2000" placeholder="2024"></div>
            </div>
            <button class="btn btn-primary mt-16" onclick="submitAddStudent()">Add Student</button>
        </div>
    `;
}

async function submitAddStudent() {
  try {
    await api("POST", "/api/students", {
      rollNo: $("add-roll").value.trim(),
      name: $("add-name").value.trim(),
      dept: $("add-dept").value.trim(),
      semester: $("add-sem").value,
      year: $("add-year").value,
    });
    showToast("Student added successfully", "success");
    $("add-roll").value = "";
    $("add-name").value = "";
    $("add-dept").value = "";
    $("add-sem").value = "";
    $("add-year").value = "";
  } catch (e) {
    showToast(e.error || "Failed to add student", "error");
  }
}

// ============ SUBJECTS PAGE ============
async function loadSubjectsPage() {
  const subjects = await api("GET", "/api/subjects");
  const area = $("content-area");

  if (!subjects.length) {
    area.innerHTML = `<div class="empty-state"><div class="empty-state-icon">📚</div><p>No subjects loaded. Import subjects first.</p></div>`;
    return;
  }

  let html = `<div class="table-container"><table>
        <thead><tr><th>Code</th><th>Name</th><th>Credits</th><th>Actions</th></tr></thead><tbody>`;
  for (const s of subjects) {
    html += `<tr>
            <td><strong>${escapeHtml(s.code)}</strong></td>
            <td>${escapeHtml(s.name)}</td>
            <td>${s.credits.toFixed(1)}</td>
            <td>
                <button class="btn btn-secondary btn-sm" onclick="editSubject('${escapeAttr(
                  s.code
                )}')">Edit</button>
                <button class="btn btn-danger btn-sm" onclick="deleteSubject('${escapeAttr(
                  s.code
                )}', '${escapeAttr(s.name)}')">Delete</button>
            </td>
        </tr>`;
  }
  html += `</tbody></table></div>
        <p class="text-center mt-16" style="color:var(--text-muted);font-size:13px">${subjects.length} subject(s)</p>`;
  area.innerHTML = html;
}

async function editSubject(code) {
  const subjects = await api("GET", "/api/subjects");
  const s = subjects.find((x) => x.code === code);
  if (!s) return;
  openModal(`
        <h2 style="margin-bottom:20px">Edit Subject: ${escapeHtml(s.code)}</h2>
        <div class="form-group"><label>Name</label><input class="form-input" id="edsub-name" value="${escapeHtml(
          s.name
        )}"></div>
        <div class="form-group"><label>Credits</label><input class="form-input" id="edsub-credits" type="number" step="0.5" value="${
          s.credits
        }"></div>
        <button class="btn btn-primary mt-16" onclick="submitEditSubject('${escapeAttr(
          code
        )}')">Save Changes</button>
    `);
}

async function submitEditSubject(code) {
  try {
    await api("PUT", `/api/subjects/${code}`, {
      name: $("edsub-name").value,
      credits: $("edsub-credits").value,
    });
    closeModal();
    showToast("Subject updated", "success");
    loadSubjectsPage();
  } catch (e) {
    showToast(e.error || "Update failed", "error");
  }
}

async function deleteSubject(code, name) {
  if (!confirm(`Delete subject ${name} (${code})?`)) return;
  try {
    await api("DELETE", `/api/subjects/${code}`);
    showToast("Subject removed", "success");
    loadSubjectsPage();
  } catch (e) {
    showToast(e.error || "Delete failed", "error");
  }
}

// ============ ADD SUBJECT ============
function loadAddSubjectPage() {
  $("content-area").innerHTML = `
        <div class="card" style="max-width:600px">
            <div class="card-title">➕ Add New Subject</div>
            <div class="form-group"><label>Subject Code</label><input class="form-input" id="addsub-code" placeholder="e.g. CSL101"></div>
            <div class="form-group"><label>Subject Name</label><input class="form-input" id="addsub-name" placeholder="e.g. Data Structures"></div>
            <div class="form-group"><label>Credits</label><input class="form-input" id="addsub-credits" type="number" step="0.5" min="0.5" placeholder="3"></div>
            <button class="btn btn-primary mt-16" onclick="submitAddSubject()">Add Subject</button>
        </div>
    `;
}

async function submitAddSubject() {
  try {
    await api("POST", "/api/subjects", {
      code: $("addsub-code").value.trim(),
      name: $("addsub-name").value.trim(),
      credits: $("addsub-credits").value,
    });
    showToast("Subject added", "success");
    $("addsub-code").value = "";
    $("addsub-name").value = "";
    $("addsub-credits").value = "";
  } catch (e) {
    showToast(e.error || "Failed to add", "error");
  }
}

// ============ ENROLL PAGE ============
async function loadEnrollPage() {
  const students = await api("GET", "/api/students?sort=roll");
  const subjects = await api("GET", "/api/subjects");

  let studentOpts = students
    .map((s) => `<option value="${s.rollNo}">${s.rollNo} — ${s.name}</option>`)
    .join("");
  let subjectOpts = subjects
    .map((s) => `<option value="${s.code}">${s.code} — ${s.name}</option>`)
    .join("");

  $("content-area").innerHTML = `
        <div class="card" style="max-width:600px">
            <div class="card-title">📝 Enroll Student in Subject</div>
            <div class="form-group"><label>Student</label><select class="form-input" id="enroll-roll"><option value="">Select student...</option>${studentOpts}</select></div>
            <div class="form-group"><label>Subject</label><select class="form-input" id="enroll-sub"><option value="">Select subject...</option>${subjectOpts}</select></div>
            <div class="form-row">
                <div class="form-group"><label>Semester</label><input class="form-input" id="enroll-sem" type="number" min="1" placeholder="1"></div>
                <div class="form-group"><label>Year</label><input class="form-input" id="enroll-year" type="number" min="2000" placeholder="2024"></div>
            </div>
            <button class="btn btn-primary mt-16" onclick="submitEnroll()">Enroll</button>
        </div>
    `;
}

async function submitEnroll() {
  try {
    await api("POST", "/api/enroll", {
      rollNo: $("enroll-roll").value,
      subCode: $("enroll-sub").value,
      semester: $("enroll-sem").value,
      year: $("enroll-year").value,
    });
    showToast("Student enrolled", "success");
  } catch (e) {
    showToast(e.error || "Enrollment failed", "error");
  }
}

// ============ ASSIGN GRADE PAGE ============
async function loadAssignGradePage() {
  const students = await api("GET", "/api/students?sort=roll");
  const subjects = await api("GET", "/api/subjects");

  let studentOpts = students
    .map((s) => `<option value="${s.rollNo}">${s.rollNo} — ${s.name}</option>`)
    .join("");
  let subjectOpts = subjects
    .map((s) => `<option value="${s.code}">${s.code} — ${s.name}</option>`)
    .join("");
  const gradeOpts = ["A", "A-", "B", "B-", "C", "C-", "D", "F", "W"]
    .map((g) => `<option value="${g}">${g}</option>`)
    .join("");

  $("content-area").innerHTML = `
        <div class="card" style="max-width:600px">
            <div class="card-title">✏️ Assign / Update Grade</div>
            <div class="form-group"><label>Student</label><select class="form-input" id="grade-roll"><option value="">Select student...</option>${studentOpts}</select></div>
            <div class="form-group"><label>Subject</label><select class="form-input" id="grade-sub"><option value="">Select subject...</option>${subjectOpts}</select></div>
            <div class="form-row">
                <div class="form-group"><label>Semester</label><input class="form-input" id="grade-sem" type="number" min="1" placeholder="1"></div>
                <div class="form-group"><label>Year</label><input class="form-input" id="grade-year" type="number" min="2000" placeholder="2024"></div>
            </div>
            <div class="form-group"><label>Grade</label><select class="form-input" id="grade-grade"><option value="">Select grade...</option>${gradeOpts}</select></div>
            <button class="btn btn-primary mt-16" onclick="submitGrade()">Assign Grade</button>
        </div>
    `;
}

async function submitGrade() {
  try {
    await api("POST", "/api/grades", {
      rollNo: $("grade-roll").value,
      subCode: $("grade-sub").value,
      semester: $("grade-sem").value,
      year: $("grade-year").value,
      grade: $("grade-grade").value,
    });
    showToast("Grade assigned", "success");
  } catch (e) {
    showToast(e.error || "Failed to assign grade", "error");
  }
}



// ============ RESET ============
async function resetSystem() {
  const confirm1 = confirm(
    "⚠️ This will ERASE ALL DATA permanently. Continue?"
  );
  if (!confirm1) return;
  const confirm2 = prompt("Type CONFIRM to proceed:");
  if (confirm2 !== "CONFIRM") {
    showToast("Reset cancelled", "info");
    return;
  }

  try {
    await api("POST", "/api/reset", { confirm: "CONFIRM" });
    showToast("System reset. All data erased.", "success");
    navigateTo("import");
  } catch (e) {
    showToast(e.error || "Reset failed", "error");
  }
}

// ============ STUDENT PORTAL ============
async function loadStudentPortal(roll) {
  try {
    const s = await api("GET", `/api/students/${roll}`);
    let html = `
        <div class="profile-card">
            <div class="profile-avatar">${getInitials(s.name)}</div>
            <div class="profile-info">
                <h2>${s.name}</h2>
                <p>${s.rollNo} • ${s.dept} • Semester ${s.semester} • Year ${
      s.year
    }</p>
            </div>
            <div class="profile-cgpa">
                <div class="cgpa-big" style="color:${
                  s.cgpa >= 8 ? "#4caf50" : s.cgpa >= 5 ? "#ff9800" : "#f44336"
                }">${s.cgpa.toFixed(2)}</div>
                <div class="profile-cgpa-label">CGPA / 10</div>
            </div>
        </div>`;

    // Current Subjects
    html += `<div class="card"><div class="card-title">📝 Current Subjects</div>`;
    if (s.currentSubjects.length === 0) {
      html += `<p style="color:var(--text-muted);font-size:13px">No current enrollments</p>`;
    } else {
      html += `<div class="table-container"><table>
                <thead><tr><th>Code</th><th>Subject</th><th>Credits</th></tr></thead><tbody>`;
      for (const cs of s.currentSubjects) {
        html += `<tr><td>${cs.subCode}</td><td>${
          cs.subName
        }</td><td>${cs.credits.toFixed(1)}</td></tr>`;
      }
      html += `</tbody></table></div>`;
    }
    html += `</div>`;

    // Academic Record
    if (s.semesters.length > 0) {
      html += `<div class="card"><div class="card-title">📊 Academic Record</div>`;
      for (const sem of s.semesters) {
        html += `
                <div class="semester-card">
                    <div class="semester-header" onclick="this.classList.toggle('open'); this.nextElementSibling.classList.toggle('open')">
                        <span class="semester-title">Semester ${
                          sem.semester
                        } (${sem.year})</span>
                        <span>
                            <span class="semester-sgpa ${cgpaClass(
                              sem.sgpa
                            )}">SGPA: ${sem.sgpa.toFixed(2)}</span>
                            <span class="semester-chevron"> ▼</span>
                        </span>
                    </div>
                    <div class="semester-body">
                        <table style="margin-top:12px">
                            <thead><tr><th>Code</th><th>Subject</th><th>Credits</th><th>Grade</th></tr></thead>
                            <tbody>`;
        for (const g of sem.grades) {
          html += `<tr>
                        <td>${g.subCode}</td>
                        <td>${g.subName}</td>
                        <td>${g.credits.toFixed(1)}</td>
                        <td><span class="grade-badge ${gradeClass(g.grade)}">${
            g.grade
          }</span>${
            !g.countsForGpa
              ? ' <span style="color:var(--text-muted);font-size:11px">(not in GPA)</span>'
              : ""
          }</td>
                    </tr>`;
        }
        html += `</tbody></table></div></div>`;
      }
      html += `</div>`;
    }

    $("student-content").innerHTML = html;
  } catch (e) {
    $(
      "student-content"
    ).innerHTML = `<div class="empty-state"><p>Failed to load student data</p></div>`;
  }
}

// ============ INIT ============
window.addEventListener("DOMContentLoaded", () => {
  initTheme();
  loadLoginStats();
});
