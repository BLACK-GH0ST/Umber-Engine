#ifndef TIMER_H
#define TIMER_H

/**********************************************************************************************
*
*  File Name   : Timer.h
*
*  Description :
*      High-resolution timer used for measuring elapsed time and frame delta time.
*      This class is typically used in the main engine loop to provide timing
*      information required for frame updates, animations, and physics simulation.
*
*      The timer supports start, stop, reset, and per-frame tick updates.
*
*  Notes :
*      - Uses high-precision performance counters internally.
*      - DeltaTime() returns time between frames.
*      - TotalTime() excludes paused duration.
*
**********************************************************************************************/

class Timer
{
public:

    /**
     * Default constructor.
     * Initializes internal timer state.
     */
  Timer ();

  /**
   * Returns the total elapsed time since reset, excluding paused time.
   *
   * @return Total running time in seconds.
   */
  float TotalTime () const;

  /**
   * Returns the time elapsed between the current frame and the previous frame.
   *
   * @return Delta time in seconds.
   */
  float DeltaTime () const;

  /**
   * Resets the timer to zero.
   * Clears all accumulated time and restarts the timer.
   */
  void Reset ();

  /**
   * Starts or resumes the timer.
   * If the timer was paused, it continues from the last recorded time.
   */
  void Start ();

  /**
   * Stops (pauses) the timer.
   * Elapsed time is preserved but no longer updated until Start() is called.
   */
  void Stop ();

  /**
   * Updates the timer.
   * Should be called once per frame to compute delta time.
   */
  void Tick ();

private:

    /** Conversion factor from counts to seconds. */
  double mSecondsPerCount;

  /** Time between the current frame and the previous frame (in seconds). */
  double mDeltaTime;

  /** Time when the timer was reset. */
  __int64 mBaseTime;

  /** Total time the timer has been paused. */
  __int64 mPausedTime;

  /** Time when the timer was stopped. */
  __int64 mStopTime;

  /** Time at the previous Tick call. */
  __int64 mPrevTime;

  /** Current time at the latest Tick call. */
  __int64 mCurrTime;

  /** Indicates whether the timer is currently stopped. */
  bool mStopped;
};

#endif // TIMER_H