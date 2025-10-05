# WICC Python++ Build Script
# -------------------------
# Cleans, builds, and runs the Python+ interpiler with a test file.

# Move into your project folder
Set-Location "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\compilers\wpy++"
# Clear Old Output
Write-Host "Cleaning old output..." -ForegroundColor DarkYellow
clear
Write-Host ""
# Clean old build artifacts
Write-Host "Cleaning..." -ForegroundColor Yellow
make clean

# Build the project
Write-Host "Building..." -ForegroundColor Cyan
make

# Run the resulting executable with a test argument
Write-Host "Running hello.pypp..." -ForegroundColor Green
.\wpy++.exe .\hello.pypp

# Run the help command
Write-Host "Running help command..." -ForegroundColor DarkGreen
.\wpy++.exe --help

# Run the version command
Write-Host "Running version command..." -ForegroundColor DarkGreen
.\wpy++.exe --version

# Run the win32 command
# Write-Host "Running win32 command..." -ForegroundColor DarkGreen
# .\wpy++.exe graphics.pypp --win32
