#define I2CBUS 1
#define SENSOR_ADDR 0x76

#include <cstdio>
#include <cstring>
#include <string>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>  /* sleep */

#include "log.h"
#include "config.h"
#include "file_operate.h"

#include "BME280_I2C.h"

#define DEFAULT_CHECK_SPAN 900
static int interrupt = 0;

void usage(const char *command) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", command);
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "  -l: output log directory\n");
    fprintf(stderr, "  -s: check span second\n");
}

void sigcatch(int sig) {
    printf("caught signal: %d\n", sig);
    interrupt = 1;
}

int main(int argc, char *argv[])
{
    int extopt;
    time_t timer;
    struct tm *local;

    daemon_config_t conf = {
        "./log", DEFAULT_CHECK_SPAN
    };

    struct sigaction act = {
        act.sa_handler = sigcatch,
        act.sa_flags = SA_RESTART,
    };
    struct sigaction ign_act = {
        ign_act.sa_handler = SIG_IGN,
        ign_act.sa_flags = SA_RESTART,
    };

    if (sigaction(SIGINT, &act, NULL)  != 0 ||
        sigaction(SIGTERM, &act, NULL) != 0 ||
        sigaction(SIGHUP, &ign_act, NULL) != 0
    ) {
        fprintf(stderr, "failed to set signal handler: %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    if (get_option(argc, argv, &conf, &extopt) != 0) {
        usage(argv[0]);
        return 1;
    }

    char errlog[MAX_PATH_LEN] = {0};
    snprintf(errlog, MAX_PATH_LEN, "%s/error.txt", conf.logdir);


    while(!interrupt) {
        BME280_I2C sensor;

        double humidity    = 0;
        double temperature = 0;
        double pressure    = 0;

        char filename[MAX_PATH_LEN] = {0};

        timer = time(NULL);
        while(timer < 1420038000) { // at least 2015/01/01
            // waiting for system clock set (prevent logs at 1970)
            sleep(30);
            timer = time(NULL);
        }
        local = localtime(&timer);

        snprintf(filename, MAX_PATH_LEN, "%s/%d-%02d-%02d.txt",
               conf.logdir,
               local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);

        try {
            sensor = BME280_I2C();
            sensor.init(I2CBUS, SENSOR_ADDR);

            temperature = sensor.getTemperature();
            humidity = sensor.getHumidity();
            pressure = sensor.getPressure();
        } catch(std::bad_alloc ex) {
            write_log(
                errlog,
                "%d-%02d-%02d %02d:%02d:%02d %s : %s\n",
                local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
                "Error: allocation failed", ex.what());
        } catch(std::string &err_mes) {
            write_log(
                errlog,
                "%d-%02d-%02d %02d:%02d:%02d %s : %s\n",
                local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
                "Error", err_mes.c_str());
        }

        write_log(
            filename,
            "%02d:%02d:%02d,%.2f,%.2f,%.2f\n", local->tm_hour, local->tm_min, local->tm_sec,
            temperature, humidity, pressure);
            
        sleep(conf.span);
    }

    return 0;
}

