# Performance Analysis

Performance analysis is a critical part in instrumenting performance in an application.
A common method to determine how long something takes to run is to use the "stopwatch
method"[^1]. It's a pretty simple test that can be placed just about anywhere in code.
When considering how long an operation takes, millisecond precision is typically
granular enough for most use cases where consistent wallclock time is required.
For performance analysis, this precision isn't granular enough to determine how long
something is running since instructions run on the sub-nanosecond scale. 

Interestingly enough, at least for Windows 11 as of 2024[^2], the underlying assembly
that composes their high resolution timer boils down to a call to `rdtsc` or `rdtscp`
and scaled to a smaller frequency than what the actual instructions are using.

> [!CAUTION]
>   When it comes to analyzing performance and having high resolution wallclock time,
    using a compiler intrinsic like `rdtsc()` comes with some caveats that might not
    be right in instances where we care more about predictable behavior over more
    accurate timing. If the underlying instruction code changes or better timing
    functions are made available in later models of CPUs, it would make sense to leverage
    the operating system's timing functions than a compiler intrinsic to ensure both
    forward and backward compatibility.

The return value of `rdtsc` used to be the number of clock cycles per-instruction, but
over the years, AMD and Intel modified the x86 instruction set specification such that
`rdtsc` returns the number of fixed cycles for all CPUs. Consistency is nice, but we no
longer know exactly how many cycles for a set of instructions took. Anyone familiar with
how modern CPUs work probably might guess why this may be the case. Variability in
processor speed to favor energy efficiency coupled with multi-tasking makes it quite
difficult to measure raw cycles-per-second. As a consequence, we don't actually know
the time correspondence between an `rdtsc` time delta and its wallclock equivalent.
Determining what this actually be, ironically, is wallclocking `rdtsc` instructions to
some fixed time inverval.

In `system.h`, some definitions are created to query the OS timer as well as `rdtsc`.
Within `main.cpp`, a simple loop runs until the difference between two timestamps
reach the OS frequency. This means one second has passed. We can then inject `rdtsc`
in this loop so we can see how many cycles it calculates. The result between a
start and end of `rdtsc` will be the frequency.

Running this program on the machine that I tested this on yields 2,592,000,462. This is a
substantially larger number than 10,000,000. This frequency is not some random number
either. The number that I receive on the machine I ran this on will most likely be different
from the machine that someone else runs it on. Although there is no guarantee that this is
true, the frequency we found actually corresponds to the base-clock speed of the processor.
For my machine, the base-clock speed is 2.59GHz, which corresponds to the returned frequency.
We can't assume that the frequency we get back is always this number since it can be user
modified at the BIOS level. What this means is that we have to time `rdtsc` once to determine
the frequency at the start of the application so that we have this constant cached. We can't
it is a fixed number for all systems.

You might have noticed that the frequency we get back is never the same number between
each run. It might vary by some 100 or so cycles. It's never going to be accurate since
we are effectively guessing this frequency based on how long some slower frequency was
running (using what is effectively the same assembly instruction too).

[^1]:   The stopwatch method simply takes the difference of two times, point A and
        point B, to get the delta time between them. This method is a reliable yet
        intrusive method. Since it requires generating real instructions in code to
        time how long a set of operations take, one of the problems this method has
        is that nesting these stopwatches in code will cause the actual runtime
        performance to "drift" slightly by the amount of time it takes to measure
        a block of code by the amount of time it takes to actually run the stopwatch.
        One of the ways of minimizing this is by estimating the drift and subtracting
        it from the runtime of nested blocks so that timing is more accurate.
[^2]:   Windows' performance frequency is 10,000,000Hz/s, or 10MHz/s which can easily
        be found by running `QueryPerformanceFrequency()` and reading out the value.
        This value is drastically less than what `rdtsc` is clocking at.
