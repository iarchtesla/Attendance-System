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

// Sample employees - you can add more as needed
const vector<Employee> sampleEmployees = {
    {"EMP001", "John Smith"},
    {"EMP002", "Sarah Johnson"},
    {"EMP003", "Michael Brown"},
    {"EMP004", "Emily Davis"},
    {"EMP005", "David Wilson"},
    {"EMP006", "Lisa Anderson"},
    {"EMP007", "Robert Taylor"},
    {"EMP008", "Jennifer Martinez"}
};

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
    for (const auto& emp : sampleEmployees) {
        if (emp.id == id) {
            return emp.name;
        }
    }
    return "Unknown Employee";
}

// Get employee ID by name (case-insensitive)
string getEmployeeIdByName(const string& name) {
    string lowerName = name;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& emp : sampleEmployees) {
        string lowerEmpName = emp.name;
        transform(lowerEmpName.begin(), lowerEmpName.end(), lowerEmpName.begin(), ::tolower);

        if (lowerEmpName.find(lowerName) != string::npos) {
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

        // Parse the line
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos ||
            pos4 == string::npos || pos5 == string::npos) {
            tempFile << line << endl;
            continue;
        }

        string empId = line.substr(0, pos1);
        string empName = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string empDate = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string checkIn = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string checkOut = line.substr(pos4 + 1, pos5 - pos4 - 1);
        string isCheckedOut = line.substr(pos5 + 1);

        // Check if employee ID and date match and employee hasn't checked out yet
        if (empId == id && empDate == date && isCheckedOut == "0") {
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
    string employeeId = searchInput;
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

        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos ||
            pos4 == string::npos || pos5 == string::npos) {
            continue;
        }

        string empId = line.substr(0, pos1);
        string empName = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string date = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string checkIn = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string checkOut = line.substr(pos4 + 1, pos5 - pos4 - 1);
        string isCheckedOut = line.substr(pos5 + 1);

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

        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos ||
            pos4 == string::npos || pos5 == string::npos) {
            continue;
        }

        string empId = line.substr(0, pos1);
        string empName = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string empDate = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string checkIn = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string checkOut = line.substr(pos4 + 1, pos5 - pos4 - 1);
        string isCheckedOut = line.substr(pos5 + 1);

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

// Initialize sample data (optional)
void initializeSampleData() {
    ofstream file("attendance.txt");
    if (file.is_open()) {
        // Sample attendance data for demonstration
        file << "EMP001,John Smith,2024-01-15,08:30,17:15,1\n";
        file << "EMP002,Sarah Johnson,2024-01-15,09:00,18:00,1\n";
        file << "EMP003,Michael Brown,2024-01-15,08:45,16:30,1\n";
        file << "EMP004,Emily Davis,2024-01-15,10:00,19:00,1\n";
        file << "EMP001,John Smith,2024-01-16,08:15,17:00,1\n";
        file << "EMP005,David Wilson,2024-01-16,09:30,18:30,1\n";
        file << "EMP002,Sarah Johnson,2024-01-16,08:50,17:45,1\n";
        file << "EMP006,Lisa Anderson,2024-01-16,09:15,18:15,1\n";
        file << "EMP007,Robert Taylor,2024-01-17,08:00,16:45,1\n";
        file << "EMP008,Jennifer Martinez,2024-01-17,09:45,18:45,1\n";
        file << "EMP003,Michael Brown,2024-01-17,08:30,17:30,1\n";
        file << "EMP004,Emily Davis,2024-01-17,10:30,19:30,1\n";
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
