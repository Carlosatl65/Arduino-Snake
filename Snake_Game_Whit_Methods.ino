  #include <LiquidCrystal_I2C.h>
  #include "LedControl.h"
  #include <EEPROM.h>
  

  LedControl lc = LedControl(11, 13, 10, 1);
  LiquidCrystal_I2C lcd(0x3f,16,2);

  const int boton_inicio = 2;
  int joy_Pin1 = A0; 
  int joy_Pin2 = A1; 
  int eje_x;
  int eje_y;

  String direccion;
  String direccion_anterior;

  int serpiente_X[36]; // son 36 las posibles posiciones en pantalla para el eje x
  int serpiente_Y[36]; // son 36 las posibles posiciones en pantalla para el eje y

  int longitudSerpiente;

  int comida_X;
  int comida_Y;

  boolean fin = true;
  int puntaje = 0;
  int puntajeMax = 0;
  int velocidad = 200;
  
  // Dirección en la EEPROM donde se almacenará el puntaje máximo
  int direccionEEPROM = 0;


  // Banner "Inicio y Game Over"
  byte inicio_bitmap[8] = {
    B00000000,
    B00100100,
    B00100100,
    B00100100,
    B00000000,
    B01000010,
    B00111100,
    B00000000
  };

  byte num3[8] = {
    B0000000,
    B00111100,
    B00000100,
    B00011100,
    B00000100,
    B00111100,
    B00000000,
    B00000000
  };

  byte num2[8] = {
    B00000000,
    B00011100,
    B00100100,
    B00001000,
    B00010000,
    B00111100,
    B00000000,
    B00000000
  };

  byte num1[8] = {
    B00000000,
    B00011000,
    B00001000,
    B00001000,
    B00001000,
    B00011100,
    B00000000,
    B00000000
  };

  byte MAX[8] = {
    B00111100,
    B00111100,
    B00111100,
    B00011000,
    B01111110,
    B00011000,
    B00111100,
    B01100110
  };

  void setup() {
    Serial.begin(9600);
    
    lcd.init();
    lcd.backlight();

    lc.shutdown(0, false); //limpiar toda la matriz
    lc.setIntensity(0, 5); //brillo de la matriz
    lc.clearDisplay(0); // limpiar el matriz

    pinMode(boton_inicio, INPUT); // configuracion como entrada
    digitalWrite(boton_inicio, HIGH);// lo posicionamos en alto

    // Cargar el puntaje máximo almacenado en la EEPROM
    EEPROM.get(direccionEEPROM, puntajeMax);

  }

  void loop() {
    lc.clearDisplay(0);

    if(!fin){
      eje_x = analogRead(joy_Pin1);
      eje_y = analogRead(joy_Pin2);
    
      // en que dirección nos movemos?
      if (eje_y >= 0 && eje_y <480) {
        if (direccion != "abajo") direccion = "arriba";
      }
      if (eje_y > 520 && eje_y <=1023) {
        if (direccion != "arriba") direccion = "abajo";
      }
      if (eje_x >= 0 && eje_x <480) {
        if (direccion != "derecha") direccion = "izquierda";
      }
      if (eje_x > 520 && eje_x <=1023) {
        if (direccion != "izquierda") direccion = "derecha";
      }

      // Restringe el movimiento opuesto
      if (direccion == "arriba" && direccion_anterior == "abajo") {
        direccion = "abajo";
      } else if (direccion == "abajo" && direccion_anterior == "arriba") {
        direccion = "arriba";
      } else if (direccion == "izquierda" && direccion_anterior == "derecha") {
        direccion = "derecha";
      } else if (direccion == "derecha" && direccion_anterior == "izquierda") {
        direccion = "izquierda";
      }

      direccion_anterior = direccion;
    
      mover_serpiente(direccion);
      dibujar_serpiente();
      dibujar_comida();
      comprobar_choque(); // comprueba si choca consigo mismo
        
      if (Serial.available() > 0) {
        velocidad = Serial.parseInt(); // Lee la velocidad desde el puerto serial
      }
      
      delay(velocidad); // Usa la velocidad leída para el retardo
    }else{
      mostrar_inicio();
      if(digitalRead(boton_inicio)==0){
        cuenta_regresiva();
        fin = false;
      }
    }
    
  }
  
  void cuenta_regresiva (){
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, MAX[i]);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Puntaje Maximo ");
    lcd.setCursor(0,1);
    lcd.print("  Actual: ");
    lcd.print(puntajeMax);
    delay(2000);
    //3
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, num3[i]);
    }
    lcd.clear();
    lcd.print("** Preparad@ **");
    delay(1000);
    //2
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, num2[i]);
    }
    lcd.clear();
    lcd.print("**** List@ ****");
    delay(1000);
    //1
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, num1[i]);
    }
    lcd.clear();
    lcd.print("   Aprende y   ");
    lcd.setCursor(0,1);
    lcd.print("   Diviertete   ");
    delay(1000);
    lcd.clear();
    comenzar_juego();
    
  }

  void mostrar_inicio() {
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, inicio_bitmap[i]);
    }
    lcd.setCursor(0,0);
    lcd.print("* Presiona el *");
    lcd.setCursor(0,1);
    lcd.print("*    boton    *");
  }


  void comenzar_juego(){
    comida_X = random(0, 8);
    comida_Y = random(0, 8);
    eje_x = 0;
    eje_y = 0;
    longitudSerpiente = 1;
    direccion = "arriba";
    puntaje = 0;
    lcd.clear();
    lcd.print("Puntos:");
    lcd.print(puntaje);
  }

  void mover_serpiente(String dire){
      for (int i = longitudSerpiente - 1; i > 0; i--) {
        serpiente_X[i] = serpiente_X[i - 1];
        serpiente_Y[i] = serpiente_Y[i - 1];
      }
    
      if (dire == "arriba") {
        if (serpiente_Y[0] == 0) {
          serpiente_Y[0] = 7;
        } else {
          serpiente_Y[0]--;
        }
      } else if (dire == "abajo") {
        if (serpiente_Y[0] == 7) {
          serpiente_Y[0] = 0;
        } else {
          serpiente_Y[0]++;
        }
      } else if (dire == "izquierda") {
        if (serpiente_X[0] == 0) {
          serpiente_X[0] = 7;
        } else {
          serpiente_X[0]--;
        }
      } else if (dire == "derecha") {
        if (serpiente_X[0] == 7) {
          serpiente_X[0] = 0;
        } else {
          serpiente_X[0]++;
        }
      }  
  }

  void dibujar_serpiente(){
    for (int i = 0; i < longitudSerpiente; i++) {
      lc.setLed(0, serpiente_Y[i], serpiente_X[i], true); // (nº dispositivo, fila, columna, valor)
    }
  }

  void dibujar_comida(){
    int nuevaComidaX, nuevaComidaY;
    if (serpiente_X[0] == comida_X && serpiente_Y[0] == comida_Y){
      // si coincide la cabeza de la serpiente con la comida, es que se la ha comido y crece un LED
      longitudSerpiente++;
      puntaje += 10;
      lcd.clear();
      lcd.print("Puntos:");
      lcd.print(puntaje);
      do {
        nuevaComidaX = random(0, 8);
        nuevaComidaY = random(0, 8);
      } while (verificar_colision_con_serpiente(nuevaComidaX, nuevaComidaY));
      comida_X = nuevaComidaX;
      comida_Y = nuevaComidaY;
    }
    lc.setLed(0, comida_Y, comida_X, true);
    delay(50); // haremos que parpade
    lc.setLed(0, comida_Y, comida_X, false);  
  }

  boolean verificar_colision_con_serpiente(int x, int y) {
    for (int i = 0; i < longitudSerpiente; i++) {
      if (serpiente_X[i] == x && serpiente_Y[i] == y) {
        return true; // Hay una colisión con la serpiente
      }
    }
    return false; // No hay colisión con la serpiente
  }

  void comprobar_choque(){
    for (int i = 1; i < longitudSerpiente; i++) {
      if (serpiente_X[0] == serpiente_X[i] && serpiente_Y[0] == serpiente_Y[i]) {
        fin = true;
        // Si se obtiene un nuevo puntaje máximo, guardar en la EEPROM
        if (puntaje > puntajeMax) {
          puntajeMax = puntaje;
          EEPROM.put(direccionEEPROM, puntajeMax);
          mostrar_felicitaciones();
        }
      }
    }
  }

  void mostrar_felicitaciones() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("* Felicidades *");
    lcd.setCursor(0, 1);
    lcd.print("New Record: ");
    lcd.print(puntajeMax);
    delay(2000);
  }