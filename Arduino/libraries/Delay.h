// Developed by @lucns

#ifndef DELAY_H
#define DELAY_H

#define INTERVAL_1S 1000UL
#define INTERVAL_10S 10000UL
#define INTERVAL_30S 30000UL
#define INTERVAL_60S 60000UL

class Delay {
  private:
    unsigned long start;
    unsigned long timeStipulated;
    bool locked;
  public:
    Delay();
    Delay(unsigned long interval);
    void reset();
	void cancel();
	bool isRunning();
    void setTime(unsigned long interval);
    bool gate();
	void forceGate();
	bool isOverTime();
};
#endif
