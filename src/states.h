#include <fact/statemachine.h>

using namespace util;

enum State
{
  Waking,
  Awake,
  EnteringDoze,
  Doze,
  Sleep
};

enum CapState
{
  RunningOffCap,
  EnteringRunningOffCap,
  ChargingCap,
};

void dozeStateHandler();
void belowThresholdStateHandler();
void aboveThresholdStateHandler(uint16_t vbat);

class CapStateMachine : public StateMachineStatic<CapState>
{
public:
  void process(uint16_t vcap);
};

extern CapStateMachine capStateMachine;
