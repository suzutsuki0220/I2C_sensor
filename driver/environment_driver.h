#ifndef __ENVIRONMENT_DRIVER_H__
#define __ENVIRONMENT_DRIVER_H__

class environment_driver {
private:

public:
    environment_driver();
    ~environment_driver();

    void init(void);
    double getTemperature(void);
    double getHumidity(void);
    double getPressure(void);
};

#endif  // __ENVIRONMENT_DRIVER_H__
