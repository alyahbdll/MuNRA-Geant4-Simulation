# MuNRA Data Acquisition

This directory contains the Python script used for simultaneous data acquisition from two MuNRA detectors during the experimental measurement campaign.

## Acquisition script

`capture.py` reads the serial output of two MuNRA detectors in parallel and stores the data from each detector in a separate text file.

The acquisition duration is specified in seconds when launching the script.

## Usage

Example for a two-hour acquisition:

```bash
python3 capture.py /dev/ttyACM0 out1.txt /dev/ttyACM1 out2.txt 7200
```

where:

- `/dev/ttyACM0` is the serial port of the first detector;
- `out1.txt` is the corresponding output file;
- `/dev/ttyACM1` is the serial port of the second detector;
- `out2.txt` is the corresponding output file;
- `7200` is the acquisition duration in seconds (2 hours).

The two detector streams are recorded simultaneously using separate threads.

## Output

The script preserves the raw text output produced by each MuNRA detector. Each detector is recorded in an independent output file for subsequent data analysis.
