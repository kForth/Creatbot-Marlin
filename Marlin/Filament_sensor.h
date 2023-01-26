#pragma once
#include <inttypes.h>

#include "pins.h"
#include "fastio.h"
#include "Timer.h"
#include "pat9125.h"

#define FSENSOR_IR 1
#define FSENSOR_IR_ANALOG 2
#define FSENSOR_PAT9125 3

/// Base class Filament sensor
/// 
/// Ideally, there could have been a nice class hierarchy of filament sensor types with common functionality
/// extracted into this base class.
/// But:
/// - virtual methods take more space
/// - we don't need to switch among different filament sensors at runtime
/// Therefore the class hierarchy carefully avoids using virtual methods and doesn't look too fancy.
#if ENABLED(LASER_FILAMENT_MONITOR)
class Filament_sensor {
public:
    enum class State : uint8_t {
        disabled = 0,
        initializing,
        ready,
        error,
    };
    
    bool isError() const { return state == State::error; }
    bool isReady() const { return state == State::ready; }
    bool isEnabled() const { return state != State::disabled; }
    
protected:    
    bool checkFilamentEvents();
        
    void triggerFilamentRemoved();
            
    void triggerError();
    
    State state;
    ShortTimer eventBlankingTimer;
    bool oldFilamentPresent;
};

class PAT9125_sensor: public Filament_sensor {
public:
    void init();
    void deinit();
    bool update();
    bool getFilamentPresent() const { return filterFilPresent; }
#ifdef FSENSOR_PROBING
    bool probeOtherType(); //checks if the wrong fsensor type is detected.
#endif
    
    void stStep(bool rev) { //from stepper isr
        stepCount += rev ? -1 : 1;
    }
    
private:
    static constexpr uint16_t pollingPeriod = 10; //[ms]
    static constexpr uint8_t filterCnt = 5; //how many checks need to be done in order to determine the filament presence precisely.
    ShortTimer pollingTimer;
    uint8_t filter;
    uint8_t filterFilPresent;
    
    bool jamDetection;
    int16_t oldPos;
    int16_t stepCount;
    int16_t chunkSteps;
    uint8_t jamErrCnt;
    
    void calcChunkSteps(float u) {
        chunkSteps = (int16_t)(1.25 * u); //[mm]
    }
    
    int16_t getStepCount();
    
    void resetStepCount();
    
    void filJam();
    
    bool updatePAT9125();
};

extern PAT9125_sensor fsensor;

#endif //FILAMENT_SENSOR
