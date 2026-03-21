#!/bin/bash

# Dota 2 RPC Client - Service Uninstallation Script
# This script stops and removes the Dota 2 RPC Client systemd user service

set -e

SERVICE_NAME="dota2-rpc-client.service"
INSTALL_DIR="$HOME/.local/share/dota2-rpc-client"
SYSTEMD_DIR="$HOME/.config/systemd/user"

# Check if service is installed
if [ ! -f "$SYSTEMD_DIR/$SERVICE_NAME" ]; then
    echo "Service is not installed."
    echo ""
    read -p "Do you want to remove the installation directory anyway? (y/N): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if [ -d "$INSTALL_DIR" ]; then
            rm -rf "$INSTALL_DIR"
            echo "Installation directory removed."
        else
            echo "Installation directory not found."
        fi
    fi
    exit 0
fi

# Stop service if running
if systemctl --user is-active --quiet dota2-rpc-client.service; then
    systemctl --user stop dota2-rpc-client.service
fi

# Disable service
systemctl --user disable dota2-rpc-client.service 2>/dev/null || true
rm -f "$SYSTEMD_DIR/$SERVICE_NAME"
systemctl --user daemon-reload

# Reset failed state if any
systemctl --user reset-failed dota2-rpc-client.service 2>/dev/null || true

echo "Done!"

read -p "Do you want to remove the installation directory and all data? (y/N): " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    if [ -d "$INSTALL_DIR" ]; then
        rm -rf "$INSTALL_DIR"
        echo "Installation directory removed."
    else
        echo "Installation directory not found."
    fi
fi
