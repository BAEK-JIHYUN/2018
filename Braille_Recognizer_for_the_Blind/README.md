# Requirement
Arduino 1.8.5<br>
Arduino mege 2560<br>
Arduino-compliant digital speaker module<br>
Arduino-compliant Audio module<br>

# About
This model is a sliding type Braille-Recognizer for blind who cannot read Braille. The Braille-Recognizer and braille were produced using 3D printing. Because actual braille sizes are too small to find suitable parts, they are designed in larger sizes.

# Block Diagram
![block diagram](https://user-images.githubusercontent.com/47517000/52558812-e95feb80-2e36-11e9-9fb7-624bc23946fb.jpg)
-  ① After passing through a 2mm-high-braille, the spring is raised by 2mm to reach the PCB substrate accurately, short and the signal is transmitted. It has digital data of 0V for non-spring contact and 3.3V for contact. 
- ② The entered data is processed by Arduino. The data are stored six times using 0 and 1, which is converted into decimal numbers and stored again in an array. When you finish sliding, read the decimal data in sequence from the front to compare the pre-stored 49 Braille data to determine which character it is. The determined characters are combined into letters and stored in an array.
- ③ When finished, locate the recording file of the characters stored in the array and print them out voice in turn using audio and speaker modules.

# Finished Model
![final_model](https://user-images.githubusercontent.com/47517000/52558818-ed8c0900-2e36-11e9-958d-a1df4e182e72.png)
