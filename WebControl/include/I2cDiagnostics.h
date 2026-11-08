#pragma once

// Einmalige Adress- und Chip-ID-Diagnose des bereits initialisierten I2C-Busses.
// Keine Konfigurationsregister schreiben; keine Motor-/Servobefehle.
void printI2cDiagnostics();
