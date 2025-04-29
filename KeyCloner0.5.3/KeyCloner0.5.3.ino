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
int sectionWidths[sections];  // <<< CAMBIO: Ancho por sección

bool flattenMode = true;

int sectionStartX = 155;
int selected = 12;

void drawLines() {
  // <<< Recalcular el inicio para mantener anclado a la derecha
  int totalWidth = 0;
  for (int i = 0; i < sections; i++) {
    totalWidth += sectionWidths[i];
  }
  int rightBorder = 320;  // <<< Ajusta este valor si quieres otro borde
  sectionStartX = rightBorder - totalWidth;

  tft.fillScreen(TFT_BLACK);

  // Botones
  tft.fillRect(0, 0, 60, 40, TFT_GREEN); 
  tft.setTextColor(TFT_BLACK);
  tft.drawString("U", 25, 10, 2);

  tft.fillRect(0, 40, 60, 40, TFT_RED);  
  tft.setTextColor(TFT_WHITE);
  tft.drawString("D", 25, 50, 2);

  tft.fillRect(0, 80, 60, 40, TFT_WHITE);
  for (int i = 0; i < 60; i += 6) {
    tft.drawFastHLine(0, 80 + i, 60, TFT_CYAN);
  }
  tft.setTextColor(TFT_BLACK);
  tft.drawString(flattenMode ? "FLAT" : "NORM", 10, 90, 2);

  tft.fillRect(0, 120, 60, 40, TFT_PURPLE);  
  tft.setTextColor(TFT_WHITE);
  tft.drawString("WIDTH", 5, 130, 2);  // <<< Botón para cambiar ancho

  tft.setTextColor(TFT_WHITE);

  tft.fillRect(60, 0, 60, 40, TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("L", 85, 10, 2);

  tft.fillRect(120, 0, 60, 40, TFT_YELLOW);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("R", 145, 10, 2);

  // Dibujar líneas
  int x = sectionStartX;
  for (int i = 0; i < sections - 1; i++) {
    int w = sectionWidths[i];
    int nextW = sectionWidths[i + 1];

    if (modified[i]) {
      int midX1 = x + (w / 2) - (w * 0.6) / 2;
      int midX2 = midX1 + w * 0.6;
      tft.drawLine(x, topLineY[i], midX1, topLineY[i], TFT_ORANGE);
      tft.drawLine(midX1, topLineY[i], midX2, topLineY[i], TFT_ORANGE);
      tft.drawLine(midX2, topLineY[i], x + w, topLineY[i + 1], TFT_ORANGE);
    } else {
      tft.drawLine(x, topLineY[i], x + w, topLineY[i + 1], TFT_ORANGE);
    }
    tft.drawLine(x, bottomLineY[i], x + w, bottomLineY[i + 1], TFT_ORANGE);

    x += w;
  }

  // Indicador de selección
  int selX = sectionStartX;
  for (int i = 0; i < selected; i++) {
    selX += sectionWidths[i];
  }
  int selW = sectionWidths[selected];
  int sy = topLineY[selected] - 10;
  tft.drawRect(selX - 2, sy, selW + 4, 20, TFT_WHITE);
}

void applyFlattenState() {
  modified[selected] = flattenMode;
}

void toggleWidth() {
  if (sectionWidths[selected] == 13) {
    sectionWidths[selected] = 5;
  } else {
    sectionWidths[selected] = 13;
  }
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
    sectionWidths[i] = 13; // <<< Ancho normal por default
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
      topLineY[selected] -= 2;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 500 - 130 && tx <= 500 + 130 && ty >= 1290 - 130 && ty <= 1290 + 130) {
      topLineY[selected] += 2;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 1119 - 130 && tx <= 1119 + 130 && ty >= 608 - 130 && ty <= 608 + 130) {
      selected--;
      if (selected < 0) selected = 0;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 1823 - 130 && tx <= 1823 + 130 && ty >= 592 - 130 && ty <= 592 + 130) {
      selected++;
      if (selected >= sections) selected = sections - 1;
      applyFlattenState();
      drawLines();
      delay(150);
    } else if (tx >= 533 - 130 && tx <= 533 + 130 && ty >= 1987 - 130 && ty <= 1987 + 130) {
      flattenMode = !flattenMode;
      applyFlattenState();
      drawLines();
      delay(300);
    } else if (tx >= 573 - 130 && tx <= 573 + 130 && ty >= 2462 - 130 && ty <= 2462 + 130) {
      toggleWidth();  // <<< Solo cambiar ancho de sección actual
      drawLines();
      delay(300);
    }
  }
}

