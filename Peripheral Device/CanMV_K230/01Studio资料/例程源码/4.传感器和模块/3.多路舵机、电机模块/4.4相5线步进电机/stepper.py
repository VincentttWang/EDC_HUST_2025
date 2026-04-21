# Stepper Motor Shield/Wing Driver
# Based on Adafruit Motorshield library:
# https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
# Author: Tony DiCola
import pca9685
import time

# Constants that specify the direction and style of steps.
FORWARD = const(1)
BACKWARD = const(2)
SINGLE = const(1)
DOUBLE = const(2)
INTERLEAVE = const(3)
MICROSTEP = const(4)

# Not a const so users can change this global to 8 or 16 to change step size
MICROSTEPS = 16

# Microstepping curves (these are constants but need to be tuples/indexable):
_MICROSTEPCURVE8 = (0, 50, 98, 142, 180, 212, 236, 250, 255)
_MICROSTEPCURVE16 = (0, 25, 50, 74, 98, 120, 141, 162, 180, 197, 212, 225, 236, 244, 250, 253, 255)

# Define PWM outputs for each of two available steppers.
# Each tuple defines for a stepper: pwma, ain2, ain1, pwmb, bin2, bin1
_STEPPERS = ((8, 9, 10, 13, 12, 11), (2, 3, 4, 7, 6, 5))

#4线2相步进电机
#红 蓝 黑 绿  B D A C(0,1,2,3)
_STEP_MOTORS_F = ((0, 1, 2, 3), (4, 5, 6, 7)) #顺时针
_STEP_MOTORS_B = ((1, 0, 2, 3), (5, 4, 6, 7)) #逆时针

#5线4相步进电机
#红 橙 黄 粉 蓝 （VCC A B C D） 
_5STEP_MOTORS_F = ((3, 2, 1, 0), (4, 5, 6, 7)) #顺时针
_5STEP_MOTORS_B = ((1, 0, 2, 3), (5, 4, 6, 7)) #逆时针

class StepperMotor:
    def __init__(self, pca, pwma, ain2, ain1, pwmb, bin2, bin1):
        self.pca9685 = pca
        self.pwma = pwma
        self.ain2 = ain2
        self.ain1 = ain1
        self.pwmb = pwmb
        self.bin2 = bin2
        self.bin1 = bin1
        self.currentstep = 0

    def _pwm(self, pin, value):
        if value > 4095:
            self.pca9685.pwm(pin, 4096, 0)
        else:
            self.pca9685.pwm(pin, 0, value)

    def _pin(self, pin, value):
        if value:
            self.pca9685.pwm(pin, 4096, 0)
        else:
            self.pca9685.pwm(pin, 0, 0)

    def onestep(self, direction, style):
        ocra = 255
        ocrb = 255
        # Adjust current steps based on the direction and type of step.
        if style == SINGLE:
            if (self.currentstep//(MICROSTEPS//2)) % 2:
                if direction == FORWARD:
                    self.currentstep += MICROSTEPS//2
                else:
                    self.currentstep -= MICROSTEPS//2
            else:
                if direction == FORWARD:
                    self.currentstep += MICROSTEPS
                else:
                    self.currentstep -= MICROSTEPS
        elif style == DOUBLE:
            if not (self.currentstep//(MICROSTEPS//2)) % 2:
                if direction == FORWARD:
                    self.currentstep += MICROSTEPS//2
                else:
                    self.currentstep -= MICROSTEPS//2
            else:
                if direction == FORWARD:
                    self.currentstep += MICROSTEPS
                else:
                    self.currentstep -= MICROSTEPS
        elif style == INTERLEAVE:
            if direction == FORWARD:
                self.currentstep += MICROSTEPS//2
            else:
                self.currentstep -= MICROSTEPS//2
        elif style == MICROSTEP:
            if direction == FORWARD:
                self.currentstep += 1
            else:
                self.currentstep -= 1
            self.currentstep += MICROSTEPS*4
            self.currentstep %= MICROSTEPS*4
            ocra = 0
            ocrb = 0
            if MICROSTEPS == 8:
                curve = _MICROSTEPCURVE8
            elif MICROSTEPS == 16:
                curve = _MICROSTEPCURVE16
            else:
                raise RuntimeError('MICROSTEPS must be 8 or 16!')
            if 0 <= self.currentstep < MICROSTEPS:
                ocra = curve[MICROSTEPS - self.currentstep]
                ocrb = curve[self.currentstep]
            elif MICROSTEPS <= self.currentstep < MICROSTEPS*2:
                ocra = curve[self.currentstep - MICROSTEPS]
                ocrb = curve[MICROSTEPS*2 - self.currentstep]
            elif MICROSTEPS*2 <= self.currentstep < MICROSTEPS*3:
                ocra = curve[MICROSTEPS*3 - self.currentstep]
                ocrb = curve[self.currentstep - MICROSTEPS*2]
            elif MICROSTEPS*3 <= self.currentstep < MICROSTEPS*4:
                ocra = curve[self.currentstep - MICROSTEPS*3]
                ocrb = curve[MICROSTEPS*4 - self.currentstep]
        self.currentstep += MICROSTEPS*4
        self.currentstep %= MICROSTEPS*4
        # Set PWM outputs.
        self._pwm(self.pwma, ocra*16)
        self._pwm(self.pwmb, ocrb*16)
        latch_state = 0
        # Determine which coils to energize:
        if style == MICROSTEP:
            if 0 <= self.currentstep < MICROSTEPS:
                latch_state |= 0x3
            elif MICROSTEPS <= self.currentstep < MICROSTEPS*2:
                latch_state |= 0x6
            elif MICROSTEPS*2 <= self.currentstep < MICROSTEPS*3:
                latch_state |= 0xC
            elif MICROSTEPS*3 <= self.currentstep < MICROSTEPS*4:
                latch_state |= 0x9
        else:
            latch_step = self.currentstep//(MICROSTEPS//2)
            if latch_step == 0:
                latch_state |= 0x1  # energize coil 1 only
            elif latch_step == 1:
                latch_state |= 0x3  # energize coil 1+2
            elif latch_step == 2:
                latch_state |= 0x2  # energize coil 2 only
            elif latch_step == 3:
                latch_state |= 0x6  # energize coil 2+3
            elif latch_step == 4:
                latch_state |= 0x4  # energize coil 3 only
            elif latch_step == 5:
                latch_state |= 0xC  # energize coil 3+4
            elif latch_step == 6:
                latch_state |= 0x8  # energize coil 4 only
            elif latch_step == 7:
                latch_state |= 0x9  # energize coil 1+4
        # Energize coils as appropriate:
        if latch_state & 0x1:
            self._pin(self.ain2, True)
        else:
            self._pin(self.ain2, False)
        if latch_state & 0x2:
            self._pin(self.bin1, True)
        else:
            self._pin(self.bin1, False)
        if latch_state & 0x4:
            self._pin(self.ain1, True)
        else:
            self._pin(self.ain1, False)
        if latch_state & 0x8:
            self._pin(self.bin2, True)
        else:
            self._pin(self.bin2, False)
        return self.currentstep


class Steppers:

    def __init__(self, i2c, address=0x40, freq=1600):
        self.pca9685 = pca9685.PCA9685(i2c, address)
        self.pca9685.freq(freq)

    def _pin(self, pin, value=None):
        if value is None:
            return bool(self.pca9685.pwm(pin)[0])
        if value:
            self.pca9685.pwm(pin, 4096, 0)
        else:
            self.pca9685.pwm(pin, 0, 0)

    def Step(self, index, steps=0, direction=1,mode=4, interval=2):
        
        #判断顺时针或逆时针旋转
        if direction == 1:
            in1, in2, in3, in4 = _STEP_MOTORS_F[index]
        else:
            in1, in2, in3, in4 = _STEP_MOTORS_B[index]


        if mode == 4: #4拍
        
            # Forward
            for i in range(steps):

                self._pin(in1, 1) #A
                self._pin(in2, 0) #C
                self._pin(in3, 1) #B
                self._pin(in4, 0) #D
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 1)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 1)
                self._pin(in2, 0)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
               
        elif mode == 8: #八拍
        
            # Backward
            for i in range(steps):
                self._pin(in1, 1)
                self._pin(in2, 0)
                self._pin(in3, 0)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 1) #A
                self._pin(in2, 0) #C
                self._pin(in3, 1) #B
                self._pin(in4, 0) #D
                
                time.sleep_ms(interval)
                
                self._pin(in1, 0)
                self._pin(in2, 0)
                self._pin(in3, 1)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 1)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 0)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 0)
                self._pin(in2, 0)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 1)
                self._pin(in2, 0)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)

class Steppers_5W:

    def __init__(self, i2c, address=0x40, freq=1600):
        self.pca9685 = pca9685.PCA9685(i2c, address)
        self.pca9685.freq(freq)

    def _pin(self, pin, value=None):
        if value is None:
            return bool(self.pca9685.pwm(pin)[0])
        if value:
            self.pca9685.pwm(pin, 4096, 0)
        else:
            self.pca9685.pwm(pin, 0, 0)

    def Step(self, index, steps=0, direction=1,mode=4, interval=2):
        
        #判断顺时针或逆时针旋转
        if direction == 1:
            in1, in2, in3, in4 = _5STEP_MOTORS_F[index]
        else:
            in1, in2, in3, in4 = _5STEP_MOTORS_B[index]


        if mode == 4: #4拍
        
            # Forward
            for i in range(steps):

                self._pin(in1, 0) #A
                self._pin(in2, 0) #C
                self._pin(in3, 1) #B
                self._pin(in4, 1) #D
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 1)
                self._pin(in2, 0)
                self._pin(in3, 1)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 1)
                self._pin(in2, 1)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 1)
                self._pin(in2, 1)
                self._pin(in3, 1)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
               
        elif mode == 8: #八拍
        
            # Backward
            for i in range(steps):
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 1)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 0) 
                self._pin(in2, 0) 
                self._pin(in3, 1) 
                self._pin(in4, 1) 
                
                time.sleep_ms(interval)
                
                self._pin(in1, 1)
                self._pin(in2, 0)
                self._pin(in3, 1)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 1)
                self._pin(in2, 0)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 1)
                self._pin(in2, 1)
                self._pin(in3, 0)
                self._pin(in4, 1)
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 1)
                self._pin(in2, 1)
                self._pin(in3, 0)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                self._pin(in1, 1)
                self._pin(in2, 1)
                self._pin(in3, 1)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
                
                self._pin(in1, 0)
                self._pin(in2, 1)
                self._pin(in3, 1)
                self._pin(in4, 0)
                
                time.sleep_ms(interval)
                
'''
    def __init__(self, i2c, address=0x40, freq=1600):
        self.pca9685 = pca9685.PCA9685(i2c, address)
        self.pca9685.freq(freq)

    def get_stepper(self, num):
        pwma, ain2, ain1, pwmb, bin2, bin1 = _STEPPERS[num]
        return StepperMotor(self.pca9685, pwma, ain2, ain1, pwmb, bin2, bin1)
'''