#pragma once

enum class SensorStatus
{
    UNINITIALIZED, // begin() never called / never succeeded
    NOMINAL,       // healthy, ACKing, data trustworthy
    DEGRADED,      // some failures but under threshold — still using data, flagged
    FAILED         // over threshold, data should not be trusted
};

