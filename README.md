# Rocketry_Easy_Telemetry

This project has been done to celebrate my Bachelor Degree in Mechanical Engineering.

In this repository I've uploaded all the files related not only with the flight computer but also with the design of the rockets themselves.

## Flight Computer

The flight computer is based on an Arduino Nano, a BMP280 sensor, a MPU-6050 sensor and a microSD card reader.
The code is written in C++ using of course the Arduino IDE.

Since I've used rocket engine that already have a built-in delay time, I've decided to not implement a parachute deployment system controlled by the onboard computer.

![Flight Computer Schematics](/Arduino/Schematics.png)

### Sensors

As already said, in the rocket I've used:
- BMP280: it measures the pressure and the temperature of the environment.
- MPU-6050: it measures the acceleration of the rocket.

From a physical point of view, it's interesting to notice that the BMP280 sensor is used to calculate the altitude of the rocket.
In fact, the pressure of the environment decreases with the altitude.
Moreover, supposing that the perfect gas law is valid and knowing the pressure at the sea level, it's possible to compute the altitude of the rocket.

Starting from the perfect gas law and the Stevino's law, it's possible to derive the following formulas:

```math
\Delta p = -\rho g \Delta h \rightarrow \Delta h = -\frac{\Delta p}{\rho g} \\
```

```math
P = \rho \frac{RT}{M} \rightarrow \frac{\Delta p}{p} = \Delta ln\left(p\right) = -\frac{gM}{RT}\Delta h \\
```

Finally, combining the two equations above, it's possible to obtain the following formula:

```math
h = \frac{RT}{gM}ln\left(\frac{P_0}{P}\right)
```

where:
- h is the altitude measured from the sea level
- R is the universal gas constant
- T is the temperature measured by the sensor
- g is the gravitational acceleration
- M is the molar mass of the air
- P_0 is the pressure at the sea level
- P is the pressure measured by the sensor

In reality, there was no need to implement the previous formula since the BMP280 sensor has a built-in function that computes the altitude from the pressure and the temperature.

Notice how the altitude could also be computed by integrating over time the acceleration measured by the MPU-6050 sensor.
It's known that the acceleration is the second derivative of the covered space, so by integrating it twice it's possible to obtain the positive elevation of the rocket.
However, this method is not very accurate since the sensor is not perfect and it's affected by noise.

Moreover, the altitude computed by the BMP280 sensor is more accurate than the one computed by the MPU-6050 sensor.

### Landing mode

Finally, due to the limitation given by the memory of the Arduino Nano, I've been forced to implement a landing mode that of course is not ideal.

In fact, the so-called 'landing mode' is not more than a timer that supposing the preparation, launching and fly time of 15 minutes, it turns of the sensors and activate a simple (but effective when it comes to the finding of the rocket in large areas of landing) buzzer that continue to buzz until the battery dies.

## Rocket Design

Since we are here on GitHub, I prefer to speak only about the code of this project and I strongly suggest you to watch my [personal website](https://bocchio.dev/) where I will explain in a dedicated article the detail of the design of the rockets.

## Future development

As suggested by the title of this repository, this project wasn't about creating the most sophisticated flying computer ever, but it was more about developing a simple and easy to use telemetry system.
Also, one of the main limitation was given by the physical dimension of the rocket itself.
In fact, the rocket was very small and the space inside it was very limited.

However, I'm planning to develop a more sophisticated flight computer that will be able to control the parachute deployment system and also to communicate with the ground station for real time telemetry.

Until then, I hope you will enjoy this project and if you have any question, please feel free to contact me.

Have a nice coding day,

Tommaso :panda_face:

