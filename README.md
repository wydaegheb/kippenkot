# kippenkot
This is a small project to open/close a chicken coop automatically.
It's using an esp32, a ds3231 RTC, an Adafruit_INA219 to measure current (avoid crushed chickens ;-)), a 12 dc with womrwheel (6rpm) and a l298n motor driver.
This is the software running on the esp32. It communicates with bluetooth with the corresponding chicken coop app.
