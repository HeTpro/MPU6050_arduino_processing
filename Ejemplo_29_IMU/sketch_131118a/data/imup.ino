
/**
 * 
*/

import processing.serial.*;

Serial myPort;  //Crea objeto a partir de la clase Serial

boolean firstSample = true;

float [] RwAcc = new float[3];         // Proyeccion de vector de fuerza gravitacional en los ejes x/y/z axis, medidos por el acelerometro
float [] Gyro = new float[3];          //Lectura de Giroscopio
float [] RwGyro = new float[3];        //Rw obtenido del ultimo movimiento y valor estimadodel giroscopio
float [] Awz = new float[2];           //angulos entre proyeccion de R en el plano de XZ/YZ y el eje Z (grados)
float [] RwEst = new float[3];


int lastTime = 0;
int interval = 0;
float wGyro = 10.0;

int lf = 10; // el valor decimal 10 es '\n' en ASCII
byte[] inBuffer = new byte[22]; //Este en el numero de caracteres en cada linea que se recibe del arduino (incluye /r/n)

PFont font;
final int VIEW_SIZE_X = 600, VIEW_SIZE_Y = 600;


void setup() 
{
  size(VIEW_SIZE_X, VIEW_SIZE_Y, P3D);
  myPort = new Serial(this, "COM53", 115200);  
  
  // Carga tipo de fuente, debe esta localizado en directorio "data" del sketch
  font = loadFont("CourierNew36.vlw"); 

  
  delay(100);
  myPort.clear();
  myPort.write("start");
}


float decodeFloat(String inString) {
  byte [] inData = new byte[4];
  
  inString = inString.substring(2, 10); // Descarta "f:" de la cadena  inData[0] = (byte) unhex(inString.substring(0, 2));
  inData[1] = (byte) unhex(inString.substring(2, 4));
  inData[2] = (byte) unhex(inString.substring(4, 6));
  inData[3] = (byte) unhex(inString.substring(6, 8));
      
  int intbits = (inData[3] << 24) | ((inData[2] & 0xff) << 16) | ((inData[1] & 0xff) << 8) | (inData[0] & 0xff);
  return Float.intBitsToFloat(intbits);
}


void readSensors() {
  if(myPort.available() >= 18) {
    String inputString = myPort.readStringUntil((int) '\n');
    if (inputString != null && inputString.length() > 0) {
      String [] inputStringArr = split(inputString, ",");
      
      RwAcc[0] = decodeFloat(inputStringArr[0]);
      RwAcc[1] = decodeFloat(inputStringArr[1]);
      RwAcc[2] = decodeFloat(inputStringArr[2]);
      
      Gyro[0] = decodeFloat(inputStringArr[3]);
      Gyro[1] = decodeFloat(inputStringArr[4]);
      Gyro[2] = decodeFloat(inputStringArr[5]);
      
      RwGyro[0] = decodeFloat(inputStringArr[6]);
      RwGyro[1] = decodeFloat(inputStringArr[7]);
      RwGyro[2] = decodeFloat(inputStringArr[8]);
      
      Awz[0] = decodeFloat(inputStringArr[9]);
      Awz[1] = decodeFloat(inputStringArr[10]);
      
      RwEst[0] = decodeFloat(inputStringArr[11]);
      RwEst[1] = decodeFloat(inputStringArr[12]);
      RwEst[2] = decodeFloat(inputStringArr[13]);
      
    }
  }
}



void buildBoxShape() {
  //box(60, 10, 40);
  noStroke();
  beginShape(QUADS);
  
  //Z+ (to the drawing area)
  fill(#00ff00);
  vertex(-30, -5, 20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);
  
  //Z-
  fill(#0000ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, 5, -20);
  vertex(-30, 5, -20);
  
  //X-
  fill(#ff0000);
  vertex(-30, -5, -20);
  vertex(-30, -5, 20);
  vertex(-30, 5, 20);
  vertex(-30, 5, -20);
  
  //X+
  fill(#ffff00);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(30, 5, -20);
  
  //Y-
  fill(#ff00ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(-30, -5, 20);
  
  //Y+
  fill(#00ffff);
  vertex(-30, 5, -20);
  vertex(30, 5, -20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);
  
  endShape();
}


void drawCube() {  
  pushMatrix();
    translate(300, 450, 0);
    scale(4,4,4);
    
    rotateX(HALF_PI * -RwEst[0]);
    rotateZ(HALF_PI * RwEst[1]);
    
    buildBoxShape();
    
  popMatrix();
}


void draw() {  
  readSensors();
  
  background(#000000);
  fill(#ffffff);
  
  textFont(font, 20);
  text("RwAcc (G):\n" + RwAcc[0] + "\n" + RwAcc[1] + "\n" + RwAcc[2] + "\ninterval: " + interval, 20, 50);
  text("Gyro (°/s):\n" + Gyro[0] + "\n" + Gyro[1] + "\n" + Gyro[2], 220, 50);
  text("Awz (°):\n" + Awz[0] + "\n" + Awz[1], 420, 50);
  text("RwGyro (°/s):\n" + RwGyro[0] + "\n" + RwGyro[1] + "\n" + RwGyro[2], 20, 180);
  text("RwEst :\n" + RwEst[0] + "\n" + RwEst[1] + "\n" + RwEst[2], 220, 180);
  
  // display axes
  pushMatrix();
    translate(450, 250, 0);
    stroke(#ffffff);
    scale(100, 100, 100);
    line(0,0,0,1,0,0);
    line(0,0,0,0,-1,0);
    line(0,0,0,0,0,1);
    line(0,0,0, -RwEst[0], RwEst[1], RwEst[2]);
  popMatrix();
  
  drawCube();
}


