#!/bin/bash

PACKAGES=(
  libcurl4-openssl-dev
  libjsoncpp-dev
  libasio-dev
)

echo Installing packages...
sudo apt update

for pkg in "${PACKAGES[@]}"; do
  echo "Installing $pkg..."
  sudo apt install -y $pkg
done
