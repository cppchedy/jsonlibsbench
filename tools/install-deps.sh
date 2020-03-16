#!/bin/bash

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
else
    echo "couldn't determine Linux distribution"
    echo "please install the following packages manually before running the script:"
    echo "1. numactl"
    echo "2. hwloc"
    exit -1
fi

case $OS in
Fedora)
    echo "Operating System is Fedora with version $VERSION_ID"
    PCK_MNG="dnf"
    sudo dnf groupinstall "Development Tools"
    ;;
Ubuntu)
    echo "Operating System is Ubuntu with version $VERSION_ID"
    PCK_MNG="apt install"
    ;;
*)
    echo "Error the script shouldn't get here if we are neither in Ubuntu nor in Fedora"
    ;;
esac


sudo $PCK_MNG install -y numactl hwloc cmake
