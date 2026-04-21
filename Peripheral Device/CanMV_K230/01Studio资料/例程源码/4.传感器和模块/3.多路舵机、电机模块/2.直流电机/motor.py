import pca9685
import time

_DC_MOTORS = ((8, 9, 10), (13, 12, 11), (2, 3, 4), (7, 6, 5))
_2W_MOTORS = ((0, 1), (2, 3), (4,5), (6, 7)) #4 channel 2 wire motors


class DCMotors:
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

    def speed(self, index, value=None):
        pwm, in2, in1 = _DC_MOTORS[index]
        if value is None:
            value = self.pca9685.duty(pwm)
            if self._pin(in2) and not self._pin(in1):
                value = -value
            return value
        if value > 0:
            # Forward
            self._pin(in2, False)
            self._pin(in1, True)
        elif value < 0:
            # Backward
            self._pin(in1, False)
            self._pin(in2, True)
        else:
            # Release
            self._pin(in1, False)
            self._pin(in2, False)
        self.pca9685.duty(pwm, abs(value))

    def brake(self, index):
        pwm, in2, in1 = _DC_MOTORS[index]
        self._pin(in1, True)
        self._pin(in2, True)
        self.pca9685.duty(pwm, 0)

class Motors:
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

    def speed(self, index, value=0):
    
        in1, in2 = _2W_MOTORS[index]

        if value > 0:
            # Forward
            self.pca9685.duty(in1,abs(value))
            self._pin(in2, False)

        elif value < 0:
            # Backward
            self.pca9685.duty(in2,abs(value))
            self._pin(in1, False)
        else:
            # Release
            self._pin(in1, False)
            self._pin(in2, False)

    def brake(self, index):
        in2, in1 = _2W_MOTORS[index]
        self._pin(in1, True)
        self._pin(in2, True)
        #self.pca9685.duty(pwm, 0)