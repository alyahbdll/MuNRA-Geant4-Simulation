import argparse
import threading
import time

# Global event used to stop both acquisition threads
stop_event = threading.Event()


def read_serial(port, filename):
    print(f"Starting acquisition: {port} -> {filename}")

    try:
        # Read directly from the Linux serial device
        with open(port, "r") as src, open(filename, "w") as out:
            while not stop_event.is_set():
                line = src.readline()

                if line:
                    out.write(line)
                    out.flush()

    except Exception as e:
        print(f"Error while reading {port}: {e}")


# Command-line arguments
# Example:
# python3 capture.py /dev/ttyACM0 out1.txt /dev/ttyACM1 out2.txt 7200
#
# The acquisition duration is given in seconds.

parser = argparse.ArgumentParser(
    description="Simultaneous timed acquisition from two MuNRA detectors."
)

parser.add_argument(
    "port1",
    help="Serial port of detector 1 (e.g. /dev/ttyACM0)"
)

parser.add_argument(
    "file1",
    help="Output file for detector 1"
)

parser.add_argument(
    "port2",
    help="Serial port of detector 2 (e.g. /dev/ttyACM1)"
)

parser.add_argument(
    "file2",
    help="Output file for detector 2"
)

parser.add_argument(
    "timeout",
    type=float,
    help="Acquisition duration in seconds (e.g. 7200 for 2 hours)"
)

args = parser.parse_args()


# Start one acquisition thread for each detector
t1 = threading.Thread(
    target=read_serial,
    args=(args.port1, args.file1)
)

t2 = threading.Thread(
    target=read_serial,
    args=(args.port2, args.file2)
)

t1.start()
t2.start()


# Acquisition timer
start_time = time.time()

try:
    while time.time() - start_time < args.timeout:
        elapsed = int(time.time() - start_time)

        # Format elapsed time as HH:MM:SS
        timer_str = time.strftime(
            "%H:%M:%S",
            time.gmtime(elapsed)
        )

        print(
            f"\rElapsed time: {timer_str}",
            end="",
            flush=True
        )

        time.sleep(1)

except KeyboardInterrupt:
    print("\nManual interruption requested.")


# Stop both acquisition threads
print("\nStopping acquisition...")

stop_event.set()

t1.join()
t2.join()

print("Acquisition completed.")
