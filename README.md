Introduction

The purpose of this project is the construction and programming of an autonomous robotic vehicle (Robot Car). The vehicle is able to navigate its environment without human intervention, using sensors to detect and avoid obstacles.

The Logic (Algorithm)

Here is what we will program the robot to do:

Move forward.

Detect an obstacle (e.g., at a distance < 30 cm) → STOP.

Reverse briefly (to back away / get unstuck).

Look right: the servo rotates to 10 degrees → measure distance → store the measurement.

Look left: the servo rotates to 170 degrees → measure distance → store the measurement.

Compare: which side has more free space (right or left)?

Turn: rotate toward the side with more available space.

Return the servo to the center (90 degrees) and move forward again.
