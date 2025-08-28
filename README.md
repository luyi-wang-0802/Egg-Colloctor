# Egg Collector

This project is based on **Arduino** and uses **stepper motors**, **limit switches**, **an infrared sensor**, and a **servo motor** to implement automatic egg sorting and placement.

---

## Features
1. **Infrared detection**: Detects when an egg is present at the drop position.  
2. **Transverse stepper motor**: Moves to the correct column within a row.  
3. **Longitudinal stepper motor**: Switches between rows of the tray.  
4. **Servo motor**: Simulates a gripper to release the egg.  
5. **Counting logic**:  
   - Each row holds up to 5 eggs.  
   - Each tray holds up to 6 rows (30 eggs total).  
   - When the tray is full, the system stops.

---

## Hardware Requirements
- **Arduino UNO or compatible board**  
- **2 stepper motors**  
  - Transverse motor  
  - Longitudinal motor  
- **2 stepper motor drivers**  
- **1 servo motor**  
- **1 infrared sensor** (egg detection)  
- **2 limit switches** (transverse and longitudinal homing)  
- Power supply and wiring

---

## Pin Assignment
| Function                | Pin |
|-------------------------|-----|
| Transverse motor PULL   | 3   |
| Transverse motor DIR    | 5   |
| Longitudinal motor PULL | 7   |
| Longitudinal motor DIR  | 8   |
| Servo motor             | 8   |
| Transverse limit switch | 10  |
| Longitudinal limit sw.  | 11  |
| Infrared sensor         | 4   |

> Note: The code uses pin **8** for both the servo and longitudinal DIR.  
> You must reassign one of them before wiring the system.

---

## Parameters
- `STEP_DELAY_US = 100`  
  Stepper pulse interval (microseconds).  
- `COL_PITCH = 4.8`  
  Column spacing (logical units).  
- `STEPS_PER_UNIT = 1600`  
  Steps per logical unit.

---

## Workflow
1. **Initialization**:  
   - Transverse and longitudinal motors return to home.  
   - System moves to the starting run position.  

2. **Main loop**:  
   - Wait for infrared sensor to detect an egg.  
   - Move transverse motor to target column.  
   - Open servo to drop the egg, then close it.  
   - Return transverse motor to start position.  
   - Increment egg counter.  
   - If row is full (5 eggs), reset transverse counter and move longitudinal motor to next row.  
   - If tray is full (6 rows), stop system.  

---

## Usage
1. Upload the `.ino` file to your Arduino board.  
2. Wire all components according to the pin assignment table.  
3. Power the system and it will automatically begin sorting eggs.  

---

## Notes
- Ensure motor drivers match the rated voltage and current of the stepper motors.  
- It is recommended to power the servo separately from the stepper motors to avoid interference.  
- If motor type or gear ratio changes, recalibrate `STEPS_PER_UNIT` and `COL_PITCH`.  
- To adjust the number of eggs per row or total rows, update the threshold values of `txEggCnt` and `lgRowCnt` in the code.  

---

## Author
This program was refactored from the original source for clarity and maintainability.  
It provides a modular structure for easier calibration and expansion.  
