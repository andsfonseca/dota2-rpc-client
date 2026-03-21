#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SERVICE_NAME="dota2-rpc-client.service"
INSTALL_DIR="$HOME/.local/share/dota2-rpc-client"
SYSTEMD_DIR="$HOME/.config/systemd/user"

# Check if running from build directory
if [ ! -f "$SCRIPT_DIR/dota2-rpc-client" ]; then
    echo "Error: dota2-rpc-client binary not found in the same directory as this script."
    echo "Please run this script from the build directory where the binary is located."
    exit 1
fi

# Check if service file exists
if [ ! -f "$SCRIPT_DIR/$SERVICE_NAME" ]; then
    echo "Error: $SERVICE_NAME not found in the same directory as this script."
    exit 1
fi

# Stop and disable service if already running
if systemctl --user is-active --quiet dota2-rpc-client.service; then
    echo "Stopping existing service..."
    systemctl --user stop dota2-rpc-client.service
fi

# Create installation directory
mkdir -p "$INSTALL_DIR"

# Copy binary
cp "$SCRIPT_DIR/dota2-rpc-client" "$INSTALL_DIR/"
chmod +x "$INSTALL_DIR/dota2-rpc-client"

# Copy configuration files
if [ -f "$SCRIPT_DIR/config.json" ]; then
    cp "$SCRIPT_DIR/config.json" "$INSTALL_DIR/"
else
    echo "Warning: config.json not found. Please configure manually."
fi

# Copy lang directory
if [ -d "$SCRIPT_DIR/lang" ]; then
    cp -r "$SCRIPT_DIR/lang" "$INSTALL_DIR/"
else
    echo "Warning: lang directory not found."
fi

# Copy templates directory
if [ -d "$SCRIPT_DIR/templates" ]; then
    cp -r "$SCRIPT_DIR/templates" "$INSTALL_DIR/"
else
    echo "Warning: templates directory not found."
fi

# Create uploads directory (Bug fix drogon: Ensure uploads directory exists)
mkdir -p "$INSTALL_DIR/uploads"

# Install systemd service
mkdir -p "$SYSTEMD_DIR"
cp "$SCRIPT_DIR/$SERVICE_NAME" "$SYSTEMD_DIR/"

# Reload systemd daemon
systemctl --user daemon-reload

# Start service
systemctl --user enable dota2-rpc-client.service
systemctl --user start dota2-rpc-client.service
sleep 2

# Check service status
if systemctl --user is-active --quiet dota2-rpc-client.service; then
    echo "Done!"
else
    echo "Service was installed but failed to start."
    echo "Check the logs with: journalctl --user -u dota2-rpc-client.service"
    exit 1
fi
