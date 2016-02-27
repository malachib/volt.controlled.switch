enum State
{
  Waking,
  Awake,
  EnteringDoze,
  Doze,
  Sleep
};

void dozeStateHandler();
void belowThresholdStateHandler();
void aboveThresholdStateHandler();
