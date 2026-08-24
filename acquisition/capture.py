import argparse
import threading
import time

# Variable globale pour signaler l'arrêt aux threads
stop_event = threading.Event()


def read_serial(port, filename):
    print(f"Démarrage : {port} -> {filename}")
    try:
        # Remplacer open par serial.Serial(port, 9600, timeout=1) si pySerial est utilisé
        with open(port, "r") as src, open(filename, "w") as out:
            while not stop_event.is_set():
                line = src.readline()
                if line:
                    out.write(line)
                    out.flush()
    except Exception as e:
        print(f"Erreur sur {port} : {e}")


# Configuration des 5 arguments
parser = argparse.ArgumentParser()
parser.add_argument("port1", default="/dev/ACM0", help="Serial Port 1 | Example: python3 capture.py /dev/ttyACM0 out1.txt /dev/ttyACM1 out2.txt 7200")
parser.add_argument("file1", default="ACM0.txt",  help="File 1        | Example: python3 capture.py /dev/ttyACM0 out1.txt /dev/ttyACM1 out2.txt 7200")
parser.add_argument("port2", default="/dev/ACM1", help="Serial Port 2 | Example: python3 capture.py /dev/ttyACM0 out1.txt /dev/ttyACM1 out2.txt 7200")
parser.add_argument("file2", default="ACM1.txt",  help="File 2        | Example: python3 capture.py /dev/ttyACM0 out1.txt /dev/ttyACM1 out2.txt 7200")
parser.add_argument(
    "timeout", type=float,default=7200.0,  help="Limite de temps en secondes (ex: 10)"
)
args = parser.parse_args()

# Lancement des threads
t1 = threading.Thread(target=read_serial, args=(args.port1, args.file1))
t2 = threading.Thread(target=read_serial, args=(args.port2, args.file2))
t1.start()
t2.start()

start_time = time.time()
try:
    # Boucle d'attente avec affichage du compteur
    while time.time() - start_time < args.timeout:
        elapsed = int(time.time() - start_time)
        # Formate le temps en HH:MM:SS
        timer_str = time.strftime("%H:%M:%S", time.gmtime(elapsed))
        # \r permet de réécrire sur la même ligne dans la console
        print(f"\rTemps écoulé : {timer_str}", end="", flush=True)
        time.sleep(1)
except KeyboardInterrupt:
    print("\nInterruption manuelle demandée...")

print("\nArrêt des enregistrements...")
stop_event.set()
stop_event.set()

t1.join()
t2.join()
print("Terminé.")
