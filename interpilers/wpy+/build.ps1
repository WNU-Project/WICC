# WICC Python+ Build Script
# -------------------------
# Cleans, builds, and runs the Python+ interpiler with a test file.

# Move into your project folder
Set-Location "C:\Users\tadeo\OneDrive\Documents\GitHub\WICC\interpilers\wpy+"

# Clean old build artifacts
Write-Host "Cleaning..." -ForegroundColor Yellow
make clean

# Build the project
Write-Host "Building..." -ForegroundColor Cyan
make

# Run the resulting executable with a test argument
Write-Host "Running..." -ForegroundColor Green
.\wpy+.exe hello

# End of script