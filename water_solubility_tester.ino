

#include <LiquidCrystal.h>
#define samplePin A1    // Choose a pin that the sensor output is attached to

 
// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Define & initialize variables and functions
String contam[] = {"As", "B2", "Fe", "C6", "Mn", "Hg", "Pb", "Ur" ,"Tm","Ph"}; //List of possible contaminants
int avgReadings = 0;         //User input with buttons
int ppmMin = 0;              //User input with buttons
int ppmMax = 0;            //User input with buttons:  multiply actual value by 1000 for resolution in map
int minSample = 0;
int maxSample = 0;
int testSample = 0;        //Note that some variables have been defined here
int intppm = 0;
float ppm = 0;                //Use these in your code as required 
int tm = 0;
int state = 0;
int sampleCount = 0;
int avgMinSample = 0;
int avgMaxSample = 0;
int avgTestSample = 0;
int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;
int done = 0;

//fncs
int sampleAvg (); //for calculating avg voltreadings
int select(); 
void buttonCount();        
void welcomeScreen();
void ctmCount();

//Setup the board
void setup() 
{
   lcd.begin(16, 2);        //Set the characters and column numbers.
   Serial.begin(9600);    //For serial monitor use in trouble shooting
}

int val1 = 5;
//Run main program
void loop() 
{
  switch (state) 
  {
      case 0:
        welcomeScreen();         //Call welcomeScreen function
        while(!select());        //If no button press, don't do anything
        delay(100);              //For stability
        state = 1;                //Switch to state 1           
        break;
      
     case 1:                         //Contaminant selection
        while(!select());
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Contaminant: ");
        lcd.print(contam[0]);
        lcd.setCursor(0, 1);
        lcd.print("SLCT=CNT, RT=END");
        ctmCount();
        
        state = 2;
        delay(100);
        break;
      
      case 2:                         //Enter # of samples to average
        done = 0;
        buttonPushCounter = 0;
        lastButtonState = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("# to AVG = ");
        lcd.print(buttonPushCounter);
        lcd.setCursor(0, 1);
        lcd.print("SLCT=CNT, RT=END");
        buttonCount();

        avgReadings = buttonPushCounter;
      	//Serial.println (avgReadings);
        state = 3;
        delay(100);
        break;
        
      
      case 3:                         //Enter min concentration
        done = 0;
        buttonPushCounter = 0;
        lastButtonState = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Min PPM =  ");
        lcd.print(buttonPushCounter);
        lcd.setCursor(0, 1);
        lcd.print("SLCT=CNT, RT=END");
        buttonCount();

        ppmMin = buttonPushCounter;
        state = 4;
        delay(100);
        break;
      
      case 4:                         //Enter max concentration
        done = 0;
        buttonPushCounter = 0;
        lastButtonState = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Max PPM =  ");
        lcd.print(buttonPushCounter);
        lcd.setCursor(0, 1);
        lcd.print("SLCT=CNT, RT=END");
        buttonCount();

        ppmMax = buttonPushCounter;
        state = 5;
        delay(100);
        break;   //Don't change this message for proper display
       
      case 5:                         //Determine voltage of min concentration sample
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Insert minSample");
        lcd.setCursor(0, 1);
        lcd.print("SLCT = ANLYZ+END");
        
        while(!select());
    
        //loop has purpose of taking a larger number of samples, making data more accurate (Law of Large Numbers)
        for (int i = 0; i < avgReadings; i++)
        {
          //reads in samples and adds them up
          sampleCount = analogRead (samplePin);
          minSample += sampleCount;
        }
      
        //takes avg of voltage samples
        avgMinSample = minSample / avgReadings;
        //Serial.println (avgMinSample);
      minSample = 0; //reset
    
        state = 6;
        delay(500);
    break;
    
       case 6:                       //Determine voltage of max concentration sample
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Insert maxSample");
        lcd.setCursor(0, 1);
        lcd.print("SLCT = ANLYZ+END");
    
        while(!select());
      
        //loop has purpose of taking a larger number of samples, making data more accurate (Law of Large Numbers)
        for (int i = 0; i < avgReadings; i++)
        {
          //reads in samples and adds them up
          sampleCount = analogRead (samplePin);
          maxSample += sampleCount;
        }
      
        //takes avg of voltage samples
        avgMaxSample = maxSample / avgReadings;
        //Serial.println (avgMaxSample);
        maxSample = 0; //reset
    
        state = 7;
        delay(500);
        break;
    
       case 7:                      //Measure unknown sample and convert to ppm     
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Insrt testSample");
        lcd.setCursor(0, 1);
        lcd.print("SLCT = ANLYZ+CNT");
    
        while(!select());
    
        //loop has purpose of taking a larger number of samples, making data more accurate (Law of Large Numbers)
        for (int i = 0; i < avgReadings; i++)
        {
          //reads in samples and adds them up
          sampleCount = analogRead (samplePin);
          testSample += sampleCount;
        }
      
        //takes avg of voltage samples
        avgTestSample = testSample / avgReadings;
        //Serial.println (avgTestSample);
        testSample = 0; //reset
    
        intppm = map(avgTestSample, avgMaxSample, avgMinSample, (ppmMax * 1000), (ppmMin * 1000));
        ppm = intppm;
        ppm = ppm / 1000;
           
        if (ppm <= 0)                //Samples should never read below 0
          ppm = 0;
    
        state = 8;
        delay(500);
        break;
    
    case 8:             //displays final readings
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("ppm =  ");         //water quality.  Don't change this message for proper display
        lcd.print(ppm);                //Output to float type and divide by 1000
        lcd.setCursor (0,1);
        lcd.print ("SLCT=CNT Off=RST");  //Don't change this message for proper display
        
        while(!select());

        state = 9;                    //Restart process if SELECT is pressed
        //state = 0;                    //allows user to skip re-calibration instead of restart

            
        delay(500);
        break;
    
     case 9:             //displays temperature
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Temp =  ");         //water quality.  Don't change this message for proper display
        
          val1= analogRead(A2);
        val1 = (ppm*val1)/50;
    	tm = val1;
        lcd.print(val1);                //Output to float type and divide by 1000
        lcd.setCursor (0,1);
        lcd.print ("SLCT=CNT Off=RST");  //Don't change this message for proper display
        
        while(!select());

        state = 10;                    //Restart process if SELECT is pressed
        //state = 0;                    //allows user to skip re-calibration instead of restart

            
        delay(500);
        break;
    
     case 10:             //displays final readings
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Ph =  ");         //water quality.  Don't change this message for proper display
    	int val2 = analogRead(A3);
        val2 = (tm*val2)*7;
        if (val2 > 3000)
          val2 = 3000;
        int val3 = map(val2, 0, 3000, 0, 14);
        Serial.print(val3);
        lcd.print(val3);                //Output to float type and divide by 1000
        lcd.setCursor (0,1);
        lcd.print ("SLCT=CNT Off=RST");  //Don't change this message for proper display
        
        while(!select());

        state = 0;                    //Restart process if SELECT is pressed
        //state = 0;                    //allows user to skip re-calibration instead of restart

            
        delay(500);
        break;
  
  }
  
  
  
}

//Special Functions and Variables
int select()
{
  int buttonRead = analogRead(0);
  delay(250);                          //debounce
  if ((buttonRead > 800) && (buttonRead < 850))
    return 1;
  else
    return 0;  
}


//Print a start-up message update as desired
void welcomeScreen() 
  {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  H2O Quality");      
      lcd.setCursor(0,1); 
      lcd.print ("  Press SELECT  ");
  }
 

//Get input for map function
void buttonCount()
{
  //initialization here....
  while(!done)
  {
    buttonState = analogRead(0);           // read the pushbutton input pin
    //Serial.println(buttonState);
    delay(250);
    if ((buttonState > 800) && (buttonState < 850) && ((lastButtonState <800) || (lastButtonState > 850)))
    {   
       buttonPushCounter += 1;           //If SELECT has been pressed, update counter
       lastButtonState = buttonState;          // save the current state as the last state, for the next loop
       lcd.setCursor(11,0);
       lcd.print(buttonPushCounter);     
     } 
    else if((buttonState < 50) && (buttonState > 30))  //If RIGHT has been pressed, end count
    {
      done = 1;  
      lastButtonState = buttonState; 
    }
   else 
   {
    lastButtonState = buttonState;
   }
  delay(250);                               // debounce time
  }
}

void ctmCount()
{
  //initialization here....
  while(!done)
  {
    buttonState = analogRead(0);           // read the pushbutton input pin
    //Serial.println(buttonState);
    delay(250);
    if ((buttonState > 800) && (buttonState < 850) && ((lastButtonState <800) || (lastButtonState > 850)))
    {   
       buttonPushCounter += 1;
       if (buttonPushCounter == 8)
           buttonPushCounter = 0;  
       lastButtonState = buttonState;          // save the current state as the last state, for the next loop
       lcd.setCursor(14,0);
       lcd.print(contam[buttonPushCounter]);     
     } 
    else if((buttonState < 50) && (buttonState > 30))
    {
      done = 1;  
      lastButtonState = buttonState; 
    }
   else 
   {
    lastButtonState = buttonState;
   }
  delay(250);                               // debounce time
  }
}