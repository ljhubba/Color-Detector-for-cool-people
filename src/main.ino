#include <SD.h>  // SD library to manage SD card read/write operations.
#include <math.h>  // Math library to perform mathematical operations.
#include <LiquidCrystal.h>  // LCD library for text display

#define SAMPLES_PER_GROUP 8  // Number of samples taken per color group for averaging.
#define NUM_COLOR_VALUES 11 //Red, blue, green, and yellow values  // Number of distinct colors being tracked (Red, Blue, Green, Yellow).
// Array of color names for LCD display
const char* colorNames[NUM_COLOR_VALUES] = {
    "Red", "Orange", "Yellow", "Green", "Blue", "Purple",
    "Pink", "Brown", "Gray", "Black", "White"
};


#define BUTTON_PIN 32  // Pin for button input to trigger actions in the program.
#define YELLOW_LED 13  // Pin for Yellow LED output to indicate yellow color detection.
#define GREEN_LED 27  // Pin for Green LED output to indicate green color detection.
#define RED_LED 33 
#define BLUE_LED 12 
#define PHOTOTRANSISTOR_PIN 25
#define TOGGLE_SWITCH_PIN 14 
#define POTENTIOMETER_PIN 26 
#define PIN_SPI_CS 5//4

LiquidCrystal lcd(17, 4, 16, 35, 36, 39); //change to whatever pins we use
//RS E D4 D5 D6 D7
struct sample{
	//A sample of the values read by the phototransistor from the colored LEDs
	unsigned int groupValue : 4; //We only need 4 bits to categorize 11 groups
	uint8_t colorValues[NUM_COLOR_VALUES]; //RGBY values for the sample (1 byte per entry for a total of 4 bytes)
	unsigned int distance : 11; //The biggest distance can be 2046 units (comparing all zeros to all 1023s) so we allocate 2048 bits.
	
	//Overloading the equal operator to set all values of one sample equal to another sample
	sample& operator =(const sample& s)
	{
		for(int a = 0; a < NUM_COLOR_VALUES; a++)
		{
			colorValues[a] = s.colorValues[a];
		}
		distance = s.distance;
		groupValue = s.groupValue;
		return *this;
	}
};

struct Group
{
	unsigned int id : 4; //There are only 11 groups, so we only need 4 bits
	unsigned int frequency : 4; //We're limiting it to 15 samples per group, and 4 bits give us 15
  //We can do up to 15 samples per group, 10 may be easier, anything less than 15
};

struct{
	//Used to keep track of global memory in a tight compartment
	unsigned char trainingModeActive : 1; //Basically a boolean
	unsigned char currentlyReading : 1; //Debounce for button press
	unsigned char loadingRequired : 1; //When we flip the toggle switch to sampling, we need to load the data
	unsigned int selectedGroupId : 4; //What's the potentiometer pointing to?
	sample trainingData[11*SAMPLES_PER_GROUP];
}colorDetector;

void insertionSort(sample arr[], int n)
{
	//Copied and modified from GeeksForGeeks.com because I won't want libraries bloating things
	int i, key, j;
	for(i = 1; i < n; i++)
	{
		key = arr[i].distance;
		j = i - 1;
		
		while(j >= 0 && arr[j].distance > key)
		{
			sample temp = arr[j+1];
			arr[j+1] = arr[j];
			arr[j] = temp;
			j = j - 1;
		}
		arr[j+1].distance = key;
	}
}

unsigned int colorValueDistance(const sample s1, sample s2)
{
	//Takes two colorValue arrays and finds the distance between each color value
	//We're using the formula d=sqrt(sum((pixel_1 - pixel_2)^2))
	sample differences; //Node's RGBY data will be the differences squared of each RGBY values from s1 and s2
	float diff;
	for(int a = 0; a < NUM_COLOR_VALUES; a++)
	{
		diff = s1.colorValues[a] - s2.colorValues[a];
		differences.colorValues[a] = diff * diff;
	}
	
	//Now we need to sum up the squared differences we just calculated
	float sum = 0;
	for(int a = 0; a < NUM_COLOR_VALUES; a++)
	{
		sum += differences.colorValues[a];
	}
	
	//we are returning integer versions of the sqrt(sum) (up to 2^9 bits) because that's the max distance a sample can have
	sum = sqrt(sum);
	if(sum > 511)
		return 511;
	else if(sum < 1)
		return 0;
	else
		return (int)sum;
}

/*
The classify() function utilizes the KNN algorithm
k is the number of nearby neighbors
*/
int classify(sample arr[], int numTrainingSamples, int k, const sample node)
{
	Group g0 {0};
	Group g1 {1};
	Group g2 {2};
	Group g3 {3};
	Group g4 {4};
	Group g5 {5};
 	Group g6 {6};
	Group g7 {7};
	Group g8 {8};
	Group g9 {9};
	Group g10 {10};
	Group allGroups[11] = {g0,g1,g2,g3,g4,g5,g6,g7,g8,g9,g10};
	
	//For each neighbor of the sample (node) we use the distance formula and set it's "distance" member variable
	for(int a = 0; a < numTrainingSamples; a++)
	{
		arr[a].distance = colorValueDistance(node, arr[a]);
	}
	//Then we sort the array of training data based on the what's closest to the sample node
	insertionSort(arr, numTrainingSamples-1);
	//Now we count how many times each group is in the nearest k number of neighbors
	for(int a = 0; a < k; a++)
	{
		switch(arr[a].groupValue)
		{
			case 0:
				allGroups[0].frequency++;
				break;
			case 1:
				allGroups[1].frequency++;
				break;
			case 2:
				allGroups[2].frequency++;
				break;
			case 3:
				allGroups[3].frequency++;
				break;
			case 4:
				allGroups[4].frequency++;
				break;
			case 5:
				allGroups[5].frequency++;
				break;
			case 6:
				allGroups[6].frequency++;
				break;
    			case 7:
				allGroups[7].frequency++;
				break;
      			case 8:
				allGroups[8].frequency++;
				break;
      			case 9:
				allGroups[9].frequency++;
				break;
      			case 10:
				allGroups[10].frequency++;
				break;
		}
	}
	
	//Finally, we look at the most common group in the neighboring samples
	int highestFrequency = 0;
	int groupNumber;
	for(int a = 0; a < 11; a++) //11 because there are 11 groups
	{
		if(allGroups[a].frequency >= highestFrequency)
		{
			highestFrequency = allGroups[a].frequency;
			groupNumber = allGroups[a].id;
		}
	}
	
	return groupNumber;
}

void getDataFromCard()
{
	//Read each file on the SD card line by line, storing the data in each sample in the "trainingData" array (arr)
	File myFile;
	String value; //Number we read from each file
	int row = 0;
	int nodeCount = 0;
	int counter = 0;
	for(int groupNumber = 0; groupNumber < 6; groupNumber++)
	{
    Serial.println(groupNumber);
		myFile = SD.open(String(groupNumber) + ".txt", FILE_READ);
		while(1)
		{
			value = myFile.readStringUntil('.');
			if(value == NULL || value == (String)'\n' || counter == SAMPLES_PER_GROUP)
			{
				row = 0;
				counter = 0;
				break;
			}
			//Store the value from the file into the sample
			colorDetector.trainingData[nodeCount].colorValues[row] = value.toInt();
			colorDetector.trainingData[nodeCount].groupValue = groupNumber;
      Serial.println(colorDetector.trainingData[nodeCount].colorValues[row]);
			row++;
			if(row == 4)
			{
				row = 0;
				nodeCount++;
				counter++;
			}
		}
		myFile.close();
	}
  Serial.println(colorDetector.trainingData[0].colorValues[0]);
}

void displayColor(int number)
{
	//Displays the given number on the 7-segment display
	//Displays a letter on the 7-segment display based on the number given
	//If these are backwards, swap the positive and negative wires on your potentiometer
  lcd.clear();
	switch(number)
	{
    case 0:
      lcd.print("Red");
      break;
    case 1:
      lcd.print("Orange");
      break;
    case 2:
      lcd.print("Yellow");
      break;
		case 3:
      lcd.print("Green");
      break;
    case 4:
      lcd.print("Blue");
      break;
    case 5:
      lcd.print("Purple");
      break;
    case 6:
      lcd.print("Pink");
      break;
    case 7:
      lcd.print("Brown");
      break;
    case 8:
      lcd.print("Gray");
      break;
    case 9:
      lcd.print("Black");
      break;
    case 10:
      lcd.print("White");
      break;
	}
}

int getNumDigits(int num)
{
	//Returns the number of digits in an integer
	return (int) log10((double) num) + 1;
}

void writeColor(int pin, File myFile)
{
	//Turns on an LED at the specified pin, reads the value on the transistor, and stores it in 4-char format on the SD card
	digitalWrite(pin, HIGH);
	delay(100);
	int reading = analogRead(PHOTOTRANSISTOR_PIN);
	int numDigits = getNumDigits(reading);
	switch(numDigits)
	{
		case 0:
			myFile.print("0000");
			break;
		case 1:
			myFile.print("000" + String(reading));
			break;
		case 2:
			myFile.print("00" + String(reading));
			break;
		case 3:
			myFile.print("0" + String(reading));
			break;
		default:
			myFile.print(String(reading));
	}
	myFile.print(".");
	digitalWrite(pin, LOW);
	delay(100);
}

int readColor(int pin)
{
	//Very similar to writeColor() but we don't involve the SD card
	digitalWrite(pin, HIGH);
	delay(100);
	int reading = analogRead(PHOTOTRANSISTOR_PIN);
	digitalWrite(pin, LOW);
	delay(100);
	return reading;
}

void updateSelectedGroupId()
{
	int val = analogRead(POTENTIOMETER_PIN);
	//Values go from 0 to 1023
	val = map(val, 0, 1023, 0, 10);
	colorDetector.selectedGroupId = val;
	displayColor(val); //Display the group ID on the LCD 
	delay(3000);
	lcd.clear();
}

void setup()
{
  Serial.begin(9600);
    lcd.begin(16, 2);

  if (!SD.begin(PIN_CS_SPI)) {
    Serial.println("SD Card Mount Failed");
    lcd.print("SD Card Error");
    delay(2000);
    lcd.clear();
    while(1);  // don't do anything else
  } 

  Serial.println("SD Card Initialized");
  lcd.print("Hello!");
  delay(1000);
  lcd.clear();
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(TOGGLE_SWITCH_PIN, INPUT_PULLUP);
	pinMode(POTENTIOMETER_PIN, INPUT);
	pinMode(PHOTOTRANSISTOR_PIN, INPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	
	
	getDataFromCard();
	colorDetector.loadingRequired = 0;

  if (digitalRead(TOGGLE_SWITCH_PIN)) {
        lcd.print("Training Mode");
  } else {
        lcd.print("Sampling Mode");
  }
  delay(2000);  // Show mode for 2 seconds
  lcd.clear();

}


void loop()
{
  // Check and update the mode (training or sampling) based on the toggle switch
  bool currentMode = digitalRead(TOGGLE_SWITCH_PIN);
  if (colorDetector.trainingModeActive != currentMode) {
    colorDetector.trainingModeActive = currentMode;

    // Display the mode on the LCD for 2 seconds
    lcd.clear();
    if (colorDetector.trainingModeActive) {
      lcd.print("Training Mode");
    } else {
      lcd.print("Sampling Mode");
    }
    delay(2000);  // Show mode for 2 seconds
    lcd.clear();
  }

  // Update the selected color group ID in training mode
  if (colorDetector.trainingModeActive) {
    updateSelectedGroupId();
  }

  // Check if button is pressed for sampling or training actions
  if (!digitalRead(BUTTON_PIN) && !colorDetector.currentlyReading) {
    colorDetector.currentlyReading = 1;

    if (colorDetector.trainingModeActive) {
      // Store data into the selected group's file on the SD card in training mode
      File myFile = SD.open(String(colorDetector.selectedGroupId) + ".txt", FILE_WRITE);
      colorDetector.loadingRequired = 1;  // Flag to reload data on next sample

      // Write color data to SD card in RGBY order
      writeColor(RED_LED, myFile);
      writeColor(GREEN_LED, myFile);
      writeColor(BLUE_LED, myFile);
      writeColor(YELLOW_LED, myFile);
      myFile.print("\n");
      myFile.close();

      // Confirmation message for data storage in training mode
      lcd.clear();
      lcd.print("Sample saved! :)");
      delay(1000);  // Show confirmation for 1 second
      lcd.clear();

      } else {
        // Sampling mode: Read color values and classify the sample
        lcd.clear();
        lcd.print("Sampling...");
        sample newSample;
        newSample.colorValues[0] = readColor(RED_LED);
        newSample.colorValues[1] = readColor(GREEN_LED);
        newSample.colorValues[2] = readColor(BLUE_LED);
        newSample.colorValues[3] = readColor(YELLOW_LED);

        // Load training data if required
        if (colorDetector.loadingRequired) {
          getDataFromCard();
          colorDetector.loadingRequired = 0;
        }

        // Classify the sample based on nearest neighbors
        int newSample_groupId = classify(colorDetector.trainingData, 11 * SAMPLES_PER_GROUP, 9, newSample);

        // Display the detected color name from the colorNames array
        lcd.clear();
        lcd.print("Detected: ");
        lcd.setCursor(0, 1);  // Move to the second line on the LCD
        lcd.print(colorNames[newSample_groupId]);
        delay(5000);  // Show detected color for 5 seconds
        lcd.clear();
      }

      colorDetector.currentlyReading = 0;
  }
}
