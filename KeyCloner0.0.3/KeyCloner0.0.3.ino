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

int centerX = 120;
int centerY = 160;
int size = 40;

void drawTriangle() {
  tft.fillScreen(TFT_BLACK);

  // Dibujo de botones (sólo visual, el toque se calibra por coordenadas reales del touch)
  tft.fillRect(0, 0, 60, 40, TFT_GREEN);       // Arriba
  tft.drawString("↑", 25, 10, 2);
  tft.fillRect(0, 40, 60, 40, TFT_RED);        // Abajo
  tft.drawString("↓", 25, 50, 2);
  tft.fillRect(60, 0, 60, 40, TFT_BLUE);       // Izquierda
  tft.drawString("←", 85, 10, 2);
  tft.fillRect(120, 0, 60, 40, TFT_YELLOW);    // Derecha
  tft.drawString("→", 145, 10, 2);

  // Dibujo del triángulo
  int x1 = centerX;
  int y1 = centerY - size;
  int x2 = centerX - size;
  int y2 = centerY + size;
  int x3 = centerX + size;
  int y3 = centerY + size;

  tft.drawLine(x1, y1, x2, y2, TFT_ORANGE);
  tft.drawLine(x2, y2, x3, y3, TFT_ORANGE);
  tft.drawLine(x3, y3, x1, y1, TFT_ORANGE);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  touchSPI.begin(T_CLK, T_MISO, T_MOSI, T_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);

  drawTriangle();
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    int tx = p.x;
    int ty = p.y;

    Serial.printf("Touch -> X: %d, Y: %d\n", tx, ty);

    // Botón arriba (centro: 455, 572)
    if (tx >= 455 - 200 && tx <= 455 + 200 && ty >= 572 - 150 && ty <= 572 + 150) {
      centerY -= 10; drawTriangle(); delay(150);
    }

    // Botón abajo (centro: 500, 1290)
    else if (tx >= 500 - 200 && tx <= 500 + 200 && ty >= 1290 - 150 && ty <= 1290 + 150) {
      centerY += 10; drawTriangle(); delay(150);
    }

    // Botón izquierda (centro: 1119, 608)
    else if (tx >= 1119 - 200 && tx <= 1119 + 200 && ty >= 608 - 150 && ty <= 608 + 150) {
      centerX -= 10; drawTriangle(); delay(150);
    }

    // Botón derecha (centro: 1823, 592)
    else if (tx >= 1823 - 200 && tx <= 1823 + 200 && ty >= 592 - 150 && ty <= 592 + 150) {
      centerX += 10; drawTriangle(); delay(150);
    }
  }
}
