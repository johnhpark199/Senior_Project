
import ctypes
import os
import platform
import serial
import serial.tools.list_ports

import usb.core
import usb.util

if __name__ == "__main__":
    import sys

    print("OS Name: {}".format(os.name))
    print("Machine Architecture: {}".format(platform.architecture()))

    if(os.name == "nt"):
        _DLL = ctypes.WinDLL(os.path.join(os.path.dirname(__file__), "./CP210xRuntime.dll"))