  #include <EEPROM.h>
  int direccionEEPROM = 0; //Misma sloot que se utiliza en el juego SNAKE (Default 0)
void setup() {
  //Valor de puntaje máximo que se quiera cargar en la EEPROM 
  EEPROM.put(direccionEEPROM, 30); //30 será el nuevo puntaje máximo
}

void loop() {
  

}
