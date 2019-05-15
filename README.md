# Penelopean Robotics

Penelopean robots are modular, reconfigurable, dissessemblable and
soft. Part of the [Penelope Project](https://penelope.hypotheses.org/), 
a Study of Weaving as Technical Mode of Existence.

See a video of our [first swarm robotics maypole dancing test.](https://www.youtube.com/watch?v=NbzEvIek4nM)

![](https://raw.githubusercontent.com/fo-am/penelopean-robotics/master/pic.jpg)

Penelopean robots designed for weaving ancient Greek looms and maypole
dancing. They can be livecoded remotely by a Raspberry Pi and can be
synced to musical beats.

All source code and hardware files are here, full instructions on building them will follow - but for now, each robot is made from:

* Tablet woven flexible structure holding everything together
* 3 SG92R microservos to drive the legs
* 1 3.7v li-ion battery
* Nrf24L01 radio module for remote control/programming
* GY91 accellerometer/gyro/compass sensor module
* Custom Atmega328 main pcb
* Radio, sensor and servo breakout secondary pcb
* 3 laser cut ply leg pairs

# More detailed description

## Woven construction

Each robot is constructed using tablet weaving, an ancient human
technology dating back to neolithic times (and one of the few forms of
weaving that has never been mechanised).

[tablet loom]

Tablet looms are simple to set up, and consist of a stack of cards
with holes punched in the corners through which the warp yarn passes.
[See here for a description of how to set up tablet loom]().

Usually tablet weaving produces a strong braid. For the penelopean
robotics we use a technique known as Icelandic double weave, where we
weave with the cards rotated at 45 degrees and use two wefts at the
same time. This converts the usually flat "2D" weave into a "3D"
structure, which we can use to support all the components for the
robot.

The exact spacing required will depend on the kind of yarn you are
using (I recommend 100% cotton or other plant based yarns - as wool is
too fluffy) but using this yarn I measured out 10 wefts to hold an
SG92R microservo and 8 for the 18mm diameter cylidrical battery.

Here is an attempt at notating the steps required in full. We start
with a supporting weave using normal single weft weaving, and then
start splitting to to hold the 3 servos and a battery, finishing with
an final supporting weave [todo: later version included a spacer to
make the legs equdistant]

[weaving instructions]

## Raspberry Pi swarm controller setup

The swarm of robots is controlled by a Raspberry Pi running a python
program which is in charge of distributing commands to the robots. 

## Movement sequencer

[diagram]

## The Yarn bytecode language

One of the problems with having numerous robots is updating and
distributing code changes to them. In order to make this process more
versatile we use a livecoding system where bytecode can be sent over
radio and written directly to memory. This bytecode is interpreted by
a machine running on the robot which can be halted and reset over
radio.

The robot bytecode (called yarn) has access to a set of registers that
describe the current state of the robot, including the movement
sequencer and sensor information. Registers can also be written to
that change the current servo motion pattern or light the LED for
example.

Writing the bytecode directly is considered a temporary inconvenience
before a compiler for a high level language is designed.

## Register description

0  ROBOT_ID (r)    

Identifier of this robot (same code as the last byte of the radio pipe
address)

1  PC_MIRROR (r)    

A copy of the program counter for debugging. Writing to this has no
effect.

2  LED (r/w)

State of the onboard blue LED.

3  COMP_ANGLE (r)

Current absolute compass angle from the magnetometer.

4  COMP_DELTA_RESET (w)    

Resets the relative compass angle calculation.

5  COMP_DELTA (r)    

Relative compass angle calculation, from last reset.

6  STEP_COUNT (r)    

How many times the current movement sequence has run (not really a
step as each sequence has two steps).

7  STEP_COUNT_RESET (w)

Resets the step counter register to zero.

8  NEXT_PATTERN (r/w)

Stores a pattern ID for use when the current motion sequence has
finished. Can be one of:

NULL (0)           : Keep the last pattern
ALL_STOP (1)       : Cease all servo movement
WALK_FORWARDS (2)  : Move forward
WALK_BACKWARDS (3) : Move backwards

9  A (r/w)
10 B (r/w) 
11 C (r/w)
12 D (r/w)

User defined registers for convenience. A is currently used as the
maypole dance status to syncronise multiple groups of robots.

13 SERVO_MS_PER_STEP (r/w)

This controls the overall speed of the motion sequencer - in
milliseconds per step.

14 SERVO_1_AMP (r/w)
15 SERVO_2_AMP (r/w) 
16 SERVO_3_AMP (r/w)

Seperate control over the amplitude of each servos movement. Set these
to zero will immediately prevent any movement while the sequencer
still runs. This is a fixed point value where 1000 (the default) is
equal to one, so setting to 2000 will double the amplitude and range
of the servo.

17 SERVO_1_BIAS (r/w)
18 SERVO_2_BIAS (r/w)
19 SERVO_3_BIAS (r/w)

Sets the effective zero degrees position of each servo. This can be
used either for fine tuning or in the case of the middle servo (2) can
be used to steer the robot when walking.

20 SERVO_1_SMOOTH 
21 SERVO_2_SMOOTH 
22 SERVO_3_SMOOTH 

To prevent sudden movements from breaking the robot or causing it to
jump, each servo has a smoothing (low pass filter) applied in software
to the desired rotation. This can be removed by setting this to zero,
this is a fixed point number where 200 (0.2) is the default value.
 
# Yarn instruction set

Yarn bytecode is a 16 bit stack based microcode language. 

| Bytecode instruction name | Example             | Description                                         |
| ------------------------- | --------------------| --------------------------------------------------- |
| nop                       | nop                 | No operation, do nothing for one cycle              |
| jmp                       | jump label          | Unconditional jump                                  |       
| jpr                       | jpr  -2             | Unconditional jump relative (can be negative)       |
| jpz                       | jpz  label          | Jump only if the top of stack is zero.              |
| jprz                      | jprz 10             | Jump relative if the top of the stack is zero.      |
| ldl                       | ldl  1207           | Load literal value on to stack                      |
| ld                        | ld   STEP_COUNT     | Load the contents of address on to the sta          |
| ldi                       | ldi  A              | Load indirect: the contents of the address pointed to by value (for arrays). | 
| st                        | st   NEXT_PATTERN   | Stores the top of the stack in the supplied address |
| sti                       | sti  A              | Stores indirectly the stack top to the address at this location |
| dup                       | dup                 | Pushes the value at the top of the stack to duplicate it |  



lt 
gt 
lte 
gte
slt
sgt
slte
sgte
add
sub
inc
dec
and
or
xor
not
rr
rl
sin
cos
tan
rnd
inci
deci
incp
decp


