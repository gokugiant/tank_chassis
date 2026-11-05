# Testnachweis

Am 5. November 2026 am angeschlossenen Fahrzeug durchgeführt. Keine automatische Positions- oder Drehzahlmessung vorhanden.

| Prüfung | Nachweis / Ergebnis |
| --- | --- |
| USB/Chip | Serielle Verbindung; Flashwerkzeug erkennt ESP32-D0WDQ6 Rev. 1.0 |
| Firmware | Build und Upload erfolgreich; Flash-Prüfsummen bestätigt |
| Startzustand | Serielle Antwort `v4 active=0 next=1 button=0` nach Stopp/Reset der Testfolge |
| RGB | Besitzer bestätigt Rot, Grün und Blau einzeln |
| Display und RGB | Besitzer bestätigt Ampelanzeige mit synchronem Taster-Farbwechsel unter PinTest |
| Vier Motortests | Besitzer bestätigt nach Korrektur links vor/zurück und rechts vor/zurück passend zur Anzeige |
| Displayorientierung | Besitzer bestätigt Rotation0 und Hochformat als richtig |
| Impulsabschaltung | ESP-Timer im Code, serielle Stoppmeldungen; Besitzer bestätigt die Testfolge, Zeit nicht mit Messgerät geprüft |
| Tasterstopp während Bewegung | Implementiert, nicht ausdrücklich als eigener Hardwaretest bestätigt |
| Servo | deaktiviert; keine Prüfung |

## Maßgeblicher Schlussbefund

Besitzer: „ok jetzt funktioniert es genau richtig so. beide ketten machen genau was das display zeigt.“ Dies bestätigt die korrigierte v4-Firmware. Frühere kurze Tests mit Zucken und widersprüchlichen Links-/Richtungsangaben werden nicht als finale Zuordnung verwendet.

Die Webcamaufnahme bestätigte eine kurze Bewegung der oberen/linken Kette, ließ die Richtung ohne Markierung jedoch nicht zuverlässig bestimmen. Maßgeblich für die Richtungen ist der abschließende Vier-Schritte-Test des Besitzers.

## Offene Prüfungen

- Lastbetrieb auf dem Boden, Mindest-PWM, Drehzahl und Fahrgeschwindigkeit.
- Thermik und Stromaufnahme bei längerer Belastung.
- Elektrischer Versorgungspfad, Jumper, Reset-/Bootverhalten bei verschiedenen Einschaltreihenfolgen.
- Eigener Test des vorzeitigen Tasterstopps.
- Exakte Boardrevision und Widerstandswerte.
- Netzwerksteuerung samt Befehls-Timeout und Verbindungsabbruch; noch nicht implementiert.

Originaler Flash vor der ersten Änderung lokal gesichert; Wiederherstellung nicht getestet. Keine Sensorik für gemessene Geschwindigkeit, Position oder Batteriespannung.
