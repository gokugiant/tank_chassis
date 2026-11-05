# MotorTest v4 – bestätigter Tastertest

Aktuelle Firmware für den vorhandenen Tank. **Besitzer bestätigt: beide Ketten stimmen mit den Displaypfeilen überein, Displayausrichtung passt.**

## Bedienung

Ein Tastendruck startet genau einen Versuch für **750 ms bei PWM128/255**, danach automatisch Stopp:

1. Links vorwärts
2. Links rückwärts
3. Rechts vorwärts
4. Rechts rückwärts

Nach Schritt 4 wieder Schritt 1. Halten wiederholt nichts. Beim Einschalten muss der Taster zuerst losgelassen werden. Ein erneuter Druck während der Bewegung stoppt; der nächste Druck startet den bereits nächsten Schritt. Zum zuverlässigen Test jeden automatischen Stopp abwarten.

Display im Hochformat: VORNE oben, linke/rechte Kette mit Pfeil, Testnummer und nächster Schritt. Pfeil orange während des Versuchs, weiß im Stillstand. Der letzte Test bleibt angezeigt. RGB rot bei Stopp, blau während der Bewegung. Der Taster schaltet hier keine RGB-Farben mehr durch. Servo bleibt deaktiviert, kein WLAN.

## Bestätigte Ansteuerung

| Schritt | Richtung | Enable-GPIO | HIGH / LOW |
| --- | --- | ---: | --- |
| 1 | Links vorwärts | 21 | 17 / 22 |
| 2 | Links rückwärts | 21 | 22 / 17 |
| 3 | Rechts vorwärts | 13 | 15 / 2 |
| 4 | Rechts rückwärts | 13 | 2 / 15 |

Vollständige [Hardwarebeschreibung](../docs/HARDWARE.md). Der Timer schaltet unabhängig von Display-/Konsolenverarbeitung ab. Tasterstopp läuft entprellt im Hauptprogramm; er ist kein unabhängiger Not-Aus. Displayzustand bezeichnet den ausgegebenen Befehl, keine gemessene Bewegung.

## Bauen und Aufspielen

PlatformIO, Projektordner `MotorTest/`:

```sh
pio run
pio run -t upload --upload-port /dev/cu.usbserial-01E5F16A
pio device monitor --baud 115200 --port /dev/cu.usbserial-01E5F16A
```

Monitor vor Upload schließen; Port gegebenenfalls anpassen. Die Konfiguration verwendet espressif32 6.10.0 und TFT_eSPI 2.5.43. Displayrotation 0, 135 × 240 Pixel. Lokale Buildflags vermeiden globale Bibliotheksänderungen.

Serielle Befehle mit Zeilenende: `stop` stoppt, `status` zeigt `active`, `next` und Tasterwert, `reset` setzt die Folge im Stillstand auf Schritt 1. Kein serieller Fahrbefehl in v4. Das Öffnen des seriellen Anschlusses kann je nach Werkzeug einen Reset auslösen; danach beginnt die Folge bei 1 und die Motoren sind aus.

Die vorherige Quelle `legacy/main-v3.cpp` wird nicht mitgebaut. Historische, teils verworfene Beobachtungen stehen im [archivierten Testprotokoll](../docs/archive/motor-test-log.md). Maßgeblich ist die obige bestätigte Tabelle.
