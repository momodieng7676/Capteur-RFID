from pydispatch import Dispatcher, DictProperty
import colorama

__all__ = ['Action', 'ArduinoAction', 'SoundAction','RfidAction', 'LightAction', 'sketch']

sketch = None


class Action(Dispatcher):
    """base class for all actions"""

    def __init__(self, action_json):
        super().__init__()
        self.id = action_json['id']
        self.name = action_json['name']
        self.action = action_json['action']
        self.parameters = action_json['options'] if 'options' in action_json else {}

    @staticmethod
    def from_json(action_json):
        if action_json['type'] == "sound_action":
            return SoundAction(action_json=action_json)
        if action_json['type'] == 'arduino_action':
            return ArduinoAction(action_json=action_json)
        if action_json['type'] == 'light_action':
            return LightAction(action_json=action_json)
        if action_json['type'] == 'rfid_action':
            return RfidAction(action_json=action_json)	
        raise ValueError(f"Unknown action type {action_json['type']}")

    def fire(self, *args, verbose=True, **kwargs):
        if verbose:
            print(f"Action {colorama.Fore.RED}{self.name}{colorama.Style.RESET_ALL}, id {self.id} fired")

    def __call__(self, *args, **kwargs):
        return self.fire(*args, **kwargs)

class ArduinoAction(Action):
    """base class for all actions that involve an Arduino (ex: LED, motors...)"""

    parameters = DictProperty()

    def __init__(self, action_json):
        super().__init__(action_json)
        self.params_order = action_json['options_order']
        if 'arduino' in self.params_order:
            self.params_order.remove('arduino')
        self.arduino_target = self.parameters['arduino'] if 'arduino' in self.parameters else None


    @property
    def parameters_list(self):
        return [self.parameters[param] for param in self.params_order]

    def fire(self):
        super().fire()
        sketch.send(self.action, *self.parameters_list, arduino=self.arduino_target)

    def update_params(self):
        sketch.send(self.action + "_params", *self.parameters_list, arduino=self.arduino_target)

    def stop(self):
        sketch.send(self.action + "_stop", arduino=self.arduino_target)


class SoundAction(Action):
    """Class for any sound action (play/pause a channel, play a sound...)"""

    def __init__(self, action_json):
        super().__init__(action_json)

    def fire(self):
        super().fire()

        # play_channel
        if self.action == 'play_channel':
            if 'fade_time' in self.parameters:
                sketch.mediamanager[self.parameters['channel']].fade_and_play(self.parameters['fade_time'])
            else:
                sketch.mediamanager.play(self.parameters['channel'])

        # pause_channel
        elif self.action == 'pause_channel':
            if 'fade_time' in self.parameters:
                sketch.mediamanager[self.parameters['channel']].fade_and_pause(self.parameters['fade_time'])
            else:
                sketch.mediamanager.pause(self.parameters['channel'])

        # set_volume
        elif self.action == 'set_volume':
            sketch.mediamanager.set_volume(
                self.parameters['channel'],
                self.parameters['volume'],
                self.parameters['fade_time'] if 'fade_time' in self.parameters else 0
            )

        # set_loop_mode
        elif self.action == 'set_loop_mode':
            sketch.mediamanager[self.parameters['channel']].set_loop(self.parameters['loop_mode'])

        # play_media_on_channel
        elif self.action == 'play_media_on_channel':
            sketch.mediamanager[self.parameters['channel']].play_media(
                self.parameters['media_index'],
                self.parameters['transition_time'] if 'transition_time' in self.parameters else 0
            )

        # play_sound
        elif self.action == 'play_sound':
            volume = self.parameters['volume'] if 'volume' in self.parameters else 100
            other_volume = self.parameters['other_volume'] if 'other_volume' in self.parameters else 50
            fade_time = self.parameters['fade_time'] if 'fade_time' in self.parameters else 0
            if 'fade_others' in self.parameters and self.parameters['fade_others']:
                sketch.mediamanager.play_now(
                    sound_filename=self.parameters['filename'],
                    volume=volume,
                    other_channels_volume=other_volume,
                    fade_time=fade_time
                )
            else:
                sketch.mediamanager.play_now(
                    sound_filename=self.parameters['filename'],
                    volume=volume,
                )

class LightAction(Action):
    """Class for any light action (turn on/off a bulb, fade to color...)"""

    def __init__(self, action_json):
        super().__init__(action_json)
        self.parameters['bulbs'] = self.parameters['bulbs'].split(',')

    def fire(self):
        super().fire()

        # turn_on
        if self.action == 'turn_on':
            sketch.lightmanager.turn_on(self.parameters['bulbs'])

        # turn_off
        elif self.action == 'turn_off':
            sketch.lightmanager.turn_off(self.parameters['bulbs'])

        # set_color
        elif self.action == 'set_color':
            sketch.lightmanager.set_color(
                self.parameters['bulbs'],
                self.parameters['red'],
                self.parameters['green'],
                self.parameters['blue']
            )


        # set_brightness
        elif self.action == 'set_brightness':
            sketch.lightmanager.set_brightness(
                self.parameters['bulbs'],
                self.parameters['brightness']
            )

        # fade
        elif self.action == 'fade':
            sketch.lightmanager.fade(
                self.parameters['bulbs'],
                self.parameters['red'],
                self.parameters['green'],
                self.parameters['blue'],
                self.parameters['brightness'],
                self.parameters['fade_time']
            )
            
class RfidAction(Action):
    """Class for any rfid action (read, read&write)"""

    def __init__(self, action_json):
        super().__init__(action_json)

    def fire(self):
        super().fire()

        # read
        if self.action == 'read':
            sketch.arduinos_manager.send_command(self.parameters['arduino'], "readRFID " + self.parameters['blockRead'])

        # read&write
        elif self.action == 'read&write':
            sketch.arduinos_manager.send_command(
            	self.parameters['arduino'],
            	"readAndWriteRFID " + self.parameters['blockRead']
            )#améliorer si nécessaire en utilisant send de sketch

