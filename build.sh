#!/bin/bash

# TaskManager Build Script

set -e  # Exit on any error

echo "=== TaskManager Build Script ==="

# Check if sqlite3 is installed
if ! pkg-config --exists sqlite3; then
    echo "Error: sqlite3 development libraries not found!"
    echo "Please install sqlite3 development packages:"
    echo "  Ubuntu/Debian: sudo apt-get install libsqlite3-dev"
    echo "  CentOS/RHEL:   sudo yum install sqlite-devel"
    echo "  Fedora:        sudo dnf install sqlite-devel"
    echo "  macOS:         brew install sqlite3"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j$(nproc 2>/dev/null || echo 4)

echo ""
echo "=== Build Complete ==="
echo "Executable location: build/bin/TaskManager"
echo "Test executable location: build/bin/RunTests"
echo ""
echo "To run the demo:"
echo "  cd build/bin && ./TaskManager"
echo ""
echo "To run from project root:"
echo "  ./build/bin/TaskManager"
echo ""
echo "To run tests:"
echo "  ./build/bin/RunTests" 