//Visita apashura2.com y pronto podrás ganar dinero y maquillaje 
//https://github.com/LittleHackid

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h> //Modifica los pines de tu librería, esta está configurada para el ESP32 CYD
#include <SPI.h>

#define T_CS    33
#define T_IRQ   36
#define T_MOSI  32
#define T_MISO  39
#define T_CLK   25

SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(T_CS, T_IRQ);
TFT_eSPI tft = TFT_eSPI();

// Coordenadas de cada sección
const int sections = 8;
int topLineY[sections];
int bottomLineY[sections];
int sectionWidth = 13;
int sectionStartX = 200;
int selected = 0;

void drawLines() {
  tft.fillScreen(TFT_BLACK);

  // Dibujo de botones (solo visual)
  tft.fillRect(0, 0, 60, 40, TFT_GREEN);       // Arriba
  tft.drawString("↑", 25, 10, 2);
  tft.fillRect(0, 40, 60, 40, TFT_RED);        // Abajo
  tft.drawString("↓", 25, 50, 2);
  tft.fillRect(60, 0, 60, 40, TFT_BLUE);       // Izquierda
  tft.drawString("←", 85, 10, 2);
  tft.fillRect(120, 0, 60, 40, TFT_YELLOW);    // Derecha
  tft.drawString("→", 145, 10, 2);

  // Dibujar líneas
  for (int i = 0; i < sections - 1; i++) {
    int x1 = sectionStartX + i * sectionWidth;
    int x2 = sectionStartX + (i + 1) * sectionWidth;

    // Top
    tft.drawLine(x1, topLineY[i], x2, topLineY[i + 1], TFT_ORANGE);
    // Bottom
    tft.drawLine(x1, bottomLineY[i], x2, bottomLineY[i + 1], TFT_ORANGE);
  }

  // Indicador de sección seleccionada (más pequeño)
  int sx = sectionStartX + selected * sectionWidth;
  int sy = topLineY[selected] - 10; // Un poquito arriba del pico
  tft.drawRect(sx - 2, sy, sectionWidth + 4, 20, TFT_WHITE); // Rectángulo pequeño
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  touchSPI.begin(T_CLK, T_MISO, T_MOSI, T_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);

  // Inicializar alturas
  for (int i = 0; i < sections; i++) {
    topLineY[i] = 130;
    bottomLineY[i] = 160;
  }

  drawLines();
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    int tx = p.x;
    int ty = p.y;
    Serial.printf("Touch -> X: %d, Y: %d\n", tx, ty);

    if (tx >= 455 - 130 && tx <= 455 + 130 && ty >= 572 - 130 && ty <= 572 + 130) {
      // Arriba
      topLineY[selected] -= 2;
      drawLines();
      delay(150);
    } else if (tx >= 500 - 130 && tx <= 500 + 130 && ty >= 1290 - 130 && ty <= 1290 + 130) {
      // Abajo
      topLineY[selected] += 2;
      drawLines();
      delay(150);
    } else if (tx >= 1119 - 130 && tx <= 1119 + 130 && ty >= 608 - 130 && ty <= 608 + 130) {
      // Izquierda
      selected--;
      if (selected < 0) selected = 0;
      drawLines();
      delay(150);
    } else if (tx >= 1823 - 130 && tx <= 1823 + 130 && ty >= 592 - 130 && ty <= 592 + 130) {
      // Derecha
      selected++;
      if (selected >= sections) selected = sections - 1;
      drawLines();
      delay(150);
    }
  }
}
