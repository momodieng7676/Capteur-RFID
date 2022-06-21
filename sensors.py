from pydispatch import Dispatcher, ListProperty

__all__ = ["Sensor", "DistanceSensor", "MovementSensor", "ColorSensor", "RfidSensor"]


class Sensor(Dispatcher):
    """Base class for all sensors"""

    values = ListProperty(copy_on_change=True)

    def __init__(self, name):
        super().__init__()
        self.name = name
        self.nbr_values = None

    def data_received(self, data: list):
        self.values = data


class DistanceSensor(Sensor):
    """Class for the ultrasonic distance sensor"""
    def __init__(self, name):
        super().__init__(name)
        self.nbr_values = 1

    @property
    def distance(self):
        """distance value"""
        return self.values[0]


class MovementSensor(Sensor):
    """Class for the movement sensor"""
    def __init__(self, name):
        super().__init__(name)
        self.nbr_values = 1

    @property
    def movement(self):
        """current movement"""
        return self.values[0]


class ColorSensor(Sensor):
    """Class for the color sensor"""
    def __init__(self, name):
        super().__init__(name)
        self.nbr_values = 3

    @property
    def red(self):
        """red value"""
        return self.values[0]
    @property
    def green(self):
        """green value"""
        return self.values[1]
    @property
    def blue(self):
        """blue value"""
        return self.values[2]

class RfidSensor(Sensor):
	"""Class for the Rfid Sensor"""
    def __init__(self, name):
        super().__init__(name)
        self.nbr_values = 1

    @property
    def information(self):
        """red value"""
        return self.values[0]
