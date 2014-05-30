
#define pulse_cnt_pin  5
#define relayPin  8

int sum = 0;

unsigned char status_relay=0;
unsigned char status_relay_confirmation=0;
char buffout[80];
char buffin[80];

unsigned char battery_level = 0xFF;
unsigned char pressure_units = 0x42;
unsigned char pressure_decimals = 0x42;

//--------Contador pulsos-----------
unsigned long tiempoanterior;
int tiempo=60000; //Tiempo que durará contando los pulsos (milisegundos)
unsigned int discharge;


void setup() {

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
   pinMode(pulse_cnt_pin,INPUT);
  // digitalWrite(pulse_cnt_pin, HIGH);  //Habilitar resistencia de pull up 
   
   Serial.begin(9600);
   
   TCCR1A = 0x00;
   TCCR1B = 0b11000111;
   TCNT1 = 0x00;  
   PRR &= 0b11110111;
  //Counter on pin5 arduino UNO

  
}


void loop() {
  

  
  unsigned long tiempoactual=millis();
  
  if(tiempoactual - tiempoanterior > tiempo) {
       
        discharge_calc(TCNT1);
    
    }
}

char change_status_relay(unsigned char sta){
 
    switch (sta) {
    case 0:
      digitalWrite(relayPin,LOW);
      status_relay=0;
      break;
    case 1:
      digitalWrite(relayPin ,HIGH);
      status_relay=1;
      break;
    // default:
        }

return status_relay;
}


void serialEvent() {

   int array_bien=0;
 
      while(!Serial.available());
      
      Serial.readBytes(buffin,20);
      
      while(!array_bien){
        
        if(!(buffin[0] == 0x7E)){
          for(int k=0; k<13; k++){
          buffin[k] = buffin[k+1];
          }}
          else{
           array_bien = 1;
          }
      
      }

  
     if(buffin[0] == 0x7E){ // && buffin[15] == 0x41 && buffin[16] == 0x31){
               
            
                 switch (buffin[18]) {
                  case 0:
                    status_relay_confirmation = change_status_relay(0);
                    break;
                  case 1:
                    status_relay_confirmation = change_status_relay(1);
                    break;
              }
               
       
       
       
                buffout[0]=0x7E;//API mode start delimeter
		buffout[1]=0x00;//Data length MSB
		buffout[2]=0x16;//Data length LSB
		
                buffout[3]=0x10;//API ID TX Request 16 bit address
		buffout[4]=0x00;//Frame ID, if equals to 0x00 disable response frame
		buffout[5]=0x00;
               
                buffout[6]=0x13;
                buffout[7]=0xA2;
                buffout[8]=0x00;
                buffout[9]=0x40;
                buffout[10]=buffin[9];
                buffout[11]=buffin[10];
                buffout[12]=buffin[11];//Destination Address LSB
		buffout[13]=0xFF;
		buffout[14]=0xFE;
		buffout[15]=0x00;
		buffout[16]=0x00;
                buffout[17]=0x15; //Node ID 21 -29, 0x15-0x1D 
		buffout[18]=status_relay_confirmation;//Data 1
                buffout[19]=highByte(TCNT1);//Water Flow 0-50 gallons Byte 1
                buffout[20]=lowByte(TCNT1);//Water Flow 0-50 gallons Byte 2
                buffout[21]=pressure_units;//Pressure Byte 1
                buffout[22]=pressure_decimals;//Pressure Byte 2
                buffout[23]=battery_level;//Battery level 0-255
                buffout[24]=0x0A;//End of package
		
                buffout[25]=0x00;//Checksum

            for(int i=3;i<25;i++){

                  sum+=buffout[i]; //Checksum
                  

           }
          buffout[25]=0xFF-lowByte(sum);
         
          sum=0;
        
            Serial.write(buffout[0]);
            Serial.write(buffout[1]);
            Serial.write(buffout[2]);
            Serial.write(buffout[3]);
            Serial.write(buffout[4]);
            Serial.write(buffout[5]);
            Serial.write(buffout[6]);
            Serial.write(buffout[7]);
            Serial.write(buffout[8]);
            Serial.write(buffout[9]);
            Serial.write(buffout[10]);
            Serial.write(buffout[11]);
            Serial.write(buffout[12]);
            Serial.write(buffout[13]);
            Serial.write(buffout[14]);
            Serial.write(buffout[15]);
            Serial.write(buffout[16]);
            Serial.write(buffout[17]);
            Serial.write(buffout[18]);
            Serial.write(buffout[19]);
            Serial.write(buffout[20]);
            Serial.write(buffout[21]);
            Serial.write(buffout[22]);
            Serial.write(buffout[23]);
            Serial.write(buffout[24]);
            Serial.write(buffout[25]);  
         
   } 	
            
      while((Serial.available()>0));
     
}
void discharge_calc(unsigned int pulses){
  
  #define offset 0.39
  #define K 0.44

  discharge = ((pulses/60) + offset)*K;  // in gallons
 
}


