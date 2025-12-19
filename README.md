
<div align="center">

# ⏰ Employee Attendance Management System

**A clean, efficient, console-based attendance system built with modern C++**

</div>

## Overview
A fully functional Employee Attendance Management System written in C++. It provides a simple command-line interface to:
- Record employee check-in and check-out times
- Calculate daily work hours automatically
- Search employee attendance history
- Generate detailed daily reports with totals and averages
- View all employees and their departments

Data is stored in a plain text file (`attendance.txt`) using CSV format.

## Key Features
- Check-in & check-out with time validation
- Automatic calculation of hours worked
- Case-insensitive employee search
- Comprehensive daily attendance reports
- Sample data generated on first run
- Clean, well-commented, and modular C++ code
- No external dependencies

## Tech Stack
- **Language**: Modern C++ (Standard Library only)
- **Storage**: Plain text file (`attendance.txt`)
- **Interface**: Console (CLI)

## How to Run

### Prerequisites
- C++ compiler (g++, clang++, MSVC)

### Build & Run
```bash
git clone https://github.com/iarchtesla/Attendance-System.git
cd Attendance-System
g++ AttendanceSystem.cpp -o attendance
./attendance          # Linux/macOS
attendance.exe        # Windows
