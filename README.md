# SCARAB

SCARAB is a six legged robot that you drive with a glove. Tilt your hand forward and it
walks forward. Press one of the six buttons on the glove and it stands, waves, flinches
back, or drops flat on the floor.

The name is a backronym: Self-Contained Automated Robotic Android-like Bug.

I built it over the 2023 to 2024 school year for a TSA competition and it won first at
Nationals in 2024. It took 141 logged hours and about $200 in parts.

The full build guide, with the parts list, the wiring and the printed files, is at
[noahpuckett.me/scarab.html](https://noahpuckett.me/scarab.html). This repo is just the code.

## The two programs

There are two sketches and they run on two different ESP32s. `Gauntlet` is the glove
you wear. `Hexapod` is the robot. They talk over ESP-NOW, which is a radio mode built
into the ESP32 that connects two of them directly without a WiFi network in between.

The glove never knows anything about legs. Four times a second it sends four values and
that is the whole conversation:

| Value | What it is |
| --- | --- |
| `ax`, `ay` | Tilt from the accelerometer, in m/s/s |
| `walkDir` | Forward, backward, left, right or idle |
| `state` | Which of the six buttons is lit |

The robot gets those four values and decides what to do with them. Splitting it up that
way is probably why I got it finished, because I could break one side without breaking
the other.

### Gauntlet

```
comm_gauntlet_full.ino   Reads the six buttons and the tilt sensor, sends the result
helperButton.h           One button plus its LED, with debouncing
helper_MPU.h             MPU6050 setup, and turning tilt into a direction
helper_COM_send.h        ESP-NOW setup and sending
helper_DATA.h            The struct and the enums both sides share
```

Steering does not use the buttons. The MPU6050 sits on the back of the hand and if you
lean it past 6 m/s/s of sideways acceleration, which is about 38 degrees, the robot
walks that way. Hold your hand flat and it marches in place.

The six buttons each pick a state, and pressing one turns the other five off in
software. That matters more than it sounds like it should. If all six could be on at
once you could select walking and flattening at the same time, and the robot cannot do
both. One light on means one mode, so you can look at your hand and know what the robot
is about to do.

| Button | State | What happens |
| --- | --- | --- |
| 0, yellow | `stateON` | Wakes it up on the first press, then unpowers the servos |
| 1, orange | `stateWALK` | Walks wherever you are tilting |
| 2, red | `stateSTAND` | Stands still with every joint at 90 degrees |
| 3, blue | `stateHELLO` | Waves, then goes back to standing |
| 4, purple | `stateSCARED` | Flinches, then goes back to standing |
| 5, green | `stateFLATTEN` | Drops flat, then goes back to standing |

Blue, purple and green are one shots. The robot runs the pose and puts itself back into
standing on its own. Orange and red are modes that it sits in until you press something
else.

The glove only transmits when something actually changed, and not more often than every
500 ms. Sending the same packet over and over just fills the air and makes the debounce
worse.

### Hexapod

```
hexapod.ino              Setup and the main loop, which is one switch on the state
gesture.ino              My poses: stretch, flatten, hello, scared
State_initialize.ino     The wake up move, plus stateAngle and stateZero
State_standing.ino       Every joint back to 90 degrees
helper_COM_recv.h        ESP-NOW receiving
```

Six legs, three joints each, so eighteen MG996R servos. Sixteen channels is all one
PCA9685 driver has, so there are two of them: one for the right legs, one for the left.
The left side is mirrored, so every angle sent to it is 180 minus the angle sent to the
right.

Every gesture is stepped through in small increments instead of jumped to. `stateA2B`
takes a start pose and an end pose and walks between them in 100 steps with a 5 ms pause
each, which is the difference between a robot that moves and a robot that snaps.

## Building it

Board is **DOIT ESP32 DEVKIT V1** in the Arduino IDE, serial monitor at 115200.

You need these libraries: `Adafruit_MPU6050`, `Adafruit_Sensor` and
`Adafruit_PWMServoDriver`. `esp_now.h` and `WiFi.h` come with the ESP32 board package.

The `helper_*.h` files are included with angle brackets, so put them in your Arduino
libraries folder rather than next to the sketch.

**The Hexapod sketch does not compile on its own.** It uses the walking code and the
inverse kinematics from the AecertRobotics hexapod, which is not mine and is not in this
repo. You need `Helpers.h`, the `LEG` class, `Vector2`, `Vector3`, the gait code and
`state_Car()` from
[Ryan-Mirch/Aecerts_Hexapod_V1](https://github.com/Ryan-Mirch/Aecerts_Hexapod_V1), plus a
`servoHex.h` that creates `myServoL` and `myServoR` and sets `SERVO_FREQ`. Drop them in
beside mine.

Both ESP32s need to know the receiver's MAC address. I forced the robot's to
`AA:BB:CC:DD:00:00` so I did not have to read it off the board and retype it into the
glove every time I reflashed. It is at the top of `helper_COM_send.h` and
`helper_COM_recv.h` and it has to match in both.

## Credit

The frame and the legs are the [MakeYourPet hexapod](https://github.com/MakeYourPet/hexapod),
which is free and open source and not my design. The walking and the inverse kinematics
are Ryan Mirch's, linked above, and the maths behind them is explained properly in
JustAnotherMakerChannel's video [Robot Inverse Kinematics with a Hexapod Leg](https://www.youtube.com/watch?v=HjmIOKSp7v4).
The ESP-NOW code started from [Random Nerd Tutorials](https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/).
The shell, the head, the battery sheath, the Gauntlet and all the state and gesture code
are mine.
