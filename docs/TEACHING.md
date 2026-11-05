# Einsatz in der Vorlesung

Die lokal bestätigte Basis umfasst Taster, RGB, Display und einzeln geschaltete Motoren. Damit lässt sich vom GPIO über PWM und Zustandsautomaten zur Webanwendung auf demselben Gerät übergehen.

## Nächste Ausbaustufen

1. Bestätigte Pins und Richtungen in eine gemeinsame Hardwarekonfiguration übernehmen; den Vier-Schritte-Test als Diagnosemodus behalten.
2. ESP32-Access-Point und lokale Webseite: zuerst Taster, LED-Farbe, Laufzeit und Motor-Sollwerte als JSON anzeigen.
3. Browsersteuerung: Freigabe, begrenzte PWM, Bedienung nur bei gehaltenem Steuerelement und regelmäßig erneuerten Befehlen. Ausbleibende Auffrischung muss geräteseitig stoppen. Fokusverlust, mehrere Browser und Verbindungsabbruch testen.
4. Ausgehende Internet-Telemetrie per HTTPS oder MQTT/TLS an Server mit Verlauf. Keine Portfreigabe auf das Fahrzeug nötig; Internet-Fahrsteuerung gesondert behandeln.

Diese Funktionen sind geplant, noch nicht implementiert. PWM-Sollwerte immer von gemessener Geschwindigkeit unterscheiden. Für Sensordaten muss ein zusätzlicher Sensor ergänzt werden.

## Vorschlag für 90 Minuten

| Zeit | Inhalt |
| --- | --- |
| 15 min | Mikrocontroller, GPIO, Motorleistung und Treiber |
| 15 min | Taster, Display, PWM, Zustandsautomat anhand des funktionierenden Tests |
| 25 min | Lokale Webseite und JSON-Status auf dem ESP32 |
| 20 min | Begrenzte Browsersteuerung und Stopp bei Verbindungsverlust |
| 15 min | Vorbereitete Internet-Telemetrie und Aufgaben für eigene Projekte |

KI-Aufgabe: Eine Statusseite ergänzen, Kommunikationswege erklären lassen und veraltete Daten sichtbar machen. Studierende prüfen Pins, Laufzeitort des Codes und Fehlerverhalten selbst. Die Web-/Internet-Beispiele müssen vor dem Einsatz noch implementiert und geprüft werden.
