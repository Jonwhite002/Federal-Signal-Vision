#ifndef LIGHTBAR.H
#define LIGHTBAR.H

#include "Adafruit_TLC5947.h"
#include <millisDelay.h>

#define NUM_TLC5947 1
const int data = 4;
#define clock   5
#define latch   6
#define oe  -1  // set to -1 to not use the enable pin (its optional)

const int arr_LED = 24; // arr_LED[3*rows + cols] to index

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5947, clock, data, latch);
class LightBar {

    public:

        // default constructor
        LightBar() {
            _code_config = 0;
            _signal_config = 0;
        }

        // constructor with code and signalmaster config
        LightBar(int code_config, int signal_config) {
            _code_config = code_config;
            _signal_config = signal_config;
        }

        // destructor
        ~LightBar() {}

        /* public functions*/
        // initialize lightbar LED's
        void Initialize() {

            // begin LED driver
            tlc.begin();

            if (oe >= 0) {
                pinMode(oe, OUTPUT);
                digitalWrite(oe, LOW);
            }

            Reset();
            _Main_Cycle_Delay.start(10);

            // initialize signalmaster
            _SignalMaster_Warning_RefreshRate = true;
        }

        // return current status of flag
        bool get_Flag_Status(int flag) {
            bool temp;
            switch (flag) {
                case 0: // set low beam
                    temp = _TOGGLE_LOW_BEAM;
                    break;
                case 1: // set high beam
                    temp = _TOGGLE_HIGH_BEAM;
                    break;
                case 2: // set wig-wag
                    temp = _TOGGLE_WIG_WAG;
                    break;
                case 3: // set strobe
                    temp = _TOGGLE_STROBE;
                    break;
                case 4: // set lightbar on/off
                    temp = _TOGGLE_LIGHT_BAR;
                    break;
                case 5: // set signalmaster on/off
                    temp = _TOGGLE_SIGNAL_MASTER;
                    break;
            }
            return temp;
        }

        // return current code
        int get_Code_Status() {
            return this->_code_config;
        }
        
        // set flag value
        void set_Flag_Status(int flag, bool val) {
            switch (flag) {
                case 0: // set low beam
                    if (this->_TOGGLE_LOW_BEAM == val)
                        this->_TOGGLE_LOW_BEAM = !val;
                    break;
                case 1: // set high beam
                    if (this->_TOGGLE_HIGH_BEAM == val)
                        this->_TOGGLE_HIGH_BEAM = !val;
                    break;
                case 2: // set wig-wag
                    if (this->_TOGGLE_WIG_WAG == val)
                        this->_TOGGLE_WIG_WAG = !val;
                    break;
                case 3: // set strobe
                    if (this->_TOGGLE_STROBE == val)
                        this->_TOGGLE_STROBE = !val;
                    break;
                case 4: // set lightbar
                    if (this->_TOGGLE_LIGHT_BAR == val)
                        this->_TOGGLE_LIGHT_BAR = !val;
                    break;
                case 5: // set signalmaster
                    if (this->_TOGGLE_SIGNAL_MASTER == val)
                        this->_TOGGLE_SIGNAL_MASTER = !val;
            }
        }

        // begin running lightbar
        void Run() {

            // wait of min/max reached
            OnOff_Delay(0,23); // headlights

            // cycle through/iterate led's
            if (_TOGGLE_LIGHT_BAR) {
                if (_Main_Cycle_Delay.justFinished()) {
                    _Main_Cycle_Delay.repeat();

                    Code_Set(_code_config);
                    Headlight_Control();
                    Taillight_Control();
                }
                Signal_Master(_signal_config);
            }
            else {
                Reset();
            }
        }

        // set lightbar code
        void set_Code(int config) {
            Reset_LightBar();
            this->_INIT_CODE_FLAG = false;
            this->_code_config = config;
        }

        // set signalmaster config
        void set_SignalMaster(int config) {
            Reset_SignalMaster();
            this->_signal_config = config;
        }

    private:

        /* private variables */ 
        // lightbar code & signalmaster configs
        int _code_config;
        int _signal_config;

        // toggle flags
        bool _TOGGLE_LIGHT_BAR = false;
        bool _TOGGLE_SIGNAL_MASTER = false;
        bool _TOGGLE_LOW_BEAM = false; 
        bool _TOGGLE_HIGH_BEAM = false;
        bool _TOGGLE_PARKING_LIGHTS = false;
        bool _TOGGLE_WIG_WAG = false; 
        bool _TOGGLE_STROBE = false;

        // timer objects
        millisDelay _Signal_Arrow_Delay;
        millisDelay _Signal_Center_Delay;
        millisDelay _Signal_Warn_Delay;
        millisDelay _Main_Cycle_Delay;
        millisDelay _ON_Delay[arr_LED]; 
        millisDelay _OFF_Delay[arr_LED]; 

        // lightbar specific variables
        int _arr_val_LED[arr_LED]; // brightness values of each LED
        int _MAX[arr_LED]; // max brightness level (fix this dogshit code)
        int _MIN[arr_LED]; // min brightness level
        int _DELAY_ON[arr_LED]; // time LED stays at MAX
        int _DELAY_OFF[arr_LED]; // time LED stays at MIN
        int _iter[arr_LED]; // how fast LED lights up/down (steps)

        // flag variables
        bool _FLIP_FLAG[arr_LED]; // switch flag to increase/decrease brightness
        bool _WAIT_FLAG_OFF[arr_LED]; // time LED remains OFF
        bool _WAIT_FLAG_ON[arr_LED]; // time LED remains ON
        bool _INIT_SIGNAL_DIRECT_FLAG = false; // initializer flag for signalmaster direct
        bool _INIT_SIGNAL_WARN_FLAG = false;
        bool _INIT_CODE_FLAG = false; // initializer flag for lightbar code number

        // headlight specific
        int _lowBeam_Brightness = 1000, _highBeam_Brightness = 3000, _turnSignal_Brightness = 500;
        int _parkingLight_Brightness = 100;

        //taillight specific
        int _cornerLight_Brightness = 10, _tailLight_Brightness = 70;

        // strobe specific
        int _strobe_iter = 0;
        bool _STROBE_FLIP_FLAG = false;

        // signalmaster specific
        int i, _signalMaster_Brightness = 100;
        bool _SignalMaster_Warning_RefreshRate; // switch flag for warn config

        /* private functions*/
        // // when the LED has been on/off for the desired amount on time, resume
        void OnOff_Delay(int startLED, int endLED) {
            for (int i = startLED; i < endLED; ++i) {
                if (_OFF_Delay[i].justFinished()) {
                    _WAIT_FLAG_OFF[i] = false;
                }
                if (_ON_Delay[i].justFinished()) {
                    _WAIT_FLAG_ON[i] = false;
                }
            }
        }

        // if min/max reached, brighten/dim LED respectively
        void Flip_Direction(int startLED, int endLED) {
            for (int i = startLED; i < endLED; ++i) {
                // check for min/max brightness level
                if (_arr_val_LED[i] > _MAX[i]) { // maximum brightness --> decrease
                    _arr_val_LED[i] = _MAX[i];
                    _FLIP_FLAG[i] = true;

                    _ON_Delay[i].restart();
                    _WAIT_FLAG_ON[i] = true;
                }
                else if (_arr_val_LED[i] < _MIN[i]) { // minimum brightness --> increase
                    _arr_val_LED[i] = _MIN[i];
                    _FLIP_FLAG[i] = false;
                    
                    _OFF_Delay[i].restart();
                    _WAIT_FLAG_OFF[i] = true;
                }
            }
        }

        // brighten or dim LED if not waiting
        void Flip_Update(int startLED, int endLED) {
            for (int i = startLED; i < endLED; ++i) {
                if (!_WAIT_FLAG_OFF[i] && !_WAIT_FLAG_ON[i]) {
                    // iterate (make brighter each step)
                    if (!_FLIP_FLAG[i]) { // minimum brightness --> increase
                    _arr_val_LED[i] += _iter[i];
                    }
                    else { // maximum brightness --> decrease
                    _arr_val_LED[i] -= _iter[i];
                    }
                }
            }
        }

        // update values of all LED's
        void UpdateLED() {
            int j = 0;
            for (int i = 0; i < 8; ++i) {
                if (j < 24) {
                    tlc.setLED(i, _arr_val_LED[j], _arr_val_LED[j+1], _arr_val_LED[j+2]);
                }
                j+=3;
            }
            tlc.write();
        }


        /* lightbar control functions */

        // set lightbar code
        void Code_Set(int config) {

            // initialize LED parameters based on selected code number
            if (!_INIT_CODE_FLAG) {
                
                switch (config) {

                    case 0: // OFF
                        Reset_LightBar();
                        break;

                    case 1: // CODE 1
                        _MIN[13] = 0;
                        _MAX[13] = 4095;
                        _DELAY_ON[13] = 100;
                        _DELAY_OFF[13] = 100;
                        _iter[13] = 176;
                        break;

                    case 2: // CODE 2
                        _MIN[15] = 500;
                        _MAX[12] = 4095; _MAX[13] = 4095; _MAX[14] = 4095; _MAX[15] = 4095;
                        _DELAY_ON[12] = 133; _DELAY_ON[13] = 133, _DELAY_ON[14] = 100, _DELAY_ON[15] = 67;
                        _DELAY_OFF[12] = 400; _DELAY_OFF[13] = 267; _DELAY_OFF[14] = 167; _DELAY_OFF[15] = 233;
                        _iter[12] = 307; _iter[13] = 410; _iter[14] = 614; _iter[15] = 176;
                        break;

                    case 3: // CODE 3
                        for (int i = 15; i > 11; --i) {
                            _arr_val_LED[i] = 4095;
                        }
                        
                        for (int i = 15; i > 11; --i) {
                            _MIN[i] = 50;
                            _MAX[i] = 4095;
                            _DELAY_ON[i] = 33;
                            _DELAY_OFF[i] = 333;
                            _iter[i] = 246;
                            _arr_val_LED[i] -= (15-i)*1365;
                        }
                        break;
                    case 4: // TAKEDOWNS
                        Reset_LightBar();
                        break;
                }
                for (int i = 11; i < 20; ++i) {
                    _OFF_Delay[i].start(_DELAY_OFF[i]);
                    _ON_Delay[i].start(_DELAY_ON[i]);
                }
                _INIT_CODE_FLAG = true;
            }

            // alternate headlights for strobe
            int pinLED;
            bool old_FLAG;
            if (_TOGGLE_STROBE) {
                if (!_WAIT_FLAG_OFF[16] && !_WAIT_FLAG_OFF[17] && !_WAIT_FLAG_ON[16]  && !_WAIT_FLAG_ON[17]) {
                    switch (_STROBE_FLIP_FLAG) {
                        case false:
                            pinLED = 16;
                            break;
                        case true:
                            pinLED =  17;
                            break;
                    }
                }
                old_FLAG = _FLIP_FLAG[pinLED];
            }

            // at min/max, brighten/dim LED

            if (_TOGGLE_WIG_WAG) {
                Flip_Direction(0, 1); // headlights
                Flip_Direction(11, 12); // left taillight
                if (_TOGGLE_LOW_BEAM) {
                    _MIN[11] = _tailLight_Brightness;
                    _MAX[11] = 500;
                    _iter[11] = 21;
                }
                else {
                    _MIN[11] = 0;
                    _MAX[11] = _tailLight_Brightness;
                    _iter[11] = 4;
                }
            }
            else {
                _MIN[11] = 0;
                _MAX[11] = _tailLight_Brightness;
            }
            if (_TOGGLE_STROBE) {
                Flip_Direction(pinLED, pinLED+1); // strobe
                if (_TOGGLE_LOW_BEAM) {
                    _MIN[16] = _parkingLight_Brightness; _MIN[17] = _parkingLight_Brightness;
                    _MAX[16] = _turnSignal_Brightness; _MAX[17] = _turnSignal_Brightness;
                }
                else {
                    _MIN[16] = 0; _MIN[17] = 0;
                    _MAX[16] = _turnSignal_Brightness; _MAX[17] = _turnSignal_Brightness;
                }
            }
            else {
                if (_TOGGLE_LOW_BEAM) {
                    _arr_val_LED[16] = _parkingLight_Brightness;
                    _arr_val_LED[17] = _parkingLight_Brightness;
                }
            }


            Flip_Direction(12, 16); // lightbar
            
            if (config == 1 || config == 2 || config == 3) {
                Flip_Direction(18, 20); // grille
            }
            else {
                _arr_val_LED[18] = 0;
                _arr_val_LED[19] = 0;
            }
            

            // update all LED's
            if (_TOGGLE_WIG_WAG) {
                _arr_val_LED[1] = _MAX[0] - _arr_val_LED[0]; // headlight
                _arr_val_LED[21] = _arr_val_LED[11]; //right taillight

                if (_MAX[11] - _arr_val_LED[11] < 0) {
                    _arr_val_LED[22] = 0;
                }
                else {
                    _arr_val_LED[22] = _MAX[11] - _arr_val_LED[11]; // reverse light
                    }
                _arr_val_LED[23] = _arr_val_LED[22]; // reverse light
                    
            }
            if (config == 1 || config == 2 || config == 3) {
                _arr_val_LED[19] = _MAX[18] - _arr_val_LED[18]; //grille
            }
            if (config == 1) {
                _arr_val_LED[12] = _MAX[13] - _arr_val_LED[13];
            }
            if (config == 4) {
                _arr_val_LED[14] = 4095;
            }
            UpdateLED();

            // iterate only if not waiting
            if (_TOGGLE_WIG_WAG) {
                Flip_Update(0, 1); // headlights
                Flip_Update(11, 12); // taillights
            }
            Flip_Update(12, 16); // lightbar
            if (config == 1 || config == 2 || config == 3) {
                Flip_Update(18, 20); // grille
            }

            if (_TOGGLE_STROBE) {
                Flip_Update(pinLED, pinLED+1); // strobe
                if (_FLIP_FLAG[pinLED] != old_FLAG) {// if it changes, then
                    ++_strobe_iter;
                        if (_strobe_iter > 3) {
                        _strobe_iter = 0;

                        switch (_STROBE_FLIP_FLAG) {
                            case true:
                                _STROBE_FLIP_FLAG = false;
                                break;
                            case false:
                                _STROBE_FLIP_FLAG = true;
                                break;
                        }
                    }
                }
            }
        }

        // reset all
        void Reset() {
            // initialize timer flags
            for (int i = 0; i < arr_LED; ++i) {
                _FLIP_FLAG[i] = false;
                _WAIT_FLAG_ON[i] = false;
                _WAIT_FLAG_OFF[i] = false;
            }

            // initialize lightbar arrays
            for (int i = 0; i < arr_LED; ++i) {
                _arr_val_LED[i] = 0;
                _MAX[i] = 0; _MIN[i] = 0;
                _DELAY_ON[i] = 0; _DELAY_OFF[i] = 0;
                _iter[i] = 0;
            }

             // set const array values
            _MAX[0] = _highBeam_Brightness; _MAX[1] = _highBeam_Brightness; _MAX[2] = _lowBeam_Brightness; // headlights
            for (int i = 3; i < 11; ++i) { // signalmaster
                _MAX[i] = _signalMaster_Brightness;
            }

            // headlights
            _DELAY_ON[0] = 67; _DELAY_OFF[0] = 67;
            _iter[0] = 112; _iter[2] = 37;

            // taillights
            for (int i = 20; i < 24; i++) {
                _MAX[i] = _tailLight_Brightness;
                _DELAY_ON[i] = 233;
                _iter[i] = 4;
            }
            _iter[20] = 1;
            _MAX[20] = _cornerLight_Brightness; _MAX[11] = _tailLight_Brightness;
            _DELAY_ON[11] = 333; _DELAY_OFF[11] = 333;
            _iter[11] = 4;

            // grille lights
            _MAX[18] = 300;
            _DELAY_ON[18] = 167; _DELAY_OFF[18] = 167;
            _iter[18] = 18;
            
            // strobe
            _MAX[16] = 4095; _DELAY_ON[16] = 0; _DELAY_OFF[16] = 133; _iter[16] = 4095;
            _MAX[17] = 4095; _DELAY_ON[17] = 0; _DELAY_OFF[17] = 133; _iter[17] = 4095;
            UpdateLED();

            // reset timers
            _Main_Cycle_Delay.restart();
            _Signal_Arrow_Delay.restart();
            _Signal_Center_Delay.restart();
            _Signal_Warn_Delay.restart();
        }
        
        // reset lightbar
        void Reset_LightBar() {
            for (int i = 12; i < 16; ++i) {
                _arr_val_LED[i] = 0;
                _MAX[i] = 0; _MIN[i] = 0;
                _DELAY_ON[i] = 0; _DELAY_OFF[i] = 0;
                _iter[i] = 0;
            }
            UpdateLED();
        }

        // reset signalmaster
        void Reset_SignalMaster() {
            _INIT_SIGNAL_DIRECT_FLAG = false;
            _INIT_SIGNAL_WARN_FLAG = false;
            for (int i = 3; i < 11; ++i) {
                _arr_val_LED[i] = 0;
            }
        }
        
        // signalmaster direct
        void Signal_Direct(char config) {

            // intitialize start index
            if (!_INIT_SIGNAL_DIRECT_FLAG) {
                switch (config) {
                    case 'R':
                    _Signal_Arrow_Delay.start(167);
                        i = 3;
                        break;
                    case 'L':
                    _Signal_Arrow_Delay.start(167);
                        i = 10;
                        break;
                    case 'C':
                     _Signal_Center_Delay.start(367);
                        i = 6;
                        break;
                }
                _INIT_SIGNAL_DIRECT_FLAG = true;
            }

            if (config == 'R' || config == 'L') {
                if (_Signal_Arrow_Delay.justFinished()) {
                    _Signal_Arrow_Delay.repeat();

                    // set LED values
                    UpdateLED();

                    switch (config) {
                        case 'R': // RIGHT ARROW
                            if (i < 11) { // change LED values right until end
                                _arr_val_LED[i] = _MAX[i];
                            }
                            else {
                                for (int i = 3; i < 11; ++i) { // reset LED's
                                    _arr_val_LED[i] = _MIN[i];
                                }
                                i = 2;
                            }
                            ++i;
                            break;
                        case 'L': // LEFT ARROW
                            if (i > 2) { // change LED values left until end
                                _arr_val_LED[i] = _MAX[i];
                            }
                            else {
                                for (int i = 3; i < 11; ++i) { // reset LED's
                                    _arr_val_LED[i] = _MIN[i];
                                }
                                i = 11;
                            }
                            --i;
                            break;
                    }
                }
            }
            if (config == 'C') {
                if (_Signal_Center_Delay.justFinished()) {
                    _Signal_Center_Delay.repeat();

                    // set LED values
                    UpdateLED();

                    if (i > 2) { // change LED values left until end
                        _arr_val_LED[i] = _MAX[i];
                        _arr_val_LED[13-i] = _MAX[i];
                    } 
                    else {
                        for (int i = 3; i < 11; ++i) { // reset LED's
                            _arr_val_LED[i] = _MIN[i];
                        }
                        i = 7;
                    }
                    --i;
                }
            }
        }

        // signalmaster "Warn 4" config
        void Signal_Warn(int config) {
            int configMAX, configMIN;

            if (!_INIT_SIGNAL_WARN_FLAG) {
                _Signal_Warn_Delay.start(533);
                _INIT_SIGNAL_WARN_FLAG = true;
            }
        
            if (_Signal_Warn_Delay.justFinished()) {
            _Signal_Warn_Delay.repeat();

            switch (config) {
                case 1:
                    configMAX = 4; configMIN = 9;
                    break;
                case 2:
                    configMAX = 5; configMIN = 8;
                    break;
                case 3:
                    configMAX = 7; configMIN = 6;
                    break;
            }

            // warn config 1 through 3
            if (config > 0 && config < 4) {
                switch (_SignalMaster_Warning_RefreshRate) {
                    case false:
                        for (int i = 3; i < 11; ++i) {
                            _arr_val_LED[i] = _MIN[i];
                            if (i > 2 && i < configMAX) {
                                _arr_val_LED[i] = _MAX[i];
                            }
                        }
                        _SignalMaster_Warning_RefreshRate = true;
                        break;
                    case true:
                        for (int i = 3; i < 11; ++i) {
                            _arr_val_LED[i] = _MIN[i];
                            if (i > configMIN && i < 11) {
                                _arr_val_LED[i] = _MAX[i];
                            }
                        }
                        _SignalMaster_Warning_RefreshRate = false;
                        break;
                    }
            }

            if (config == 4) {
                // warn config 4
                switch (_SignalMaster_Warning_RefreshRate) {
                case false:
                    for (int i = 3; i < 11; ++i) {
                        if (i > 4 && i < 9) {
                            _arr_val_LED[i] = _MAX[i];
                        }
                        else {
                            _arr_val_LED[i] = _MIN[i];
                        }
                    }
                    _SignalMaster_Warning_RefreshRate = true;
                    break;
                    
                case true:
                    for (int i = 3; i < 11; ++i) {
                        if (i > 4 && i < 9) {
                            _arr_val_LED[i] = _MIN[i];
                        }
                        else {
                            _arr_val_LED[i] = _MAX[i];
                        }
                    }
                    _SignalMaster_Warning_RefreshRate = false;
                    break;
                }
            }

            // update LED's
            UpdateLED();

        }
        }

        // signalmaster config
        void Signal_Master(int config) {
            switch (config) {
                case 0: // OFF
                    Reset_SignalMaster();
                    break;
                case 1: // warning 1
                    Signal_Warn(config);
                    break;
                case 2: // warning 2
                    Signal_Warn(config);
                    break;
                case 3: // warning 3
                    Signal_Warn(config);
                    break;
                case 4: // warning 4
                    Signal_Warn(config);
                    break;
                case 5: // direct left
                    Signal_Direct('L');
                    break;
                case 6: // direct right
                    Signal_Direct('R');
                    break;
                case 7: // direct center
                    Signal_Direct('C');
                    break;
            }
        }

        // control headlight bulbs
        void Headlight_Control() {
            // LOW BEAM
            if (_TOGGLE_LOW_BEAM) {
                if (_arr_val_LED[2] < _lowBeam_Brightness) {
                    _arr_val_LED[2] += _iter[2]; // low beam
                }
                if (!_TOGGLE_STROBE && _arr_val_LED[16] < _parkingLight_Brightness) { // parking lights
                    _arr_val_LED[16] += 5;
                }
                if (!_TOGGLE_STROBE && _arr_val_LED[17] < _parkingLight_Brightness) { // parking lights
                    _arr_val_LED[17] += 5;
                }
            }
            else {
                if (_arr_val_LED[2] > 0) {
                    _arr_val_LED[2] -= _iter[2]; // low beam
                }
                if (!_TOGGLE_STROBE && _arr_val_LED[16] > 0) { // parking lights
                    _arr_val_LED[16] -= 5;
                }
                if (!_TOGGLE_STROBE && _arr_val_LED[17] > 0) { // parking lights
                    _arr_val_LED[17] -= 5;
                }
                if (!_TOGGLE_STROBE && _arr_val_LED[16] > _parkingLight_Brightness) {
                    _arr_val_LED[16] = 0;
                }
                if (!_TOGGLE_STROBE && _arr_val_LED[17] > _parkingLight_Brightness) {
                    _arr_val_LED[17] = 0;
                }
            }

            // HIGH BEAM
            if (!_TOGGLE_WIG_WAG) {
                if (_TOGGLE_HIGH_BEAM) {
                    if (_arr_val_LED[0] < _highBeam_Brightness)
                        _arr_val_LED[0] += _iter[0];
                    else
                        _arr_val_LED[0] = _highBeam_Brightness;

                    if (_arr_val_LED[1] < _highBeam_Brightness)
                        _arr_val_LED[1] += _iter[0];
                    else
                        _arr_val_LED[1] = _highBeam_Brightness;
                }
                else {
                    if (_arr_val_LED[0] > 112)
                        _arr_val_LED[0] -= _iter[0];
                    else 
                        _arr_val_LED[0] = 0;

                    if (_arr_val_LED[1] > 112)
                        _arr_val_LED[1] -= _iter[0];
                    else 
                        _arr_val_LED[1] = 0;
                }
            }
        }

        // control headlight bulbs
        void Taillight_Control() {
            // PARKING LIGHTS
            if (_TOGGLE_PARKING_LIGHTS || _TOGGLE_LOW_BEAM) {
                if (_arr_val_LED[20] < _cornerLight_Brightness) {
                    _arr_val_LED[20] += _iter[20];
                }
            }
            else {
                if (_arr_val_LED[20] > 0) {
                    _arr_val_LED[20] -= _iter[20];
                }
            }

            // TAIL LIGHTS
            if (!_TOGGLE_WIG_WAG) {
                // reverse ligts
                if (_arr_val_LED[22] > _iter[11])
                    _arr_val_LED[22] -= _iter[11];
                else 
                    _arr_val_LED[22] = 0;

                if (_arr_val_LED[23] > _iter[11]+4)
                    _arr_val_LED[23] -= _iter[11];
                else 
                    _arr_val_LED[23] = 0;

                // tail lights
                if (_TOGGLE_LOW_BEAM) {
                    if (_arr_val_LED[11] < _MAX[11])
                        _arr_val_LED[11] += _iter[11];
                    else
                        _arr_val_LED[11] = _MAX[11];

                    if (_arr_val_LED[21] < _MAX[11])
                        _arr_val_LED[21] += _iter[11];
                    else
                        _arr_val_LED[21] = _MAX[11];
                }
                else {
                    if (_arr_val_LED[11] > _iter[11])
                        _arr_val_LED[11] -= _iter[11];
                    else 
                        _arr_val_LED[11] = 0;

                    if (_arr_val_LED[21] > _iter[11])
                        _arr_val_LED[21] -= _iter[11];
                    else 
                        _arr_val_LED[21] = 0;
                }
            }
        }
};

#endif