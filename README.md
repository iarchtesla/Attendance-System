<div align="center">

# ⏰ Employee Attendance Management System

![Attendance System Banner](https://via.placeholder.com/1000x400/1e40af/ffffff?text=Employee+Attendance+Management+System+in+C%2B%2B)

**A robust, console-based attendance tracking system built with modern C++**

</div>

## 📖 Overview
This project is a complete Employee Attendance Management System implemented in C++. It features an intuitive command-line interface for managing employee check-ins/check-outs, calculating work hours, searching records, and generating detailed reports.

All data is persistently stored in a simple text file (`attendance.txt`) using CSV format, ensuring easy backup and inspection.

## ✨ Key Features
- **Check-In & Check-Out** → Accurate time recording with employee validation
- **Automatic Work Hours Calculation** → Precise daily hours based on entry/exit times
- **Employee Search** → Case-insensitive search with full attendance history and statistics
- **Comprehensive Daily Reports** → Total hours, average per employee, and presence count
- **Sample Data Initialization** → Auto-generates demo records on first run
- **Clean & Modular Design** → Well-structured code with reusable functions

## 🛠️ Tech Stack
- **Language**: Modern C++ (Standard Library only)
- **Storage**: Plain text CSV file
- **No external dependencies** → Pure vanilla C++

## 🚀 Getting Started

### Prerequisites
- C++ compiler (g++, clang++, MSVC)

### Build & Run
```bash
git clone https://github.com/iarchtesla/Attendance-System.git
cd Attendance-System
g++ AttendanceSystem.cpp -o attendance
./attendance          # Linux/macOS
attendance.exe        # Windows
