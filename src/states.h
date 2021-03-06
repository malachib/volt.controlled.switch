#include <fact/statemachine.h>

using namespace util;

enum State
{
  Waking, // Starting up/transitioning to awake state
  Awake,  // Solid above threshold
  EnteringDoze,
  Doze,   // Solid below threshold
  EnteringSleep,
  Sleep   // Below threshold for quite some time
};

enum CapState
{
  RunningOffCap,
  EnteringRunningOffCap,
  ChargingCap,
};

void dozeStateHandler();
void belowThresholdStateHandler();
void aboveThresholdStateHandler();

//void ledWaking();
//void ledSleep();
//void ledDoze();

class CapStateMachine : public StateMachineStatic<CapState>
{
public:
  void process(uint16_t vcap);
};

extern CapStateMachine capStateMachine;
extern State state;
