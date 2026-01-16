# Smartgreenhouse
This project is an attempt to create a self-regulating greenhouse that would provide optimal groving conditions for high-humidity plants.

## Sensors
- Gathers temperature and humidity from a DHT11 sensor module
- Measures the hours of light recieved by a photoresistorselement

## Controls
- Regulates a mister, created by an ultrasonic atomizer module (exact model unknown)
- Simulates a heating element (regular LED used since no heating element where available)
- Simulates an UV light (regular LED used since no UV lamps where available)

## Communication
The ESP runs an small server.
Regulates the set targets.