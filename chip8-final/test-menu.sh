#!/bin/bash

# Test script for the CHIP-8 Emulator menu system

echo "=== CHIP-8 Emulator Menu System Test ==="
echo

# Check if SDL2 and SDL2_ttf are installed
echo "Checking dependencies..."
MISSING_DEPS=false

if ! command -v sdl2-config &> /dev/null; then
    echo "SDL2 development libraries not found."
    MISSING_DEPS=true
fi

if ! pkg-config --exists SDL2_ttf 2>/dev/null; then
    echo "SDL2_ttf development libraries not found."
    MISSING_DEPS=true
fi

if [ "$MISSING_DEPS" = true ]; then
    # Detect OS and install packages
    if [ -f /etc/debian_version ]; then
        # Debian/Ubuntu
        echo "Installing dependencies for Debian/Ubuntu..."
        sudo apt-get update
        sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev
    elif [ -f /etc/fedora-release ]; then
        # Fedora
        echo "Installing dependencies for Fedora..."
        sudo dnf install -y SDL2-devel SDL2_ttf-devel
    elif [ -f /etc/arch-release ]; then
        # Arch Linux
        echo "Installing dependencies for Arch Linux..."
        sudo pacman -S --noconfirm sdl2 sdl2_ttf
    elif command -v brew &> /dev/null; then
        # macOS with Homebrew
        echo "Installing dependencies using Homebrew..."
        brew install sdl2 sdl2_ttf
    else
        echo "Unsupported OS. Please install SDL2 and SDL2_ttf manually."
        exit 1
    fi
fi

# Compile the emulator
echo "Compiling the emulator..."
make clean 2>/dev/null || true
make all

if [ $? -ne 0 ]; then
    echo "Error: Compilation failed."
    exit 1
fi

echo
echo "Compilation successful!"
echo

# Check if games directory exists
if [ ! -d "games" ]; then
    echo "Warning: 'games' directory not found. Creating it..."
    mkdir -p games
fi

# Check if there are any .ch8 files in the games directory
CH8_COUNT=$(find games -name "*.ch8" | wc -l)
if [ $CH8_COUNT -eq 0 ]; then
    echo "Warning: No .ch8 files found in the games directory."
    echo "The menu will work, but there won't be any games to select."
    echo "You can copy .ch8 files to the games directory before running the menu."
fi

echo
echo "=== Test Instructions ==="
echo "1. Run the menu system with: ./menu"
echo "2. Navigate using arrow keys, select with Enter, go back with Escape"
echo "3. Verify you can browse the games directory and see .ch8 files"
echo "4. Verify you can navigate into subdirectories if present"
echo "5. Try launching a game to ensure it works correctly"
echo
echo "Starting menu system..."
./menu