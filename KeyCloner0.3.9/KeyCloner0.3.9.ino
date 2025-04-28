#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

#define T_CS    33
#define T_IRQ   36
#define T_MOSI  32
#define T_MISO  39
#define T_CLK   25

SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(T_CS, T_IRQ);
TFT_eSPI tft = TFT_eSPI();

const int sections = 13;
int topLineY[sections];
int bottomLineY[sections];
bool modified[sections];
bool flattenMode = true;
int sectionWidth = 10;
int sectionStartX = 185;
int selected = 7;

void drawLines() {
  tft.fillScreen(TFT_BLACK);

  // Botones
  tft.fillRect(0, 0, 60, 40, TFT_GREEN); 
  tft.drawString("↑", 25, 10, 2);
  tft.fillRect(0, 40, 60, 40, TFT_RED);  
  tft.drawString("↓", 25, 50, 2);
  
  tft.fillRect(0, 80, 60, 40, TFT_WHITE);
  for (int i = 0; i < 60; i += 6) {
    tft.drawFastHLine(0, 80 + i, 60, TFT_CYAN);
  }
  tft.setTextColor(TFT_BLACK);
  tft.drawString(flattenMode ? "FLAT" : "NORM", 10, 90, 2); // Mostrar modo
  tft.setTextColor(TFT_WHITE);

  tft.fillRect(60, 0, 60, 40, TFT_BLUE); 
  tft.drawString("←", 85, 10, 2);
  tft.fillRect(120, 0, 60, 40, TFT_YELLOW); 
  tft.drawString("→", 145, 10, 2);

  // Dibujar la línea principal
  for (int i = 0; i < sections - 1; i++) {
    int x1 = sectionStartX + i * sectionWidth;
    int x2 = sectionStartX + (i + 1) * sectionWidth;

    if (modified[i]) {
      int midX = x1 + 8;
      tft.drawLine(x1, topLineY[i], midX, topLineY[i], TFT_ORANGE);
      tft.drawLine(midX, topLineY[i], x2, topLineY[i + 1], TFT_ORANGE);
    } else {
      tft.drawLine(x1, topLineY[i], x2, topLineY[i + 1], TFT_ORANGE);
    }
    tft.drawLine(x1, bottomLineY[i], x2, bottomLineY[i + 1], TFT_ORANGE);
  }

  // Indicador de sección
  int sx = sectionStartX + selected * sectionWidth;
  int sy = topLineY[selected] - 10;
  tft.drawRect(sx - 2, sy, sectionWidth + 4, 20, TFT_WHITE);
}

void applyFlattenState() {
  // Actualizar el estado de la sección seleccionada inmediatamente
  modified[selected] = flattenMode;
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  touchSPI.begin(T_CLK, T_MISO, T_MOSI, T_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);

  for (int i = 0; i < sections; i++) {
    topLineY[i] = 120;
    bottomLineY[i] = 173;
    modified[i] = false;
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
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 500 - 130 && tx <= 500 + 130 && ty >= 1290 - 130 && ty <= 1290 + 130) {
      // Abajo
      topLineY[selected] += 2;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 1119 - 130 && tx <= 1119 + 130 && ty >= 608 - 130 && ty <= 608 + 130) {
      // Izquierda
      selected--;
      if (selected < 0) selected = 0;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 1823 - 130 && tx <= 1823 + 130 && ty >= 592 - 130 && ty <= 592 + 130) {
      // Derecha
      selected++;
      if (selected >= sections) selected = sections - 1;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 533 - 130 && tx <= 533 + 130 && ty >= 1987 - 130 && ty <= 1987 + 130) {
      // Botón especial
      flattenMode = !flattenMode;
      applyFlattenState(); // <<< Aplicar inmediatamente al seleccionado
      drawLines();
      delay(300);
    }
  }
}
