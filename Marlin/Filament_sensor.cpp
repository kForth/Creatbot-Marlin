#include "MarlinConfig.h"

#include <avr/pgmspace.h>
#include <stdio.h>
#include <util/atomic.h>

#include "Filament_sensor.h"
// #include "Timer.h"
#include "cardreader.h"
#include "eeprom.h"
// #include "menu.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"

PAT9125_sensor fsensor;

bool Filament_sensor::checkFilamentEvents() {
    if (state != State::ready)
        return false;
    if (eventBlankingTimer.running() && !eventBlankingTimer.expired(100)) { // event blanking for 100ms
        return false;
    }

    bool newFilamentPresent = fsensor.getFilamentPresent();
    if (oldFilamentPresent != newFilamentPresent) {
        oldFilamentPresent = newFilamentPresent;
        eventBlankingTimer.start();
        if (!newFilamentPresent) {
            triggerFilamentRemoved();
        }
        return true;
    }
    return false;
}

void Filament_sensor::triggerFilamentRemoved() {
    if (planner.blocks_queued() || FILE_IS_PRINT || isSerialPrinting){
        handle_filament_runout();
    }
}

void Filament_sensor::triggerError() {
    state = State::error;
}

void PAT9125_sensor::init() {
    if (state == State::error) {
        deinit(); // deinit first if there was an error.
    }
//    puts_P(PSTR("fsensor::init()"));

    calcChunkSteps(planner.axis_steps_per_mm[E_AXIS]); // for jam detection

    if (!pat9125_init()) {
        deinit();
        triggerError();
        ; //
    }
}

void PAT9125_sensor::deinit() {
//    puts_P(PSTR("fsensor::deinit()"));
    ; //
    state = State::disabled;
    filter = 0;
}

bool PAT9125_sensor::update() {
    switch (state) {
    case State::initializing:
        if (!updatePAT9125()) {
            break; // still not stable. Stay in the initialization state.
        }
        oldFilamentPresent =
            getFilamentPresent(); // initialize the current filament state so that we don't create a switching event right after the sensor is ready.
        oldPos = pat9125_y;
        state = State::ready;
        break;
    case State::ready: {
        updatePAT9125();
        bool event = checkFilamentEvents();

        return event;
    } break;
    case State::disabled:
    case State::error:
    default:
        return false;
    }
    return false;
}

#ifdef FSENSOR_PROBING
bool PAT9125_sensor::probeOtherType() {
    SET_INPUT(IR_SENSOR_PIN); // input mode
    WRITE(IR_SENSOR_PIN, 1);  // pullup
    _delay_us(100); // wait for the pullup to pull the line high (might be needed, not really sure. The internal pullups are quite weak and there might be a
                    // long wire attached).
    bool fsensorDetected = !READ(IR_SENSOR_PIN);
    WRITE(IR_SENSOR_PIN, 0); // no pullup
    return fsensorDetected;
}
#endif

int16_t PAT9125_sensor::getStepCount() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { return stepCount; }
}

void PAT9125_sensor::resetStepCount() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { stepCount = 0; }
}

void PAT9125_sensor::filJam() {
    handle_filament_runout();
}

bool PAT9125_sensor::updatePAT9125() {
    if (jamDetection) {
        int16_t _stepCount = getStepCount();
        if (abs(_stepCount) >= chunkSteps) { // end of chunk. Check distance
            resetStepCount();
            if (!pat9125_update()) { // get up to date data. reinit on error.
                init();              // try to reinit.
            }
            bool fsDir = (pat9125_y - oldPos) > 0;
            bool stDir = _stepCount > 0;
            if (fsDir != stDir) {
                jamErrCnt++;
            } else if (jamErrCnt) {
                jamErrCnt--;
            }
            oldPos = pat9125_y;
        }
        if (jamErrCnt > 10) {
            jamErrCnt = 0;
            filJam();
        }
    }

    if (!pollingTimer.running() || pollingTimer.expired(pollingPeriod)) {
        pollingTimer.start();
        if (!pat9125_update()) {
            init(); // try to reinit.
        }

        bool present = (pat9125_s < 17) || (pat9125_s >= 17 && pat9125_b >= 50);
        if (present != filterFilPresent) {
            filter++;
        } else if (filter) {
            filter--;
        }
        if (filter >= filterCnt) {
            filter = 0;
            filterFilPresent = present;
        }
    }
    return (filter == 0); // return stability
}
