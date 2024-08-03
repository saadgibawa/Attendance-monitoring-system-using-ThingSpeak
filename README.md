
# IoT-Based Attendance Monitoring System using ThingSpeak

## Overview

This project automates attendance tracking using a NodeMCU ESP8266 microcontroller, R307s fingerprint sensor, and OLED SH1106, integrated with ThingSpeak, an IoT application. It efficiently collects and analyzes attendance data, simplifying monitoring for lecturers and administration while eliminating the drawbacks of traditional paper-based systems, such as time consumption, insecurity, and human error.

## Features

- **Automated Attendance Tracking**: Records attendance data using a fingerprint sensor and displays the status on an OLED screen.
- **Real-Time Data Processing**: Attendance data is immediately uploaded to ThingSpeak for real-time analysis via the NodeMCU ESP8266.
- **User-Friendly Interface**: Simplifies monitoring for educators and administration.
- **Improved Accuracy**: Reduces human error and enhances data security compared to traditional methods.
- **Portable Power Supply**: The system is powered through a laptop USB port, making it easy to set up and use.

## Components

### Hardware
- NodeMCU ESP8266 Microcontroller
- R307s Fingerprint Sensor
- OLED SH1106 Display
- Connecting Cables

### Software
- Arduino IDE for programming the NodeMCU ESP8266
- ThingSpeak for data analysis and visualization

## Setup Instructions

1. **Hardware Assembly**:
   - Connect the NodeMCU ESP8266 to the R307s fingerprint sensor and OLED SH1106 using the appropriate connecting cables.
    
2. **Software Configuration**:
   - Program the NodeMCU ESP8266 using the Arduino IDE, ensuring it can communicate with the ThingSpeak API.
   - Set up a ThingSpeak account and create a new channel to store attendance data.
   - Obtain the API key from ThingSpeak and include it in your NodeMCU's code for data transmission.

3. **Deploy and Test**:
   - Upload the code to the NodeMCU ESP8266 and verify that it connects to the internet.
   - Test the system by scanning a fingerprint, checking the OLED SH1106 for status updates, and ensuring data is correctly uploaded to ThingSpeak.
   - Monitor the ThingSpeak channel to confirm that the data is being recorded and analyzed as expected.

## Usage

- **Lecturers and Administration**: Access the ThingSpeak channel to monitor attendance records and perform any necessary analysis.
- **Students**: Use the fingerprint sensor to mark their attendance, with confirmation displayed on the OLED screen.

## Future Enhancements

- Integration with student management systems for automated record-keeping.
- Development of a mobile app for easy access to attendance data.
- Implementation of advanced data analytics for attendance trends and predictions.

## Contributing

Contributions are welcome! Please fork this repository and submit a pull request with your improvements or fixes.
