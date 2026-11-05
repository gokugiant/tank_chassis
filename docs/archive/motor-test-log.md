> Historisches Arbeitsprotokoll mit vorläufigen und später verworfenen Zuordnungen. Maßgeblich ist docs/HARDWARE.md.

# MotorTest v4 – vier Tests per Taster

Aktuelle Firmware: ein Tastendruck startet genau einen Versuch, 750 ms bei PWM128/255 (ca. 50 %). Danach automatisch Stopp; kein selbsttätiger Wechsel. Halten wiederholt den Test nicht. Beim Einschalten muss der Taster zunächst losgelassen werden. Ein erneuter Druck während der Bewegung stoppt und überspringt zum nächsten vorgesehenen Test.

| Schritt | Anzeige / angenommene Richtung | Enable | Richtung HIGH / LOW |
| --- | --- | --- | --- |
| 1 | Links vorwärts | GPIO21 | GPIO17 / GPIO22 |
| 2 | Links rückwärts | GPIO21 | GPIO22 / GPIO17 |
| 3 | Rechts vorwärts | GPIO13 | GPIO15 / GPIO2 |
| 4 | Rechts rückwärts | GPIO13 | GPIO2 / GPIO15 |

Nach Schritt 4 beginnt die Folge wieder bei 1. Besitzer bestätigte nach dem Vier-Schritte-Test: rechte Kette passend, linke Richtungen vertauscht. Linke Richtungen daraufhin korrigiert; erneute Prüfung steht aus. Display im Hochformat (Rotation 0 statt zuvor 1, 90 Grad nach links korrigiert) zeigt zwei Ketten (L/R), einen Pfeil für den gewählten Versuch und VORNE an der oberen Displaykante. Während der Bewegung orange, nach Stopp weiß; letzter Test bleibt sichtbar. Rechts steht der nächste Schritt. RGB-LED rot bei Stopp, blau beim Versuch. Taster schaltet hier keine RGB-Farben mehr durch.

Servo bleibt deaktiviert. Keine WLAN-Funktion. `stop` stoppt, `status` meldet Zustand, `reset` setzt die Folge im Stillstand auf Schritt 1. Konsole 115200 Baud. Die frühere v3-Quelle liegt unter `legacy/` und wird nicht mitgebaut.

Beide Ketten beim Test frei halten, Finger von Ketten und Rollen fernhalten. Timer und Taster sind Softwarefunktionen, kein unabhängiger Not-Aus. Displayzustand bezeichnet den Softwarebefehl, keine gemessene Bewegung.

## Frühere Testhistorie (v3, inzwischen ersetzt)

# MotorTest v3

Separater Nachfolger des bestätigten Display/RGB-Tests unter `PinTest/`. Beim Start sind alle Motorleitungen LOW, PWM ist 0. Servo bleibt deaktiviert. RGB-Ampel und Farbwechsel funktionieren weiter. Tasterdruck stoppt zusätzlich sofort im Hauptprogramm und löscht die Freigabe.

Nur serielle Einzelversuche, 115200 Baud mit Zeilenende:

1. `arm`: einmalige Freigabe, verfällt nach fünf Sekunden.
2. Genau ein `p13a+`, `p13a-`, `p13b+`, `p13b-`, `p21a+`, `p21a-`, `p21b+` oder `p21b-`.
3. Ein ESP-Timer stoppt nach 750 ms unabhängig von Display/serieller Verarbeitung. PWM128/255 (ca. 50 %), 1 kHz. Keine Dauerfahrt.

13/21 bezeichnet den Enable-GPIO. a = IN1/IN2 (GPIO15/2), b = IN3/IN4 (GPIO17/22). + aktiviert den ersten, - den zweiten Richtungseingang. Nur ein Enable wird aktiviert, der andere bleibt 0. Diese Kombinationen dienen der Zuordnungsprüfung; es gibt noch keine bestätigte Links/Rechts- oder Vorwärts/Rückwärts-Zuordnung.

`stop`: PWM0 und alle Motor-GPIOs LOW, Freigabe gelöscht. `status`: Softwarestatus. Auch jeder andere gewöhnliche Befehl stoppt einen laufenden Impuls. Rot/Grün/Blau/off steuern nur die LED.

Vor Motorversuchen: Chassis aufbocken, beide Ketten frei, geeignete Stromversorgung, gemeinsame Masse und Treiber-Jumper prüfen. Zwischen Versuchen Sichtkontrolle und Stillstand bestätigen. Keine automatische Testfolge. Ein fehlender Impuls kann an Mindestleistung, falscher Enable-Zuordnung oder Verdrahtung liegen; Leistung nicht ungeprüft erhöhen. Taster und Timeout sind Softwarefunktionen, kein unabhängiger Not-Aus. Boot-/Resetpegel sind davon nicht abgesichert.

Stand: Firmware erfolgreich gebaut. Upload und Motorzuordnung noch nicht bestätigt. Display, RGB-Farben und Taster wurden zuvor vom Besitzer bestätigt.

## Erster Test

Upload erfolgreich, Flash-Prüfsummen bestätigt. Nach bestätigtem Ruhepegel GPIO32=0 genau einen `p13a+`-Impuls ausgelöst (Enable GPIO13, IN1 GPIO15 HIGH, IN2 GPIO2 LOW, PWM100/255, 150 ms). Firmware meldete `AUTO/LOCAL STOP`, danach zusätzlich `stop` und Status `motor_active=0 armed=0`. Physische Kette und Drehrichtung noch nicht vom Besitzer zugeordnet; keine weiteren Impulse ausgelöst.

Besitzerbeobachtung: `p13a+` bewegte die **linke Kette rückwärts** (Links/Rechts in Fahrtrichtung). Damit vorläufig links: Enable GPIO13, IN1/IN2 GPIO15/2; rückwärts 15 HIGH / 2 LOW. Anschließend genau ein `p13a-`-Impuls mit unverändert 150 ms und PWM100/255 ausgeführt. Automatischer Stopp und abschließender Status `motor_active=0 armed=0` bestätigt. Erwartung: linke Kette vorwärts; physische Bestätigung dieses zweiten Versuchs steht noch aus.

Dritter Versuch: Besitzer meldete beim zweiten Versuch nur kurzes Zucken. Nach erneuter Bestätigung frei drehender Ketten Impuls auf 250 ms / PWM128 von 255 erhöht, erfolgreich gebaut und aufgespielt. Genau ein `p13a-` ausgeführt; `AUTO/LOCAL STOP` und danach `motor_active=0 armed=0` bestätigt. Beobachtung der tatsächlichen Bewegung noch ausstehend.

Auf ausdrücklichen Wunsch längerer Einzeltest: Timergrenze jetzt 750 ms bei weiterhin PWM128/255. Build und Upload erfolgreich. Genau ein `p21b+` (Enable21, IN3 GPIO17 HIGH / IN4 GPIO22 LOW) ausgeführt; automatischer Stopp sowie abschließend `motor_active=0 armed=0` bestätigt. Beobachtung der Kette und Richtung noch offen.

Besitzer bestätigt für `p21b+` bei 750 ms: **linke Kette rückwärts**. Damit Enable21/IN3 GPIO17/IN4 GPIO22 als linker Kanal beobachtet; frühere kurze Tests ergaben widersprüchliche Beobachtungen und gelten nicht als gesicherte Zuordnung. Anschließend genau ein `p21b-` bei 750 ms / PWM128 ausgeführt; automatischer Stopp und abschließend `motor_active=0 armed=0` bestätigt. Gegenrichtung physisch noch zu bestätigen.

Korrektur nach v4-Handtest: linke Vor-/Rückwärts-Pins vertauscht, rechte Richtungen unverändert. Display von Rotation1 auf Rotation0 geändert und Layout an 135×240 Pixel angepasst, da Besitzer die Anzeige um 90 Grad nach rechts verdreht sah. Tests bleiben auf 750 ms / 50 % PWM begrenzt.
