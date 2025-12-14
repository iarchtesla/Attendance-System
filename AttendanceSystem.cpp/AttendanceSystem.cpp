
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <map>
using namespace std;

// Structure to store employee information
struct Employee {
    string id;
    string name;
};

// Structure to store attendance records
struct AttendanceRecord {
    string employeeId;
    string employeeName;
    string date;
    string checkInTime;
    string checkOutTime;
    bool isCheckedOut;
};

// Sample employees - exactly as provided in your data
const vector<Employee> sampleEmployees = {
    {"EMP001", "mahmoud"},
    {"EMP002", "Sarah "},
    {"EMP003", "lamine"},
    {"EMP004", "nour"},
    {"EMP005", "hossamelden"},
    {"EMP006", "mona"},
    {"EMP007", "ahmed"},
    {"EMP008", "mohammed"}
};

// Trim whitespace from both ends of a string
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    
    if (start == string::npos || end == string::npos)
        return "";
    
    return str.substr(start, end - start + 1);
}

// Parse time string to time_t for time difference calculation
time_t parseTime(const string& timeStr) {
    struct tm t = {0};
    sscanf(timeStr.c_str(), "%d:%d", &t.tm_hour, &t.tm_min);
    t.tm_isdst = -1; // Daylight saving time not considered
    return mktime(&t);
}

// Calculate work hours from check-in and check-out times
double calculateWorkHours(const string& checkIn, const string& checkOut) {
    if (checkIn.empty() || checkOut.empty()) return 0.0;

    time_t inTime = parseTime(checkIn);
    time_t outTime = parseTime(checkOut);

    if (inTime == -1 || outTime == -1) return 0.0;

    double diffSeconds = difftime(outTime, inTime);
    return diffSeconds / 3600.0; // Convert seconds to hours
}

// Display employee list with IDs
void displayEmployeeList() {
    cout << "\n===== Available Employees =====\n";
    cout << left << setw(10) << "ID" << setw(25) << "Name" << endl;
    cout << "----------------------------------------\n";
    for (const auto& emp : sampleEmployees) {
        cout << left << setw(10) << emp.id << setw(25) << emp.name << endl;
    }
    cout << "----------------------------------------\n";
}

// Get employee name by ID
string getEmployeeNameById(const string& id) {
    string cleanId = trim(id);
    for (const auto& emp : sampleEmployees) {
        if (emp.id == cleanId) {
            return emp.name;
        }
    }
    return "Unknown Employee";
}

// Get employee ID by name (case-insensitive)
string getEmployeeIdByName(const string& name) {
    string cleanName = trim(name);
    string lowerName = cleanName;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& emp : sampleEmployees) {
        string empNameClean = trim(emp.name);
        string lowerEmpName = empNameClean;
        transform(lowerEmpName.begin(), lowerEmpName.end(), lowerEmpName.begin(), ::tolower);

        if (lowerEmpName.find(lowerName) != string::npos || lowerName.find(lowerEmpName) != string::npos) {
            return emp.id;
        }
    }
    return "";
}

// Display main menu
void displayMainMenu() {
    cout << "\n\n===== Employee Attendance Management System =====\n";
    cout << "1. Check-In\n";
    cout << "2. Check-Out\n";
    cout << "3. Search Employee\n";
    cout << "4. Daily Report\n";
    cout << "5. View All Employees\n";
    cout << "6. Exit Program\n";
    cout << "=========================================\n";
    cout << "Enter your choice: ";
}

// Record employee check-in
void checkIn() {
    string id, date, time;

    cout << "\n===== Employee Check-In =====\n";
    displayEmployeeList();
    cout << "Enter employee ID: ";
    getline(cin, id);

    // Validate employee ID
    string employeeName = getEmployeeNameById(id);
    if (employeeName == "Unknown Employee") {
        cout << "Invalid employee ID. Please try again.\n";
        return;
    }

    cout << "Selected Employee: " << employeeName << endl;
    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, date);
    cout << "Enter check-in time (HH:MM): ";
    getline(cin, time);

    ofstream file("attendance.txt", ios::app);
    if (!file.is_open()) {
        cerr << "Error opening attendance.txt for writing\n";
        return;
    }

    file << id << "," << employeeName << "," << date << "," << time << ",," << "0" << endl;
    file.close();

    cout << "Employee check-in recorded successfully!\n";
}

// Record employee check-out using merge technique with temp file
void checkOut() {
    string id, date, time;

    cout << "\n===== Employee Check-Out =====\n";
    displayEmployeeList();
    cout << "Enter employee ID: ";
    getline(cin, id);

    // Validate employee ID
    string employeeName = getEmployeeNameById(id);
    if (employeeName == "Unknown Employee") {
        cout << "Invalid employee ID. Please try again.\n";
        return;
    }

    cout << "Selected Employee: " << employeeName << endl;
    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, date);
    cout << "Enter check-out time (HH:MM): ";
    getline(cin, time);

    ifstream inputFile("attendance.txt");
    ofstream tempFile("temp.txt");

    if (!inputFile.is_open() || !tempFile.is_open()) {
        cerr << "Error opening files\n";
        return;
    }

    string line;
    bool found = false;

    while (getline(inputFile, line)) {
        if (line.empty()) continue;

        // Split the line by commas
        vector<string> fields;
        size_t start = 0;
        size_t end = line.find(',');

        while (end != string::npos) {
            fields.push_back(line.substr(start, end - start));
            start = end + 1;
            end = line.find(',', start);
        }
        fields.push_back(line.substr(start));

        // Ensure we have exactly 6 fields
        if (fields.size() < 6) {
            tempFile << line << endl;
            continue;
        }

        string empId = trim(fields[0]);
        string empName = trim(fields[1]);
        string empDate = trim(fields[2]);
        string checkIn = trim(fields[3]);
        string checkOut = trim(fields[4]);
        string isCheckedOut = trim(fields[5]);

        // Check if employee ID and date match and employee hasn't checked out yet
        if (empId == trim(id) && empDate == trim(date) && isCheckedOut == "0") {
            tempFile << empId << "," << empName << "," << empDate << "," << checkIn << "," << time << ",1" << endl;
            found = true;
        } else {
            tempFile << line << endl;
        }
    }

    inputFile.close();
    tempFile.close();

    // Remove original file and rename temp file
    remove("attendance.txt");
    rename("temp.txt", "attendance.txt");

    if (found) {
        cout << "Employee check-out recorded successfully!\n";
    } else {
        cout << "No check-in record found for this employee on this date, or check-out already recorded.\n";
    }
}

// Search for employee records by ID or name
void searchEmployee() {
    string searchInput;

    cout << "\n===== Search Employee Records =====\n";
    cout << "Enter employee ID or name to search: ";
    getline(cin, searchInput);

    // Try to find by ID first
    string employeeId = trim(searchInput);
    string employeeName = getEmployeeNameById(employeeId);

    // If not found by ID, try by name
    if (employeeName == "Unknown Employee") {
        employeeId = getEmployeeIdByName(searchInput);
        if (!employeeId.empty()) {
            employeeName = getEmployeeNameById(employeeId);
        }
    }

    if (employeeId.empty() || employeeName == "Unknown Employee") {
        cout << "Employee not found. Please check the ID or name.\n";
        return;
    }

    ifstream file("attendance.txt");
    if (!file.is_open()) {
        cerr << "Error opening attendance.txt\n";
        return;
    }

    string line;
    bool found = false;
    int recordCount = 0;

    cout << "\nRecords for employee '" << employeeName << "' (ID: " << employeeId << "):\n";
    cout << "================================================================================\n";
    cout << left << setw(12) << "Date" << setw(10) << "Check-In" << setw(10) << "Check-Out"
         << setw(10) << "Hours" << setw(8) << "Status" << endl;
    cout << "================================================================================\n";

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Trim the entire line first
        string cleanLine = trim(line);
        if (cleanLine.empty()) continue;

        // Split the line by commas
        vector<string> fields;
        size_t start = 0;
        size_t end = cleanLine.find(',');

        while (end != string::npos) {
            fields.push_back(trim(cleanLine.substr(start, end - start)));
            start = end + 1;
            end = cleanLine.find(',', start);
        }
        fields.push_back(trim(cleanLine.substr(start)));

        // Ensure we have exactly 6 fields
        if (fields.size() < 6) {
            continue;
        }

        string empId = fields[0];
        string empName = fields[1];
        string date = fields[2];
        string checkIn = fields[3];
        string checkOut = fields[4];
        string isCheckedOut = fields[5];

        if (empId == employeeId) {
            found = true;
            recordCount++;

            double hours = 0.0;
            if (isCheckedOut == "1" && !checkIn.empty() && !checkOut.empty()) {
                hours = calculateWorkHours(checkIn, checkOut);
            }

            string status = (isCheckedOut == "1") ? "Complete" : "Incomplete";

            cout << left << setw(12) << date
                 << setw(10) << (checkIn.empty() ? "N/A" : checkIn)
                 << setw(10) << (checkOut.empty() || isCheckedOut == "0" ? "N/A" : checkOut)
                 << setw(10) << fixed << setprecision(1) << (isCheckedOut == "1" ? hours : 0.0)
                 << setw(8) << status << endl;
        }
    }

    file.close();

    if (!found) {
        cout << "No attendance records found for this employee.\n";
    } else {
        cout << "================================================================================\n";
        cout << "Total records: " << recordCount << endl;
    }
}

// Display daily report
void dailyReport() {
    string date;

    cout << "\n===== Daily Attendance Report =====\n";
    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, date);
    date = trim(date);

    ifstream file("attendance.txt");
    if (!file.is_open()) {
        cerr << "Error opening attendance.txt\n";
        return;
    }

    string line;
    bool found = false;
    vector<AttendanceRecord> records;

    while (getline(file, line)) {
        if (line.empty()) continue;

        string cleanLine = trim(line);
        if (cleanLine.empty()) continue;

        // Split the line by commas
        vector<string> fields;
        size_t start = 0;
        size_t end = cleanLine.find(',');

        while (end != string::npos) {
            fields.push_back(trim(cleanLine.substr(start, end - start)));
            start = end + 1;
            end = cleanLine.find(',', start);
        }
        fields.push_back(trim(cleanLine.substr(start)));

        if (fields.size() < 6) {
            continue;
        }

        string empId = fields[0];
        string empName = fields[1];
        string empDate = fields[2];
        string checkIn = fields[3];
        string checkOut = fields[4];
        string isCheckedOut = fields[5];

        if (empDate == date && isCheckedOut == "1") {
            AttendanceRecord record;
            record.employeeId = empId;
            record.employeeName = empName;
            record.date = empDate;
            record.checkInTime = checkIn;
            record.checkOutTime = checkOut;
            record.isCheckedOut = true;
            records.push_back(record);
            found = true;
        }
    }

    file.close();

    if (!found) {
        cout << "No attendance records found for this date.\n";
        return;
    }

    cout << "\nDaily Report for " << date << ":\n";
    cout << "=========================================================================================\n";
    cout << left << setw(10) << "ID" << setw(25) << "Employee Name"
         << setw(10) << "Check-In" << setw(10) << "Check-Out"
         << setw(10) << "Hours" << endl;
    cout << "=========================================================================================\n";

    double totalHours = 0.0;

    for (const auto& record : records) {
        double hours = calculateWorkHours(record.checkInTime, record.checkOutTime);
        totalHours += hours;

        cout << left << setw(10) << record.employeeId
             << setw(25) << record.employeeName
             << setw(10) << record.checkInTime
             << setw(10) << record.checkOutTime
             << setw(10) << fixed << setprecision(1) << hours << endl;
    }

    cout << "=========================================================================================\n";
    cout << "Total employees present: " << records.size() << endl;
    cout << "Total work hours: " << fixed << setprecision(1) << totalHours << " hours" << endl;
    cout << "Average hours per employee: " << fixed << setprecision(1) << (totalHours / records.size()) << " hours" << endl;
}

// View all employees
void viewAllEmployees() {
    cout << "\n===== All Employees =====\n";
    cout << left << setw(10) << "ID" << setw(30) << "Name" << setw(15) << "Department" << endl;
    cout << "---------------------------------------------------------------\n";

    // Add some departments for better display
    map<string, string> departments = {
        {"EMP001", "Engineering"},
        {"EMP002", "Marketing"},
        {"EMP003", "Sales"},
        {"EMP004", "HR"},
        {"EMP005", "Finance"},
        {"EMP006", "IT Support"},
        {"EMP007", "Management"},
        {"EMP008", "Customer Service"}
    };

    for (const auto& emp : sampleEmployees) {
        string dept = departments[emp.id];
        cout << left << setw(10) << emp.id << setw(30) << emp.name << setw(15) << dept << endl;
    }

    cout << "---------------------------------------------------------------\n";
    cout << "Total employees: " << sampleEmployees.size() << endl;
}

// Initialize sample data (optional) - EXACTLY as provided but with 2025 dates
void initializeSampleData() {
    ofstream file("attendance.txt");
    if (file.is_open()) {
        // Sample attendance data exactly as provided but with 2025 dates
        file << "EMP001,mahmoud,2025-12-10,08:30,17:15,1" << endl;
        file << "EMP002,Sarah ,2025-12-10,09:00,18:00,1" << endl;
        file << "EMP003,lamine,2025-12-10,08:45,16:30,1" << endl;
        file << "EMP004,nour,2025-12-10,10:00,19:00,1" << endl;
        file << "EMP001,mahmoud,2025-12-11,08:15,17:00,1" << endl;
        file << "EMP005,hossamelden,2025-12-11,09:30,18:30,1" << endl;
        file << "EMP002,Sarah ,2025-12-11,08:50,17:45,1" << endl;
        file << "EMP006,mona,2025-12-11,09:15,18:15,1" << endl;
        file << "EMP007,ahmed,2025-12-12,08:00,16:45,1" << endl;
        file << "EMP008,mohammed,2025-12-12,09:45,18:45,1" << endl;
        file << "EMP003,lamine,2025-12-12,08:30,17:30,1" << endl;
        file << "EMP004,nour,2025-12-12,10:30,19:30,1" << endl;
        file.close();
        cout << "Sample attendance data initialized successfully!\n";
    } else {
        cerr << "Error initializing sample data\n";
    }
}

int main() {
    int choice;

    // Initialize sample data on first run
    ifstream checkFile("attendance.txt");
    if (!checkFile.is_open()) {
        cout << "No attendance file found. Initializing sample data...\n";
        initializeSampleData();
    } else {
        checkFile.close();
    }

    // Ensure cin works properly with getline
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    do {
        displayMainMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

        switch (choice) {
            case 1:
                checkIn();
                break;
            case 2:
                checkOut();
                break;
            case 3:
                searchEmployee();
                break;
            case 4:
                dailyReport();
                break;
            case 5:
                viewAllEmployees();
                break;
            case 6:
                cout << "Thank you for using the Attendance Management System. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 6.\n";
        }

        if (choice != 6) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }

    } while (choice != 6);

    return 0;
}
