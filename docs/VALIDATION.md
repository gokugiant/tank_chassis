# Validierungsstand

Stand: **8. November 2026**. Aktive Firmware: `WebControl/`.

## Aktueller Befund

Die neueste Firmware wurde erfolgreich gebaut und hochgeladen, Flash-Hashes
verifiziert. Implementiert sind neue Vorderseite, TFT-Rotation2, Kurvenpad und
±360° zusätzlich zur Messwertanzeige, Kalibrierung, Geradeauskorrektur und
Dreh-Feinkorrektur. Softwaretests bestehen.

**Sensor aktuell nicht erreichbar:** Nach dem letzten Upload sowie einem Neustart
meldete die I²C-Suche an SDA33/SCL32 `devices=0`. Zuvor BMI160 an 0x68 mit 0xD1 und
gültigen Messwerten bestätigt. Sensorpins unverändert. Deshalb ist die neueste
Winkel-/Drehsteuerung derzeit nicht physisch prüfbar.

## Evidenzgrenzen

| Bereich                                                            | Stand                                                                                         |
| ------------------------------------------------------------------ | --------------------------------------------------------------------------------------------- |
| RGB, ursprünglicher Display-/Tastertest, einzelne Kettenrichtungen | Besitzer bestätigt; historischer Test vom 5. November                                         |
| Webseite und Sensoranzeige                                         | Besitzer bestätigt funktionsfähig                                                             |
| Relative Winkelrichtung                                            | Besitzer: Nase nach links ergibt etwa +90°                                                    |
| Geradeauskorrektur / Mindest-PWM                                   | Besitzer bestätigt grundsätzlich gut; unter 50 % zu schwach                                   |
| Frühere Kipperkennung                                              | Fehlalarm beim Anfahren gemeldet, gefilterte Prüfung ergänzt                                  |
| Drehungen mit höherer PWM                                          | Besitzer bestätigt gut, bemerkt einige Grad Nachlauf                                          |
| Neueste Nachlauf-/Feinkorrektur                                    | Modelltests bestanden; reale Genauigkeit nicht bestätigt                                      |
| Neue Vorderseite, TFT-Rotation2, Kurven, ±360°                     | Host-/Browserlogiktests und Build bestanden; physischer Test offen                            |
| Aktueller Sensor                                                   | Keine Antwort; Verbindung wiederherstellen                                                    |
| Servo                                                              | Nominelle Winkel/Signale implementiert; mechanische Grenzen/Mitte nicht vollständig validiert |
| Abbruch bei Netzwerk-/Sensorfehlern                                | Im Code und in Logiktests geprüft; kein vollständiger realer Abbruch-Testnachweis             |

Hosttests prüfen Hardwareklassen, PWM-/Stoppgrenzen, Zeitüberläufe, Sensorumrechnung,
Kalibrierung, Winkelintegration und Navigation mit einfachen Trägheitsmodellen.
Browserlogiktests nutzen nachgebildetes DOM/HTTP; sie ersetzen keine echte Browser-
oder Funkprüfung. Kein Joystick implementiert, keine Positions-/Wegdrehzahlmessung.

- [Ausführlicher Implementierungs- und Testverlauf](../WebControl/VALIDATION.md)
- [Wiedereinstieg](START_HERE.md)
- [Originaler Vier-Schritte-Test vom 5. November](archive/validation-2026-11-05.md)

Bei Softwarearbeiten wurden keine autonomen Bewegungsbefehle vom Agenten gesendet.
Mechanische Genauigkeit, Nachlaufparameter, Stromversorgung und thermische
Dauerbelastung bleiben am realen Fahrzeug zu prüfen.
