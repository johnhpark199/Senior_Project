import serial
import os
from serial.serialutil import SerialException
from serial.serialwin32 import Serial
from time import gmtime, strftime

RC2_COM_PORT = "COM4"

RC2_RESULTS_FILENAME = "RC2_TEST999_RESULTS.csv"

def setup_com_ports(rc2 :Serial):
    rc2.baudrate = 115200
    rc2.port = RC2_COM_PORT
    try:
        rc2.open()
    except SerialException:
        print("Could not open the RC2 COM Port")
        rc2.close()
        return -1

def main():
    rc2 = serial.Serial()

    rc2_results_file = 0

    if not os.path.exists(RC2_RESULTS_FILENAME):
        RC2_results_file = open(RC2_RESULTS_FILENAME, 'w', newline='')
    else:
        print("RC2 results file already exists - try a new filename");


    setup_com_ports(rc2);
    rc2.flushInput()

    csv_header = "timestamp,rssi,snr,sduid,muid,data,hops,latitude,longitude,altitude,gps_time\n"
    rc2_results_file.write(csv_header)

    while True:
        rc2_line = rc2.readline().decode('utf-8', errors="ignore").strip()
        data_timestamp = strftime("%Y-%m-%d %H:%M:%S", gmtime())
        rc2_results_file.write("{},{}\n".format(data_timestamp, rc2_line))
        print("{}".format(rc2_line))

if __name__ == "__main__":
    main()