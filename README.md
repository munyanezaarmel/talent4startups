# Armel Munyaneza in collaboration with [Bikorimana saveur](https://github.com/saveur1)


## About Me
Hello, my name is Armel Munyaneza. I am currently a third-year student at the University of Rwanda, majoring in Computer and Software Engineering. Alongside my studies, I am an avid full-stack developer with a passion for integrating various technologies. In my leisure time, I enjoy playing football and maintaining an active lifestyle through regular gym sessions. Additionally, I find solace in reading self-help books to continuously enhance my personal development. Attending embedded systems training aligns with my aspirations to expand my knowledge and skill set, allowing me to seamlessly incorporate embedded systems into my projects and pursue a fulfilling career in this field.

## Project: A Smart Car Accident Alert System for Real-Time Help 

### Problem:
Road accidents are a leading cause of injury and death globally. Even with advancements in car safety features, a critical factor in improving outcomes is the time it takes for emergency responders to reach the scene. Delays can lead to serious complications and loss of life.

### Challenge:
Currently, there's no universally adopted system for automatic accident notification with real-time location data. This often relies on bystanders or injured occupants to call for help, which can be time-consuming or impossible in critical situations.

### Proposed Solution:
This project proposes a car accident alert system that can automatically detect a collision and transmit an SMS notification with the car's location to pre-designated emergency contacts.

### Benefits:
- **Faster Emergency Response:** Automatic notification can significantly reduce the time it takes for help to arrive, potentially saving lives and reducing injuries.
- **Improved Coordination:** Emergency responders can receive vital information about the accident location before reaching the scene, allowing for better response planning.
- **Peace of Mind:** The system can provide a sense of security for drivers and passengers, knowing that help will be on the way in case of an accident.

### Components Required:
#### Hardware:
- Arduino Uno(nano)
- Accelerometer 
- Wifi module(ESP8266)
- LCD screen 
- Breadboard and jumper wires
- DC power supply
- Ultrasonic motion sensor
-  GPS module

#### Software:
- Tinkercad simulation platform
- Kicad or EasyEda for PCB design
- Twilio account (for SMS notification)
- Google Maps API

### Project Functionality:
- The accelerometer continuously measures acceleration (g-forces) along the X, Y, and Z axes.
- Pre-defined thresholds are set for each axis to represent a potential collision based on estimated g-forces during an impact.
- A threshold distance (e.g., 30 cm) is set for the ultrasonic sensor to indicate an imminent collision.
- If either the accelerometer readings exceed their thresholds (sudden acceleration) or the ultrasonic sensor detects a distance less than the threshold (close proximity), a potential collision is detected.
- Upon collision detection:
  - (An LCD displays a message like "Collision Detected!" and the measured distance.
  - A buzzer sounds as an alarm.
  - The ESP8266 module connects to a Wi-Fi network.
  - An SMS notification is sent via Twilio, including:
    - A message like "Car Accident! Help Needed!"
    - A Google Maps link to the accident location (using latitude and longitude data retrieved from a GPS module or smartphone; not simulated in Tinkercad).
- The system is configured to transmit data continuously to a cloud platform like Thingspeak for data logging and visualization.
