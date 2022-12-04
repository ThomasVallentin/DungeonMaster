#ifndef TIME_H
#define TIME_H


class Application;

class Time
{
public:
    inline static double GetTime() { return s_time; }
    inline static double GetDeltaTime() { return s_deltaTime; }

private:
    inline static void SetTime(const double& time)
    {
        s_deltaTime = time - s_time; 
        s_time = time;
    } 

    static double s_time;
    static double s_deltaTime;

    friend Application;
};


#endif // TIME_H